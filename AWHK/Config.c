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
#include "Config.h"
#include <stdio.h>

#define JOIN(x, y) x##y

AWHK_KEY_COMBO NullKeyCombo()
{
	AWHK_KEY_COMBO k;
	k.dwBits = 0;
	return k;
}

AWHK_CURSOR_KEYS CreateCursorKeys(DWORD left, DWORD right, DWORD up, DWORD down)
{
	AWHK_CURSOR_KEYS k;
	k.LeftKey = left;
	k.RightKey = right;
	k.UpKey = up;
	k.DownKey = down;
	return k;
}

void InitConfiguration(AWHK_APP_CONFIG* cfg)
{
	ZeroMemory(cfg, sizeof(*cfg));

#define CONFIG_VALUE(name, type, value)	cfg-> name = value;
#	include "ConfigDefaults.inl"
#undef CONFIG_VALUE
	/*
	cfg->AllowSnapToOthers				= TRUE;
	cfg->MaxEdgeSearchSize				= 128;
	cfg->GridX							= 8;
	cfg->GridY							= 4;
	cfg->FineX							= 32;
	cfg->FineY							= 16;
	cfg->HelpCombo						= CreateKeyComboFromModAndKey(MOD_ALT, VK_F1);
	cfg->EditConfigCombo				= CreateKeyComboFromModAndKey(MOD_ALT, VK_F2);
	cfg->MoveKeyMod						= MOD_ALT;
	cfg->NextKeyMod						= MOD_CONTROL;
	cfg->FineKeyMod						= MOD_SHIFT;
	cfg->ResizeKeys.LeftKey				= VK_LEFT;
	cfg->ResizeKeys.RightKey			= VK_RIGHT;
	cfg->ResizeKeys.UpKey				= VK_UP;
	cfg->ResizeKeys.DownKey				= VK_DOWN;
	cfg->MoveKeys.LeftKey				= 'A';
	cfg->MoveKeys.RightKey				= 'D';
	cfg->MoveKeys.UpKey					= 'W';
	cfg->MoveKeys.DownKey				= 'S';
	cfg->MediaPrev						= CreateKeyComboFromModAndKey(MOD_ALT | MOD_SHIFT, VK_F11);
	cfg->MediaNext						= CreateKeyComboFromModAndKey(MOD_ALT | MOD_SHIFT, VK_F12);
	// cfg->MediaStop not bound by default.
	cfg->MediaPlayPause					= CreateKeyComboFromModAndKey(MOD_ALT, VK_F9);
	cfg->MediaVolumeMute				= CreateKeyComboFromModAndKey(MOD_ALT, VK_F10);
	cfg->MediaVolumeDown				= CreateKeyComboFromModAndKey(MOD_ALT, VK_F11);
	cfg->MediaVolumeUp					= CreateKeyComboFromModAndKey(MOD_ALT, VK_F12);
	*/
}

