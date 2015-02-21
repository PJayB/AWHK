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

#include "Version.h"

#define AWHK_REG_KEY			        (L"Software\\AWHK\\" AWHK_VERSION_STRING)

#define AWHK_REG_ALLOW_SNAP		        L"AllowSnapToOthers"
#define AWHK_REG_EDGE_SEARCH	        L"MaxEdgeSearchSize"

#define AWHK_REG_GRID_X			        L"GridX"
#define AWHK_REG_GRID_Y			        L"GridY"
#define AWHK_REG_FINE_X			        L"FineX"
#define AWHK_REG_FINE_Y			        L"FineY"

#define AWKH_REG_HELP_COMBO		        L"HelpKey"
#define AWKH_REG_CFG_COMBO  	        L"ConfigKey"

#define AWHK_REG_RESIZE_LEFT		    L"ResizeLeft"
#define AWHK_REG_RESIZE_RIGHT		    L"ResizeRight"
#define AWHK_REG_RESIZE_UP			    L"ResizeUp"
#define AWHK_REG_RESIZE_DOWN		    L"ResizeDown"

#define AWHK_REG_MOVE_LEFT		        L"MoveLeft"
#define AWHK_REG_MOVE_RIGHT	            L"MoveRight"
#define AWHK_REG_MOVE_UP		        L"MoveUp"
#define AWHK_REG_MOVE_DOWN		        L"MoveDown"

#define AWHK_REG_MOVE_KEY_MOD           L"MoveMod"
#define AWHK_REG_NEXT_KEY_MOD           L"NextMod" 
#define AWHK_REG_FINE_KEY_MOD           L"FineMod" 

#define AWHK_REG_MEDIA_PREV             L"MediaPrev"
#define AWHK_REG_MEDIA_NEXT             L"MediaNext"
#define AWHK_REG_MEDIA_STOP             L"MediaStop"
#define AWHK_REG_MEDIA_PLAY_PAUSE       L"MediaPlayPause"
#define AWHK_REG_MEDIA_VOLUME_UP        L"MediaVolumeUp"
#define AWHK_REG_MEDIA_VOLUME_DOWN      L"MediaVolumeDown"
#define AWHK_REG_MEDIA_MUTE             L"MediaMute"

DWORD GetRegistryDefaultValue( LPCWSTR strKey );

BOOL StoreRegistryDword( LPCWSTR strName, DWORD dwValue );
BOOL StoreRegistryQword( LPCWSTR strName, LONGLONG qwValue );
BOOL StoreRegistryKeyCombo( LPCWSTR strComboName, USHORT usTrigger, USHORT usModifiers );

BOOL DefaultRegistryDword( LPCWSTR strName, DWORD* pOut );
BOOL DefaultRegistryQword( LPCWSTR strName, LONGLONG* pOut );
BOOL DefaultRegistryBool( LPCWSTR strName, BOOL* pOut );
BOOL DefaultRegistryVKey( LPCWSTR strName, DWORD* pOut );
BOOL DefaultRegistryKeyMod( LPCWSTR strName, DWORD* pOut );
BOOL DefaultRegistryKeyCombo( LPCWSTR strComboName, USHORT* pusTrigger, USHORT* pusModifiers );

BOOL LoadRegistryDword( LPCWSTR strName, DWORD* pOut );
BOOL LoadRegistryQword( LPCWSTR strName, LONGLONG* pOut );
BOOL LoadRegistryBool( LPCWSTR strName, BOOL* pOut );
BOOL LoadRegistryVKey( LPCWSTR strName, DWORD* pOut );
BOOL LoadRegistryKeyMod( LPCWSTR strName, DWORD* pOut );
BOOL LoadRegistryKeyCombo( LPCWSTR strComboName, USHORT* pusTrigger, USHORT* pusModifiers );
