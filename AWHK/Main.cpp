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
#include "..\AWHKShared\Config.h"
#include "..\AWHKShared\IPC.h"
#include "..\AWHKShared\SupportFile.h"

using namespace std;

struct AWHK_HOTKEY_SET
{
	DWORD			dwKeyBits;
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
			L"Advanced Windowing HotKeys",
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
	USHORT vKey,
	USHORT mods )
{
	// Deal with system keys
	if ( vKey == cfg->HelpCombo.Trigger &&
		 mods == cfg->HelpCombo.Modifiers )
	{
		return ShowWebHelp();
	}
	if ( vKey == cfg->ConfigCombo.Trigger &&
		 mods == cfg->ConfigCombo.Modifiers )
	{
		return ShowControlPanel( state );
	}

	WINDOW_SNAP_PARAMS params;
	ConfigureWindowSnapParams(
		cfg,
		mods,
		&params );

	// Which set of arrow keys are we using?
	DIRECTION arrowKeys = DirectionFromVKey( &cfg->ResizeKeys, vKey );
	if ( arrowKeys == DIR_UNKNOWN )
	{
		arrowKeys = DirectionFromVKey( &cfg->MoveKeys, vKey );
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
	return ShowSettingsDialogAsync(
		(ASYNC_FORM_CLOSED_PROC) AsyncControlPanelClosedCallback,
		(LPVOID) pState );
}

DWORD RegisterHotKey_SetBit( DWORD* pKeys, INT hotkeyCount, DWORD keyMod, DWORD vKey )
{
	assert( hotkeyCount > 0 );
	assert( hotkeyCount <= 8 * sizeof( DWORD ) );

	pKeys[hotkeyCount - 1] = MAKELONG( keyMod, vKey );
	if ( ::RegisterHotKey( NULL, hotkeyCount, keyMod, vKey ) )
		return 1 << (hotkeyCount - 1);
	else
	{
		return 0;
	}
}

DWORD RegisterHotKey_SetBit( DWORD* pKeys, INT hotkeyCount, const AWHK_KEY_COMBO* pCombo )
{
	assert( hotkeyCount > 0 );
	assert( hotkeyCount <= 8 * sizeof( DWORD ) );

	pKeys[hotkeyCount - 1] = MAKELONG( pCombo->Modifiers, pCombo->Trigger );
	if ( ::RegisterHotKey( NULL, hotkeyCount, pCombo->Modifiers, pCombo->Trigger ) )
		return 1 << (hotkeyCount - 1);
	else
	{
		return 0;
	}
}

BOOL RegisterArrowKeys( 
	const AWHK_APP_CONFIG* cfg,
	const AWHK_CURSOR_KEYS* pArrowKeys,
	AWHK_HOTKEY_SET* pKeys )
{
	DWORD dwKeyBits = 0;
	LONG hotKeyCount = 0;

	DWORD dwMoveKeyMod = cfg->MoveKeyMod;
	DWORD dwFineKeyMod = cfg->MoveKeyMod | cfg->FineKeyMod;
	DWORD dwSoloKeyMod = cfg->MoveKeyMod | cfg->NextKeyMod;
	DWORD dwAllKeyMods = cfg->MoveKeyMod | cfg->FineKeyMod | cfg->NextKeyMod;

	dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, dwMoveKeyMod, pArrowKeys->LeftKey );
	dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, dwMoveKeyMod, pArrowKeys->RightKey );
	dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, dwMoveKeyMod, pArrowKeys->UpKey );
	dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, dwMoveKeyMod, pArrowKeys->DownKey );

	if ( cfg->FineKeyMod )
	{
		dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, dwFineKeyMod, pArrowKeys->LeftKey );
		dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, dwFineKeyMod, pArrowKeys->RightKey );
		dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, dwFineKeyMod, pArrowKeys->UpKey );
		dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, dwFineKeyMod, pArrowKeys->DownKey );
	}

	if ( cfg->NextKeyMod )
	{
		dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, dwSoloKeyMod, pArrowKeys->LeftKey );
		dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, dwSoloKeyMod, pArrowKeys->RightKey );
		dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, dwSoloKeyMod, pArrowKeys->UpKey );
		dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, dwSoloKeyMod, pArrowKeys->DownKey );
	}

	if ( cfg->FineKeyMod | cfg->NextKeyMod )
	{
		dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, dwAllKeyMods, pArrowKeys->LeftKey );
		dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, dwAllKeyMods, pArrowKeys->RightKey );
		dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, dwAllKeyMods, pArrowKeys->UpKey );
		dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, dwAllKeyMods, pArrowKeys->DownKey );
	}

	pKeys->dwKeyBits = dwKeyBits;
	pKeys->HotKeyCount = hotKeyCount;

	return dwKeyBits == ( ( 1UL << hotKeyCount ) - 1 );
}

