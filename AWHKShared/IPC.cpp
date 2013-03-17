#include "stdafx.h"
#include "IPC.h"

#define AWHK_IPC_SEM	L"AWHK_IPC_SEM"
#define AWHK_IPC_FILE	L"AWHK_IPC_MMF"

struct AWHK_IPC_DATA
{
	DWORD		cbSize;
	DWORD		dwMessage;
};

BOOL IPCExists()
{
	HANDLE hSyncSem = ::OpenSemaphore(
		SYNCHRONIZE | SEMAPHORE_MODIFY_STATE,	
		FALSE,
		AWHK_IPC_SEM );
	if ( hSyncSem )
	{
		::CloseHandle( hSyncSem );
		return TRUE;
	}

	HANDLE hFileMapping = ::OpenFileMapping(
		FILE_MAP_WRITE | FILE_MAP_READ,
		FALSE,
		AWHK_IPC_FILE );
	if ( hFileMapping )
	{
		::CloseHandle( hFileMapping );
		return TRUE;
	}

	return FALSE;
}

BOOL OpenIPC( IPC* ipc )
{
	::ZeroMemory( ipc, sizeof( IPC ) );

	ipc->hSyncSem = ::OpenSemaphore(
		SYNCHRONIZE | SEMAPHORE_MODIFY_STATE,	
		FALSE,
		AWHK_IPC_SEM );
	if ( !ipc->hSyncSem )
		return FALSE;
	
	// Just return it if it was already opened
	ipc->hFileMapping = ::OpenFileMapping(
		FILE_MAP_WRITE | FILE_MAP_READ,
		FALSE,
		AWHK_IPC_FILE );
	if ( !ipc->hFileMapping )
	{
		CloseIPC( ipc );
		return FALSE;
	}

	return TRUE;
}

BOOL CreateIPC( IPC* ipc )
{
	SECURITY_ATTRIBUTES sa;
	sa.bInheritHandle = FALSE;
	sa.lpSecurityDescriptor = nullptr;
	sa.nLength = sizeof( sa );

	::ZeroMemory( ipc, sizeof( IPC ) );

	ipc->hSyncSem = ::CreateSemaphore(
		&sa,
		0,
		MAXINT32,
		AWHK_IPC_SEM );
	if ( !ipc->hSyncSem )
		return FALSE;

	ipc->hFileMapping = ::CreateFileMapping(
		(HANDLE) -1,
		nullptr,
		PAGE_READWRITE,
		0,
		sizeof( AWHK_IPC_DATA ),
		AWHK_IPC_FILE );
	if ( !ipc->hFileMapping )
	{
		CloseIPC( ipc );
		return FALSE;
	}

	return TRUE;
}

void CloseIPC( IPC* ipc )
{
	if ( ipc->hSyncSem ) 
		::CloseHandle( ipc->hSyncSem );
	if ( ipc->hFileMapping )
		::CloseHandle( ipc->hFileMapping );
	::ZeroMemory( ipc, sizeof( IPC ) );
}

BOOL IsValidIPC( const IPC* ipc )
{
	return ipc->hFileMapping && ipc->hSyncSem;
}

BOOL WriteMessageIPC( IPC* ipc, AWHK_IPC_MSG msg )
{
	AWHK_IPC_DATA data;
	DWORD numBytes = 0;

	data.cbSize = sizeof( data );
	data.dwMessage = (DWORD) msg;

	if ( !::WriteFile(
		ipc->hFileMapping,
		&data,
		sizeof( data ),
		&numBytes,
		nullptr ) )
	{
		return FALSE;
	}

	::ReleaseSemaphore(
		ipc->hSyncSem,
		1,
		nullptr );

	return TRUE;
}

BOOL ReadMessageIPC( IPC* ipc, AWHK_IPC_MSG* msg )
{
	AWHK_IPC_DATA data;
	DWORD bytesRead = 0;

	// Block until there's data
	::WaitForSingleObject(
		ipc->hSyncSem,
		INFINITE );

	if ( !::ReadFile(
		ipc->hFileMapping,
		&data,
		sizeof( data ),
		&bytesRead,
		nullptr ) )
	{
		return FALSE;
	}

	if ( data.cbSize < sizeof( data ) ||
		 bytesRead < sizeof( data ) ||
		 data.dwMessage >= _MSG_MAX )
	{
		return FALSE;
	}

	*msg = (AWHK_IPC_MSG) data.dwMessage;

	return TRUE;
}

