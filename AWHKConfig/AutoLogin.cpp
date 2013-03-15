#include "stdafx.h"
#include "AutoLogin.h"
#include "..\AWHKShared\SupportFile.h"

#include <strsafe.h>

#define AWHK_RUN_KEY			L"Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#define AWHK_RUN_ENTRY_NAME		L"AdvancedWindowingHotKeys"

BOOL IsAutoLoginEnabled()
{
	DWORD dwValueSize = 0;
	return ::RegGetValue( 
		HKEY_CURRENT_USER,
		AWHK_RUN_KEY,
		AWHK_RUN_ENTRY_NAME,
		RRF_RT_REG_SZ,
		nullptr,
		nullptr, 
		&dwValueSize ) == ERROR_SUCCESS;
}

BOOL EnableAutoLogin()
{
	TCHAR strModuleName[MAX_PATH];
	if ( !GetSupportFilePath( AWHK_MAIN_EXE, strModuleName, MAX_PATH ) )
		return FALSE;

	HKEY hKey;
	if ( ::RegOpenKeyEx(
		HKEY_CURRENT_USER,
		AWHK_RUN_KEY,
		0,
		KEY_SET_VALUE,
		&hKey ) != ERROR_SUCCESS )
	{
		return FALSE;
	}

	size_t moduleNameLen = 0;
	StringCbLength(
		strModuleName,
		MAX_PATH,
		&moduleNameLen );

	LONG ret = ::RegSetValueEx(
		hKey,
		AWHK_RUN_ENTRY_NAME,
		0,
		REG_SZ,
		(BYTE*) strModuleName, 
		(DWORD) moduleNameLen );
	
	::RegCloseKey( hKey );

	return ret == ERROR_SUCCESS;
}

BOOL DisableAutoLogin()
{
	HKEY hKey;
	if ( ::RegOpenKeyEx(
		HKEY_CURRENT_USER,
		AWHK_RUN_KEY,
		0,
		KEY_SET_VALUE,
		&hKey ) != ERROR_SUCCESS )
	{
		return FALSE;
	}

	LONG ret = ::RegDeleteValue(
		hKey,
		AWHK_RUN_ENTRY_NAME );
	
	::RegCloseKey( hKey );

	return ret == ERROR_SUCCESS;
}

BOOL SetAutoLoginEnabled( BOOL bEnabled )
{
	if ( bEnabled )
		return EnableAutoLogin();
	else
		return DisableAutoLogin();
}


