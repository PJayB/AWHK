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

#define AWHK_IPC_IO_GRANULARITY 256
#define AWHK_IPC_SPINLOCK_COUNT 10000

struct AWHK_IPC_RING
{
    volatile UINT64 WriteCursor;
    volatile UINT64 ReadCursor;
    volatile UINT   RingBufferSize;
    volatile UINT   Count;
};

struct AWHK_IPC
{
    LPCWSTR MappedFileName;
    LPCWSTR WriteLockName;
    LPCWSTR WriteEventName;
    LPCWSTR ReadLockName;
    LPCWSTR ReadEventName;
    AWHK_IPC_RING* pRing;
    BYTE*   pBuffer;
    BYTE*   pMessageRing;
    HANDLE  hWriteLock;
    HANDLE  hWriteEvent;
    HANDLE  hReadLock;
    HANDLE  hReadEvent;
    HANDLE  hMappedFile;
    UINT    MappedFileSize;
    UINT    RingBufferSize;
    UINT    IOGranularity;
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
    pIPC->WriteEventName = JoinString( szName, L"_write" );
    pIPC->ReadLockName = JoinString( szName, L"_readlock" );
    pIPC->ReadEventName = JoinString( szName, L"_read" );
    pIPC->MappedFileName = JoinString( szName, L"_mmap" );

	SECURITY_ATTRIBUTES sa;
	sa.bInheritHandle = FALSE;
	sa.lpSecurityDescriptor = nullptr;
	sa.nLength = sizeof( sa );

	pIPC->hWriteLock = ::CreateMutex(
		&sa,
		FALSE,
    	pIPC->WriteLockName );
	if ( !pIPC->hWriteLock || 
         GetLastError() == ERROR_ALREADY_EXISTS || 
         GetLastError() == ERROR_ACCESS_DENIED )
    {
        CloseInterprocessStream( pIPC );
		return HRESULT_FROM_WIN32( GetLastError() );
    }

	pIPC->hWriteEvent = ::CreateEvent(
		&sa,
		FALSE,
        FALSE,
    	pIPC->WriteEventName );
	if ( !pIPC->hWriteEvent )
    {
        CloseInterprocessStream( pIPC );
		return HRESULT_FROM_WIN32( GetLastError() );
    }

	pIPC->hReadLock = ::CreateMutex(
		&sa,
		FALSE,
    	pIPC->ReadLockName );
	if ( !pIPC->hReadLock || 
         GetLastError() == ERROR_ALREADY_EXISTS || 
         GetLastError() == ERROR_ACCESS_DENIED )
    {
        CloseInterprocessStream( pIPC );
		return HRESULT_FROM_WIN32( GetLastError() );
    }

	pIPC->hReadEvent = ::CreateEvent(
		&sa,
		FALSE,
        FALSE,
    	pIPC->ReadEventName );
	if ( !pIPC->hReadEvent )
    {
        CloseInterprocessStream( pIPC );
		return HRESULT_FROM_WIN32( GetLastError() );
    }

    UINT uTotalBufferSize = uRingBufferSize + sizeof(AWHK_IPC_RING);

    pIPC->hMappedFile = ::CreateFileMapping(
		(HANDLE) -1,
		nullptr,
		PAGE_READWRITE,
		0,
		uTotalBufferSize,
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
		uTotalBufferSize );
    if ( pIPC->pRing == nullptr )
    {
        CloseInterprocessStream( pIPC );
		return HRESULT_FROM_WIN32( GetLastError() );
	}

    ZeroMemory( pIPC->pRing, uTotalBufferSize );
    pIPC->pRing->RingBufferSize = uRingBufferSize;
    pIPC->pRing->Count = 0;
    pIPC->pBuffer = ( (BYTE*) pIPC->pRing ) + sizeof(AWHK_IPC_RING);
    pIPC->IOGranularity = AWHK_IPC_IO_GRANULARITY;
    pIPC->MappedFileSize = uTotalBufferSize;
    pIPC->RingBufferSize = uRingBufferSize;
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
    pIPC->WriteEventName = JoinString( szName, L"_write" );
    pIPC->ReadLockName = JoinString( szName, L"_readlock" );
    pIPC->ReadEventName = JoinString( szName, L"_read" );
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

	pIPC->hWriteEvent = ::OpenEvent(
		SYNCHRONIZE | EVENT_MODIFY_STATE,
		FALSE,
		pIPC->WriteEventName );
	if ( !pIPC->hWriteEvent )
    {
        CloseInterprocessStream( pIPC );
		return HRESULT_FROM_WIN32( GetLastError() );
    }

