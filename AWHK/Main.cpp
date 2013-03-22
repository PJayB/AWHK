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

#include "SupportModule.h"
#include "WindowSnap.h"
#include "..\AWHKShared\Config.h"
#include "..\AWHKShared\IPC.h"
#include "..\AWHKShared\SupportFile.h"

using namespace std;

struct AWHK_HOTKEYS
{
	DWORD			dwKeyBits;
	LONG			HotKeyCount;
	DWORD			pdwRegisteredKeys[32];
};

struct AWHK_APP_STATE
{
	HINSTANCE		hInstance;
	DWORD			dwMainThreadID;
	IPC				Comms;

	UINT			MsgOpenControlPanel;
	UINT			MsgControlPanelClosed;
	UINT			MsgReloadConfig;

	volatile BOOL	ControlPanelOpen;
	AWHK_HOTKEYS	HotKeys;
};

DIRECTION DirectionFromVKey( 
	const AWHK_APP_CONFIG* cfg, 
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
	if ( vKey == cfg->HelpKey &&
		 mods == cfg->HelpKeyMod )
	{
		return ShowWebHelp();
	}
	if ( vKey == cfg->ConfigKey &&
		 mods == cfg->ConfigKeyMod )
	{
		return ShowControlPanel( state );
	}

	WINDOW_SNAP_PARAMS params;
	ConfigureWindowSnapParams(
		cfg,
		mods,
		&params );

	return ForegroundWindowSnap( 
		DirectionFromVKey( cfg, vKey ), // direction
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
	pKeys[hotkeyCount - 1] = MAKELONG( keyMod, vKey );
	if ( ::RegisterHotKey( NULL, hotkeyCount, keyMod, vKey ) )
		return 1 << (hotkeyCount - 1);
	else
	{
		return 0;
	}
}

BOOL RegisterHotKeys( 
	const AWHK_APP_CONFIG* cfg, 
	AWHK_HOTKEYS* pKeys )
{
	DWORD dwKeyBits = 0;
	LONG hotKeyCount = 0;

	dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, cfg->HelpKeyMod, cfg->HelpKey );
	dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, cfg->ConfigKeyMod, cfg->ConfigKey );

	if ( !cfg->MoveKeyMod )
	{
		pKeys->dwKeyBits = dwKeyBits;
		pKeys->HotKeyCount = hotKeyCount;
		return TRUE;
	}

	DWORD dwMoveKeyMod = cfg->MoveKeyMod;
	DWORD dwFineKeyMod = cfg->MoveKeyMod | cfg->FineKeyMod;
	DWORD dwSoloKeyMod = cfg->MoveKeyMod | cfg->NextKeyMod;
	DWORD dwAllKeyMods = cfg->MoveKeyMod | cfg->FineKeyMod | cfg->NextKeyMod;

	DWORD pArrowKeys[] = 
	{
		cfg->LeftKey, 
		cfg->RightKey,
		cfg->UpKey,
		cfg->DownKey 
	};
	
	dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, dwMoveKeyMod, pArrowKeys[0] );
	dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, dwMoveKeyMod, pArrowKeys[1] );
	dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, dwMoveKeyMod, pArrowKeys[2] );
	dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, dwMoveKeyMod, pArrowKeys[3] );

	if ( cfg->FineKeyMod )
	{
		dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, dwFineKeyMod, pArrowKeys[0] );
		dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, dwFineKeyMod, pArrowKeys[1] );
		dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, dwFineKeyMod, pArrowKeys[2] );
		dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, dwFineKeyMod, pArrowKeys[3] );
	}

	if ( cfg->NextKeyMod )
	{
		dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, dwSoloKeyMod, pArrowKeys[0] );
		dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, dwSoloKeyMod, pArrowKeys[1] );
		dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, dwSoloKeyMod, pArrowKeys[2] );
		dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, dwSoloKeyMod, pArrowKeys[3] );
	}

	if ( cfg->FineKeyMod | cfg->NextKeyMod )
	{
		dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, dwAllKeyMods, pArrowKeys[0] );
		dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, dwAllKeyMods, pArrowKeys[1] );
		dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, dwAllKeyMods, pArrowKeys[2] );
		dwKeyBits |= RegisterHotKey_SetBit( pKeys->pdwRegisteredKeys, ++hotKeyCount, dwAllKeyMods, pArrowKeys[3] );
	}

	pKeys->dwKeyBits = dwKeyBits;
	pKeys->HotKeyCount = hotKeyCount;

	return dwKeyBits == ( ( 1UL << hotKeyCount ) - 1 );
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
		return L"<unknown>";
	}
#undef KEYMOD
};

BOOL RegisterHotKeysAndWarn( const AWHK_APP_CONFIG* cfg, AWHK_HOTKEYS* pKeys )
{
	if ( !RegisterHotKeys( cfg, pKeys ) )
	{
		WCHAR strKeysFailed[1024] = {0};
		LPWSTR strCursor = strKeysFailed;
		LPCWSTR strEnd = strKeysFailed + sizeof(strKeysFailed) / sizeof(*strKeysFailed);
		for ( LONG i = 0; i < pKeys->HotKeyCount; ++i )
		{
			if ( ( pKeys->dwKeyBits & ( 1 << i ) ) == 0 )
			{
				DWORD dwVKey = HIWORD( pKeys->pdwRegisteredKeys[i] );
				DWORD dwMod = LOWORD( pKeys->pdwRegisteredKeys[i] );

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
					GetVKeyString( dwVKey ) );
			}
		}

		WCHAR strMsg[1024];
		swprintf_s(
			strMsg,
			sizeof( strMsg ) / sizeof( WCHAR ),
			L"Some keys failed to register. Please check the following keys in the settings:\n\n"
			L"%s\n\n"
			L"(Debugging stuff: Count: %d, dwKeyBits: 0x%08X)",
			strKeysFailed,
			pKeys->HotKeyCount,
			pKeys->dwKeyBits );

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
	for ( LONG i = 0; i < pKeys->HotKeyCount; ++i )
	{
		if ( pKeys->dwKeyBits & ( 1 << i ) )
			::UnregisterHotKey( NULL, i + 1 );
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

