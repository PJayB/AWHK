// WindowEnum.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <vector>
#include <sstream>

using namespace std;

BOOL GetWindowMonitorRect( HWND hWnd, RECT* out )
{
	HMONITOR hMonitor = ::MonitorFromWindow(
		hWnd,
		MONITOR_DEFAULTTONEAREST );

	MONITORINFO moni;
	::ZeroMemory( &moni, sizeof( moni ) );
	moni.cbSize = sizeof( moni );

	if ( !::GetMonitorInfo( 
		hMonitor,
		&moni ) )
	{
		return FALSE;
	}

	*out = moni.rcWork; 
	// rcMonitor is also good but doesn't account for taskbar

	return TRUE;
}

BOOL SetWindowRect( HWND hWnd, const RECT& r )
{
	return ::MoveWindow(
		hWnd, 
		r.left,
		r.top,
		r.right - r.left,
		r.bottom - r.top, 
		TRUE );
}

void RollWindowUp( const RECT* displayRect, RECT* wndRect )
{
	LONG y = wndRect->top;
	LONG height = wndRect->bottom - wndRect->top;

	// If the window is at the top of the screen, roll it up.
	if ( y == displayRect->top )
	{
		height /= 2;
	}
	else
	{
		height += y;
		y = displayRect->top;
	}

	wndRect->top = y;
	wndRect->bottom = wndRect->top + height;
}

void RollWindowDown( const RECT* displayRect, RECT* wndRect )
{
	LONG y = wndRect->top;
	LONG height = wndRect->bottom - wndRect->top;

	LONG desktopHeight = displayRect->bottom - displayRect->top;

	// If the window is at the bottom of the screen, roll it down.
	// TODO:
	if ( y + height * 2 > desktopHeight )
	{
		y += height / 2;
		height /= 2;
	}
	else
	{
		height *= 2;
	}

	wndRect->top = y;
	wndRect->bottom = wndRect->top + height;
}

void RollWindowLeft( const RECT* displayRect, RECT* wndRect )
{
	// Roll it by one eighth of the screen resolution
	LONG displayWidth = displayRect->right - displayRect->left;
	LONG increment = displayWidth / 8;

	// If the window already hits the left edge, pull in the right
	// edge:
	if ( wndRect->left == displayRect->left )
	{
		wndRect->right -= increment;
	}
	else // push out the left edge
	{
		wndRect->left = max( displayRect->left, wndRect->left - increment );
	}
}

void RollWindowRight( const RECT* displayRect, RECT* wndRect )
{
	// Roll it by one eighth of the screen resolution
	LONG displayWidth = displayRect->right - displayRect->left;
	LONG increment = displayWidth / 8;

	// If the window already hits the right edge, pull in the left
	// edge:
	if ( wndRect->right == displayRect->right )
	{
		wndRect->left += increment;
	}
	else // push out the right edge
	{
		wndRect->right = min( displayRect->right, wndRect->right + increment );
	}
}

BOOL HandleHotKey(
	USHORT vKey,
	USHORT modifiers )
{
	HWND hWnd = ::GetForegroundWindow();

	WINDOWINFO wndi;
	if ( !::GetWindowInfo( hWnd, &wndi ) )
		return FALSE;

	// We should only really do this on resizable windows
	// (although we can do it on non-resizable for lulz)
	if ( ( wndi.dwStyle & WS_SIZEBOX ) == 0 )
		return FALSE;

	RECT monitorRect;
	if ( !GetWindowMonitorRect( hWnd, &monitorRect ) )
		return FALSE;

	RECT outRect = wndi.rcWindow;

	switch ( vKey )
	{
	case VK_UP:
		{
			RollWindowUp( &monitorRect, &outRect );
		}
		break;
	case VK_DOWN:
		{
			RollWindowDown( &monitorRect, &outRect );
		}
		break;
	case VK_LEFT:
		{
			RollWindowLeft( &monitorRect, &outRect );
		}
		break;
	case VK_RIGHT:
		{
			RollWindowRight( &monitorRect, &outRect );
		}
		break;
	default:
		break;
	}

	if ( memcmp( &wndi.rcWindow, &outRect, sizeof( outRect ) ) != 0 )
	{
		return SetWindowRect( hWnd, outRect );
	}

	return TRUE;
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
	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT, VK_UP );
	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT, VK_DOWN );
	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT, VK_LEFT );
	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT, VK_RIGHT );
	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT, VK_HOME );
	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT, VK_END );
	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT, VK_F1 );
	
	int ret = MessageThread( hInstance );

	while ( hotkeyCount )
	{
		::UnregisterHotKey( NULL, hotkeyCount-- );
	}

	return ret;
}

