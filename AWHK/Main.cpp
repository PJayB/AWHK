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

struct AWHK_APP_STATE
{
	HINSTANCE		hInstance;
	DWORD			dwMainThreadID;
	IPC				Comms;

	UINT			MsgOpenControlPanel;
	UINT			MsgControlPanelClosed;
	UINT			MsgReloadConfig;

	volatile BOOL	ControlPanelOpen;
	INT				NumHotkeys;
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
		L"https://github.com/PJayB/AdvancedWindowHotkeys/blob/master/README.md",
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

INT RegisterHotKeys( const AWHK_APP_CONFIG* cfg, INT* pHotkeyCount )
{
	INT hotkeyCount = 0;

	RegisterHotKey( NULL, ++hotkeyCount, cfg->HelpKeyMod, cfg->HelpKey);
	RegisterHotKey( NULL, ++hotkeyCount, cfg->ConfigKeyMod, cfg->ConfigKey );

	DWORD moveKeyMod = cfg->MoveKeyMod;
	DWORD fineKeyMod = cfg->MoveKeyMod | cfg->FineKeyMod;
	DWORD soloKeyMod = cfg->MoveKeyMod | cfg->NextKeyMod;
	DWORD allKeyMods = cfg->MoveKeyMod | cfg->FineKeyMod | cfg->NextKeyMod;

	RegisterHotKey( NULL, ++hotkeyCount, moveKeyMod, cfg->ResizeKeys.LeftKey );
	RegisterHotKey( NULL, ++hotkeyCount, moveKeyMod, cfg->ResizeKeys.RightKey );
	RegisterHotKey( NULL, ++hotkeyCount, moveKeyMod, cfg->ResizeKeys.UpKey );
	RegisterHotKey( NULL, ++hotkeyCount, moveKeyMod, cfg->ResizeKeys.DownKey );

	RegisterHotKey( NULL, ++hotkeyCount, moveKeyMod, cfg->MoveKeys.LeftKey );
	RegisterHotKey( NULL, ++hotkeyCount, moveKeyMod, cfg->MoveKeys.RightKey );
	RegisterHotKey( NULL, ++hotkeyCount, moveKeyMod, cfg->MoveKeys.UpKey );
	RegisterHotKey( NULL, ++hotkeyCount, moveKeyMod, cfg->MoveKeys.DownKey );

	if ( cfg->FineKeyMod )
	{
		RegisterHotKey( NULL, ++hotkeyCount, fineKeyMod, cfg->ResizeKeys.LeftKey );
		RegisterHotKey( NULL, ++hotkeyCount, fineKeyMod, cfg->ResizeKeys.RightKey );
		RegisterHotKey( NULL, ++hotkeyCount, fineKeyMod, cfg->ResizeKeys.UpKey );
		RegisterHotKey( NULL, ++hotkeyCount, fineKeyMod, cfg->ResizeKeys.DownKey );

		RegisterHotKey( NULL, ++hotkeyCount, fineKeyMod, cfg->MoveKeys.LeftKey );
		RegisterHotKey( NULL, ++hotkeyCount, fineKeyMod, cfg->MoveKeys.RightKey );
		RegisterHotKey( NULL, ++hotkeyCount, fineKeyMod, cfg->MoveKeys.UpKey );
		RegisterHotKey( NULL, ++hotkeyCount, fineKeyMod, cfg->MoveKeys.DownKey );
	}

	if ( cfg->NextKeyMod )
	{
		RegisterHotKey( NULL, ++hotkeyCount, soloKeyMod, cfg->ResizeKeys.LeftKey );
		RegisterHotKey( NULL, ++hotkeyCount, soloKeyMod, cfg->ResizeKeys.RightKey );
		RegisterHotKey( NULL, ++hotkeyCount, soloKeyMod, cfg->ResizeKeys.UpKey );
		RegisterHotKey( NULL, ++hotkeyCount, soloKeyMod, cfg->ResizeKeys.DownKey );

		RegisterHotKey( NULL, ++hotkeyCount, soloKeyMod, cfg->MoveKeys.LeftKey );
		RegisterHotKey( NULL, ++hotkeyCount, soloKeyMod, cfg->MoveKeys.RightKey );
		RegisterHotKey( NULL, ++hotkeyCount, soloKeyMod, cfg->MoveKeys.UpKey );
		RegisterHotKey( NULL, ++hotkeyCount, soloKeyMod, cfg->MoveKeys.DownKey );
	}

	if ( cfg->FineKeyMod | cfg->NextKeyMod )
	{
		RegisterHotKey( NULL, ++hotkeyCount, allKeyMods, cfg->ResizeKeys.LeftKey );
		RegisterHotKey( NULL, ++hotkeyCount, allKeyMods, cfg->ResizeKeys.RightKey );
		RegisterHotKey( NULL, ++hotkeyCount, allKeyMods, cfg->ResizeKeys.UpKey );
		RegisterHotKey( NULL, ++hotkeyCount, allKeyMods, cfg->ResizeKeys.DownKey );

		RegisterHotKey( NULL, ++hotkeyCount, allKeyMods, cfg->MoveKeys.LeftKey );
		RegisterHotKey( NULL, ++hotkeyCount, allKeyMods, cfg->MoveKeys.RightKey );
		RegisterHotKey( NULL, ++hotkeyCount, allKeyMods, cfg->MoveKeys.UpKey );
		RegisterHotKey( NULL, ++hotkeyCount, allKeyMods, cfg->MoveKeys.DownKey );
	}

	if ( pHotkeyCount )
		*pHotkeyCount = hotkeyCount;

	return hotkeyCount;
}

void UnregisterHotkeys( INT hotkeyCount )
{
	while ( hotkeyCount )
	{
		::UnregisterHotKey( NULL, hotkeyCount-- );
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
			UnregisterHotkeys( appState->NumHotkeys );

			// Reload the settings
			LoadConfiguration( appCfg );
			RegisterHotKeys( appCfg, &appState->NumHotkeys );

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

	RegisterHotKeys(
		&appCfg,
		&appState.NumHotkeys );

	StartIPCThread( &appState );

	int ret = MessageLoop( &appState, &appCfg );

	UnregisterHotkeys( appState.NumHotkeys );

	CloseIPC( &appState.Comms );

	return ret;
}