	pIPC->hReadLock = ::OpenMutex(
		SYNCHRONIZE,
		FALSE,
    	pIPC->ReadLockName );
	if ( !pIPC->hReadLock )
    {
        CloseInterprocessStream( pIPC );
		return HRESULT_FROM_WIN32( GetLastError() );
    }

	pIPC->hReadEvent = ::OpenEvent(
		SYNCHRONIZE | EVENT_MODIFY_STATE,
		FALSE,
		pIPC->ReadEventName );
	if ( !pIPC->hReadEvent )
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

    // Cache some of the ringbuffer properties
	__try
	{
        pIPC->RingBufferSize = pTmpRing->RingBufferSize;
	}
	__except(::GetExceptionCode()==EXCEPTION_IN_PAGE_ERROR ?
	EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
	{
        CloseInterprocessStream( pIPC );
		return E_FAIL;
	}

    UnmapViewOfFile( pTmpRing );

    pIPC->MappedFileSize = pIPC->RingBufferSize + sizeof(AWHK_IPC_RING);

    pIPC->pRing = (AWHK_IPC_RING*) ::MapViewOfFile(
        pIPC->hMappedFile,
        FILE_MAP_WRITE | FILE_MAP_READ,
        0, 0, 
        pIPC->MappedFileSize );
    if ( pIPC->pRing == nullptr )
    {
        CloseInterprocessStream( pIPC );
		return HRESULT_FROM_WIN32( GetLastError() );
    }

    pIPC->pBuffer = ( (BYTE*) pIPC->pRing ) + sizeof(AWHK_IPC_RING);
    pIPC->IOGranularity = AWHK_IPC_IO_GRANULARITY;
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
         pIPC->hWriteEvent &&
         pIPC->hMappedFile )
    {
        // Wait for clients to release their write lock on the ringbuffer
        WaitForSingleObject( pIPC->hWriteLock, INFINITE );

        // Clear the mapping
        ZeroMemory( pIPC->pRing, pIPC->MappedFileSize );
        ReleaseMutex( pIPC->hWriteLock );

        // Notify listeners there's data there
        SetEvent( pIPC->hWriteEvent );
    }

    if ( pIPC->pRing )
        UnmapViewOfFile( pIPC->pRing );

    if ( pIPC->WriteLockName != nullptr )
        delete [] pIPC->WriteLockName;
    if ( pIPC->WriteEventName != nullptr )
        delete [] pIPC->WriteEventName;
    if ( pIPC->ReadLockName != nullptr )
        delete [] pIPC->ReadLockName;
    if ( pIPC->ReadEventName != nullptr )
        delete [] pIPC->ReadEventName;
    if ( pIPC->MappedFileName != nullptr )
        delete [] pIPC->MappedFileName;

    if ( pIPC->hWriteEvent != nullptr )
        CloseHandle( pIPC->hWriteEvent );
    if ( pIPC->hReadEvent != nullptr )
        CloseHandle( pIPC->hReadEvent );
    if ( pIPC->hWriteLock != nullptr )
        CloseHandle( pIPC->hWriteLock );
    if ( pIPC->hReadLock != nullptr )
        CloseHandle( pIPC->hReadLock );
    if ( pIPC->hMappedFile != nullptr )
        CloseHandle( pIPC->hMappedFile );

    delete pIPC;
    return S_OK;
}

static inline void WriteSpinlock( 
    AWHK_IPC* pIPC,
    UINT64 writeCursor )
{
    UINT spin = AWHK_IPC_SPINLOCK_COUNT;
    UINT64 readCursor = pIPC->pRing->ReadCursor;
    UINT ringBufferSize = pIPC->RingBufferSize;

    // Spin while in case the data is going to come in very soon
    while ( writeCursor - readCursor > ringBufferSize && spin-- > 0 )
    {
        SwitchToThread(); // Give up our quantum
        readCursor = pIPC->pRing->ReadCursor;
    }

    // Switch to a very slow wait 
    if ( writeCursor - pIPC->pRing->ReadCursor > ringBufferSize )
    {
        WaitForSingleObject( pIPC->hReadEvent, INFINITE );
        assert( writeCursor - pIPC->pRing->ReadCursor <= ringBufferSize );
    }
}





