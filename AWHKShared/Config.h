#pragma once

struct AWHK_APP_CONFIG
{
	AWHK_APP_CONFIG();

	BOOL			AllowSnapToOthers;

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
	DWORD			NextKeyMod;
	DWORD			FineKeyMod;
};

BOOL LoadConfiguration( AWHK_APP_CONFIG* cfg );
BOOL SaveConfiguration( const AWHK_APP_CONFIG* cfg );

