// AWHKShell.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "AWHKShell.h"
#include "..\AWHKShared\IPC.h"

extern AWHK_IPC g_IPC;

AWHKSHELL_API LRESULT CALLBACK ShellProc(
	_In_ int    nCode,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
)
{
	AWHK_IPC_MSG msg{};
	msg.Code = IPC_MSG_CLIENT_UPDATE;
	msg.wParam = wParam;
	msg.lParam = lParam;
	msg.Data = nCode;
	WriteMessageIPC(&g_IPC, &msg);
	return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

AWHKSHELL_API void CALLBACK WinEventProc(
	HWINEVENTHOOK hWinEventHook,
	DWORD event,
	HWND hwnd,
	LONG idObject,
	LONG idChild,
	DWORD idEventThread,
	DWORD dwmsEventTime
)
{
	AWHK_IPC_MSG msg{};
	msg.Code = IPC_MSG_CLIENT_UPDATE;
	msg.wParam = HIWORD(hwnd);
	msg.lParam = LOWORD(hwnd);
	msg.Data = event;
	WriteMessageIPC(&g_IPC, &msg);
}
