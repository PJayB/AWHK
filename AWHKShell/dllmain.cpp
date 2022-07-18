// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "..\AWHKShared\IPC.h"

AWHK_IPC g_IPC{};

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    AWHK_IPC_MSG msg{};
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        OpenIPC(&g_IPC);
        msg.Code = IPC_MSG_CLIENT_NEW;
        WriteMessageIPC(&g_IPC, &msg);
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        msg.Code = IPC_MSG_CLIENT_GONE;
        WriteMessageIPC(&g_IPC, &msg);
        CloseIPC(&g_IPC);
        break;
    }
    return TRUE;
}

