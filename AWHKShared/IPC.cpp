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

#define AWHK_IPC_SEM		(L"AWHK_IPC_SEM_" AWHK_VERSION_STRING) 
#define AWHK_IPC_MAILSLOT	(L"\\\\.\\mailslot\\AWHK_IPC_" AWHK_VERSION_STRING) 

struct AWHK_IPC_DATA
{
	DWORD		cbSize;
	DWORD		dwMessage;
};

BOOL OpenIPCOrDie(AWHK_IPC* ipc);

BOOL IPCExists()
{
	AWHK_IPC tmp{};
	if (OpenIPCOrDie(&tmp))
	{
		CloseIPC(&tmp);
		return TRUE;
	}

	return FALSE;
}

BOOL OpenIPCOrDie(AWHK_IPC* ipc)
{
	::ZeroMemory( ipc, sizeof(AWHK_IPC) );
		
	// Just return it if it was already opened
	ipc->hMailSlot = ::CreateFile(
		AWHK_IPC_MAILSLOT,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		(LPSECURITY_ATTRIBUTES) nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);
	if (ipc->hMailSlot == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL OpenIPC(AWHK_IPC* ipc)
{
	if ( !OpenIPCOrDie( ipc ) )
	{
		CloseIPC( ipc );
		return FALSE;
	}

	return TRUE;
}

BOOL CreateIPCOrDie(AWHK_IPC* ipc)
{
	::ZeroMemory( ipc, sizeof(AWHK_IPC) );

	ipc->hMailSlot = ::CreateMailslot(
		AWHK_IPC_MAILSLOT,
		sizeof(AWHK_IPC_MSG),
		MAILSLOT_WAIT_FOREVER,
		(LPSECURITY_ATTRIBUTES) nullptr);
	if (ipc->hMailSlot == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CreateIPC(AWHK_IPC* ipc)
{
	if ( !CreateIPCOrDie( ipc ) )
	{
		CloseIPC( ipc );
		return FALSE;
	}
	return TRUE;
}

void CloseIPC(AWHK_IPC* ipc)
{
	if ( ipc->hMailSlot )
		::CloseHandle( ipc->hMailSlot);
	::ZeroMemory( ipc, sizeof(AWHK_IPC) );
}

BOOL IsValidIPC(const AWHK_IPC* ipc)
{
	return ipc->hMailSlot &&
		ipc->hMailSlot != INVALID_HANDLE_VALUE;
}

BOOL WriteMessageIPC(AWHK_IPC* ipc, AWHK_IPC_MSG msg)
{
	DWORD written = 0;
	BOOL ok = ::WriteFile(
		ipc->hMailSlot,
		&msg,
		sizeof(msg),
		&written,
		nullptr);

	return ok;
}

BOOL ReadMessageIPC(AWHK_IPC* ipc, AWHK_IPC_MSG* msg)
{
	DWORD read = 0;
	BOOL ok = ::ReadFile(
		ipc->hMailSlot,
		msg,
		sizeof(AWHK_IPC_MSG),
		&read,
		nullptr);

	return ok;
}

