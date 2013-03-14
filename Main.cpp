// WindowEnum.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <vector>
#include <sstream>

#include "SupportModule.h"
#include "WindowSnap.h"

using namespace std;

struct AWHK_APP_STATE
{
	HINSTANCE		hInstance;
	DWORD			dwMainThreadID;
	UINT			ControlPanelOpenMsg;
	UINT			ControlPanelClosedMsg;
	volatile BOOL	ControlPanelOpen;
};

struct AWHK_APP_CONFIG
{
	AWHK_APP_CONFIG();

	BOOL			AllowSnapToOthers;
	UINT			GridX;
	UINT			GridY;
};

AWHK_APP_CONFIG::AWHK_APP_CONFIG()
	: AllowSnapToOthers( TRUE )
	, GridX( 8 )
	, GridY( 4 )
{
}

DIRECTION DirectionFromVKey( USHORT vKey )
{
	switch ( vKey )
	{
	case VK_LEFT:
		return DIR_LEFT;
	case VK_RIGHT:
		return DIR_RIGHT;
	case VK_UP:
		return DIR_UP;
	case VK_DOWN: 
		return DIR_DOWN;
	default:
		return DIR_UNKNOWN;
	}
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
	if ( ( modifiers & MOD_SHIFT ) == 0 )
		params->Flags |= WINDOW_SNAP_ADJACENT;
	if ( ( modifiers & MOD_CONTROL ) != 0 )
		params->Flags |= WINDOW_SNAP_FINE_GRID;

	params->GridDivisorX = cfg->GridX;
	params->GridDivisorY = cfg->GridY;
}

BOOL HandleHotKey(
	const AWHK_APP_STATE* state,
	const AWHK_APP_CONFIG* cfg,
	USHORT vKey,
	USHORT modifiers )
{
	// Deal with system keys
	if ( vKey == VK_F1 )
	{
		return ShowWebHelp();
	}
	if ( vKey == VK_F2 )
	{
		return ShowControlPanel( state );
	}

	WINDOW_SNAP_PARAMS params;
	ConfigureWindowSnapParams(
		cfg,
		modifiers,
		&params );

	return ForegroundWindowSnap( 
		DirectionFromVKey( vKey ), // direction
		&params // single window only?
		);
}

void LoadConfiguration( AWHK_APP_CONFIG* cfg )
{
	// TODO
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
			// Reload the settings
			LoadConfiguration( appCfg );
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

INT RegisterHotKeys()
{
	INT hotkeyCount = 0;

	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT, VK_F1 );
	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT, VK_F2 );

	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT, VK_UP );
	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT, VK_DOWN );
	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT, VK_LEFT );
	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT, VK_RIGHT );

	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT | MOD_SHIFT, VK_UP );
	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT | MOD_SHIFT, VK_DOWN );
	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT | MOD_SHIFT, VK_LEFT );
	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT | MOD_SHIFT, VK_RIGHT );
	
	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT | MOD_CONTROL, VK_UP );
	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT | MOD_CONTROL, VK_DOWN );
	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT | MOD_CONTROL, VK_LEFT );
	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT | MOD_CONTROL, VK_RIGHT );
	
	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT | MOD_SHIFT | MOD_CONTROL, VK_UP );
	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT | MOD_SHIFT | MOD_CONTROL, VK_DOWN );
	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT | MOD_SHIFT | MOD_CONTROL, VK_LEFT );
	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT | MOD_SHIFT | MOD_CONTROL, VK_RIGHT );

	return hotkeyCount;
}

void UnregisterHotkeys( INT hotkeyCount )
{
	while ( hotkeyCount )
	{
		::UnregisterHotKey( NULL, hotkeyCount-- );
	}
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

	INT hotkeyCount = RegisterHotKeys();

	AWHK_APP_CONFIG appCfg;
	LoadConfiguration( &appCfg );

	int ret = MessageLoop( &appState, &appCfg );

	UnregisterHotkeys( hotkeyCount );

	return ret;
}

