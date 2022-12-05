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
#include <stdint.h>

#define JOIN(x, y) x##y

void StreamConfiguration(FILE* f, const AWHK_APP_CONFIG* pCfg);

/*
* 
* DEFAULT CONFIGURATION
* 
*/

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

void InitConfiguration(AWHK_APP_CONFIG* cfg)
{
	ZeroMemory(cfg, sizeof(*cfg));

#define CONFIG_VALUE(name, type, value)	cfg-> name = value;
#	include "ConfigDefaults.inl"
#undef CONFIG_VALUE
}

/*
* 
* LOADING CONFIGURATION
* 
*/

USHORT StringToModifier(LPCWSTR mod)
{
	if (_wcsicmp(L"SHIFT", mod) == 0)
		return MOD_SHIFT;
	else if (_wcsicmp(L"CONTROL", mod) == 0)
		return MOD_CONTROL;
	else if (_wcsicmp(L"ALT", mod) == 0)
		return MOD_ALT;
	else
		return 0;
}

USHORT StringToKey(LPCWSTR key)
{
	if (FALSE) {}
#define KEYCODE(name, vk) \
		else if (_wcsicmp(key, JOIN(L, #name)) == 0) return vk;
#	include "KeyCodes.inl"
#undef KEYCODE
	else
	{
		// Check for printable keys
		if (wcslen(key) == 1 && *key < 0xFF)
		{
			return *key;
		}

		// Unknown key
		return 0;
	}
}

BOOL TrimStringSegment(LPWSTR* pStart, LPWSTR* pEnd)
{
	// Find start
	while ((**pStart) && iswspace(**pStart))
		(*pStart)++;

	// Find the end
	while ((*pEnd > *pStart) && iswspace(*((*pEnd)-1)))
		(*pEnd)--;

	return *pEnd > *pStart;
}

BOOL SplitKeyValuePair(LPWSTR pLine, LPCWSTR* ppKey, LPCWSTR* ppValue)
{
	// Find start of the line
	while (*pLine && iswspace(*pLine))
		pLine++;

	// Find the end of the line
	LPWSTR pEnd = pLine;
	while (*pEnd && *pEnd != '\n' && *pEnd != '#')
		pEnd++;

	// Walk back (trim right)
	while (pEnd > pLine && iswspace(*(pEnd-1)))
		pEnd--;

	// Empty line
	if (pEnd == pLine)
		return FALSE;

	// Find the delimiter
	LPWSTR pDelim = pLine;
	while (*pDelim && pDelim != pEnd && *pDelim != '=')
		pDelim++;

	// Extract the key, if there is one
	LPWSTR pKeyStart = pLine;
	LPWSTR pKeyEnd = pDelim;
	if (!TrimStringSegment(&pKeyStart, &pKeyEnd))
		return FALSE;

	// Extract the value, if there is one
	LPWSTR pValueStart = pDelim+1;
	LPWSTR pValueEnd = pEnd;
	TrimStringSegment(&pValueStart, &pValueEnd);

	// Delimit the strings and return
	*pKeyEnd = 0;
	*pValueEnd = 0;
	*ppKey = pKeyStart;
	*ppValue = pValueStart;

	return TRUE;
}

size_t SplitString(LPWSTR pStr, WCHAR delim, LPCWSTR* pTokens, size_t maxTokens)
{
	size_t count = 0;
	LPWSTR pEnd = pStr + wcslen(pStr);
	while (pStr < pEnd)
	{
		// Find the next delimiter
		LPWSTR pDelim = pStr;
		while (pDelim < pEnd && *pDelim != delim)
			pDelim++;

		// Extract the token
		LPWSTR pTokStart = pStr;
		LPWSTR pTokEnd = pDelim;
		if (TrimStringSegment(&pTokStart, &pTokEnd))
		{
			*pTokEnd = 0;
			if (count < maxTokens)
			{
				pTokens[count] = pTokStart;
			}
			count++;
		}

		// Move on to the next string
		pStr = pDelim+1;
	}

	return count;
}

PARSING_ERROR* ParsingError(PARSING_ERROR* pPrev, size_t lineNum, const wchar_t* pError, ...)
{
	PARSING_ERROR* pNew = (PARSING_ERROR*)malloc(sizeof(PARSING_ERROR));
	ZeroMemory(pNew, sizeof(*pNew));

	pNew->LineNumber = lineNum;
	pNew->pNext = pPrev;

	va_list args;
	va_start(args, pError);
	vswprintf_s(pNew->ErrorText, _countof(pNew->ErrorText), pError, args);
	va_end(args);

	return pNew;
}

PARSING_ERROR* ReverseParsingError(PARSING_ERROR* pNode, PARSING_ERROR** ppHead)
{
	if (pNode->pNext == NULL)
	{
		// I am the tail
		assert(*ppHead == NULL);
		*ppHead = pNode;
		return pNode;
	}

	PARSING_ERROR* pTail = ReverseParsingError(pNode->pNext, ppHead);

	assert(pTail->pNext == NULL);
	pTail->pNext = pNode;
	pNode->pNext = NULL;
	return pNode;
}

PARSING_ERROR* ReverseParsingErrors(PARSING_ERROR* pNode)
{
	if (pNode == NULL || pNode->pNext == NULL)
		return pNode;

	PARSING_ERROR* pNewHead = NULL;
	ReverseParsingError(pNode, &pNewHead);

	return pNewHead;
}

void FreeParsingErrors(PARSING_ERROR* pTail)
{
	if (pTail)
	{
		FreeParsingErrors(pTail->pNext);
		free(pTail);
	}
}

BOOL StringToBool(LPCWSTR pValue, BOOL* pResult)
{
	if (_wcsicmp(pValue, L"true") == 0 || _wcsicmp(pValue, L"1") == 0||
		_wcsicmp(pValue, L"yes") == 0 || _wcsicmp(pValue, L"y") == 0)
	{
		*pResult = TRUE;
		return TRUE;
	}
	else if (_wcsicmp(pValue, L"false") == 0 || _wcsicmp(pValue, L"0") == 0||
		_wcsicmp(pValue, L"no") == 0 || _wcsicmp(pValue, L"n") == 0)
	{
		*pResult = FALSE;
		return TRUE;
	}
	return FALSE;
}

BOOL StringToDword(LPCWSTR pValue, DWORD* pResult)
{
	int64_t val = 0;
	int r = swscanf_s(pValue, L"%lld", &val);
	if (r == 1 && val >= 0 && val <= 0xFFFFFFFFull)
	{
		*pResult = (DWORD)val;
		return TRUE;
	}
	return FALSE;
}

PARSING_ERROR* ReadConfig_BOOL(PARSING_ERROR* pLastError, size_t lineNum, LPCWSTR pValue, BOOL* pResult)
{
	if (!StringToBool(pValue, pResult))
	{
		return ParsingError(pLastError, lineNum,
			L"Expected bool 'true' or 'false' but got '%s'",
			pValue);
	}
	return pLastError;
}

PARSING_ERROR* ReadConfig_DWORD(PARSING_ERROR* pLastError, size_t lineNum, LPCWSTR pValue, DWORD* pResult)
{
	if (!StringToDword(pValue, pResult))
	{
		return ParsingError(pLastError, lineNum,
			L"Invalid integer, or integer out of range: '%s'",
			pValue);
	}
	return pLastError;
}

PARSING_ERROR* ReadConfig_MODKEY(PARSING_ERROR* pLastError, size_t lineNum, LPCWSTR pValue, MODKEY* pResult)
{
	USHORT mod = StringToModifier(pValue);
	if (mod == 0)
	{
		return ParsingError(pLastError, lineNum,
			L"Unknown modifier: %s",
			pValue);
	}

	*pResult = mod;

	return pLastError;
}

PARSING_ERROR* ReadConfig_AWHK_KEY_COMBO(PARSING_ERROR* pLastError, size_t lineNum, LPWSTR pValue, AWHK_KEY_COMBO* pResult)
{
	LPCWSTR tokens[4];
	size_t numTokens = SplitString(pValue, L'+', tokens, _countof(tokens));
	if (numTokens > 4)
	{
		return ParsingError(pLastError, lineNum,
			L"Can't have more than 3 modifiers for a key combo");
	}

	USHORT modifiers = 0;
	USHORT key = 0;

	for (size_t i = 0; i < numTokens; ++i)
	{
		// Is it a modifier?
		USHORT k = StringToModifier(tokens[i]);
		if (k == 0)
		{
			// Is it a key?
			k = StringToKey(tokens[i]);
			if (k == 0)
			{
				return ParsingError(pLastError, lineNum,
					L"Unknown modifier or key name: '%s'",
					tokens[i]);
			}
			else if (key)
			{
				return ParsingError(pLastError, lineNum,
					L"Can't assign more than a single key: '%s'",
					tokens[i]);
			}
			else
			{
				key = k;
			}
		}
		else
		{
			modifiers |= k;
		}
	}

	pResult->Modifiers = modifiers;
	pResult->Trigger = key;

	return pLastError;
}

PARSING_ERROR* ReadConfig_AWHK_CURSOR_KEYS(PARSING_ERROR* pLastError, size_t lineNum, LPWSTR pValue, AWHK_CURSOR_KEYS* pResult)
{
	LPCWSTR tokens[4];
	size_t numTokens = SplitString(pValue, L',', tokens, _countof(tokens));
	if (numTokens != _countof(tokens))
	{
		return ParsingError(pLastError, lineNum,
			L"Expected 4 comma-separated keys: left, right, up, down");
	}

	USHORT keys[4];

	// Convert them to keys
	for (size_t i = 0; i < numTokens; ++i)
	{
		// Is it a key?
		keys[i] = StringToKey(tokens[i]);
		if (keys[i] == 0)
		{
			return ParsingError(pLastError, lineNum,
				L"Unknown key name: '%s'",
				tokens[i]);
		}
	}

	// Check for duplicates
	for (size_t i = 0; i < _countof(keys) - 1; ++i)
	{
		for (size_t j = i+1; j < _countof(keys); ++j)
		{
			if (keys[i] == keys[j])
			{
				return ParsingError(pLastError, lineNum,
					L"Duplicate key found: %s",
					tokens[i]);
			}
		}
	}

	pResult->LeftKey = keys[0];
	pResult->RightKey = keys[1];
	pResult->UpKey = keys[2];
	pResult->DownKey = keys[3];

	return pLastError;
}

BOOL LoadConfiguration(LPCWSTR pConfigFile, AWHK_APP_CONFIG* pCfg, PARSING_ERROR** ppErrors)
{
	if (!pConfigFile || !*pConfigFile)
		return FALSE;

	FILE* f = NULL;
	if (_wfopen_s(&f, pConfigFile, L"r") != 0 || !f)
	{
		return FALSE;
	}

	// Read file line-by-line
	BOOL success = TRUE;
	WCHAR line[1024] = { 0 };
	PARSING_ERROR* pErrorTail = NULL;
	for (size_t lineNum = 1; fgetws(line, _countof(line), f); ++lineNum)
	{
		LPWSTR key = NULL, value = NULL;
		if (!SplitKeyValuePair(line, &key, &value))
		{
			continue;
		}

		// Find and set the key
		if (FALSE) {}
#define CONFIG_VALUE(name, type, val) \
		else if (_wcsicmp(key, JOIN(L, #name)) == 0) \
			pErrorTail = JOIN(ReadConfig_, type)( \
				pErrorTail, lineNum, \
				value, &pCfg-> name);
#	include "ConfigDefaults.inl"
#undef CONFIG_VALUE
		else
		{
			pErrorTail = ParsingError(pErrorTail, lineNum,
				L"Unknown key: '%s'",
				key);
		}
	}

	// If we aborted for some reason, set the error bit
	if (ferror(f))
	{
		pErrorTail = ParsingError(pErrorTail, 0, L"Unexpected end of file");
	}

	// Reverse the error linked list (if there is one)
	if (pErrorTail)
	{
		*ppErrors = ReverseParsingErrors(pErrorTail);
		success = FALSE;
	}

#ifdef _DEBUG
	// Dump out to stderr so we can verify the results
	StreamConfiguration(stderr, pCfg);
#endif

	fclose(f);
	return success;
}

/*
* 
* SAVING CONFIGURATION
* 
*/

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
		swprintf_s(tmp, _countof(tmp), L"%u", key);
		wcscat_s(buf, bufSize, tmp);
	}
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

void StreamConfiguration(FILE* f, const AWHK_APP_CONFIG* pCfg)
{
	TCHAR scratchBuffer[256] = { 0 };

#define CONFIG_VALUE(name, type, val) JOIN(WriteConfig_, type)(f, scratchBuffer, _countof(scratchBuffer), JOIN(L, #name), &pCfg-> name);
#	include "ConfigDefaults.inl"
#undef CONFIG_VALUE
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

	StreamConfiguration(f, pCfg);

	fclose(f);
	return TRUE;
}
