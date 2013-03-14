#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

struct AWHK_APP_CONFIG
{
	AWHK_APP_CONFIG();

	BOOL			AllowSnapToOthers;
	BOOL			AllowModifyAdjacent;

	DWORD			GridX;
	DWORD			GridY;
	DWORD			FineX;
	DWORD			FineY;

	DWORD			HelpKey;
	DWORD			HelpKeyMod;
	DWORD			ConfigKey;
	DWORD			ConfigKeyMod;

	DWORD			LeftKey;
	DWORD			RightKey;
	DWORD			UpKey;
	DWORD			DownKey;

	DWORD			MoveKeyMod;
	DWORD			SoloKeyMod;
	DWORD			FineKeyMod;
};

BOOL LoadConfiguration( AWHK_APP_CONFIG* cfg );
BOOL SaveConfiguration( const AWHK_APP_CONFIG* cfg );
