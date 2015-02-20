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

struct AWHK_HOTKEY_SET
{
	ULONGLONG	    qwKeyBits;
	LONG			HotKeyCount;
	DWORD			pdwRegisteredKeys[32];
};

union AWHK_HOTKEYS
{
	struct 
	{
		AWHK_HOTKEY_SET ResizeKeys;
		AWHK_HOTKEY_SET MoveKeys;
		AWHK_HOTKEY_SET	ExtraKeys;
	};

	AWHK_HOTKEY_SET	KeySets[3];
};

struct AWHK_APP_STATE
{
	HINSTANCE		hInstance;
	DWORD			dwMainThreadID;
	IPC				Comms;

	UINT			MsgOpenControlPanel;
	UINT			MsgControlPanelClosed;
	UINT			MsgReloadConfig;
    UINT            MsgSuspend;
    UINT            MsgResume;

	volatile BOOL	ControlPanelOpen;
	AWHK_HOTKEYS	HotKeys;
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
	return IPCExists();
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

BOOL RegisterHotKey_SetBit( DWORD* pKeys, INT hotkeyCount, ULONGLONG* pqwKeyBits, DWORD keyMod, DWORD vKey )
{
	assert( hotkeyCount > 0 );
	assert( hotkeyCount <= 8 * sizeof( DWORD ) );

	pKeys[hotkeyCount - 1] = AWHK_MAKE_HOTKEY( keyMod, vKey );
    if ( keyMod == 0 && vKey == 0 )
    {
        // Not bound
        return TRUE;
    }

	if ( ::RegisterHotKey( NULL, hotkeyCount, keyMod, vKey ) )
    {
		*pqwKeyBits |= 1ULL << (hotkeyCount - 1);
        return TRUE;
    }
	
    return FALSE;
}

BOOL RegisterHotKey_SetBit( DWORD* pKeys, INT hotkeyCount, ULONGLONG* pqwKeyBits, const AWHK_KEY_COMBO* pCombo )
{
	assert( hotkeyCount > 0 );
	assert( hotkeyCount <= 8 * sizeof( DWORD ) );

	pKeys[hotkeyCount - 1] = pCombo->dwBits;
    if ( pCombo->dwBits == 0 )
    {
        // Not bound
        return TRUE;
    }

	if ( ::RegisterHotKey( NULL, hotkeyCount, pCombo->Modifiers, pCombo->Trigger ) )
    {
		*pqwKeyBits |= 1ULL << (hotkeyCount - 1);
        return TRUE;
    }
	
    return FALSE;
}

BOOL RegisterArrowKeys( 
	const AWHK_APP_CONFIG* cfg,
	const AWHK_CURSOR_KEYS* pArrowKeys,
	AWHK_HOTKEY_SET* pKeys )
{
	ULONGLONG qwKeyBits = 0;
	LONG hotKeyCount = 0;
    BOOL bOK = TRUE;

	DWORD dwMoveKeyMod = cfg->MoveKeyMod;
	DWORD dwFineKeyMod = cfg->MoveKeyMod | cfg->FineKeyMod;
	DWORD dwSoloKeyMod = cfg->MoveKeyMod | cfg->NextKeyMod;
	DWORD dwAllKeyMods = cfg->MoveKeyMod | cfg->FineKeyMod | cfg->NextKeyMod;

	bOK &= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, &qwKeyBits, dwMoveKeyMod, pArrowKeys->LeftKey );
	bOK &= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, &qwKeyBits, dwMoveKeyMod, pArrowKeys->RightKey );
	bOK &= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, &qwKeyBits, dwMoveKeyMod, pArrowKeys->UpKey );
	bOK &= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, &qwKeyBits, dwMoveKeyMod, pArrowKeys->DownKey );

	if ( cfg->FineKeyMod )
	{
		bOK &= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, &qwKeyBits, dwFineKeyMod, pArrowKeys->LeftKey );
		bOK &= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, &qwKeyBits, dwFineKeyMod, pArrowKeys->RightKey );
		bOK &= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, &qwKeyBits, dwFineKeyMod, pArrowKeys->UpKey );
		bOK &= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, &qwKeyBits, dwFineKeyMod, pArrowKeys->DownKey );
	}

	if ( cfg->NextKeyMod )
	{
		bOK &= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, &qwKeyBits, dwSoloKeyMod, pArrowKeys->LeftKey );
		bOK &= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, &qwKeyBits, dwSoloKeyMod, pArrowKeys->RightKey );
		bOK &= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, &qwKeyBits, dwSoloKeyMod, pArrowKeys->UpKey );
		bOK &= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, &qwKeyBits, dwSoloKeyMod, pArrowKeys->DownKey );
	}

	if ( cfg->FineKeyMod | cfg->NextKeyMod )
	{
		bOK &= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, &qwKeyBits, dwAllKeyMods, pArrowKeys->LeftKey );
		bOK &= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, &qwKeyBits, dwAllKeyMods, pArrowKeys->RightKey );
		bOK &= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, &qwKeyBits, dwAllKeyMods, pArrowKeys->UpKey );
		bOK &= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, &qwKeyBits, dwAllKeyMods, pArrowKeys->DownKey );
	}

	pKeys->qwKeyBits = qwKeyBits;
	pKeys->HotKeyCount = hotKeyCount;

	return bOK;
}

