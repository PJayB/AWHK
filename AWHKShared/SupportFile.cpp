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

