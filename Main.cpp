// WindowEnum.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <vector>
#include <sstream>

using namespace std;

enum DIRECTION
{
	DIR_LEFT,
	DIR_RIGHT,
	DIR_UP,
	DIR_DOWN,

	DIR_UNKNOWN
};

enum WINDOW_EDGE
{
	WINDOW_EDGE_LEFT	    = 1,
	WINDOW_EDGE_RIGHT	    = 2,
	WINDOW_EDGE_TOP		    = 4,
	WINDOW_EDGE_BOTTOM	    = 8
};

enum
{
	DISPLAY_GRID_X			= 8,
	DISPLAY_GRID_Y			= 4
};

LONG SafeZoneX( const RECT& displayRect )
{
	return (displayRect.right - displayRect.left) / DISPLAY_GRID_X;
}

LONG SafeZoneY( const RECT& displayRect )
{
	return (displayRect.bottom - displayRect.top) / DISPLAY_GRID_Y;
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

BOOL IsResizeCandidateStyle( const WINDOWINFO& wndi )
{
	return ( wndi.dwStyle & WS_SIZEBOX ) != 0;
}

BOOL GetMonitorRect( HMONITOR hMonitor, RECT* out )
{
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
	// Roll it by one third of the screen resolution
	LONG increment = SafeZoneY( *displayRect );

	// If the window is at the top of the screen, roll it up.
	if ( wndRect->top <= displayRect->top )
	{
		wndRect->bottom = max( displayRect->top + increment, wndRect->bottom - increment );
	}
	else
	{
		wndRect->top = max( displayRect->top, wndRect->top - increment );
	}
}

void RollWindowDown( const RECT* displayRect, RECT* wndRect )
{
	// Roll it by one third of the screen resolution
	LONG increment = SafeZoneY( *displayRect );

	// If the window is at the top of the screen, roll it up.
	if ( wndRect->bottom >= displayRect->bottom )
	{
		wndRect->top = min( displayRect->bottom - increment, wndRect->top + increment );
	}
	else
	{
		wndRect->bottom = min( displayRect->bottom, wndRect->bottom + increment );
	}
}

void RollWindowLeft( const RECT* displayRect, RECT* wndRect )
{
	// Roll it by one eighth of the screen resolution
	LONG increment = SafeZoneX( *displayRect );

	// If the window already hits the left edge, pull in the right
	// edge:
	if ( wndRect->left <= displayRect->left )
	{
		wndRect->right = max( displayRect->left + increment, wndRect->right - increment );
	}
	else // push out the left edge
	{
		wndRect->left = max( displayRect->left, wndRect->left - increment );
	}
}

void RollWindowRight( const RECT* displayRect, RECT* wndRect )
{
	// Roll it by one eighth of the screen resolution
	LONG increment = SafeZoneX( *displayRect );

	// If the window already hits the right edge, pull in the left
	// edge:
	if ( wndRect->right >= displayRect->right )
	{
		wndRect->left = min( displayRect->right - increment, wndRect->left + increment );
	}
	else // push out the right edge
	{
		wndRect->right = min( displayRect->right, wndRect->right + increment );
	}
}

struct WINDOW_MANIPULATION_INFO
{
public:

	HWND hWnd;
	HMONITOR hMonitor;

	RECT MonitorRect;
	RECT SrcWindowRect;
};

BOOL InitalizeWindowManipulation( 
	HWND hWnd,
	WINDOW_MANIPULATION_INFO* wmi )
{
	ZeroMemory( wmi, sizeof( *wmi ) );

	wmi->hWnd = hWnd;

	WINDOWINFO wndi;
	if ( !::GetWindowInfo( hWnd, &wndi ) )
		return FALSE;

	// We should only really do this on resizable windows
	// (although we can do it on non-resizable for lulz)
	if ( !IsResizeCandidateStyle( wndi ) )
		return FALSE;

	wmi->hMonitor = ::MonitorFromWindow(
		hWnd,
		MONITOR_DEFAULTTONEAREST );

	// Get the size of the display this window lives on
	if ( !GetMonitorRect( wmi->hMonitor, &wmi->MonitorRect ) )
		return FALSE;

	// Copy the window rect
	wmi->SrcWindowRect = wndi.rcWindow;

	return TRUE;
}

DWORD DetectWindowChanges( const RECT& src, const RECT& dst )
{
	DWORD changes = 0;
	if ( src.left != dst.left )	    { changes |= WINDOW_EDGE_LEFT; }
	if ( src.right != dst.right )   { changes |= WINDOW_EDGE_RIGHT; }
	if ( src.top != dst.top )	    { changes |= WINDOW_EDGE_TOP; }
	if ( src.bottom != dst.bottom ) { changes |= WINDOW_EDGE_BOTTOM; }
	return changes;
}

LONG ManipulateSingleWindow(
	const WINDOW_MANIPULATION_INFO* wmi,
	DIRECTION direction,
	RECT* outRect )
{
	switch ( direction )
	{
	case DIR_UP:
		{
			RollWindowUp( &wmi->MonitorRect, outRect );
		}
		break;
	case DIR_DOWN:
		{
			RollWindowDown( &wmi->MonitorRect, outRect );
		}
		break;
	case DIR_LEFT:
		{
			RollWindowLeft( &wmi->MonitorRect, outRect );
		}
		break;
	case DIR_RIGHT:
		{
			RollWindowRight( &wmi->MonitorRect, outRect );
		}
		break;
	default:
		return FALSE;
	}

	return DetectWindowChanges( wmi->SrcWindowRect, *outRect );
}

struct WINDOW_ENUM_CB_INFO
{
	DWORD FilterEdges;

	HMONITOR hMonitor;
	HWND hWnd;

	LONG ModifiedCount;

	RECT MonitorRect;
	RECT SourceRect;
	RECT DestRect;
};

void SourcePushLeft( const RECT& monitorRect, const RECT& srcRect, RECT& dstRect, RECT& myRect )
{
	if ( srcRect.left == myRect.right )
	{
		// We found a match. As this is the case, we need to make sure the source window doesn't shove any other windows off the edge of the screen.
		dstRect.left = max( monitorRect.left + SafeZoneX( monitorRect ), dstRect.left );

		// Move the window's right edge to match the src's left
		myRect.right = dstRect.left;
	}
	else if ( srcRect.left == myRect.left )
	{
		// We know we can safely move
		myRect.left = dstRect.left;
	}
}

void SourcePushRight( const RECT& monitorRect, const RECT& srcRect, RECT& dstRect, RECT& myRect )
{
	if ( srcRect.right == myRect.left )
	{
		// We found a match. As this is the case, we need to make sure the source window doesn't shove any other windows off the edge of the screen.
		dstRect.right = min( monitorRect.right - SafeZoneX( monitorRect ), dstRect.right );

		// Move the window's left edge to match the src's right
		myRect.left = dstRect.right;
	}
	else if ( srcRect.right == myRect.right )
	{
		// We know we can safely move
		myRect.right = dstRect.right;
	}
}

void SourcePushTop( const RECT& monitorRect, const RECT& srcRect, RECT& dstRect, RECT& myRect )
{
	if ( srcRect.top == myRect.bottom )
	{
		// We found a match. As this is the case, we need to make sure the source window doesn't shove any other windows off the edge of the screen.
		dstRect.top = max( monitorRect.top + SafeZoneY( monitorRect ), dstRect.top );

		// Move the window's bottom edge to match the src's top
		myRect.bottom = dstRect.top;
	}
	else if ( srcRect.top == myRect.top )
	{
		// We know we can safely move
		myRect.top = dstRect.top;
	}
}

void SourcePushBottom( const RECT& monitorRect, const RECT& srcRect, RECT& dstRect, RECT& myRect )
{
	if ( srcRect.bottom == myRect.top )
	{
		// We found a match. As this is the case, we need to make sure the source window doesn't shove any other windows off the edge of the screen.
		dstRect.bottom = min( monitorRect.bottom - SafeZoneY( monitorRect ), dstRect.bottom );

		// Move the window's top edge to match the src's bottom
		myRect.top = dstRect.bottom;
	}
	else if ( srcRect.bottom == myRect.bottom )
	{
		// We know we can safely move
		myRect.bottom = dstRect.bottom;
	}
}

BOOL JoinedVertically( const RECT& a, const RECT& b )
{
	return ( a.bottom == b.top && a.left == b.left );
}

BOOL CALLBACK AlignAdjacentWindowsProc( 
	HWND hWnd,
	WINDOW_ENUM_CB_INFO* src )
{
	// Ignore the source window (already moved)
	if ( hWnd == src->hWnd )
		return TRUE;
	
	WINDOWINFO wndi;
	if ( !::GetWindowInfo( hWnd, &wndi ) )
		return TRUE;

	// We should only really do this on resizable windows
	// (although we can do it on non-resizable for lulz)
	if ( !IsResizeCandidateStyle( wndi ) )
		return TRUE;

	// Ignore if it's not on the same display
	if ( src->hMonitor != ::MonitorFromWindow(
		hWnd,
		MONITOR_DEFAULTTONEAREST ) )
	{
		return FALSE;
	}

	RECT monitorRect = src->MonitorRect;

	const RECT srcRect = src->SourceRect;
	RECT dstRect = src->DestRect;
	RECT myRect = wndi.rcWindow;

	if ( src->FilterEdges & WINDOW_EDGE_LEFT   ) { SourcePushLeft  ( monitorRect, srcRect, dstRect, myRect ); }
	if ( src->FilterEdges & WINDOW_EDGE_RIGHT  ) { SourcePushRight ( monitorRect, srcRect, dstRect, myRect ); }

	// Only consider windows with the bottom-left and top-left corners touching "joined" in the Y
	if ( JoinedVertically( srcRect, myRect ) || JoinedVertically( myRect, srcRect ) )
	{
		if ( src->FilterEdges & WINDOW_EDGE_TOP    ) { SourcePushTop   ( monitorRect, srcRect, dstRect, myRect ); }
		if ( src->FilterEdges & WINDOW_EDGE_BOTTOM ) { SourcePushBottom( monitorRect, srcRect, dstRect, myRect ); }
	}

	if ( DetectWindowChanges( src->DestRect, dstRect ) )
	{
		src->ModifiedCount++;
		src->DestRect = dstRect;
	}

	if ( DetectWindowChanges( wndi.rcWindow, myRect ) )
	{
		src->ModifiedCount++;
		SetWindowRect( hWnd, myRect );
	}

	return TRUE;
}

struct WINDOW_ADJACENCY_INFO
{
	HMONITOR hSourceMonitor;
	HWND hSourceWindow;
	RECT SourceRect;
	DWORD FilterEdges;
};

LONG AlignAdjacentWindows( 
	const WINDOW_ADJACENCY_INFO* wai,
	RECT* modifiedRect )
{
	WINDOW_ENUM_CB_INFO cbInfo;
	cbInfo.FilterEdges = wai->FilterEdges;
	cbInfo.hMonitor = wai->hSourceMonitor;
	cbInfo.hWnd = wai->hSourceWindow;
	cbInfo.SourceRect = wai->SourceRect;
	cbInfo.DestRect = *modifiedRect;
	cbInfo.ModifiedCount = 0;

	// Get the size of the display this window lives on
	if ( !GetMonitorRect( wai->hSourceMonitor, &cbInfo.MonitorRect ) )
		return 0;

	// Run a callback on each window
	if ( ::EnumWindows(
		(WNDENUMPROC) AlignAdjacentWindowsProc,
		(LPARAM) &cbInfo ) )
	{
		*modifiedRect = cbInfo.DestRect;
		return cbInfo.ModifiedCount;
	}

	return 0;
}

BOOL HandleHelpKey()
{
	return TRUE;
}

BOOL HandleHotKey(
	USHORT vKey,
	USHORT modifiers )
{
	// Deal with system keys
	if ( vKey == VK_F1 )
	{
		return HandleHelpKey();
	}

	// Get the window the user is focused on
	HWND hWnd = ::GetForegroundWindow();


	WINDOW_MANIPULATION_INFO wmi;
	if ( !InitalizeWindowManipulation( 
		hWnd,
		&wmi ) )
	{
		return false;
	}
	
	DIRECTION direction = DirectionFromVKey( vKey );

	// Manipulate the active window.
	RECT newWindowRect = wmi.SrcWindowRect;
	LONG edgeChanges = ManipulateSingleWindow( &wmi, direction, &newWindowRect );
	
	// If no changes were made, we're done.
	if ( edgeChanges == 0 )
		return TRUE;

	// If we DON'T have shift down, we could look for other reorganization possibilities
	if ( ( modifiers & MOD_SHIFT ) == 0 )
	{
		WINDOW_ADJACENCY_INFO wai;
		ZeroMemory( &wai, sizeof( wai ) );

		wai.hSourceMonitor = wmi.hMonitor;
		wai.hSourceWindow = wmi.hWnd;
		wai.FilterEdges = edgeChanges;
		wai.SourceRect = wmi.SrcWindowRect;

		// Align the windows. This may return a modified window rect.
		RECT modifiedRect = newWindowRect;
		if ( AlignAdjacentWindows( &wai, &modifiedRect ) )
			newWindowRect = modifiedRect;
	}

	// Update the window's position
	if ( !SetWindowRect( wmi.hWnd, newWindowRect ) )
		return FALSE;

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
	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT, VK_F1 );
	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT, VK_UP );
	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT, VK_DOWN );
	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT, VK_LEFT );
	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT, VK_RIGHT );
	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT | MOD_SHIFT, VK_UP );
	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT | MOD_SHIFT, VK_DOWN );
	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT | MOD_SHIFT, VK_LEFT );
	RegisterHotKey( NULL, ++hotkeyCount, MOD_ALT | MOD_SHIFT, VK_RIGHT );
	
	int ret = MessageThread( hInstance );

	while ( hotkeyCount )
	{
		::UnregisterHotKey( NULL, hotkeyCount-- );
	}

	return ret;
}