BOOL RegisterExtraHotKeys( 
	const AWHK_APP_CONFIG* cfg, 
	AWHK_HOTKEY_SET* pKeys )
{
	ULONGLONG qwKeyBits = 0;
    BOOL bOK = TRUE;
	LONG hotKeyCount = 0;

	bOK &= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, &qwKeyBits, &cfg->HelpCombo );
	bOK &= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, &qwKeyBits, &cfg->ConfigCombo );
	bOK &= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, &qwKeyBits, &cfg->MediaPlayPause );
	bOK &= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, &qwKeyBits, &cfg->MediaStop );
	bOK &= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, &qwKeyBits, &cfg->MediaNext );
	bOK &= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, &qwKeyBits, &cfg->MediaPrev );
	bOK &= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, &qwKeyBits, &cfg->MediaMute );
	bOK &= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, &qwKeyBits, &cfg->MediaVolumeUp );
	bOK &= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, &qwKeyBits, &cfg->MediaVolumeDown );

    assert(hotKeyCount < sizeof(qwKeyBits) * 8);

	pKeys->qwKeyBits = qwKeyBits;
	pKeys->HotKeyCount = hotKeyCount;

	return bOK;
}

BOOL RegisterHotKeys( 
	const AWHK_APP_CONFIG* cfg, 
	AWHK_HOTKEYS* pKeys )
{
	BOOL bRet = RegisterExtraHotKeys( cfg, &pKeys->ExtraKeys );

	if ( !cfg->MoveKeyMod )
		return bRet;

	bRet &= RegisterArrowKeys( cfg, &cfg->ResizeKeys, &pKeys->ResizeKeys );
	bRet &= RegisterArrowKeys( cfg, &cfg->MoveKeys, &pKeys->MoveKeys );

	return bRet;
}

LPCWSTR GetKeyModString( DWORD keyMod )
{
#define KEYMOD(x)	case MOD_##x: return TEXT(#x)
	switch (keyMod)
	{
	KEYMOD(ALT);
	KEYMOD(CONTROL);
	KEYMOD(SHIFT);
	KEYMOD(WIN);
	default: 
		return L"-";
	}
#undef KEYMOD
};

LPCWSTR GetVKeyString( DWORD keyMod )
{
#define KEYMOD(x)	case VK_##x: return TEXT(#x)
	switch (keyMod)
	{
	KEYMOD(LEFT);
	KEYMOD(RIGHT);
	KEYMOD(UP);
	KEYMOD(DOWN);
	KEYMOD(F1);
	KEYMOD(F2);
	default: 
		return nullptr;
	}
#undef KEYMOD
};

