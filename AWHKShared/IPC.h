#pragma once

struct IPC
{
	HANDLE hFileMapping;
	HANDLE hSyncSem;

	volatile 
	struct AWHK_IPC_DATA* pMemory;
};

BOOL IPCExists();

BOOL CreateIPC( IPC* ipc );
BOOL OpenIPC( IPC* ipc );
void CloseIPC( IPC* ipc );

BOOL IsValidIPC( const IPC* ipc );

enum AWHK_IPC_MSG
{
	IPC_MSG_RELOAD_CONFIG,
	IPC_MSG_QUIT,

	_MSG_MAX
};

BOOL WriteMessageIPC( IPC* pIPC, AWHK_IPC_MSG msg );
BOOL ReadMessageIPC( IPC* pIPC, AWHK_IPC_MSG* msg );