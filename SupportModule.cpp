#include "stdafx.h"
#include "SupportModule.h"

typedef void (* SUPPORTLIB_SHOW_DIALOG_PROC)(
	ASYNC_FORM_CLOSED_PROC pfCallback,
	LPVOID pUserData );

BOOL GetSupportLibraryFilename( LPWSTR strModuleFile, SIZE_T nNumChars );


BOOL ShowSettingsDialogAsync(
		ASYNC_FORM_CLOSED_PROC pfCallback,
		LPVOID pUserData )
{
	TCHAR strLibName[MAX_PATH];
	if ( !GetSupportLibraryFilename( strLibName, MAX_PATH ) )
		return FALSE;


	// TODO

	return TRUE;
}




BOOL GetSupportLibraryFilename( LPWSTR strModuleFile, SIZE_T nNumChars )
{
	SIZE_T nModuleFileLen = ::GetModuleFileName( 
		nullptr, 
		strModuleFile, 
		nNumChars );
	if ( ::GetLastError() == ERROR_INSUFFICIENT_BUFFER )
		return FALSE;

	// Find the last \\ .
	LPWSTR strFilename;
	for ( strFilename = strModuleFile + nModuleFileLen; strFilename >= strModuleFile; --strFilename )
	{
		if ( *strFilename == '\\' ) 
		{
			strFilename++;
			break;
		}
	}

	SIZE_T nRemainingSize = nNumChars - ( strFilename - strModuleFile );

	// Concatenate our DLL name
	StringCbCopy( strFilename, nRemainingSize, L"AWHKConfig.exe" );

	return TRUE;
}

