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
#include "IPC.h"
#include "Version.h"

#include <strsafe.h>

struct AWHK_IPC_RING
{
    volatile UINT RingBufferSize;
    volatile UINT WriteOffset;
    volatile UINT ReadOffset;
    volatile BYTE pBuffer[1];
};

struct AWHK_IPC
{
    LPCWSTR NotifySemaphoreName;
    LPCWSTR MappedFileName;
    LPCWSTR WriteLockName;
    AWHK_IPC_RING* pRing;
    BYTE*   pMessageRing;
    HANDLE  hWriteLock;
    HANDLE  hNotifySemaphore;
    HANDLE  hBufferLock;
    HANDLE  hMappedFile;
    UINT    uMappedFileSize;
    BOOL    bIsServer;
};

LPCWSTR JoinString(
    LPCWSTR szPrefix,
    LPCWSTR szSuffix )
{
    LPCWSTR versionStr = AWHK_VERSION_STRING;

    int aLen = wcslen( szPrefix );
    int bLen = wcslen( szSuffix );
    int cLen = wcslen( versionStr );
    int totalLen = aLen + bLen + cLen + 1;

    WCHAR* newStr = new WCHAR[totalLen];
    if ( newStr == nullptr )
        return nullptr;
    
    wcscpy_s( newStr, totalLen, szPrefix );
    wcscat_s( newStr, totalLen, szSuffix );
    wcscat_s( newStr, totalLen, versionStr );

    return newStr;
}

HRESULT CreateInterprocessStream(
    LPCWSTR szName,
    UINT uRingBufferSize,
    AWHK_IPC** ppIPC )
{
    if ( ppIPC == nullptr ) 
        return E_INVALIDARG;
    if ( uRingBufferSize <= sizeof(AWHK_IPC_RING) )
        return E_INVALIDARG;
    if ( szName == nullptr || *szName == 0 )
        return E_INVALIDARG;

    AWHK_IPC* pIPC = new AWHK_IPC;
    ZeroMemory( pIPC, sizeof(pIPC) );

    pIPC->WriteLockName = JoinString( szName, L"_writelock" );
    pIPC->NotifySemaphoreName = JoinString( szName, L"_notify" );
    pIPC->MappedFileName = JoinString( szName, L"_mmap" );

	SECURITY_ATTRIBUTES sa;
	sa.bInheritHandle = FALSE;
	sa.lpSecurityDescriptor = nullptr;
	sa.nLength = sizeof( sa );

	pIPC->hWriteLock = ::CreateMutex(
		&sa,
		TRUE,
    	pIPC->WriteLockName );
	if ( !pIPC->hWriteLock || 
         GetLastError() == ERROR_ALREADY_EXISTS || 
         GetLastError() == ERROR_ACCESS_DENIED )
    {
        CloseInterprocessStream( pIPC );
		return HRESULT_FROM_WIN32( GetLastError() );
    }

	pIPC->hNotifySemaphore = ::CreateSemaphore(
		&sa,
		0,
		MAXINT32,
		pIPC->NotifySemaphoreName );
	if ( !pIPC->hNotifySemaphore )
    {
        CloseInterprocessStream( pIPC );
		return HRESULT_FROM_WIN32( GetLastError() );
    }

    pIPC->hMappedFile = ::CreateFileMapping(
		(HANDLE) -1,
		nullptr,
		PAGE_READWRITE,
		0,
		uRingBufferSize,
		pIPC->MappedFileName );
	if ( !pIPC->hMappedFile )
	{
        CloseInterprocessStream( pIPC );
		return HRESULT_FROM_WIN32( GetLastError() );
	}

	pIPC->pRing = (AWHK_IPC_RING*) ::MapViewOfFile(
		pIPC->hMappedFile,
		FILE_MAP_WRITE | FILE_MAP_READ,
		0, 0,
		uRingBufferSize );
    if ( pIPC->pRing == nullptr )
    {
        CloseInterprocessStream( pIPC );
		return HRESULT_FROM_WIN32( GetLastError() );
	}

    ZeroMemory( pIPC->pRing, uRingBufferSize );
    pIPC->pRing->RingBufferSize = uRingBufferSize - sizeof(AWHK_IPC_RING);
    pIPC->uMappedFileSize = uRingBufferSize;
    pIPC->bIsServer = TRUE;

    *ppIPC = pIPC;
    return S_OK;
}

