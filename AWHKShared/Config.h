#pragma once

struct AWHK_CURSOR_KEYS
{
	DWORD				LeftKey;
	DWORD				RightKey;
	DWORD				UpKey;
	DWORD				DownKey;
};

struct AWHK_APP_CONFIG
{
	AWHK_APP_CONFIG();

	BOOL				AllowSnapToOthers;

	DWORD				MaxEdgeSearchSize;

	DWORD				GridX;
	DWORD				GridY;
	DWORD				FineX;
	DWORD				FineY;

	DWORD				HelpKey;
	DWORD				HelpKeyMod;
	DWORD				ConfigKey;
	DWORD				ConfigKeyMod;

	AWHK_CURSOR_KEYS	ResizeKeys;
	AWHK_CURSOR_KEYS	MoveKeys;

	DWORD				MoveKeyMod;
	DWORD				NextKeyMod;
	DWORD				FineKeyMod;
};

BOOL LoadConfiguration( AWHK_APP_CONFIG* cfg );
BOOL SaveConfiguration( const AWHK_APP_CONFIG* cfg );

