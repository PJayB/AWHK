#include "stdafx.h"
#include "Config.h"
#include "RegistryKeys.h"

AWHK_APP_CONFIG::AWHK_APP_CONFIG()
	: AllowSnapToOthers( FALSE ) // TODO
	, GridX( 8 )
	, GridY( 4 )
	, FineX( 32 )
	, FineY( 16 )
	, HelpKey( VK_F1 )
	, HelpKeyMod( MOD_ALT )
	, ConfigKey( VK_F2 )
	, ConfigKeyMod( MOD_ALT )
	, LeftKey( VK_LEFT )
	, RightKey( VK_RIGHT )
	, UpKey( VK_UP )
	, DownKey( VK_DOWN )
	, MoveKeyMod( MOD_ALT )
	, NextKeyMod( MOD_CONTROL )
	, FineKeyMod( MOD_SHIFT )
{
}

void LoadConfigGridValue( LPCWSTR strName, DWORD* value )
{
	DWORD gridVal = 0;
	if ( LoadRegistryDword( strName, &gridVal ) )
	{
		*value = max( gridVal, 2 );
		*value = min( *value, 0xFF );
	}
}

BOOL LoadConfiguration( AWHK_APP_CONFIG* cfg )
{
	LoadRegistryBool	( AWHK_REG_ALLOW_SNAP	, &cfg->AllowSnapToOthers );

	LoadConfigGridValue	( AWHK_REG_GRID_X		, &cfg->GridX );
	LoadConfigGridValue	( AWHK_REG_GRID_Y		, &cfg->GridY );
	LoadConfigGridValue	( AWHK_REG_FINE_X		, &cfg->FineX );
	LoadConfigGridValue	( AWHK_REG_FINE_Y		, &cfg->FineY );

	LoadRegistryVKey	( AWKH_REG_HELP_KEY		, &cfg->HelpKey );
	LoadRegistryKeyMod	( AWKH_REG_HELP_KEY_MOD	, &cfg->HelpKeyMod );
	LoadRegistryVKey	( AWKH_REG_CFG_KEY		, &cfg->ConfigKey );
	LoadRegistryKeyMod	( AWKH_REG_CFG_KEY_MOD	, &cfg->ConfigKeyMod );

	LoadRegistryVKey	( AWHK_REG_LEFT_KEY		, &cfg->LeftKey );
	LoadRegistryVKey	( AWHK_REG_RIGHT_KEY	, &cfg->RightKey );
	LoadRegistryVKey	( AWHK_REG_UP_KEY		, &cfg->UpKey );
	LoadRegistryVKey	( AWHK_REG_DOWN_KEY		, &cfg->DownKey );

	DWORD moveKeyMod = cfg->MoveKeyMod;
	DWORD fineKeyMod = cfg->FineKeyMod;
	DWORD nextKeyMod = cfg->NextKeyMod;
	LoadRegistryKeyMod	( AWHK_REG_MOVE_KEY_MOD	, &moveKeyMod );
	LoadRegistryKeyMod	( AWHK_REG_NEXT_KEY_MOD	, &nextKeyMod );
	LoadRegistryKeyMod	( AWHK_REG_FINE_KEY_MOD	, &fineKeyMod );

	// We MUST have a move key mod (otherwise what's the point?)
	if ( moveKeyMod )
		cfg->MoveKeyMod = moveKeyMod;

	// The two other modifiers must be different
	if ( fineKeyMod != moveKeyMod && fineKeyMod != nextKeyMod )
		cfg->FineKeyMod = fineKeyMod;
	if ( nextKeyMod != moveKeyMod && nextKeyMod != fineKeyMod )
		cfg->NextKeyMod = nextKeyMod;

	return TRUE;
}

BOOL SaveConfiguration( const AWHK_APP_CONFIG* cfg )
{
	StoreRegistryDword( AWHK_REG_ALLOW_SNAP		, cfg->AllowSnapToOthers );

	StoreRegistryDword( AWHK_REG_GRID_X			, cfg->GridX );
	StoreRegistryDword( AWHK_REG_GRID_Y			, cfg->GridY );
	StoreRegistryDword( AWHK_REG_FINE_X			, cfg->FineX );
	StoreRegistryDword( AWHK_REG_FINE_Y			, cfg->FineY );

	StoreRegistryDword( AWKH_REG_HELP_KEY		, cfg->HelpKey );
	StoreRegistryDword( AWKH_REG_HELP_KEY_MOD	, cfg->HelpKeyMod );
	StoreRegistryDword( AWKH_REG_CFG_KEY		, cfg->ConfigKey );
	StoreRegistryDword( AWKH_REG_CFG_KEY_MOD	, cfg->ConfigKeyMod );

	StoreRegistryDword( AWHK_REG_LEFT_KEY		, cfg->LeftKey );
	StoreRegistryDword( AWHK_REG_RIGHT_KEY		, cfg->RightKey );
	StoreRegistryDword( AWHK_REG_UP_KEY			, cfg->UpKey );
	StoreRegistryDword( AWHK_REG_DOWN_KEY		, cfg->DownKey );

	StoreRegistryDword( AWHK_REG_MOVE_KEY_MOD	, cfg->MoveKeyMod );
	StoreRegistryDword( AWHK_REG_NEXT_KEY_MOD	, cfg->NextKeyMod );
	StoreRegistryDword( AWHK_REG_FINE_KEY_MOD	, cfg->FineKeyMod );

	return TRUE;
}