HRESULT OpenInterprocessStream(
    LPCWSTR szName,
    AWHK_IPC** ppIPC )
{
    if ( ppIPC == nullptr ) 
        return E_INVALIDARG;
    if ( szName == nullptr || *szName == 0 )
        return E_INVALIDARG;

    AWHK_IPC* pIPC = new AWHK_IPC;
    ZeroMemory( pIPC, sizeof(pIPC) );

    pIPC->WriteLockName = JoinString( szName, L"_writelock" );
    pIPC->NotifySemaphoreName = JoinString( szName, L"_notify" );
    pIPC->MappedFileName = JoinString( szName, L"_mmap" );

	pIPC->hWriteLock = ::OpenMutex(
		SYNCHRONIZE,
		FALSE,
    	pIPC->WriteLockName );
	if ( !pIPC->hWriteLock )
    {
        CloseInterprocessStream( pIPC );
		return HRESULT_FROM_WIN32( GetLastError() );
    }

	pIPC->hNotifySemaphore = ::OpenSemaphore(
		SYNCHRONIZE | SEMAPHORE_MODIFY_STATE,
		FALSE,
		pIPC->NotifySemaphoreName );
	if ( !pIPC->hNotifySemaphore )
    {
        CloseInterprocessStream( pIPC );
		return HRESULT_FROM_WIN32( GetLastError() );
    }

    pIPC->hMappedFile = ::OpenFileMapping(
		FILE_MAP_WRITE | FILE_MAP_READ,
		FALSE,
		pIPC->MappedFileName );
	if ( !pIPC->hMappedFile )
	{
        CloseInterprocessStream( pIPC );
		return HRESULT_FROM_WIN32( GetLastError() );
	}

    AWHK_IPC_RING* pTmpRing = (AWHK_IPC_RING*) ::MapViewOfFile(
		pIPC->hMappedFile,
		FILE_MAP_READ,
		0, 0,
		sizeof(AWHK_IPC_RING) );
    if ( pTmpRing == nullptr )
    {
        CloseInterprocessStream( pIPC );
		return HRESULT_FROM_WIN32( GetLastError() );
	}

    // Grab the size of the ringbuffer and remap
    UINT uRingBufferSize = pTmpRing->RingBufferSize + sizeof(AWHK_IPC_RING);
    UnmapViewOfFile( pTmpRing );

    pIPC->pRing = (AWHK_IPC_RING*) ::MapViewOfFile(
        pIPC->hMappedFile,
        FILE_MAP_WRITE | FILE_MAP_READ,
        0, 0, 
        uRingBufferSize );
    if ( pIPC->pRing )
    {
        CloseInterprocessStream( pIPC );
		return HRESULT_FROM_WIN32( GetLastError() );
    }

    pIPC->uMappedFileSize = uRingBufferSize;
    pIPC->bIsServer = FALSE;

    *ppIPC = pIPC;
    return S_OK;
}

HRESULT CloseInterprocessStream( AWHK_IPC* pIPC )
{
    if ( pIPC == nullptr )
        return E_INVALIDARG;

    if ( pIPC->bIsServer && 
         pIPC->hWriteLock && 
         pIPC->hNotifySemaphore &&
         pIPC->hMappedFile )
    {
        // Wait for clients to release their write lock on the ringbuffer
        WaitForSingleObject( pIPC->hWriteLock, INFINITE );

        // Clear the mapping
        ZeroMemory( pIPC->hMappedFile, pIPC->uMappedFileSize );
        ReleaseMutex( pIPC->hWriteLock );

        // Notify listeners there's data there
        ReleaseSemaphore( pIPC->hNotifySemaphore, 1, nullptr );
    }

    if ( pIPC->pRing )
        UnmapViewOfFile( pIPC->pRing );

    if ( pIPC->WriteLockName != nullptr )
        delete [] pIPC->WriteLockName;
    if ( pIPC->NotifySemaphoreName != nullptr )
        delete [] pIPC->NotifySemaphoreName;
    if ( pIPC->MappedFileName != nullptr )
        delete [] pIPC->MappedFileName;

    if ( pIPC->hBufferLock != nullptr )
        CloseHandle( pIPC->hBufferLock );
    if ( pIPC->hNotifySemaphore != nullptr )
        CloseHandle( pIPC->hNotifySemaphore );
    if ( pIPC->hWriteLock != nullptr )
        CloseHandle( pIPC->hWriteLock );
    if ( pIPC->hMappedFile != nullptr )
        CloseHandle( pIPC->hMappedFile );

    delete pIPC;
    return S_OK;
}

HRESULT WriteInterprocessStream(
    _In_ AWHK_IPC* pIPC,
    _In_reads_(dataSize) LPCVOID* pData,
    _In_ UINT dataSize )
{
    return E_NOTIMPL;
}

HRESULT ReadInterprocessStream(
    _In_ AWHK_IPC* pIPC,
    _Out_writes_(*pDataSize) LPVOID* pData,
    _Out_ UINT* pDataSize )
{
    return E_NOTIMPL;
}




















#define AWHK_IPC_SEM	(L"AWHK_IPC_SEM_" AWHK_VERSION_STRING) 
#define AWHK_IPC_FILE	(L"AWHK_IPC_MMF_" AWHK_VERSION_STRING) 

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

	DWORD dwAccess = FILE_MAP_WRITE | FILE_MAP_READ;
	
	// Just return it if it was already opened
	ipc->hFileMapping = ::OpenFileMapping(
		dwAccess,
		FALSE,
		AWHK_IPC_FILE );
	if ( !ipc->hFileMapping )
	{
		return FALSE;
	}

	if ( !MapIPC( ipc, dwAccess ) )
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

	if ( !MapIPC( ipc, FILE_MAP_ALL_ACCESS ) )
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

