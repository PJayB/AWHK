#include "stdafx.h"
#include "SupportModule.h"

BOOL GetSupportLibraryFilename( LPWSTR strModuleFile, SIZE_T nNumChars );


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

	DWORD exitCode = -1;
	::GetExitCodeProcess( pShared->ProcessInfo.hProcess, &exitCode );

	::CloseHandle( pShared->ProcessInfo.hProcess );
	::CloseHandle( pShared->ProcessInfo.hThread );

	delete pShared;

	return exitCode;
}


BOOL ShowSettingsDialogAsync(
		ASYNC_FORM_CLOSED_PROC pfCallback,
		LPVOID pUserData )
{
	TCHAR strLibName[MAX_PATH];
	if ( !GetSupportLibraryFilename( strLibName, MAX_PATH ) )
		return FALSE;

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

