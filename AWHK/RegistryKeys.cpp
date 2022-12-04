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

    { AWKH_REG_HELP_COMBO,          AWHK_MAKE_HOTKEY( MOD_ALT, VK_F1 ) },
    { AWKH_REG_CFG_COMBO,           AWHK_MAKE_HOTKEY( MOD_ALT, VK_F2 ) },

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

    { AWHK_REG_MEDIA_PREV,          AWHK_MAKE_HOTKEY( MOD_ALT | MOD_SHIFT, VK_F11 ) },
    { AWHK_REG_MEDIA_NEXT,          AWHK_MAKE_HOTKEY( MOD_ALT | MOD_SHIFT, VK_F12 ) },
	{ AWHK_REG_MEDIA_STOP,          0 },
    { AWHK_REG_MEDIA_PLAY_PAUSE,    AWHK_MAKE_HOTKEY( MOD_ALT, VK_F9 ) },
	{ AWHK_REG_MEDIA_MUTE,          AWHK_MAKE_HOTKEY( MOD_ALT, VK_F10 ) },
	{ AWHK_REG_MEDIA_VOLUME_DOWN,   AWHK_MAKE_HOTKEY( MOD_ALT, VK_F11 ) },
    { AWHK_REG_MEDIA_VOLUME_UP,     AWHK_MAKE_HOTKEY( MOD_ALT, VK_F12 ) },

    { NULL, 0 }
};

BOOL GetRegistryDefaultValue( LPCWSTR strKey, DWORD* pOut )
{
    for ( const DEFAULT_KEY_VALUE* key = c_DefaultValues;
          key->Name;
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

BOOL DefaultRegistryDword( LPCWSTR strName, DWORD* pOut )
{
    return GetRegistryDefaultValue( strName, pOut );
}

BOOL DefaultRegistryQword( LPCWSTR strName, LONGLONG* pOut )
{
    DWORD v = 0;
    if ( !GetRegistryDefaultValue( strName, &v ) )
        return FALSE;
    *pOut = v;
    return TRUE;
}

BOOL DefaultRegistryBool( LPCWSTR strName, BOOL* pOut )
{
    DWORD v = 0;
    if ( !GetRegistryDefaultValue( strName, &v ) )
        return FALSE;
    *pOut = (v != FALSE);
    return TRUE;
}

BOOL DefaultRegistryVKey( LPCWSTR strName, DWORD* pOut )
{
    return GetRegistryDefaultValue( strName, pOut );
}

BOOL DefaultRegistryKeyMod( LPCWSTR strName, DWORD* pOut )
{
    return GetRegistryDefaultValue( strName, pOut );
}

BOOL DefaultRegistryKeyCombo( LPCWSTR strComboName, USHORT* pusTrigger, USHORT* pusModifiers )
{
    DWORD dwPacked = 0;
    if ( !GetRegistryDefaultValue( strComboName, &dwPacked ) )
        return FALSE;

    *pusTrigger = AWHK_GET_TRIGGER_KEY( dwPacked );
    *pusModifiers = AWHK_GET_MODIFIER_KEYS( dwPacked );
    return TRUE;
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

BOOL LoadRegistryKeyCombo( LPCWSTR strComboName, USHORT* pusTrigger, USHORT* pusModifiers )
{
    DWORD dwPacked = 0;
    if ( !LoadRegistryDword( strComboName, &dwPacked ) )
        return FALSE;

    // If it's zero it hasn't been set yet
    if ( dwPacked == 0 )
        return TRUE;

    DWORD dwTrigger = AWHK_GET_TRIGGER_KEY( dwPacked );
    DWORD dwModifiers = AWHK_GET_MODIFIER_KEYS( dwPacked );

    // Sanity check
    if ( dwTrigger > 0xFF )
        return FALSE;
	DWORD dwAllKeys = MOD_ALT | MOD_CONTROL | MOD_SHIFT | MOD_WIN;
	if ( dwModifiers > dwAllKeys )
		return FALSE;

    *pusTrigger = (USHORT) dwTrigger;
    *pusModifiers = (USHORT) dwModifiers;
    return TRUE;
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

BOOL StoreRegistryKeyCombo( LPCWSTR strComboName, USHORT usTrigger, USHORT usModifiers )
{
    DWORD dwPacked = AWHK_MAKE_HOTKEY( usModifiers, usTrigger );

    return StoreRegistryDword( strComboName, dwPacked );
}