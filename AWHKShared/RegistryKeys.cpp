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

#include "stdafx.h"
#include "RegistryKeys.h"

struct DEFAULT_KEY_VALUE
{
    LPCWSTR Name;
    DWORD Value;
};

static const DEFAULT_KEY_VALUE c_DefaultValues[] =
{
    { AWHK_REG_ALLOW_SNAP,          TRUE },
    { AWHK_REG_EDGE_SEARCH,         128 },

    { AWHK_REG_GRID_X,              8 },
    { AWHK_REG_GRID_Y,              4 },
    { AWHK_REG_FINE_X,              32 },
    { AWHK_REG_FINE_Y,              16 },

    { AWKH_REG_HELP_COMBO,          MAKE_KEY_COMBO( MOD_ALT, VK_F1 ) },
    { AWKH_REG_CFG_COMBO,           MAKE_KEY_COMBO( MOD_ALT, VK_F2 ) },

    { AWHK_REG_RESIZE_LEFT,         VK_LEFT },
    { AWHK_REG_RESIZE_RIGHT,        VK_RIGHT },
    { AWHK_REG_RESIZE_UP,           VK_UP },
    { AWHK_REG_RESIZE_DOWN,         VK_DOWN },

    { AWHK_REG_MOVE_LEFT,           'A' },
    { AWHK_REG_MOVE_RIGHT,          'D' },
    { AWHK_REG_MOVE_UP,             'W' },
    { AWHK_REG_MOVE_DOWN,           'S' },

    { AWHK_REG_MOVE_KEY_MOD,        MOD_ALT },
    { AWHK_REG_NEXT_KEY_MOD,        MOD_CONTROL },
    { AWHK_REG_FINE_KEY_MOD,        MOD_SHIFT },

    { AWHK_REG_MEDIA_PREV,          0 },
    { AWHK_REG_MEDIA_NEXT,          0 },
    { AWHK_REG_MEDIA_PLAY,          0 },
    { AWHK_REG_MEDIA_PAUSE,         0 },
    { AWHK_REG_MEDIA_PLAY_PAUSE,    0 },
    { AWHK_REG_MEDIA_VOLUME_UP,     0 },
    { AWHK_REG_MEDIA_VOLUME_DOWN,   0 },

    { AWHK_REG_SESSION_SAVE_0,      MAKE_KEY_COMBO( MOD_CONTROL | MOD_ALT, VK_F1 ) },
    { AWHK_REG_SESSION_SAVE_1,      MAKE_KEY_COMBO( MOD_CONTROL | MOD_ALT, VK_F2 ) },
    { AWHK_REG_SESSION_SAVE_2,      MAKE_KEY_COMBO( MOD_CONTROL | MOD_ALT, VK_F3 ) },
    { AWHK_REG_SESSION_SAVE_3,      MAKE_KEY_COMBO( MOD_CONTROL | MOD_ALT, VK_F4 ) },
    { AWHK_REG_SESSION_SAVE_4,      MAKE_KEY_COMBO( MOD_CONTROL | MOD_ALT, VK_F5 ) },
    { AWHK_REG_SESSION_SAVE_5,      MAKE_KEY_COMBO( MOD_CONTROL | MOD_ALT, VK_F6 ) },
    { AWHK_REG_SESSION_SAVE_6,      MAKE_KEY_COMBO( MOD_CONTROL | MOD_ALT, VK_F7 ) },
    { AWHK_REG_SESSION_SAVE_7,      MAKE_KEY_COMBO( MOD_CONTROL | MOD_ALT, VK_F8 ) },
    { AWHK_REG_SESSION_SAVE_8,      MAKE_KEY_COMBO( MOD_CONTROL | MOD_ALT, VK_F9 ) },
    { AWHK_REG_SESSION_SAVE_9,      MAKE_KEY_COMBO( MOD_CONTROL | MOD_ALT, VK_F10 ) },

    { AWHK_REG_SESSION_LOAD_0,      MAKE_KEY_COMBO( MOD_CONTROL | MOD_ALT, VK_F1 ) },
    { AWHK_REG_SESSION_LOAD_1,      MAKE_KEY_COMBO( MOD_CONTROL | MOD_ALT, VK_F2 ) },
    { AWHK_REG_SESSION_LOAD_2,      MAKE_KEY_COMBO( MOD_CONTROL | MOD_ALT, VK_F3 ) },
    { AWHK_REG_SESSION_LOAD_3,      MAKE_KEY_COMBO( MOD_CONTROL | MOD_ALT, VK_F4 ) },
    { AWHK_REG_SESSION_LOAD_4,      MAKE_KEY_COMBO( MOD_CONTROL | MOD_ALT, VK_F5 ) },
    { AWHK_REG_SESSION_LOAD_5,      MAKE_KEY_COMBO( MOD_CONTROL | MOD_ALT, VK_F6 ) },
    { AWHK_REG_SESSION_LOAD_6,      MAKE_KEY_COMBO( MOD_CONTROL | MOD_ALT, VK_F7 ) },
    { AWHK_REG_SESSION_LOAD_7,      MAKE_KEY_COMBO( MOD_CONTROL | MOD_ALT, VK_F8 ) },
    { AWHK_REG_SESSION_LOAD_8,      MAKE_KEY_COMBO( MOD_CONTROL | MOD_ALT, VK_F9 ) },
    { AWHK_REG_SESSION_LOAD_9,      MAKE_KEY_COMBO( MOD_CONTROL | MOD_ALT, VK_F10 ) },
};

