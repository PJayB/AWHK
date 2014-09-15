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
    DWORD dwPacked = MAKEWORD( dwModifiers, dwTrigger );

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
	return FALSE;
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
		return FALSE;

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

    DWORD dwTrigger = HIWORD( dwPacked );
    DWORD dwModifiers = LOWORD( dwPacked );

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

