#pragma once

#define AWHK_MAIN_EXE			L"AWHK.exe"
#define AWHK_CONFIG_PANEL_EXE	L"AWHKConfig.exe"

BOOL 
GetSupportFilePath( 
	_In_z_ LPCWSTR strSupportFile, 
	_Out_writes_(nNumChars) LPWSTR strModuleFile, 
	_In_ SIZE_T nNumChars );
