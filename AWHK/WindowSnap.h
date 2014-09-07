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

#pragma once

enum DIRECTION
{
	DIR_LEFT,
	DIR_RIGHT,
	DIR_UP,
	DIR_DOWN,

	DIR_UNKNOWN
};

enum WINDOW_SNAP_FLAGS
{
	WINDOW_SNAP_TO_GRID		= 1,
	WINDOW_SNAP_TO_OTHERS	= 2,
	WINDOW_SNAP_FINE_GRID	= 4,
	WINDOW_MODIFY_ADJACENT	= 8,
	WINDOW_MOVE_ONLY		= 16,

	WINDOW_SNAP_DEFAULT		= 0xFF
};

struct WINDOW_SNAP_PARAMS
{
	WINDOW_SNAP_PARAMS();

	DWORD Flags;
	DWORD GridDivisorX;
	DWORD GridDivisorY;
	DWORD FineDivisorX;
	DWORD FineDivisorY;

	// Size is in max possible searchable /edges in each direction/
	// (i.e. num windows * 2)
	DWORD MaxEdgeSearchSize;
};

BOOL 
ForegroundWindowSnap( 
	DIRECTION direction, 
	const WINDOW_SNAP_PARAMS* params );
