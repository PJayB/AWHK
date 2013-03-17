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

BOOL MapIPC( IPC* ipc, DWORD dwAccess )
{
	return ( ipc->pMemory = (AWHK_IPC_DATA*) ::MapViewOfFile(
		ipc->hFileMapping,
		dwAccess,
		0, 0,
		sizeof( AWHK_IPC_DATA ) ) ) != NULL;
}

BOOL OpenIPCOrDie( IPC* ipc )
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
		return FALSE;
	}

	if ( !MapIPC( ipc, FILE_MAP_READ ) )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL OpenIPC( IPC* ipc )
{
	if ( !OpenIPCOrDie( ipc ) )
	{
		CloseIPC( ipc );
		return FALSE;
	}

	return TRUE;
}

BOOL CreateIPCOrDie( IPC* ipc )
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
		return FALSE;
	}

	if ( !MapIPC( ipc, FILE_MAP_READ ) )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CreateIPC( IPC* ipc )
{
	if ( !CreateIPCOrDie( ipc ) )
	{
		CloseIPC( ipc );
		return FALSE;
	}
	return TRUE;
}

void CloseIPC( IPC* ipc )
{
	if ( ipc->pMemory )
		::UnmapViewOfFile( (LPVOID) ipc->pMemory );
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
	__try
	{
		ipc->pMemory->cbSize = sizeof( AWHK_IPC_DATA );
		ipc->pMemory->dwMessage = msg;
	}
	__except (::GetExceptionCode() == EXCEPTION_IN_PAGE_ERROR ? 
	EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
	{
		return FALSE;
	}

	if ( !::FlushViewOfFile( (LPCVOID) ipc->pMemory, sizeof( AWHK_IPC_DATA ) ) ) 
	{
		// TODO: reconsider this?
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
	// Block until there's data
	::WaitForSingleObject(
		ipc->hSyncSem,
		INFINITE );

	__try
	{
		DWORD dwSize = ipc->pMemory->cbSize;
		if ( dwSize != sizeof( AWHK_IPC_DATA ) )
			return FALSE;

		*msg = (AWHK_IPC_MSG) ipc->pMemory->dwMessage;
	}
	__except(::GetExceptionCode()==EXCEPTION_IN_PAGE_ERROR ?
	EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
	{
		// Failed to read from the view.
		return FALSE;
	}

	return TRUE;
}

