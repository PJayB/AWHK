/*
Copyright (c) 2022 Pete Lewis

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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
};

BOOL InitializeWindowManipulation( 
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

LONG FindPrevIncrement( LONG e, const LONG* edges, DWORD numEdges )
{
	LONG last = edges[0];
	for ( DWORD i = 1; i < numEdges; ++i )
	{
		if ( edges[ i ] >= e  ) 
			return last;
		last = edges[ i ];
	}
	return last;
}

LONG FindNextIncrement( LONG e, const LONG* edges, DWORD numEdges )
{
	for ( DWORD i = 0; i < numEdges; ++i )
	{
		if ( edges[ i ] > e  ) 
			return edges[ i ];
	}
	return edges[ numEdges - 1 ];
}







struct WINDOW_EDGE_SNAP_INFO
{
	LONG* pHorizontalEdges;
	DWORD dwNumHorizontalEdges;
	LONG* pVerticalEdges;
	DWORD dwNumVerticalEdges;
};

BOOL InitializeEdgeMode( 
	const WINDOW_MANIPULATION_INFO* wmi, 
	DWORD dwMaxEdgeSearchSize, 
	BOOL bSnapToGrid,
	WINDOW_EDGE_SNAP_INFO* esi )
{
	esi->pHorizontalEdges = nullptr;
	esi->dwNumHorizontalEdges = 0;
	esi->pVerticalEdges = nullptr;
	esi->dwNumVerticalEdges = 0;

	// Divide up the monitor rect into increments
	LONG nHorizIncrements = 0;
	LONG nVertIncrements = 0;
	if ( bSnapToGrid )
	{
		nHorizIncrements = 1 + ( wmi->MonitorRect.right - wmi->MonitorRect.left ) / wmi->IncrementX;
		nVertIncrements = 1 + ( wmi->MonitorRect.bottom - wmi->MonitorRect.top ) / wmi->IncrementY;
	}

	//
	// Allocate space for the grid
	//
	esi->pHorizontalEdges = new LONG[ nHorizIncrements + dwMaxEdgeSearchSize ];
	if ( !esi->pHorizontalEdges )
		return FALSE;
	
	esi->pVerticalEdges = new LONG[ nVertIncrements + dwMaxEdgeSearchSize ];
	if ( !esi->pVerticalEdges )
	{
		delete [] esi->pHorizontalEdges;
		esi->pHorizontalEdges = nullptr;
		return FALSE;
	}

	// Add the increments
	if ( bSnapToGrid )
	{
		for ( LONG x = wmi->MonitorRect.left; x <= wmi->MonitorRect.right; x += wmi->IncrementX )
			esi->pVerticalEdges[ esi->dwNumVerticalEdges++ ] = x;
		for ( LONG y = wmi->MonitorRect.top; y <= wmi->MonitorRect.bottom; y += wmi->IncrementY )
			esi->pHorizontalEdges[ esi->dwNumHorizontalEdges++ ] = y;
	}

	return TRUE;
}






void SnapWindowUp( const RECT* displayRect, const LONG* edges, DWORD numEdges, RECT* wndRect, LONG safeZone )
{
	if ( !numEdges ) 
		return;

	// If the window is at the top of the screen, roll it up.
	if ( wndRect->top <= displayRect->top )
	{
		wndRect->bottom = max( 
			displayRect->top + safeZone, 
			FindPrevIncrement( wndRect->bottom, edges, numEdges ) );
	}
	else
	{
		wndRect->top = max( 
			displayRect->top, 
			FindPrevIncrement( wndRect->top, edges, numEdges ) );
	}
}

void SnapWindowDown( const RECT* displayRect, const LONG* edges, DWORD numEdges, RECT* wndRect, LONG safeZone )
{
	if ( !numEdges ) 
		return;

	// If the window is at the top of the screen, roll it up.
	if ( wndRect->bottom >= displayRect->bottom )
	{
		wndRect->top = min( 
			displayRect->bottom - safeZone, 
			FindNextIncrement( wndRect->top, edges, numEdges ) );
	}
	else
	{
		wndRect->bottom = min( 
			displayRect->bottom, 
			FindNextIncrement( wndRect->bottom, edges, numEdges ) );
	}
}

void SnapWindowLeft( const RECT* displayRect, const LONG* edges, DWORD numEdges, RECT* wndRect, LONG safeZone )
{
	if ( !numEdges ) 
		return;

	// If the window already hits the left edge, pull in the right
	// edge:
	if ( wndRect->left <= displayRect->left )
	{
		wndRect->right = max( 
			displayRect->left + safeZone, 
			FindPrevIncrement( wndRect->right, edges, numEdges ) );
	}
	else // push out the left edge
	{
		wndRect->left = max( 
			displayRect->left, 
			FindPrevIncrement( wndRect->left, edges, numEdges ) );
	}
}

void SnapWindowRight( const RECT* displayRect, const LONG* edges, DWORD numEdges, RECT* wndRect, LONG safeZone )
{
	if ( !numEdges ) 
		return;

	// If the window already hits the right edge, pull in the left
	// edge:
	if ( wndRect->right >= displayRect->right )
	{
		wndRect->left = min( 
			displayRect->right - safeZone, 
			FindNextIncrement( wndRect->left, edges, numEdges ) );
	}
	else // push out the right edge
	{
		wndRect->right = min( 
			displayRect->right, 
			FindNextIncrement( wndRect->right, edges, numEdges ) );
	}
}

LONG SnapWindowToNearestEdge( 
	const WINDOW_MANIPULATION_INFO* wmi,
	const WINDOW_EDGE_SNAP_INFO* esi,
	DIRECTION direction,
	RECT* outRect )
{
	switch ( direction )
	{
	case DIR_UP:
		{
			SnapWindowUp( 
				&wmi->MonitorRect, 
				esi->pHorizontalEdges, 
				esi->dwNumHorizontalEdges, 
				outRect,
				wmi->SafeZoneY );
		}
		break;
	case DIR_DOWN:
		{
			SnapWindowDown( 
				&wmi->MonitorRect, 
				esi->pHorizontalEdges, 
				esi->dwNumHorizontalEdges, 
				outRect, 
				wmi->SafeZoneY );
		}
		break;
	case DIR_LEFT:
		{
			SnapWindowLeft( 
				&wmi->MonitorRect,
				esi->pVerticalEdges, 
				esi->dwNumVerticalEdges, 
				outRect, 
				wmi->SafeZoneX );
		}
		break;
	case DIR_RIGHT:
		{
			SnapWindowRight( 
				&wmi->MonitorRect, 
				esi->pVerticalEdges, 
				esi->dwNumVerticalEdges, 
				outRect, 
				wmi->SafeZoneX );
		}
		break;
	default:
		return FALSE;
	}

	return DetectWindowChanges( wmi->SrcWindowRect, *outRect );
}





void MoveWindowUp( const RECT* displayRect, const LONG* edges, DWORD numEdges, RECT* wndRect )
{
	if ( !numEdges ) 
		return;

	if ( wndRect->top > displayRect->top )
	{
		LONG height = wndRect->bottom - wndRect->top;
		wndRect->top = FindPrevIncrement( wndRect->top, edges, numEdges );
		wndRect->bottom = wndRect->top + height;
	}
}

void MoveWindowDown( const RECT* displayRect, const LONG* edges, DWORD numEdges, RECT* wndRect )
{
	if ( !numEdges ) 
		return;

	if ( wndRect->bottom < displayRect->bottom )
	{
		LONG height = wndRect->bottom - wndRect->top;
		wndRect->bottom = FindNextIncrement( wndRect->bottom, edges, numEdges );
		wndRect->top = wndRect->bottom - height;
	}
}

void MoveWindowLeft( const RECT* displayRect, const LONG* edges, DWORD numEdges, RECT* wndRect )
{
	if ( !numEdges ) 
		return;

	if ( wndRect->left > displayRect->left )
	{
		LONG width = wndRect->right - wndRect->left;
		wndRect->left = FindPrevIncrement( wndRect->left, edges, numEdges );
		wndRect->right = wndRect->left + width;
	}
}

void MoveWindowRight( const RECT* displayRect, const LONG* edges, DWORD numEdges, RECT* wndRect )
{
	if ( !numEdges ) 
		return;

	if ( wndRect->right < displayRect->right )
	{
		LONG width = wndRect->right - wndRect->left;
		wndRect->right = FindNextIncrement( wndRect->right, edges, numEdges );
		wndRect->left = wndRect->right - width;
	}
}

LONG MoveWindowToNearestEdge( 
	const WINDOW_MANIPULATION_INFO* wmi,
	const WINDOW_EDGE_SNAP_INFO* esi,
	DIRECTION direction,
	RECT* outRect )
{
	switch ( direction )
	{
	case DIR_UP:
		{
			MoveWindowUp( 
				&wmi->MonitorRect, 
				esi->pHorizontalEdges, 
				esi->dwNumHorizontalEdges, 
				outRect );
		}
		break;
	case DIR_DOWN:
		{
			MoveWindowDown( 
				&wmi->MonitorRect, 
				esi->pHorizontalEdges, 
				esi->dwNumHorizontalEdges, 
				outRect );
		}
		break;
	case DIR_LEFT:
		{
			MoveWindowLeft( 
				&wmi->MonitorRect,
				esi->pVerticalEdges, 
				esi->dwNumVerticalEdges, 
				outRect );
		}
		break;
	case DIR_RIGHT:
		{
			MoveWindowRight( 
				&wmi->MonitorRect, 
				esi->pVerticalEdges, 
				esi->dwNumVerticalEdges, 
				outRect );
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
	HWND	hIgnoreWnd;

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
	if ( hWnd == params->hIgnoreWnd )
		return TRUE;

	if ( !::IsWindowVisible( hWnd ) ||
		  ::IsIconic( hWnd ) )
		return TRUE;
	
	WINDOWINFO wndi;
	if ( !::GetWindowInfo( hWnd, &wndi ) )
		return TRUE;

	if ( ( wndi.dwStyle & WS_CHILD ) != 0 )
	{
		return TRUE;
	}

	if ( ( wndi.dwExStyle & WS_EX_NOACTIVATE ) != 0 ||
		 ( wndi.dwExStyle & WS_EX_TOOLWINDOW ) != 0 ||
		 ( wndi.dwExStyle & WS_EX_STATICEDGE ) != 0 )
	{
		return TRUE;
	}

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
	if ( !InitializeWindowManipulation( 
		hWnd,
		params,
		&wmi ) )
	{
		return FALSE;
	}

	RECT newWindowRect = wmi.SrcWindowRect;
	LONG edgeChanges = 0;
	BOOL bSnapToGrid = ( params->Flags & WINDOW_SNAP_TO_GRID ) != 0;
	DWORD dwMaxEdgeSearchSize = params->MaxEdgeSearchSize;

	WINDOW_EDGE_SNAP_INFO esi;
	if ( !InitializeEdgeMode( 
		&wmi, 
		params->MaxEdgeSearchSize, 
		bSnapToGrid,
		&esi ) )
	{
		return FALSE;
	}
	
	if ( ( params->Flags & WINDOW_SNAP_TO_OTHERS ) &&
		 ( dwMaxEdgeSearchSize > 0 ) )
	{
		WINDOW_EDGE_SEARCH_PARAMS wesp;
		wesp.hIgnoreWnd = wmi.hWnd;
		wesp.dwMaxHorizontalEdges = params->MaxEdgeSearchSize;
		wesp.dwMaxVerticalEdges = params->MaxEdgeSearchSize;
		wesp.dwNumHorizontalEdges = esi.dwNumHorizontalEdges;
		wesp.dwNumVerticalEdges = esi.dwNumVerticalEdges;
		wesp.pHorizontalEdges = esi.pHorizontalEdges;
		wesp.pVerticalEdges = esi.pVerticalEdges;
		wesp.ClipRect = wmi.MonitorRect;

		WindowEdgeSearch( &wesp );

		esi.dwNumHorizontalEdges = wesp.dwNumHorizontalEdges;
		esi.dwNumVerticalEdges = wesp.dwNumVerticalEdges;
	}
	
	BOOL bMoveOnly = ( params->Flags & WINDOW_MOVE_ONLY ) != 0;
	BOOL bModifyAdjacent = ( params->Flags & WINDOW_MODIFY_ADJACENT ) != 0;

	if ( bMoveOnly )
	{
		edgeChanges = MoveWindowToNearestEdge(
			&wmi,
			&esi,
			direction,
			&newWindowRect );
	}
	else
	{
		edgeChanges = SnapWindowToNearestEdge(
			&wmi,
			&esi,
			direction,
			&newWindowRect );
	}

	// Delete the edge data (if any)
	delete [] esi.pHorizontalEdges;
	delete [] esi.pVerticalEdges;
	
	// If no changes were made, we're done.
	if ( edgeChanges == 0 )
		return TRUE;

	// If we DON'T have shift down, we could look for other reorganization possibilities
	if ( bModifyAdjacent )
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

