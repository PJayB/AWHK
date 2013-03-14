// WindowEnum.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <vector>
#include <sstream>

#include "SupportModule.h"
#include "WindowSnap.h"

using namespace std;

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

BOOL ShowControlPanel()
{
	SupportModule supportLib;

	return supportLib.ShowSettingsDialog();
}

BOOL HandleHotKey(
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
	// TODO: reinstate this
	//	return ShowControlPanel();
		return TRUE;
	}

	WINDOW_SNAP_PARAMS params;
	// TODO: hook these up to some settings
	params.Flags = WINDOW_SNAP_TO_OTHERS | WINDOW_SNAP_TO_GRID;
	if ( ( modifiers & MOD_SHIFT ) == 0 )
		params.Flags |= WINDOW_SNAP_ADJACENT;
	if ( ( modifiers & MOD_CONTROL ) != 0 )
		params.Flags |= WINDOW_SNAP_FINE_GRID;

	// TODO: hook these up to some settings
	params.GridDivisorX = 8;
	params.GridDivisorY = 4;

	return ForegroundWindowSnap( 
		DirectionFromVKey( vKey ), // direction
		&params // single window only?
		);
}

int MessageThread( HINSTANCE hInstance )
{
	MSG msg;

	while ( GetMessage( &msg, nullptr, 0, 0 ) )
	{
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
	
	int ret = MessageThread( hInstance );

	while ( hotkeyCount )
	{
		::UnregisterHotKey( NULL, hotkeyCount-- );
	}

	return ret;
}

