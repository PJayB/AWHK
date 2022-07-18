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

struct AWHK_IPC
{
    HANDLE hMailSlot;
};

BOOL IPCExists();

BOOL CreateIPC(AWHK_IPC* ipc);
BOOL OpenIPC(AWHK_IPC* ipc);
void CloseIPC(AWHK_IPC* ipc);

BOOL IsValidIPC(const AWHK_IPC* ipc);

enum AWHK_IPC_MSG_CODE
{
    IPC_MSG_RELOAD_CONFIG,
    IPC_MSG_QUIT,
    IPC_MSG_SUSPEND,
    IPC_MSG_RESUME,

    IPC_MSG_CLIENT_NEW,
    IPC_MSG_CLIENT_UPDATE,
    IPC_MSG_CLIENT_GONE,

    _IPC_MSG_MAX
};

struct AWHK_IPC_MSG
{
    AWHK_IPC_MSG_CODE Code;
    DWORD Data;
    DWORD lParam;
    DWORD wParam;
};

BOOL WriteMessageIPC(const AWHK_IPC* pIPC, const AWHK_IPC_MSG* msg);
BOOL ReadMessageIPC(const AWHK_IPC* pIPC, AWHK_IPC_MSG* msg);