BOOL GetRegistryDefaultValue( LPCWSTR strKey, DWORD* pOut )
{
    for ( const DEFAULT_KEY_VALUE* key = c_DefaultValues;
          *key->Name;
          key++ )
    {
        if ( wcscmp( key->Name, strKey ) == 0 )
        {
            *pOut = key->Value;
            return TRUE;
        }
    }

    return FALSE;
}

BOOL StoreRegistryDword( LPCWSTR strName, DWORD dwValue )
{
	HKEY hKey;
	if ( ::RegCreateKey(
		HKEY_CURRENT_USER,
		AWHK_REG_KEY,
		&hKey ) != ERROR_SUCCESS )
	{
		return FALSE;
	}

	DWORD dwValueSize = sizeof( DWORD );
	LONG ret = ::RegSetValueEx(
		hKey,
		strName,
		0,
		REG_DWORD,
		(BYTE*) &dwValue, 
		dwValueSize );
	
	::RegCloseKey( hKey );

	return ret == ERROR_SUCCESS;
}

BOOL StoreRegistryQword( LPCWSTR strName, LONGLONG qwValue )
{
	HKEY hKey;
	if ( ::RegCreateKey(
		HKEY_CURRENT_USER,
		AWHK_REG_KEY,
		&hKey ) != ERROR_SUCCESS )
	{
		return FALSE;
	}

	DWORD dwValueSize = sizeof( LONGLONG );
	LONG ret = ::RegSetValueEx(
		hKey,
		strName,
		0,
		REG_QWORD,
		(BYTE*) &qwValue, 
		dwValueSize );
	
	::RegCloseKey( hKey );

	return ret == ERROR_SUCCESS;
}

BOOL StoreRegistryKeyCombo( LPCWSTR strComboName, DWORD dwTrigger, DWORD dwModifiers )
{
    DWORD dwPacked = MAKE_KEY_COMBO( dwModifiers, dwTrigger );

    return StoreRegistryDword( strComboName, dwPacked );
}

BOOL LoadRegistryDword( LPCWSTR strName, DWORD* pOut )
{
	DWORD dwValue = 0;
	DWORD dwValueSize = sizeof( DWORD );
	if ( ::RegGetValue( 
		HKEY_CURRENT_USER,
		AWHK_REG_KEY,
		strName,
		RRF_RT_DWORD,
		nullptr,
		&dwValue, 
		&dwValueSize ) == ERROR_SUCCESS )
	{
		*pOut = (DWORD) dwValue;
		return TRUE;
	}

    return GetRegistryDefaultValue( strName, pOut );
}

BOOL LoadRegistryQword( LPCWSTR strName, LONGLONG* pOut )
{
	LONGLONG qwValue = 0;
	DWORD dwValueSize = sizeof( LONGLONG );
	if ( ::RegGetValue( 
		HKEY_CURRENT_USER,
		AWHK_REG_KEY,
		strName,
		RRF_RT_QWORD,
		nullptr,
		&qwValue, 
		&dwValueSize ) == ERROR_SUCCESS )
	{
		*pOut = (LONGLONG) qwValue;
		return TRUE;
	}

    DWORD dwDefault = 0;
    if ( GetRegistryDefaultValue( strName, &dwDefault ) )
    {
        *pOut = dwDefault;
        return TRUE;
    }

	return FALSE;
}

BOOL LoadRegistryBool( LPCWSTR strName, BOOL* pOut )
{
	DWORD dwValue = 0;
	if ( !LoadRegistryDword( 
		strName,
		&dwValue ) )
	{
		return FALSE;
	}

	if ( dwValue != 0 && dwValue != 1 )
    {
		return FALSE;
    }

	*pOut = dwValue == 1;
	return TRUE;
}

BOOL LoadRegistryVKey( LPCWSTR strName, DWORD* pOut )
{
	DWORD dwValue = 0;
	if ( !LoadRegistryDword( 
		strName,
		&dwValue ) )
	{
		return FALSE;
	}

	// Sanity check the key
	if ( dwValue > 0xFF )
		return FALSE;

	*pOut = dwValue;
	
	return TRUE;
}

BOOL LoadRegistryKeyMod( LPCWSTR strName, DWORD* pOut )
{
	DWORD dwValue = 0;
	if ( !LoadRegistryDword( 
		strName,
		&dwValue ) )
	{
		return FALSE;
	}

	if ( dwValue == 0 )
	{
		*pOut = 0;
		return TRUE;
	}

	// Sanity check the key
	//if ( dwValue != MOD_SHIFT &&
	//	 dwValue != MOD_CONTROL &&
	//	 dwValue != MOD_ALT )
	//	return FALSE;
	DWORD dwAllKeys = MOD_ALT | MOD_CONTROL | MOD_SHIFT | MOD_WIN;
	if ( dwValue > dwAllKeys )
		return FALSE;

	*pOut = dwValue;

	return TRUE;
}

BOOL LoadRegistryKeyCombo( LPCWSTR strComboName, DWORD* pdwTrigger, DWORD* pdwModifiers )
{
    DWORD dwPacked = 0;
    if ( !LoadRegistryDword( strComboName, &dwPacked ) )
        return FALSE;

    // If it's zero it hasn't been set yet
    if ( dwPacked == 0 )
        return FALSE;

    DWORD dwTrigger = GET_TRIGGER_KEY( dwPacked );
    DWORD dwModifiers = GET_MODIFIER_KEYS( dwPacked );

    // Sanity check
    if ( dwTrigger > 0xFF )
        return FALSE;
	DWORD dwAllKeys = MOD_ALT | MOD_CONTROL | MOD_SHIFT | MOD_WIN;
	if ( dwModifiers > dwAllKeys )
		return FALSE;

    *pdwTrigger = dwTrigger;
    *pdwModifiers = dwModifiers;
    return TRUE;
}

