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

#pragma once

enum AWHK_CLIENT_MESSAGE_ID
{
    AWHK_CLIENT_CONNECT,            // The client is active and requests updates from the server
    AWHK_CLIENT_DISCONNECT,         // The client is no longer active; unsubscribe from updates
    AWHK_CLIENT_RELOAD_CONFIG,      // Ask the server to reload the configuration from the registry
    AWHK_CLIENT_VALIDATE_CONFIG,    // Ask the server to validate an example configuration
    AWHK_CLIENT_QUIT_SERVER,        // Instruct the server to unload
    AWHK_CLIENT_UNBIND_ACTION,      // Unbind a specific action
    AWHK_CLIENT_BIND_ACTION,        // Rebind a specific action
};

struct AWHK_CLIENT_MESSAGE
{
    HINSTANCE hInstance;
    UINT uMessage;
};

enum AWHK_SERVER_CLIENT_MESSAGE_ID
{
    AWHK_SERVER_QUIT,               // The server is shutting down
    AWHK_SERVER_NEW_CONFIG,         // The server just successfully loaded a new config
    AWHK_SERVER_CONFIG_ERROR,       // The server just failed to load a config
    AWHK_SERVER_KEY_ERROR,          // The server failed to bind a specific key
    AWHK_SERVER_KEY_BOUND,          // The server bound a key
    AWHK_SERVER_KEY_UNBOUND,        // The server unbound a key
};

struct AWHK_SERVER_CONFIG_MESSAGE
{
    HRESULT hErrorCode;
};

struct AWHK_SERVER_KEY_MESSAGE
{
    DWORD   dwKeyCombo;
    HRESULT hErrorCode;
    UINT    uActionLength;
    WCHAR   szAction[1];
};

struct AWHK_SERVER_MESSAGE
{
    UINT uMessage;
    UINT uMessageSize;
    union
    {
        AWHK_SERVER_CONFIG_MESSAGE Config;
        AWHK_SERVER_KEY_MESSAGE Key;
    };
};





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
    IPC_MSG_SUSPEND,
    IPC_MSG_RESUME,

	_MSG_MAX
};

BOOL WriteMessageIPC( IPC* pIPC, AWHK_IPC_MSG msg );
BOOL ReadMessageIPC( IPC* pIPC, AWHK_IPC_MSG* msg );