/*
	Copyright (C) 2013 Peter J. B. Lewis

    Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
    and associated documentation files (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge, publish, distribute, 
    sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all copies or 
    substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
    BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
    DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

// WindowEnum.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <vector>
#include <sstream>
#include <assert.h>

#include "SupportModule.h"
#include "WindowSnap.h"
#include "MediaKeys.h"
#include "..\AWHKShared\Config.h"
#include "..\AWHKShared\IPC.h"
#include "..\AWHKShared\SupportFile.h"

using namespace std;

#define AWHK_MAX_HOTKEYS    64
#define AWHK_APP_SEM	    L"AWHK_APP_OPEN_SEM" 

struct AWHK_REGISTER_STATUS
{
    DWORD           dwCombo;
    HRESULT         Reason;
};

struct AWHK_REGISTRATION
{
    LONG            RegisteredCount;
    LONG            AttemptCount;
    LONG            ErrorCount;
    AWHK_REGISTER_STATUS StatusCodes[AWHK_MAX_HOTKEYS];
};

struct AWHK_APP_STATE
{
	HINSTANCE		    hInstance;
	DWORD			    dwMainThreadID;
    HANDLE              hAppOpenSemaphore;
	IPC				    Comms;

	UINT			    MsgOpenControlPanel;
	UINT			    MsgControlPanelClosed;
	UINT			    MsgReloadConfig;
    UINT                MsgSuspend;
    UINT                MsgResume;

	volatile BOOL	    ControlPanelOpen;
	AWHK_REGISTRATION   Registration;
};

DIRECTION DirectionFromVKey( 
	const AWHK_CURSOR_KEYS* cfg, 
	USHORT vKey )
{
	if ( vKey == cfg->LeftKey ) 
		return DIR_LEFT;
	if ( vKey == cfg->RightKey )
		return DIR_RIGHT;
	if ( vKey == cfg->UpKey )
		return DIR_UP;
	if ( vKey == cfg->DownKey )
		return DIR_DOWN;

	return DIR_UNKNOWN;
}

BOOL IsAppAlreadyOpen()
{
	HANDLE hSyncSem = ::OpenSemaphore(
		SYNCHRONIZE | SEMAPHORE_MODIFY_STATE,	
		FALSE,
		AWHK_APP_SEM );
	if ( hSyncSem )
	{
		::CloseHandle( hSyncSem );
		return TRUE;
	}

    return FALSE;
}

BOOL AppAlreadyOpenCheck()
{
	// Check if the app is already running
	if ( IsAppAlreadyOpen() )
	{
#ifdef _DEBUG
		if ( ::IsDebuggerPresent() )
		{
			// Kill the old app and allow this to run
			//return FALSE;
		}
#endif

		::MessageBox( 
			nullptr,
			AWHK_MAIN_EXE L" is already running.",
			APPLICATION_TITLE,
			MB_OK | MB_ICONSTOP );

		return TRUE;
	}

	return FALSE;
}

INT IPCThread( AWHK_APP_STATE* appState )
{
	AWHK_IPC_MSG msg;
	while ( ReadMessageIPC( &appState->Comms, &msg ) )
	{
		switch (msg)
		{
		case IPC_MSG_QUIT:
			::PostThreadMessage(
				appState->dwMainThreadID,
				WM_QUIT,
				0, 0 );
			break;

		case IPC_MSG_RELOAD_CONFIG:
			::PostThreadMessage(
				appState->dwMainThreadID,
				appState->MsgReloadConfig,
				0, 0 );
			break;

        case IPC_MSG_SUSPEND:
            ::PostThreadMessage(
                appState->dwMainThreadID,
                appState->MsgSuspend,
                0, 0 );
			break;

        case IPC_MSG_RESUME:
            ::PostThreadMessage(
                appState->dwMainThreadID,
                appState->MsgResume,
                0, 0 );
			break;

		default:
			break;
		}
	}

	return 0;
}

BOOL StartIPCThread( AWHK_APP_STATE* appState )
{
	return ::CreateThread( 
		nullptr,
		0,
		(LPTHREAD_START_ROUTINE) IPCThread,
		(LPVOID) appState,
		0,
		nullptr ) != nullptr;
}

BOOL ShowWebHelp()
{
	return (INT) ShellExecute(
		nullptr,
		L"open",
		L"http://gibbering.net/AWHK/",
		nullptr,
		nullptr,
		SW_SHOWNORMAL ) > 32;
}

BOOL ShowControlPanel( const AWHK_APP_STATE* pState )
{
	return ::PostThreadMessage(
		pState->dwMainThreadID,
		pState->MsgOpenControlPanel,
		0, 0 );
}

void ConfigureWindowSnapParams( 
	const AWHK_APP_CONFIG* cfg,
	USHORT modifiers,
	WINDOW_SNAP_PARAMS* params )
{
	params->Flags = 0;

	if ( cfg->AllowSnapToOthers )
		params->Flags |= WINDOW_SNAP_TO_GRID | WINDOW_SNAP_TO_OTHERS;
	else
		params->Flags |= WINDOW_SNAP_TO_GRID;

	if ( ( modifiers & cfg->NextKeyMod ) == cfg->NextKeyMod )
		params->Flags |= WINDOW_MODIFY_ADJACENT;
	if ( ( modifiers & cfg->FineKeyMod ) == cfg->FineKeyMod )
		params->Flags |= WINDOW_SNAP_FINE_GRID;

	params->MaxEdgeSearchSize = cfg->MaxEdgeSearchSize * 2;
	params->GridDivisorX = cfg->GridX;
	params->GridDivisorY = cfg->GridY;
	params->FineDivisorX = cfg->FineX;
	params->FineDivisorY = cfg->FineY;
}

BOOL HandleHotKey(
	const AWHK_APP_STATE* state,
	const AWHK_APP_CONFIG* cfg,
	AWHK_KEY_COMBO key )
{
	// Deal with system keys
	if ( key == cfg->HelpCombo )
		return ShowWebHelp();
	else if ( key == cfg->ConfigCombo )
		return ShowControlPanel( state );
    else if ( key == cfg->MediaPlayPause )
        return MediaPlayPause();
    else if ( key == cfg->MediaStop )
        return MediaStop();
    else if ( key == cfg->MediaNext )
        return MediaNextTrack();
    else if ( key == cfg->MediaPrev )
        return MediaPrevTrack();
    else if ( key == cfg->MediaMute )
        return MediaMute();
    else if ( key == cfg->MediaVolumeDown )
        return MediaVolumeDown();
    else if ( key == cfg->MediaVolumeUp )
        return MediaVolumeUp();
    

	WINDOW_SNAP_PARAMS params;
	ConfigureWindowSnapParams(
		cfg,
		key.Modifiers,
		&params );

	// Which set of arrow keys are we using?
	DIRECTION arrowKeys = DirectionFromVKey( &cfg->ResizeKeys, key.Trigger );
	if ( arrowKeys == DIR_UNKNOWN )
	{
		arrowKeys = DirectionFromVKey( &cfg->MoveKeys, key.Trigger );
		if ( arrowKeys == DIR_UNKNOWN )
			return FALSE;

		params.Flags |= WINDOW_MOVE_ONLY;
	}

	return ForegroundWindowSnap( 
		arrowKeys, // direction
		&params // single window only?
		);
}

void AsyncControlPanelClosedCallback(
	const AWHK_APP_STATE* pState )
{
	::PostThreadMessage( 
		pState->dwMainThreadID,
		pState->MsgReloadConfig,
		0, 0 );

	::PostThreadMessage( 
		pState->dwMainThreadID,
		pState->MsgControlPanelClosed,
		0, 0 );
}

BOOL OpenSupportControlPanel( const AWHK_APP_STATE* pState )
{
	if ( !ShowSettingsDialogAsync(
		(ASYNC_FORM_CLOSED_PROC) AsyncControlPanelClosedCallback,
		(LPVOID) pState ) )
    {
        ::MessageBox( NULL, 
			APPLICATION_TITLE L" was unable to open the control panel. Please verify your installation.",
			APPLICATION_TITLE,
			MB_ICONERROR | MB_OK );

        return FALSE;
    }

    return TRUE;
}

void RegisterHotKeyAtIndex( DWORD keyMod, DWORD vKey, AWHK_REGISTRATION* pKeyStatus )
{
	assert( pKeyStatus->AttemptCount < AWHK_MAX_HOTKEYS );

    AWHK_REGISTER_STATUS* pResult = &pKeyStatus->StatusCodes[pKeyStatus->AttemptCount++];

	pResult->dwCombo = AWHK_MAKE_HOTKEY( keyMod, vKey );
    if ( keyMod == 0 && vKey == 0 )
    {
        // Not bound
        pResult->Reason = S_FALSE;
        return;
    }

    ::SetLastError( ERROR_SUCCESS );
	if ( ::RegisterHotKey( NULL, pKeyStatus->RegisteredCount, keyMod, vKey ) )
    {
        pKeyStatus->RegisteredCount++;
		pResult->Reason = S_OK;
    }
    else
    {
        pKeyStatus->ErrorCount++;
        pResult->Reason = HRESULT_FROM_WIN32( ::GetLastError() );
    }
}

void RegisterHotKeyAtIndex( const AWHK_KEY_COMBO* pCombo, AWHK_REGISTRATION* pKeyStatus )
{
    RegisterHotKeyAtIndex( pCombo->Modifiers, pCombo->Trigger, pKeyStatus );
}

void RegisterArrowKeys( 
	const AWHK_APP_CONFIG* cfg,
	const AWHK_CURSOR_KEYS* pArrowKeys,
	AWHK_REGISTRATION* pKeyStatus )
{
	DWORD dwMoveKeyMod = cfg->MoveKeyMod;
	DWORD dwFineKeyMod = cfg->MoveKeyMod | cfg->FineKeyMod;
	DWORD dwSoloKeyMod = cfg->MoveKeyMod | cfg->NextKeyMod;
	DWORD dwAllKeyMods = cfg->MoveKeyMod | cfg->FineKeyMod | cfg->NextKeyMod;

	RegisterHotKeyAtIndex( dwMoveKeyMod, pArrowKeys->LeftKey, pKeyStatus );
	RegisterHotKeyAtIndex( dwMoveKeyMod, pArrowKeys->RightKey, pKeyStatus );
	RegisterHotKeyAtIndex( dwMoveKeyMod, pArrowKeys->UpKey, pKeyStatus );
	RegisterHotKeyAtIndex( dwMoveKeyMod, pArrowKeys->DownKey, pKeyStatus );

	if ( cfg->FineKeyMod )
	{
		RegisterHotKeyAtIndex( dwFineKeyMod, pArrowKeys->LeftKey, pKeyStatus );
		RegisterHotKeyAtIndex( dwFineKeyMod, pArrowKeys->RightKey, pKeyStatus );
		RegisterHotKeyAtIndex( dwFineKeyMod, pArrowKeys->UpKey, pKeyStatus );
		RegisterHotKeyAtIndex( dwFineKeyMod, pArrowKeys->DownKey, pKeyStatus );
	}

	if ( cfg->NextKeyMod )
	{
		RegisterHotKeyAtIndex( dwSoloKeyMod, pArrowKeys->LeftKey, pKeyStatus );
		RegisterHotKeyAtIndex( dwSoloKeyMod, pArrowKeys->RightKey, pKeyStatus );
		RegisterHotKeyAtIndex( dwSoloKeyMod, pArrowKeys->UpKey, pKeyStatus );
		RegisterHotKeyAtIndex( dwSoloKeyMod, pArrowKeys->DownKey, pKeyStatus );
	}

	if ( cfg->FineKeyMod | cfg->NextKeyMod )
	{
		RegisterHotKeyAtIndex( dwAllKeyMods, pArrowKeys->LeftKey, pKeyStatus );
		RegisterHotKeyAtIndex( dwAllKeyMods, pArrowKeys->RightKey, pKeyStatus );
		RegisterHotKeyAtIndex( dwAllKeyMods, pArrowKeys->UpKey, pKeyStatus );
		RegisterHotKeyAtIndex( dwAllKeyMods, pArrowKeys->DownKey, pKeyStatus );
	}
}

void RegisterExtraHotKeys( 
	const AWHK_APP_CONFIG* cfg,
	AWHK_REGISTRATION* pKeyStatus )
{
	RegisterHotKeyAtIndex( &cfg->HelpCombo,         pKeyStatus );
	RegisterHotKeyAtIndex( &cfg->ConfigCombo,       pKeyStatus );
	RegisterHotKeyAtIndex( &cfg->MediaPlayPause,    pKeyStatus );
	RegisterHotKeyAtIndex( &cfg->MediaStop,         pKeyStatus );
	RegisterHotKeyAtIndex( &cfg->MediaNext,         pKeyStatus );
	RegisterHotKeyAtIndex( &cfg->MediaPrev,         pKeyStatus );
	RegisterHotKeyAtIndex( &cfg->MediaMute,         pKeyStatus );
	RegisterHotKeyAtIndex( &cfg->MediaVolumeUp,     pKeyStatus );
	RegisterHotKeyAtIndex( &cfg->MediaVolumeDown,   pKeyStatus );
}

void RegisterHotKeys( 
	const AWHK_APP_CONFIG* cfg,
	AWHK_REGISTRATION* pKeyStatus )
{
	RegisterExtraHotKeys( cfg, pKeyStatus );

	if ( !cfg->MoveKeyMod )
		return;

	RegisterArrowKeys( cfg, &cfg->ResizeKeys, pKeyStatus );
	RegisterArrowKeys( cfg, &cfg->MoveKeys, pKeyStatus );
}

LPCWSTR GetKeyModString( DWORD keyMod )
{
#define KEYMOD(x)	case MOD_##x: return TEXT(#x) TEXT(" ")
	switch (keyMod)
	{
	KEYMOD(ALT);
	KEYMOD(CONTROL);
	KEYMOD(SHIFT);
	KEYMOD(WIN);
	default: 
		return L"";
	}
#undef KEYMOD
};

LPCWSTR HResultToString( HRESULT hr, LPWSTR buf, DWORD bufSize )
{
    INT len = (INT) ::FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		buf,
		bufSize,
		nullptr);
    if (len == 0)
    {
        swprintf_s( buf, bufSize, L"Unknown error." );
        return buf;
    }

    // Strip newlines and periods from the end of the string.
    for ( INT i = len-1; i > 0 && (buf[i] == L'\n' || buf[i] == L'\r' || buf[i] == L'.'); --i )
    {
        buf[i] = 0;
    }

    return buf;
}

void ShowRegistrationFailures( const AWHK_REGISTRATION* pKeyState )
{
    // TODO: pass this to the listeners instead
    if ( pKeyState->ErrorCount == 0 )
        return;

    WCHAR strHR[256] = {0};
	WCHAR strMsg[2048] = {0};
	LPCWSTR strEnd = strMsg + _countof(strMsg);
	LPWSTR strCursor = strMsg;

	strCursor += swprintf_s(
		strMsg,
		_countof(strMsg),
		L"Some keys failed to register. Please check the following keys in the settings:\n\n" );

	for ( LONG k = 0; k < pKeyState->AttemptCount; ++k )
	{
		const AWHK_REGISTER_STATUS* pStatus = &pKeyState->StatusCodes[k];

        if ( FAILED( pStatus->Reason ) )
		{
			DWORD dwVKey = AWHK_GET_TRIGGER_KEY( pStatus->dwCombo );
			DWORD dwMod = AWHK_GET_MODIFIER_KEYS( pStatus->dwCombo );
			UINT uScanCode = ::MapVirtualKey( dwVKey, 0 );

            WCHAR strVKey[64] = {0};
            ::GetKeyNameText( 
                uScanCode << 16, 
                strVKey, _countof(strVKey) );
            if ( wcslen(strVKey) == 0 )
			{
				swprintf_s(
					strVKey,
					_countof(strVKey),
					L"0x%X",
					dwVKey );
			}

			strCursor += swprintf_s(
				strCursor, 
				strEnd - strCursor,
				L"%s%s%s%s%s: %s.\n",
				GetKeyModString( dwMod & MOD_ALT ),
				GetKeyModString( dwMod & MOD_SHIFT ),
				GetKeyModString( dwMod & MOD_CONTROL ),
				GetKeyModString( dwMod & MOD_WIN ),
				strVKey,
                HResultToString( pStatus->Reason, strHR, _countof(strHR) ) );
		}
	}

	::MessageBox( NULL, 
		strMsg,
		APPLICATION_TITLE,
		MB_ICONERROR | MB_OK );
}

void RegisterHotKeysAndWarn( AWHK_APP_STATE* appState, const AWHK_APP_CONFIG* cfg )
{
    ZeroMemory( &appState->Registration, sizeof(appState->Registration) );

	RegisterHotKeys( cfg, &appState->Registration );

    if ( appState->Registration.ErrorCount > 0 )
    {
        ShowRegistrationFailures( &appState->Registration );
    }
}

void UnregisterHotkeys( AWHK_REGISTRATION* pReg )
{
    LONG count = pReg->RegisteredCount;
	while ( count-- > 0 )
	{
		::UnregisterHotKey( NULL, count );
	}

    ZeroMemory( pReg, sizeof(*pReg) );
}

int MessageLoop( AWHK_APP_STATE* appState, AWHK_APP_CONFIG* appCfg )
{
	MSG msg;

	while ( GetMessage( &msg, nullptr, 0, 0 ) )
	{
		if ( msg.message == appState->MsgOpenControlPanel &&
			 !appState->ControlPanelOpen )
		{
			appState->ControlPanelOpen = OpenSupportControlPanel( appState );
			continue;
		}

		if ( msg.message == appState->MsgControlPanelClosed &&
			 !appState->ControlPanelOpen )
		{
			appState->ControlPanelOpen = FALSE;
			continue;
		}

		if ( msg.message == appState->MsgReloadConfig )
		{
			UnregisterHotkeys( &appState->Registration );

			// Reload the settings
			LoadConfiguration( appCfg );
			RegisterHotKeysAndWarn( 
                appState,
				appCfg );

			appState->ControlPanelOpen = FALSE;
			continue;
		}

        // The control panel can suspend and resume the functionality without quitting the application
        if ( msg.message == appState->MsgSuspend )
        {
            UnregisterHotkeys( &appState->Registration );
            continue;
        }
        if ( msg.message == appState->MsgResume )
        {
            // Silent reload here.
			RegisterHotKeysAndWarn( 
				appState,
				appCfg );

            continue;
        }

		switch ( msg.message )
		{
		case WM_QUIT:
			{
				return 0;
			}
			break;
		case WM_HOTKEY:
			{
				BOOL bOK = HandleHotKey(
					appState,
					appCfg,
					CreateKeyCombo( msg.lParam ) );
#ifdef _DEBUG
                assert( bOK );
#else
                (void) bOK;
#endif
				break;
			}
			break;
		default:
			{
				TranslateMessage( &msg ); 
				DispatchMessage( &msg );
			}
			break;
		}
	}

	return 0;
}

int test(void* poo)
{
    AWHK_IPC* pIPC = nullptr;
    OpenInterprocessStream( L"AWHKTEST", &pIPC);

    WCHAR tmp[256];
    
    for (UINT i = 0; i < 4096; ++i)
    {
        UINT len = swprintf_s( tmp, _countof(tmp), L"%d: Hello, world!\n", i );
        WriteInterprocessStream( pIPC, &len, sizeof(UINT) );
        WriteInterprocessStream( pIPC, tmp, len * sizeof(WCHAR) );
    }

    CloseInterprocessStream(pIPC);
    return 0;
}

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow )
{
    AWHK_IPC* pIPC = nullptr;
    CreateInterprocessStream( L"AWHKTEST", 1024, &pIPC );
    CreateThread( nullptr, 0, (LPTHREAD_START_ROUTINE) test, nullptr, 0, nullptr );

    for (UINT i = 0; i < 4096; ++i)
    {
        DWORD j = 0;
        ReadInterprocessStream( pIPC, &j, sizeof(j) );

        WCHAR t[256];
        ReadInterprocessStream( pIPC, t, j * sizeof(WCHAR) );

        t[j] = 0;
        OutputDebugStringW( t );
    }

    CloseInterprocessStream(pIPC);


























	if ( AppAlreadyOpenCheck() )
		return -1;

	AWHK_APP_STATE appState;
    ZeroMemory( &appState, sizeof(appState) );
	appState.hInstance = hInstance;
	appState.dwMainThreadID = ::GetCurrentThreadId();
	appState.ControlPanelOpen = FALSE;
	appState.MsgOpenControlPanel = ::RegisterWindowMessage( L"AWHKOpenControlPanelMsg" );
	appState.MsgControlPanelClosed = ::RegisterWindowMessage( L"AWHKControlPanelClosedMsg" );
	appState.MsgReloadConfig = ::RegisterWindowMessage( L"AWHKReloadConfigMsg" );
	appState.MsgSuspend = ::RegisterWindowMessage( L"AWHKSuspendMsg" );
	appState.MsgResume = ::RegisterWindowMessage( L"AWHKResumeMsg" );

    // Open the application semaphore
	SECURITY_ATTRIBUTES sa;
	sa.bInheritHandle = FALSE;
	sa.lpSecurityDescriptor = nullptr;
	sa.nLength = sizeof( sa );

    appState.hAppOpenSemaphore = ::CreateSemaphore(
		&sa,
		0,
		MAXINT32,
		AWHK_APP_SEM );

    // Host the interprocess communication ringbuffer
    CreateIPC( &appState.Comms );

	AWHK_APP_CONFIG appCfg;
    ZeroMemory( &appCfg, sizeof(appCfg) );
	LoadConfiguration( &appCfg );

	RegisterHotKeysAndWarn(
        &appState,
		&appCfg );

	StartIPCThread( &appState );

	int ret = MessageLoop( &appState, &appCfg );

	UnregisterHotkeys( &appState.Registration );
	CloseIPC( &appState.Comms );
    CloseHandle( appState.hAppOpenSemaphore );

	return ret;
}

