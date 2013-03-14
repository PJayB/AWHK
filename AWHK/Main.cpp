// WindowEnum.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <vector>
#include <sstream>

#include "SupportModule.h"
#include "WindowSnap.h"
#include "..\AWHKShared\Config.h"

using namespace std;

struct AWHK_APP_STATE
{
	HINSTANCE		hInstance;
	DWORD			dwMainThreadID;
	UINT			ControlPanelOpenMsg;
	UINT			ControlPanelClosedMsg;
	volatile BOOL	ControlPanelOpen;
	INT				NumHotkeys;
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
		pState->ControlPanelOpenMsg,
		0, 0 );
}

void ConfigureWindowSnapParams( 
	const AWHK_APP_CONFIG* cfg,
	USHORT modifiers,
	WINDOW_SNAP_PARAMS* params )
{
	params->Flags = WINDOW_SNAP_TO_GRID;

	if ( cfg->AllowSnapToOthers )
		params->Flags |= WINDOW_SNAP_TO_OTHERS;
	if ( cfg->AllowModifyAdjacent )
		params->Flags |= WINDOW_MODIFY_ADJACENT;
	if ( ( modifiers & cfg->SoloKeyMod ) == cfg->SoloKeyMod )
		params->Flags |= WINDOW_SNAP_ADJACENT;
	if ( ( modifiers & cfg->FineKeyMod ) == cfg->FineKeyMod )
		params->Flags |= WINDOW_SNAP_FINE_GRID;

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
		pState->ControlPanelClosedMsg,
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
	DWORD soloKeyMod = cfg->MoveKeyMod | cfg->SoloKeyMod;
	DWORD allKeyMods = cfg->MoveKeyMod | cfg->FineKeyMod | cfg->SoloKeyMod;

	RegisterHotKey( NULL, ++hotkeyCount, moveKeyMod, cfg->LeftKey );
	RegisterHotKey( NULL, ++hotkeyCount, moveKeyMod, cfg->RightKey );
	RegisterHotKey( NULL, ++hotkeyCount, moveKeyMod, cfg->UpKey );
	RegisterHotKey( NULL, ++hotkeyCount, moveKeyMod, cfg->DownKey );

	if ( cfg->FineKeyMod )
	{
		RegisterHotKey( NULL, ++hotkeyCount, fineKeyMod, cfg->LeftKey );
		RegisterHotKey( NULL, ++hotkeyCount, fineKeyMod, cfg->RightKey );
		RegisterHotKey( NULL, ++hotkeyCount, fineKeyMod, cfg->UpKey );
		RegisterHotKey( NULL, ++hotkeyCount, fineKeyMod, cfg->DownKey );
	}

	if ( cfg->SoloKeyMod )
	{
		RegisterHotKey( NULL, ++hotkeyCount, soloKeyMod, cfg->LeftKey );
		RegisterHotKey( NULL, ++hotkeyCount, soloKeyMod, cfg->RightKey );
		RegisterHotKey( NULL, ++hotkeyCount, soloKeyMod, cfg->UpKey );
		RegisterHotKey( NULL, ++hotkeyCount, soloKeyMod, cfg->DownKey );
	}

	if ( cfg->FineKeyMod | cfg->SoloKeyMod )
	{
		RegisterHotKey( NULL, ++hotkeyCount, allKeyMods, cfg->LeftKey );
		RegisterHotKey( NULL, ++hotkeyCount, allKeyMods, cfg->RightKey );
		RegisterHotKey( NULL, ++hotkeyCount, allKeyMods, cfg->UpKey );
		RegisterHotKey( NULL, ++hotkeyCount, allKeyMods, cfg->DownKey );
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
		if ( msg.message == appState->ControlPanelOpenMsg &&
			 !appState->ControlPanelOpen )
		{
			appState->ControlPanelOpen = TRUE;
			OpenSupportControlPanel( appState );
			continue;
		}

		if ( msg.message == appState->ControlPanelClosedMsg )
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
	AWHK_APP_STATE appState;
	appState.hInstance = hInstance;
	appState.dwMainThreadID = ::GetCurrentThreadId();
	appState.ControlPanelOpenMsg = ::RegisterWindowMessage( L"AWHKControlPanelOpenMsg" );
	appState.ControlPanelClosedMsg = ::RegisterWindowMessage( L"AWHKControlPanelClosedMsg" );
	appState.ControlPanelOpen = FALSE;
	AWHK_APP_CONFIG appCfg;
	LoadConfiguration( &appCfg );

	RegisterHotKeys(
		&appCfg,
		&appState.NumHotkeys );

	int ret = MessageLoop( &appState, &appCfg );

	UnregisterHotkeys( appState.NumHotkeys );

	return ret;
}

