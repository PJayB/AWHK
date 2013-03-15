#include "stdafx.h"
#include "SupportFile.h"

#include <strsafe.h>

BOOL GetSupportFilePath( LPCWSTR strSupportFile, LPWSTR strModuleFile, SIZE_T nNumChars )
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
	StringCbCopy( strFilename, nRemainingSize, strSupportFile );

	return TRUE;
}

