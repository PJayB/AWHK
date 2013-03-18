#include "stdafx.h"
#include "WindowSnap.h"

#include <vector>

enum WINDOW_EDGE
{
	WINDOW_EDGE_LEFT	    = 1,
	WINDOW_EDGE_RIGHT	    = 2,
	WINDOW_EDGE_TOP		    = 4,
	WINDOW_EDGE_BOTTOM	    = 8
};

enum
{
	DEFAULT_GRID_X			= 8,
	DEFAULT_GRID_Y			= 4
};

WINDOW_SNAP_PARAMS::WINDOW_SNAP_PARAMS()
	: Flags( WINDOW_SNAP_DEFAULT )
	, GridDivisorX( DEFAULT_GRID_X )
	, GridDivisorY( DEFAULT_GRID_Y )
	, MaxEdgeSearchSize( 128 )
{
}

LONG SafeZoneX( const RECT& displayRect, DWORD grid )
{
	return (displayRect.right - displayRect.left) / grid;
}

LONG SafeZoneY( const RECT& displayRect, DWORD grid )
{
	return (displayRect.bottom - displayRect.top) / grid;
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

void RollWindowUp( const RECT* displayRect, RECT* wndRect, LONG increment, LONG safeZone )
{
	// If the window is at the top of the screen, roll it up.
	if ( wndRect->top <= displayRect->top )
	{
		wndRect->bottom = max( displayRect->top + safeZone, wndRect->bottom - increment );
	}
	else
	{
		wndRect->top = max( displayRect->top, wndRect->top - increment );
	}
}

void RollWindowDown( const RECT* displayRect, RECT* wndRect, LONG increment, LONG safeZone )
{
	// If the window is at the top of the screen, roll it up.
	if ( wndRect->bottom >= displayRect->bottom )
	{
		wndRect->top = min( displayRect->bottom - safeZone, wndRect->top + increment );
	}
	else
	{
		wndRect->bottom = min( displayRect->bottom, wndRect->bottom + increment );
	}
}

void RollWindowLeft( const RECT* displayRect, RECT* wndRect, LONG increment, LONG safeZone )
{
	// If the window already hits the left edge, pull in the right
	// edge:
	if ( wndRect->left <= displayRect->left )
	{
		wndRect->right = max( displayRect->left + safeZone, wndRect->right - increment );
	}
	else // push out the left edge
	{
		wndRect->left = max( displayRect->left, wndRect->left - increment );
	}
}

void RollWindowRight( const RECT* displayRect, RECT* wndRect, LONG increment, LONG safeZone )
{
	// If the window already hits the right edge, pull in the left
	// edge:
	if ( wndRect->right >= displayRect->right )
	{
		wndRect->left = min( displayRect->right - safeZone, wndRect->left + increment );
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

	DWORD Flags;
	LONG IncrementX;
	LONG IncrementY;
	LONG SafeZoneX;
	LONG SafeZoneY;

	LONG* pHorizontalEdges;
	DWORD dwNumHorizontalEdges;
	LONG* pVerticalEdges;
	DWORD dwNumVerticalEdges;
};

BOOL InitalizeWindowManipulation( 
	HWND hWnd,
	const WINDOW_SNAP_PARAMS* params,
	WINDOW_MANIPULATION_INFO* wmi )
{
	ZeroMemory( wmi, sizeof( *wmi ) );

	wmi->hWnd = hWnd;
	wmi->Flags = params->Flags;

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

	// Set the safe zone
	LONG gridX = max( 1, (LONG) params->GridDivisorX );
	LONG gridY = max( 1, (LONG) params->GridDivisorY );
	wmi->SafeZoneX = SafeZoneX( wmi->MonitorRect, gridX );
	wmi->SafeZoneY = SafeZoneY( wmi->MonitorRect, gridY );

	// Get the increment, which might be at a finer resolution
	if ( ( params->Flags & WINDOW_SNAP_FINE_GRID ) != 0 )
	{
		gridX = max( 1, (LONG) params->FineDivisorX );
		gridY = max( 1, (LONG) params->FineDivisorY );
	}
	wmi->IncrementX = SafeZoneX( wmi->MonitorRect, gridX );
	wmi->IncrementY = SafeZoneY( wmi->MonitorRect, gridY );

	// Copy the window rect
	wmi->SrcWindowRect = wndi.rcWindow;

	wmi->pHorizontalEdges = nullptr;
	wmi->dwNumHorizontalEdges = 0;
	wmi->pVerticalEdges = nullptr;
	wmi->dwNumVerticalEdges = 0;

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
			RollWindowUp( &wmi->MonitorRect, outRect, wmi->IncrementY, wmi->SafeZoneY );
		}
		break;
	case DIR_DOWN:
		{
			RollWindowDown( &wmi->MonitorRect, outRect, wmi->IncrementY, wmi->SafeZoneY );
		}
		break;
	case DIR_LEFT:
		{
			RollWindowLeft( &wmi->MonitorRect, outRect, wmi->IncrementX, wmi->SafeZoneX );
		}
		break;
	case DIR_RIGHT:
		{
			RollWindowRight( &wmi->MonitorRect, outRect, wmi->IncrementX, wmi->SafeZoneX );
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

	LONG SafeZoneX;
	LONG SafeZoneY;
};

void SourcePushLeft( const RECT& monitorRect, const RECT& srcRect, LONG safeZone, RECT& dstRect, RECT& myRect )
{
	if ( srcRect.left == myRect.right )
	{
		// We found a match. As this is the case, we need to make sure the source window doesn't shove any other windows off the edge of the screen.
		dstRect.left = max( monitorRect.left + safeZone, dstRect.left );

		// Move the window's right edge to match the src's left
		myRect.right = dstRect.left;
	}
	else if ( srcRect.left == myRect.left )
	{
		// We know we can safely move
		myRect.left = dstRect.left;
	}
}

void SourcePushRight( const RECT& monitorRect, const RECT& srcRect, LONG safeZone, RECT& dstRect, RECT& myRect )
{
	if ( srcRect.right == myRect.left )
	{
		// We found a match. As this is the case, we need to make sure the source window doesn't shove any other windows off the edge of the screen.
		dstRect.right = min( monitorRect.right - safeZone, dstRect.right );

		// Move the window's left edge to match the src's right
		myRect.left = dstRect.right;
	}
	else if ( srcRect.right == myRect.right )
	{
		// We know we can safely move
		myRect.right = dstRect.right;
	}
}

void SourcePushTop( const RECT& monitorRect, const RECT& srcRect, LONG safeZone, RECT& dstRect, RECT& myRect )
{
	if ( srcRect.top == myRect.bottom )
	{
		// We found a match. As this is the case, we need to make sure the source window doesn't shove any other windows off the edge of the screen.
		dstRect.top = max( monitorRect.top + safeZone, dstRect.top );

		// Move the window's bottom edge to match the src's top
		myRect.bottom = dstRect.top;
	}
	else if ( srcRect.top == myRect.top )
	{
		// We know we can safely move
		myRect.top = dstRect.top;
	}
}

void SourcePushBottom( const RECT& monitorRect, const RECT& srcRect, LONG safeZone, RECT& dstRect, RECT& myRect )
{
	if ( srcRect.bottom == myRect.top )
	{
		// We found a match. As this is the case, we need to make sure the source window doesn't shove any other windows off the edge of the screen.
		dstRect.bottom = min( monitorRect.bottom - safeZone, dstRect.bottom );

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

	if ( !::IsWindowVisible( hWnd ) ||
		  ::IsIconic( hWnd ) )
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

	if ( src->FilterEdges & WINDOW_EDGE_LEFT   ) { SourcePushLeft  ( monitorRect, srcRect, src->SafeZoneX, dstRect, myRect ); }
	if ( src->FilterEdges & WINDOW_EDGE_RIGHT  ) { SourcePushRight ( monitorRect, srcRect, src->SafeZoneX, dstRect, myRect ); }

	// Only consider windows with the bottom-left and top-left corners touching "joined" in the Y
	if ( JoinedVertically( srcRect, myRect ) || JoinedVertically( myRect, srcRect ) )
	{
		if ( src->FilterEdges & WINDOW_EDGE_TOP    ) { SourcePushTop   ( monitorRect, srcRect, src->SafeZoneY, dstRect, myRect ); }
		if ( src->FilterEdges & WINDOW_EDGE_BOTTOM ) { SourcePushBottom( monitorRect, srcRect, src->SafeZoneY, dstRect, myRect ); }
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

LONG AlignAdjacentWindows( 
	const WINDOW_MANIPULATION_INFO* wmi,
	DWORD FilterEdges,
	RECT* modifiedRect )
{
	WINDOW_ENUM_CB_INFO cbInfo;
	cbInfo.FilterEdges = FilterEdges;
	cbInfo.hMonitor = wmi->hMonitor;
	cbInfo.hWnd = wmi->hWnd;
	cbInfo.SourceRect = wmi->SrcWindowRect;
	cbInfo.MonitorRect = wmi->MonitorRect;
	cbInfo.DestRect = *modifiedRect;
	cbInfo.ModifiedCount = 0;
	cbInfo.SafeZoneX = wmi->SafeZoneX;
	cbInfo.SafeZoneY = wmi->SafeZoneY;

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

struct WINDOW_EDGE_SEARCH_PARAMS
{
	DWORD	dwMaxHorizontalEdges;
	DWORD	dwMaxVerticalEdges;

	LPLONG  pHorizontalEdges;
	DWORD   dwNumHorizontalEdges;
	LPLONG  pVerticalEdges;
	DWORD   dwNumVerticalEdges;

	RECT	ClipRect;
};

BOOL CALLBACK WindowEdgeSearchProc( 
	HWND hWnd,
	WINDOW_EDGE_SEARCH_PARAMS* params )
{
	if ( !::IsWindowVisible( hWnd ) ||
		  ::IsIconic( hWnd ) )
		return TRUE;
	
	WINDOWINFO wndi;
	if ( !::GetWindowInfo( hWnd, &wndi ) )
		return TRUE;

	if ( wndi.rcWindow.right == wndi.rcWindow.left ||
		 wndi.rcWindow.top == wndi.rcWindow.right )
	{
		 return TRUE;
	}

	if ( wndi.rcWindow.top >= params->ClipRect.top && 
		 wndi.rcWindow.top <= params->ClipRect.bottom )
	{
		params->pHorizontalEdges[ params->dwNumHorizontalEdges++ ] = 
			wndi.rcWindow.top;
	}

	if ( wndi.rcWindow.bottom <= params->ClipRect.bottom &&
		 wndi.rcWindow.bottom >= params->ClipRect.top )
	{
		params->pHorizontalEdges[ params->dwNumHorizontalEdges++ ] = 
			wndi.rcWindow.bottom;
	}

	if ( wndi.rcWindow.left >= params->ClipRect.left &&
		 wndi.rcWindow.left <= params->ClipRect.right )
	{
		params->pVerticalEdges[ params->dwNumVerticalEdges++ ] = 
			wndi.rcWindow.left;
	}

	if ( wndi.rcWindow.right >= params->ClipRect.left &&
		 wndi.rcWindow.right <= params->ClipRect.right )
	{
		params->pVerticalEdges[ params->dwNumVerticalEdges++ ] = 
			wndi.rcWindow.right;
	}

	return 
		( params->dwNumHorizontalEdges != params->dwMaxHorizontalEdges ) &&
		( params->dwNumVerticalEdges != params->dwMaxVerticalEdges );
}

int LongCmp( const void* a, const void* b )
{
	return *((LONG*) a) - *((LONG*) b);
}

DWORD CollapseSearchSpace( LONG* pArray, DWORD dwCount )
{
	if ( !dwCount )
		return 0;

	std::qsort( pArray, dwCount, sizeof( LONG ), LongCmp );

	LONG* pCursor = pArray;
	for ( DWORD i = 1; i < dwCount; ++i )
	{
		if ( pArray[ i ] == *pCursor )
			continue;

		*( ++pCursor ) = pArray[ i ];
	}

	return 1 + ( pCursor - pArray );
}

BOOL WindowEdgeSearch( WINDOW_EDGE_SEARCH_PARAMS* params )
{
	WINDOW_EDGE_SEARCH_PARAMS paramsCopy = *params;

	// Make sure our maximums are divisible by 2
	paramsCopy.dwMaxHorizontalEdges &= ~1UL;
	paramsCopy.dwMaxVerticalEdges &= ~1UL;

	if ( !::EnumWindows(
		(WNDENUMPROC) WindowEdgeSearchProc,
		(LPARAM) &paramsCopy ) )
	{
		return FALSE;
	}

	// Sort and collapse the search space
	params->dwNumHorizontalEdges = CollapseSearchSpace( 
		params->pHorizontalEdges,
		paramsCopy.dwNumHorizontalEdges );
	params->dwNumVerticalEdges = CollapseSearchSpace( 
		params->pVerticalEdges,
		paramsCopy.dwNumVerticalEdges );

	return TRUE;
}

BOOL ForegroundWindowSnap( DIRECTION direction, const WINDOW_SNAP_PARAMS* params )
{
	// Get the window the user is focused on
	HWND hWnd = ::GetForegroundWindow();

	if ( !::IsWindowVisible( hWnd ) ||
		  ::IsIconic( hWnd ) )
		return FALSE;
	
	WINDOW_MANIPULATION_INFO wmi;
	if ( !InitalizeWindowManipulation( 
		hWnd,
		params,
		&wmi ) )
	{
		return FALSE;
	}

	// TODO: build a list of edges to snap to
	// TODO: integrate WINDOW_SNAP_TO_OTHERS
	if ( ( params->Flags & WINDOW_SNAP_TO_OTHERS ) &&
		 ( params->MaxEdgeSearchSize > 0 ) )
	{
		// Build the list of edges
		wmi.pHorizontalEdges = new LONG[ params->MaxEdgeSearchSize ];
		wmi.pVerticalEdges = new LONG[ params->MaxEdgeSearchSize ];

		if ( wmi.pHorizontalEdges && wmi.pVerticalEdges )
		{
			WINDOW_EDGE_SEARCH_PARAMS wesp;
			wesp.dwMaxHorizontalEdges = params->MaxEdgeSearchSize;
			wesp.dwMaxVerticalEdges = params->MaxEdgeSearchSize;
			wesp.dwNumHorizontalEdges = wmi.dwNumHorizontalEdges;
			wesp.dwNumVerticalEdges = wmi.dwNumVerticalEdges;
			wesp.pHorizontalEdges = wmi.pHorizontalEdges;
			wesp.pVerticalEdges = wmi.pVerticalEdges;
			wesp.ClipRect = wmi.MonitorRect;

			WindowEdgeSearch( &wesp );
		}
	}
		
	// Manipulate the active window.
	RECT newWindowRect = wmi.SrcWindowRect;
	LONG edgeChanges = ManipulateSingleWindow( 
		&wmi, 
		direction, 
		&newWindowRect );

	// Delete the edge data (if any)
	delete [] wmi.pHorizontalEdges;
	delete [] wmi.pVerticalEdges;
	
	// If no changes were made, we're done.
	if ( edgeChanges == 0 )
		return TRUE;

	// If we DON'T have shift down, we could look for other reorganization possibilities
	if ( ( params->Flags & WINDOW_MODIFY_ADJACENT ) != 0 )
	{
		// Align the windows. This may return a modified window rect.
		RECT modifiedRect = newWindowRect;
		if ( AlignAdjacentWindows( &wmi, edgeChanges, &modifiedRect ) )
			newWindowRect = modifiedRect;
	}

	// Update the window's position
	if ( !SetWindowRect( wmi.hWnd, newWindowRect ) )
		return FALSE;

	return TRUE;
}

