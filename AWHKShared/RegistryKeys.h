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

#define AWHK_REG_KEY			L"Software\\AWHK"

#define AWHK_REG_ALLOW_SNAP		L"AllowSnapToOthers"
#define AWHK_REG_EDGE_SEARCH	L"MaxEdgeSearchSize"

#define AWHK_REG_GRID_X			L"GridX"
#define AWHK_REG_GRID_Y			L"GridY"
#define AWHK_REG_FINE_X			L"FineX"
#define AWHK_REG_FINE_Y			L"FineY"

#define AWKH_REG_HELP_KEY		L"HelpKey"
#define AWKH_REG_HELP_KEY_MOD	L"HelpKeyMod"
#define AWKH_REG_CFG_KEY		L"ConfigKey"
#define AWKH_REG_CFG_KEY_MOD	L"ConfigKeyMod"

#define AWHK_REG_LEFT_KEY		L"LeftKey"
#define AWHK_REG_RIGHT_KEY		L"RightKey"
#define AWHK_REG_UP_KEY			L"UpKey"
#define AWHK_REG_DOWN_KEY		L"DownKey"

#define AWHK_REG_MOVE_KEY_MOD	L"MoveKeyMod"
#define AWHK_REG_NEXT_KEY_MOD	L"NextKeyMod"
#define AWHK_REG_FINE_KEY_MOD	L"FineKeyMod"


BOOL StoreRegistryDword( LPCWSTR strName, DWORD dwValue );

BOOL LoadRegistryDword( LPCWSTR strName, DWORD* pOut );
BOOL LoadRegistryBool( LPCWSTR strName, BOOL* pOut );
BOOL LoadRegistryVKey( LPCWSTR strName, DWORD* pOut );
BOOL LoadRegistryKeyMod( LPCWSTR strName, DWORD* pOut );