BOOL RegisterHotKeysAndWarn( const AWHK_APP_CONFIG* cfg, AWHK_HOTKEYS* pKeys )
{
	static_assert( sizeof( pKeys->KeySets ) > sizeof( AWHK_HOTKEY_SET ), "You messed up, bro." );
	static_assert( ( sizeof( pKeys->KeySets ) % sizeof( AWHK_HOTKEY_SET ) ) == 0, "You messed up, bro." );

	if ( !RegisterHotKeys( cfg, pKeys ) )
	{
		WCHAR strKeysFailed[1024] = {0};
		LPWSTR strCursor = strKeysFailed;
		LPCWSTR strEnd = strKeysFailed + sizeof(strKeysFailed) / sizeof(*strKeysFailed);

        GetLastError();

		for ( SIZE_T k = 0; k < sizeof( pKeys->KeySets ) / sizeof( AWHK_HOTKEY_SET ); ++k )
		{
			const AWHK_HOTKEY_SET* pKeySet = &pKeys->KeySets[k];
			for ( LONG i = 0; i < pKeySet->HotKeyCount; ++i )
			{
				if ( ( pKeySet->qwKeyBits & ( 1ULL << i ) ) == 0 && 
                       pKeySet->pdwRegisteredKeys[i] != 0 )
				{
					DWORD dwVKey = AWHK_GET_TRIGGER_KEY( pKeySet->pdwRegisteredKeys[i] );
					DWORD dwMod = AWHK_GET_MODIFIER_KEYS( pKeySet->pdwRegisteredKeys[i] );
					UINT uScanCode = ::MapVirtualKey( dwVKey, 0 );

                    WCHAR strVKey[256] = {0};
                    ::GetKeyNameText( 
                        uScanCode << 16, 
                        strVKey, _countof(strVKey) );
                    if ( wcslen(strVKey) == 0 )
					{
                        DWORD dwLastError = GetLastError();
						swprintf_s(
							strVKey,
							_countof(strVKey),
							L"0x%X (0x%08X)",
							dwVKey, dwLastError );
					}

					strCursor += swprintf_s(
						strCursor, 
						strEnd - strCursor,
						L"%04X %04X:\t%s\t%s\t%s\t%s\t%s\n",
						dwMod, 
						dwVKey,
						GetKeyModString( dwMod & MOD_ALT ),
						GetKeyModString( dwMod & MOD_SHIFT ),
						GetKeyModString( dwMod & MOD_CONTROL ),
						GetKeyModString( dwMod & MOD_WIN ),
						strVKey );
				}
			}
		}

		WCHAR strMsg[1024];
		swprintf_s(
			strMsg,
			sizeof( strMsg ) / sizeof( WCHAR ),
			L"Some keys failed to register. Please check the following keys in the settings:\n\n"
			L"%s\n\n"
            L"%d Extra: 0x%llX\n"
            L"%d Resize: 0x%llX\n"
            L"%d Move: 0x%llX",
			strKeysFailed,
			pKeys->ExtraKeys.HotKeyCount,
			pKeys->ExtraKeys.qwKeyBits,
			pKeys->ResizeKeys.HotKeyCount,
			pKeys->ResizeKeys.qwKeyBits,
			pKeys->MoveKeys.HotKeyCount,
			pKeys->MoveKeys.qwKeyBits );

		::MessageBox( NULL, 
			strMsg,
			APPLICATION_TITLE,
			MB_ICONERROR | MB_OK );

		return FALSE;
	}

	return TRUE;
}

void UnregisterHotkeys( AWHK_HOTKEYS* pKeys )
{
	for ( SIZE_T k = 0; k < sizeof( pKeys->KeySets ) / sizeof( AWHK_HOTKEY_SET ); ++k )
	{
		for ( LONG i = 0; i < pKeys->KeySets[k].HotKeyCount; ++i )
		{
			if ( pKeys->KeySets[k].qwKeyBits & ( 1ULL << i ) )
				::UnregisterHotKey( NULL, i + 1 );
		}
	}
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
			UnregisterHotkeys( &appState->HotKeys );

			// Reload the settings
			LoadConfiguration( appCfg );
			RegisterHotKeysAndWarn( 
				appCfg,
				&appState->HotKeys );

			appState->ControlPanelOpen = FALSE;
			continue;
		}

        // The control panel can suspend and resume the functionality without quitting the application
        if ( msg.message == appState->MsgSuspend )
        {
            UnregisterHotkeys( &appState->HotKeys );
            continue;
        }
        if ( msg.message == appState->MsgResume )
        {
            // Silent reload here.
			RegisterHotKeys( 
				appCfg,
				&appState->HotKeys );

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

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow )
{
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

	CreateIPC( &appState.Comms );

	AWHK_APP_CONFIG appCfg;
    ZeroMemory( &appCfg, sizeof(appCfg) );
	LoadConfiguration( &appCfg );

	RegisterHotKeysAndWarn(
		&appCfg,
		&appState.HotKeys );

	StartIPCThread( &appState );

	int ret = MessageLoop( &appState, &appCfg );

	UnregisterHotkeys( &appState.HotKeys );

	CloseIPC( &appState.Comms );

	return ret;
}

