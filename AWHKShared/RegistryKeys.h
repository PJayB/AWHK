#pragma once

#define AWHK_REG_KEY			L"Software\\AWHK"

#define AWHK_REG_ALLOW_SNAP		L"AllowSnapToOthers"
#define AWHK_REG_ADJACENCY		L"ModifyAdjacent"

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
#define AWHK_REG_SOLO_KEY_MOD	L"SoloKeyMod"
#define AWHK_REG_FINE_KEY_MOD	L"FineKeyMod"


BOOL StoreRegistryDword( LPCWSTR strName, DWORD dwValue );

BOOL LoadRegistryDword( LPCWSTR strName, DWORD* pOut );
BOOL LoadRegistryBool( LPCWSTR strName, BOOL* pOut );
BOOL LoadRegistryVKey( LPCWSTR strName, DWORD* pOut );
BOOL LoadRegistryKeyMod( LPCWSTR strName, DWORD* pOut );
