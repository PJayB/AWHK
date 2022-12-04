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
#include "SupportModule.h"
#include "SupportFile.h"

struct CONFIG_PROCESS_THREAD_DATA
{
	ASYNC_FORM_CLOSED_PROC pfCallback;
	LPVOID pUserData;
	PROCESS_INFORMATION ProcessInfo;
};

INT ConfigProcessWaitThread( CONFIG_PROCESS_THREAD_DATA* pShared )
{
	::WaitForSingleObject( pShared->ProcessInfo.hProcess, INFINITE );

	if ( pShared->pfCallback )
		pShared->pfCallback( pShared->pUserData );

	DWORD exitCode = 255;
	::GetExitCodeProcess( pShared->ProcessInfo.hProcess, &exitCode );

	::CloseHandle( pShared->ProcessInfo.hProcess );
	::CloseHandle( pShared->ProcessInfo.hThread );

	delete pShared;

	return exitCode;
}

BOOL ShowConfigEditorAsync(
		ASYNC_FORM_CLOSED_PROC pfCallback,
		LPVOID pUserData )
{
	// todo: look up main editor
	// todo: hook up to config path

	TCHAR strLibName[MAX_PATH] = _T("notepad.exe");

	CONFIG_PROCESS_THREAD_DATA* pShared = new CONFIG_PROCESS_THREAD_DATA;
	pShared->pfCallback = pfCallback;
	pShared->pUserData = pUserData;
	ZeroMemory( &pShared->ProcessInfo, sizeof( pShared->ProcessInfo ) );

	STARTUPINFO startInfo;
	ZeroMemory( &startInfo, sizeof( startInfo ) );
	startInfo.cb = sizeof( startInfo );

	// Start the process
	if ( !::CreateProcess(
		nullptr,
		strLibName,
		nullptr,
		nullptr,
		FALSE,
		0,
		nullptr,
		nullptr,
		&startInfo,
		&pShared->ProcessInfo ) )
	{
		return FALSE;
	}

	// Wait for it
	::CreateThread(
		nullptr,
		0,
		(LPTHREAD_START_ROUTINE) ConfigProcessWaitThread,
		pShared,
		0,
		nullptr );

	return TRUE;
}