BOOL RegisterExtraHotKeys( 
	const AWHK_APP_CONFIG* cfg, 
	AWHK_HOTKEY_SET* pKeys )
{
	DWORD dwKeyBits = 0;
	LONG hotKeyCount = 0;

	dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, &cfg->HelpCombo );
	dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, &cfg->ConfigCombo );

	pKeys->dwKeyBits = dwKeyBits;
	pKeys->HotKeyCount = hotKeyCount;

	return dwKeyBits == ( ( 1UL << hotKeyCount ) - 1 );
}

BOOL RegisterHotKeys( 
	const AWHK_APP_CONFIG* cfg, 
	AWHK_HOTKEYS* pKeys )
{
	if ( !RegisterExtraHotKeys( cfg, &pKeys->ExtraKeys ) )
		return FALSE;

	if ( !cfg->MoveKeyMod )
		return TRUE;

	if ( !RegisterArrowKeys( cfg, &cfg->ResizeKeys, &pKeys->ResizeKeys ) )
		return FALSE;
	
	if ( !RegisterArrowKeys( cfg, &cfg->MoveKeys, &pKeys->MoveKeys ) )
		return FALSE;

	return TRUE;
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

// TODO: we'll need to expand this as we open up more options
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

		WCHAR strTmp[2];

		for ( SIZE_T k = 0; k < sizeof( pKeys->KeySets ) / sizeof( AWHK_HOTKEY_SET ); ++k )
		{
			const AWHK_HOTKEY_SET* pKeySet = &pKeys->KeySets[k];
			for ( LONG i = 0; i < pKeySet->HotKeyCount; ++i )
			{
				if ( ( pKeySet->dwKeyBits & ( 1 << i ) ) == 0 )
				{
					DWORD dwVKey = HIWORD( pKeySet->pdwRegisteredKeys[i] );
					DWORD dwMod = LOWORD( pKeySet->pdwRegisteredKeys[i] );

					LPCWSTR strVKey = GetVKeyString( dwVKey );
					if ( !strVKey )
					{
						swprintf_s(
							strTmp,
							sizeof( strTmp ) / sizeof( *strTmp ),
							L"%c",
							dwVKey );
						strVKey = strTmp;
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
			L"(Debugging stuff: Count: %d %d %d, dwKeyBits: 0x%08X 0x%08X 0x%08X)",
			strKeysFailed,
			pKeys->ExtraKeys.HotKeyCount,
			pKeys->ExtraKeys.dwKeyBits,
			pKeys->ResizeKeys.HotKeyCount,
			pKeys->ResizeKeys.dwKeyBits,
			pKeys->MoveKeys.HotKeyCount,
			pKeys->MoveKeys.dwKeyBits );

		::MessageBox( NULL, 
			strMsg,
			L"Advanced Window HotKeys",
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
			if ( pKeys->KeySets[k].dwKeyBits & ( 1 << i ) )
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
			appState->ControlPanelOpen = TRUE;
			OpenSupportControlPanel( appState );
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
				HandleHotKey(
					appState,
					appCfg,
					HIWORD( msg.lParam ), 
					LOWORD( msg.lParam ) );
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

