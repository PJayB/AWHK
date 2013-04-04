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
