/*
Copyright (c) 2022 Pete Lewis

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "KeyCombo.h"

typedef struct
{
	DWORD				LeftKey;
	DWORD				RightKey;
	DWORD				UpKey;
	DWORD				DownKey;
} AWHK_CURSOR_KEYS;

typedef struct PARSING_ERROR_
{
	WCHAR					ErrorText[1020];
	DWORD					LineNumber;
	struct PARSING_ERROR_*	pNext;
} PARSING_ERROR;

typedef DWORD MODKEYS;

typedef struct
{
#define CONFIG_VALUE(name, type, value)			type name;
#	include "ConfigDefaults.inl"
#undef CONFIG_VALUE
} AWHK_APP_CONFIG;

BOOL CursorKeysAreValid(const AWHK_CURSOR_KEYS* pKeys);

LPCWSTR ModifierToString(DWORD mod);
LPCWSTR KeyToString(DWORD key);

void InitConfiguration(AWHK_APP_CONFIG* cfg);
BOOL LoadConfiguration(LPCWSTR pPath, AWHK_APP_CONFIG* cfg, PARSING_ERROR** ppErrors);
BOOL SaveConfiguration(LPCWSTR pPath, const AWHK_APP_CONFIG* cfg);

void FreeParsingErrors(PARSING_ERROR* pError);
