// This is the main DLL file.

#include "stdafx.h"

#include "SupportLibrary.h"
#include "AboutForm.h"
#include "SettingsForm.h"

using namespace System;

#define EXPORT extern "C" __declspec(dllexport)


EXPORT void ShowHelpDialog()
{
	( gcnew SupportLibrary::AboutForm() )->Show();
}

EXPORT void ShowSettingsDialog()
{
	( gcnew SupportLibrary::SettingsForm() )->Show();
}

