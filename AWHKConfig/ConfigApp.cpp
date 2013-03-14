// This is the main DLL file.

#include "stdafx.h"

#include "ConfigApp.h"
#include "SettingsForm.h"

#include <Windows.h>

using namespace System;

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow )
{
	Windows::Forms::Form^ frm = gcnew AWHKConfig::SettingsForm();

	frm->ShowDialog();
	
	return 0;
}
