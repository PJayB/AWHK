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