BOOL LoadConfiguration(LPCWSTR pConfigFile, AWHK_APP_CONFIG* cfg)
{
	UNUSED(pConfigFile);
	UNUSED(cfg);
	return FALSE;
	/*
	if (!pConfigFile || !*pConfigFile)
		return FALSE;

	HANDLE file = CreateFile(pConfigFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == INVALID_HANDLE_VALUE)
		return FALSE;

	LoadRegistryBool	( AWHK_REG_ALLOW_SNAP	    , &cfg->AllowSnapToOthers );
	LoadConfigGridValue	( AWHK_REG_EDGE_SEARCH	    , &cfg->MaxEdgeSearchSize );

	LoadConfigGridValue	( AWHK_REG_GRID_X		    , &cfg->GridX );
	LoadConfigGridValue	( AWHK_REG_GRID_Y		    , &cfg->GridY );
	LoadConfigGridValue	( AWHK_REG_FINE_X		    , &cfg->FineX );
	LoadConfigGridValue	( AWHK_REG_FINE_Y		    , &cfg->FineY );

	LoadKeyCombo	    ( AWKH_REG_HELP_COMBO	    , &cfg->HelpCombo );
	LoadKeyCombo	    ( AWKH_REG_CFG_COMBO	    , &cfg->EditConfigCombo );
    
	LoadRegistryVKey	( AWHK_REG_RESIZE_LEFT		, &cfg->ResizeKeys.LeftKey );
	LoadRegistryVKey	( AWHK_REG_RESIZE_RIGHT	    , &cfg->ResizeKeys.RightKey );
	LoadRegistryVKey	( AWHK_REG_RESIZE_UP	    , &cfg->ResizeKeys.UpKey );
	LoadRegistryVKey	( AWHK_REG_RESIZE_DOWN		, &cfg->ResizeKeys.DownKey );

	LoadRegistryVKey	( AWHK_REG_MOVE_LEFT	    , &cfg->MoveKeys.LeftKey );
	LoadRegistryVKey	( AWHK_REG_MOVE_RIGHT	    , &cfg->MoveKeys.RightKey );
	LoadRegistryVKey	( AWHK_REG_MOVE_UP		    , &cfg->MoveKeys.UpKey );
	LoadRegistryVKey	( AWHK_REG_MOVE_DOWN	    , &cfg->MoveKeys.DownKey );

    LoadKeyCombo        ( AWHK_REG_MEDIA_PREV       , &cfg->MediaPrev );
    LoadKeyCombo        ( AWHK_REG_MEDIA_NEXT       , &cfg->MediaNext );
    LoadKeyCombo        ( AWHK_REG_MEDIA_STOP       , &cfg->MediaStop );
    LoadKeyCombo        ( AWHK_REG_MEDIA_PLAY_PAUSE , &cfg->MediaPlayPause );
    LoadKeyCombo        ( AWHK_REG_MEDIA_VOLUME_UP  , &cfg->MediaVolumeUp );
    LoadKeyCombo        ( AWHK_REG_MEDIA_VOLUME_DOWN, &cfg->MediaVolumeDown );
    LoadKeyCombo        ( AWHK_REG_MEDIA_MUTE       , &cfg->MediaMute );

	DWORD moveKeyMod = cfg->MoveKeyMod;
	DWORD fineKeyMod = cfg->FineKeyMod;
	DWORD nextKeyMod = cfg->NextKeyMod;
	LoadRegistryKeyMod	( AWHK_REG_MOVE_KEY_MOD	, &moveKeyMod );
	LoadRegistryKeyMod	( AWHK_REG_NEXT_KEY_MOD	, &nextKeyMod );
	LoadRegistryKeyMod	( AWHK_REG_FINE_KEY_MOD	, &fineKeyMod );

	// We MUST have a move key mod (otherwise what's the point?)
	if ( moveKeyMod )
		cfg->MoveKeyMod = moveKeyMod;

	// The two other modifiers must be different
	if ( !fineKeyMod || ( fineKeyMod != moveKeyMod && fineKeyMod != nextKeyMod ) )
		cfg->FineKeyMod = fineKeyMod;
	if ( !nextKeyMod || ( nextKeyMod != moveKeyMod && nextKeyMod != fineKeyMod ) )
		cfg->NextKeyMod = nextKeyMod;

	CloseHandle(file);
    return TRUE;
	*/
}

void WriteConfig_BOOL(FILE* file, LPCTSTR pKey, const BOOL* value)
{
	fwprintf_s(file, L"%s=%s\n", pKey, *value ? L"true" : L"false");
}

void WriteConfig_DWORD(FILE* file, LPCTSTR pKey, const DWORD* value)
{
	fwprintf_s(file, L"%s=%u\n", pKey, *value);
}

LPCWSTR ModifierToString(DWORD mod)
{
	switch (mod)
	{
	case MOD_SHIFT:
		return L"SHIFT";
	case MOD_CONTROL:
		return L"CONTROL";
	case MOD_ALT:
		return L"ALT";
	default:
		assert(0 && "Modifier needs to be only one of SHIFT, ALT or CONTROL");
		return NULL;
	}
}

void WriteConfig_AWHK_KEY_COMBO(FILE* file, LPCTSTR pKey, const AWHK_KEY_COMBO* value)
{
	// todo
	fwprintf_s(file, L"%s=%u\n", pKey, value->dwBits);
}

void WriteConfig_AWHK_CURSOR_KEYS(FILE* file, LPCTSTR pKey, const AWHK_CURSOR_KEYS* value)
{
	// todo
	fwprintf_s(file, L"%s=%u,%u,%u,%u\n", pKey, value->LeftKey, value->RightKey, value->UpKey, value->DownKey);
}

BOOL SaveConfiguration(LPCWSTR pConfigFile, const AWHK_APP_CONFIG* pCfg)
{
	if (!pConfigFile || !*pConfigFile)
		return FALSE;

	FILE* f = NULL;
	if (_wfopen_s(&f, pConfigFile, L"w") != 0)
	{
		return FALSE;
	}

#define CONFIG_VALUE(name, type, val) JOIN(WriteConfig_, type)(f, JOIN(L, #name), &pCfg-> name);
#	include "ConfigDefaults.inl"
#undef CONFIG_VALUE

	fclose(f);
	return TRUE;
}
