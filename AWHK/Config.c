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

#include "stdafx.h"
#include "Config.h"

#define JOIN(x, y) x##y

AWHK_KEY_COMBO NullKeyCombo()
{
	AWHK_KEY_COMBO k;
	k.dwBits = 0;
	return k;
}

AWHK_CURSOR_KEYS CreateCursorKeys(DWORD left, DWORD right, DWORD up, DWORD down)
{
	AWHK_CURSOR_KEYS k;
	k.LeftKey = left;
	k.RightKey = right;
	k.UpKey = up;
	k.DownKey = down;
	return k;
}

LPCWSTR ModifierToString(DWORD mod)
{
	switch (mod)
	{
	case MOD_SHIFT:
		return L"SHIFT";
	case MOD_CONTROL:
		return L"CONTROL";
	case MOD_ALT:
		return L"ALT";
	default:
		assert(0 && "Modifier needs to be only one of SHIFT, ALT or CONTROL");
		return NULL;
	}
}

LPCWSTR KeyToString(DWORD key)
{
	static TCHAR single[2] = { 0, 0 };
#define KEYCODE(name, vk)	case vk: return JOIN(L, #name);
	switch (key)
	{
	case 0:
		return NULL;
	default:
		if (isprint((int)key))
		{
			single[0] = (TCHAR)toupper(key);
			return single;
		}
		else
		{
			return NULL;
		}
#	include "KeyCodes.inl"
	}
#undef KEYCODE
}

void AppendModifierToString(DWORD mod, PWSTR buf, size_t bufSize, LPCWSTR delim)
{
	if (*buf)
		wcscat_s(buf, bufSize, delim);
	wcscat_s(buf, bufSize, ModifierToString(mod));
}

void AppendModifiersToString(DWORD mods, PWSTR buf, size_t bufSize, LPCWSTR delim)
{
	if (mods & MOD_SHIFT)
		AppendModifierToString(MOD_SHIFT, buf, bufSize, delim);
	if (mods & MOD_CONTROL)
		AppendModifierToString(MOD_CONTROL, buf, bufSize, delim);
	if (mods & MOD_ALT)
		AppendModifierToString(MOD_ALT, buf, bufSize, delim);
}

void AppendKeyToString(DWORD key, PWSTR buf, size_t bufSize, LPCWSTR delim)
{
	if (*buf)
		wcscat_s(buf, bufSize, delim);
	LPCWSTR name = KeyToString(key);
	if (name)
	{
		wcscat_s(buf, bufSize, KeyToString(key));
	}
	else
	{
		TCHAR tmp[64];
		swprintf_s(tmp, _countof(tmp), L"%x", key);
		wcscat_s(buf, bufSize, tmp);
	}
}

void InitConfiguration(AWHK_APP_CONFIG* cfg)
{
	ZeroMemory(cfg, sizeof(*cfg));

#define CONFIG_VALUE(name, type, value)	cfg-> name = value;
#	include "ConfigDefaults.inl"
#undef CONFIG_VALUE
}

BOOL LoadConfiguration(LPCWSTR pConfigFile, AWHK_APP_CONFIG* cfg)
{
	UNUSED(pConfigFile);
	UNUSED(cfg);

	// todo

	return FALSE;
}

void WriteConfig_BOOL(FILE* file, PTSTR pScratch, size_t scratchSize, LPCTSTR pKey, const BOOL* value)
{
	UNUSED(pScratch);
	UNUSED(scratchSize);
	fwprintf_s(file, L"%s=%s\n", pKey, *value ? L"true" : L"false");
}

void WriteConfig_DWORD(FILE* file, PTSTR pScratch, size_t scratchSize, LPCTSTR pKey, const DWORD* value)
{
	UNUSED(pScratch);
	UNUSED(scratchSize);
	fwprintf_s(file, L"%s=%u\n", pKey, *value);
}

void WriteConfig_MODKEY(FILE* file, PTSTR pScratch, size_t scratchSize, LPCTSTR pKey, const DWORD* value)
{
	UNUSED(pScratch);
	UNUSED(scratchSize);
	fwprintf_s(file, L"%s=%s\n", pKey, ModifierToString(*value));
}

void WriteConfig_AWHK_KEY_COMBO(FILE* file, PTSTR pScratch, size_t scratchSize, LPCTSTR pKey, const AWHK_KEY_COMBO* value)
{
	// Special case if no key is set
	if (value->dwBits == 0)
	{
		fwprintf_s(file, L"%s=\n", pKey);
	}
	else
	{
		pScratch[0] = 0;
		AppendModifiersToString(value->Modifiers, pScratch, scratchSize, L"+");
		AppendKeyToString(value->Trigger, pScratch, scratchSize, L"+");
		fwprintf_s(file, L"%s=%s\n", pKey, pScratch);
	}
}

void WriteConfig_AWHK_CURSOR_KEYS(FILE* file, PTSTR pScratch, size_t scratchSize, LPCTSTR pKey, const AWHK_CURSOR_KEYS* value)
{
	pScratch[0] = 0;
	AppendKeyToString(value->LeftKey, pScratch, scratchSize, L",");
	AppendKeyToString(value->RightKey, pScratch, scratchSize, L",");
	AppendKeyToString(value->UpKey, pScratch, scratchSize, L",");
	AppendKeyToString(value->DownKey, pScratch, scratchSize, L",");
	fwprintf_s(file, L"%s=%s\n", pKey, pScratch);
}

BOOL SaveConfiguration(LPCWSTR pConfigFile, const AWHK_APP_CONFIG* pCfg)
{
	if (!pConfigFile || !*pConfigFile)
		return FALSE;

	FILE* f = NULL;
	if (_wfopen_s(&f, pConfigFile, L"w") != 0)
	{
		return FALSE;
	}

	TCHAR scratchBuffer[256] = { 0 };

#define CONFIG_VALUE(name, type, val) JOIN(WriteConfig_, type)(f, scratchBuffer, _countof(scratchBuffer), JOIN(L, #name), &pCfg-> name);
#	include "ConfigDefaults.inl"
#undef CONFIG_VALUE

	fclose(f);
	return TRUE;
}