HRESULT WriteInterprocessStream(
    _In_ AWHK_IPC* pIPC,
    _In_reads_(dataSize) LPCVOID pData,
    _In_ UINT dataSize )
{
    if ( dataSize == 0 )
    {
        // Just release the semaphore and quit
        ::SetEvent( pIPC->hWriteEvent );
        return S_OK;
    }

    UINT numPackets = ( dataSize + pIPC->IOGranularity - 1 ) / pIPC->IOGranularity;
    UINT ringBufferSize = pIPC->RingBufferSize;
    const BYTE* pSource = (const BYTE*) pData;
    BYTE* pRingEnd = pIPC->pBuffer + pIPC->RingBufferSize;
    
    // Lock the ring
    ::WaitForSingleObject( pIPC->hWriteLock, INFINITE );

    // Extract the current ring properties
	__try
	{
        UINT64 writeCursor = pIPC->pRing->WriteCursor;

        BYTE* pDest = pIPC->pBuffer + ( writeCursor % pIPC->RingBufferSize );

        while ( dataSize > 0 )
        {
            UINT packetSize = min( dataSize, pIPC->IOGranularity );

            // Wait until the memory becomes available
            WriteSpinlock( pIPC, writeCursor + packetSize );

            // Check for wrap: if we do, split the write
            if ( pDest + packetSize > pRingEnd )
            {
                UINT splitPoint = pRingEnd - pDest;
                memcpy( pDest, pSource, splitPoint );
                memcpy( pIPC->pBuffer, pSource + splitPoint, packetSize - splitPoint );
            }
            else
            {
                memcpy( pDest, pData, packetSize );
            }

            pSource += packetSize;
            writeCursor += packetSize;
            dataSize -= packetSize;

            // Update the write position so reads can consume the data
            pIPC->pRing->WriteCursor = writeCursor;
            ::SetEvent( pIPC->hWriteEvent );
        }
	}
	__except(::GetExceptionCode() == EXCEPTION_IN_PAGE_ERROR ?
	    EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
	{
        ReleaseMutex( pIPC->hWriteLock );
		return E_FAIL;
	}

    // Release the lock
    ReleaseMutex( pIPC->hWriteLock );
    return S_OK;
}






static inline UINT64 ReadSpinlock( 
    AWHK_IPC* pIPC,
    UINT64 readCursor )
{
    UINT spin = AWHK_IPC_SPINLOCK_COUNT;
    while ( readCursor >= pIPC->pRing->WriteCursor && spin-- > 0 )
    {
        SwitchToThread();
    }

    if ( readCursor >= pIPC->pRing->WriteCursor )
    {
        WaitForSingleObject( pIPC->hWriteEvent, INFINITE );
    }

    return pIPC->pRing->WriteCursor;
}

HRESULT ReadInterprocessStream(
    _In_ AWHK_IPC* pIPC,
    _Out_writes_(*pDataSize) LPVOID pData,
    _Out_ UINT dataSize )
{
    // Secure the read lock
    ::WaitForSingleObject( pIPC->hReadLock, INFINITE );

    UINT ioGranularity = pIPC->IOGranularity;
    UINT ringBufferSize = pIPC->RingBufferSize;
    BYTE* pBuffer = pIPC->pBuffer;
    BYTE* pBufferEnd = pBuffer + ringBufferSize;

	__try
	{
        UINT64 readCursor = pIPC->pRing->ReadCursor;

        const BYTE* pSrc = pBuffer + ( readCursor % ringBufferSize );
        BYTE* pDest = (BYTE*) pData;
        BYTE* pEnd = pDest + dataSize;

        while ( pDest < pEnd )
        {
            // Wait until the memory becomes available
            UINT64 writeCursor = ReadSpinlock( pIPC, readCursor );

            // How much memory is available?
            UINT available = min( dataSize, min( ioGranularity, (UINT) (writeCursor - readCursor) ) );

            // If we're about to overrun the buffer, split the read
            if ( pSrc + available > pBufferEnd )
            {
                UINT splitPoint = pBufferEnd - pSrc;
                memcpy( pDest, pSrc, splitPoint );
                memcpy( pDest + splitPoint, pBuffer, available - splitPoint );
            }
            else
            {
                memcpy( pDest, pSrc, available );
            }

            readCursor += available;
            dataSize -= available;
            pDest += available;

            // Free it up so writes can resume
            pIPC->pRing->ReadCursor = readCursor;
            ::SetEvent( pIPC->hReadEvent );
        }
	}
	__except(::GetExceptionCode() == EXCEPTION_IN_PAGE_ERROR ?
	    EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
	{
        ::ReleaseMutex( pIPC->hReadLock );
		return E_FAIL;
	}

    ::ReleaseMutex( pIPC->hReadLock );
    return S_OK;
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

