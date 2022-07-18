// This is the main DLL file.

#include "stdafx.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "../AWHKShared/IPC.h"

#include "ServiceController.h"

BOOL WriteMessageIPC(AWHK_IPC* ipc, AWHK_IPC_MSG_CODE code)
{
    AWHK_IPC_MSG msg{};
    msg.Code = code;
    return WriteMessageIPC(ipc, &msg);
}

namespace AWHKConfigShared {

    void ServiceController::ReloadConfiguration()
    {
        AWHK_IPC ipc;
        if ( !OpenIPC( &ipc ) )
        {
            throw gcnew ServiceNotRunningException();
        }

        WriteMessageIPC( &ipc, IPC_MSG_RELOAD_CONFIG );
        CloseIPC( &ipc );
    }

    void ServiceController::Unload()
    {
        AWHK_IPC ipc;
        if ( !OpenIPC( &ipc ) )
        {
            throw gcnew ServiceNotRunningException();
        }

        WriteMessageIPC( &ipc, IPC_MSG_QUIT );
        CloseIPC( &ipc );
    }

    void ServiceController::Suspend()
    {
        AWHK_IPC ipc;
        if ( !OpenIPC( &ipc ) )
        {
            throw gcnew ServiceNotRunningException();
        }

        WriteMessageIPC( &ipc, IPC_MSG_SUSPEND );
        CloseIPC( &ipc );
    }

    void ServiceController::Resume()
    {
        AWHK_IPC ipc;
        if ( !OpenIPC( &ipc ) )
        {
            throw gcnew ServiceNotRunningException();
        }

        WriteMessageIPC( &ipc, IPC_MSG_RESUME );
        CloseIPC( &ipc );
    }

    bool ServiceController::internalIsLoaded()
    {
        AWHK_IPC ipc;
        if ( OpenIPC( &ipc ) )
        {
            CloseIPC( &ipc );
            return true;
        }
        return false;
    }
}
