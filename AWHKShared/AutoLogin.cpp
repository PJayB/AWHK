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


