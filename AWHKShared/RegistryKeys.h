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

#define AWHK_REG_SESSION_SAVE_0         L"SessionSave0"
#define AWHK_REG_SESSION_SAVE_1         L"SessionSave1"
#define AWHK_REG_SESSION_SAVE_2         L"SessionSave2"
#define AWHK_REG_SESSION_SAVE_3         L"SessionSave3"
#define AWHK_REG_SESSION_SAVE_4         L"SessionSave4"
#define AWHK_REG_SESSION_SAVE_5         L"SessionSave5"
#define AWHK_REG_SESSION_SAVE_6         L"SessionSave6"
#define AWHK_REG_SESSION_SAVE_7         L"SessionSave7"
#define AWHK_REG_SESSION_SAVE_8         L"SessionSave8"
#define AWHK_REG_SESSION_SAVE_9         L"SessionSave9"
 
#define AWHK_REG_SESSION_LOAD_0         L"SessionLoad0"
#define AWHK_REG_SESSION_LOAD_1         L"SessionLoad1"
#define AWHK_REG_SESSION_LOAD_2         L"SessionLoad2"
#define AWHK_REG_SESSION_LOAD_3         L"SessionLoad3"
#define AWHK_REG_SESSION_LOAD_4         L"SessionLoad4"
#define AWHK_REG_SESSION_LOAD_5         L"SessionLoad5"
#define AWHK_REG_SESSION_LOAD_6         L"SessionLoad6"
#define AWHK_REG_SESSION_LOAD_7         L"SessionLoad7"
#define AWHK_REG_SESSION_LOAD_8         L"SessionLoad8"
#define AWHK_REG_SESSION_LOAD_9         L"SessionLoad9"

#define AWHK_REG_MEDIA_PREV             L"MediaPrev"
#define AWHK_REG_MEDIA_NEXT             L"MediaNext"
#define AWHK_REG_MEDIA_PLAY             L"MediaPlay"
#define AWHK_REG_MEDIA_PAUSE            L"MediaPause"
#define AWHK_REG_MEDIA_PLAY_PAUSE       L"MediaPlayPause"
#define AWHK_REG_MEDIA_VOLUME_UP        L"MediaVolumeUp"
#define AWHK_REG_MEDIA_VOLUME_DOWN      L"MediaVolumeDown"
#define AWHK_REG_MEDIA_MUTE             L"MediaMute"

#define MAKE_KEY_COMBO( mods, trigger ) MAKELONG( mods, trigger )
#define GET_TRIGGER_KEY( packed ) HIWORD( packed )
#define GET_MODIFIER_KEYS( packed) LOWORD( packed )

DWORD GetRegistryDefaultValue( LPCWSTR strKey );

BOOL StoreRegistryDword( LPCWSTR strName, DWORD dwValue );
BOOL StoreRegistryQword( LPCWSTR strName, LONGLONG qwValue );
BOOL StoreRegistryKeyCombo( LPCWSTR strComboName, DWORD dwTrigger, DWORD dwModifiers );

BOOL DefaultRegistryDword( LPCWSTR strName, DWORD* pOut );
BOOL DefaultRegistryQword( LPCWSTR strName, LONGLONG* pOut );
BOOL DefaultRegistryBool( LPCWSTR strName, BOOL* pOut );
BOOL DefaultRegistryVKey( LPCWSTR strName, DWORD* pOut );
BOOL DefaultRegistryKeyMod( LPCWSTR strName, DWORD* pOut );
BOOL DefaultRegistryKeyCombo( LPCWSTR strComboName, DWORD* pdwTrigger, DWORD* pdwModifiers );

BOOL LoadRegistryDword( LPCWSTR strName, DWORD* pOut );
BOOL LoadRegistryQword( LPCWSTR strName, LONGLONG* pOut );
BOOL LoadRegistryBool( LPCWSTR strName, BOOL* pOut );
BOOL LoadRegistryVKey( LPCWSTR strName, DWORD* pOut );
BOOL LoadRegistryKeyMod( LPCWSTR strName, DWORD* pOut );
BOOL LoadRegistryKeyCombo( LPCWSTR strComboName, DWORD* pdwTrigger, DWORD* pdwModifiers );
