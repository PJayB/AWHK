#include "stdafx.h"
#include "Config.h"
#include "RegistryKeys.h"

AWHK_APP_CONFIG::AWHK_APP_CONFIG()
	: AllowSnapToOthers( TRUE )
	, AllowModifyAdjacent( TRUE )
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
	, SoloKeyMod( MOD_SHIFT )
	, FineKeyMod( MOD_CONTROL )
{
}

void LoadConfigGridValue( LPCWSTR strName, DWORD* value )
{
	DWORD gridVal = 0;
	if ( LoadRegistryDword( AWHK_REG_GRID_X, &gridVal ) )
	{
		*value = max( gridVal, 2 );
		*value = min( *value, 0xFF );
	}
}

BOOL LoadConfiguration( AWHK_APP_CONFIG* cfg )
{
	LoadRegistryBool	( AWHK_REG_ALLOW_SNAP	, &cfg->AllowSnapToOthers );
	LoadRegistryBool	( AWHK_REG_ADJACENCY	, &cfg->AllowModifyAdjacent );

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

	DWORD moveKeyMod = 0;
	DWORD fineKeyMod = 0;
	DWORD soloKeyMod = 0;
	LoadRegistryKeyMod	( AWHK_REG_MOVE_KEY_MOD	, &moveKeyMod );
	LoadRegistryKeyMod	( AWHK_REG_SOLO_KEY_MOD	, &soloKeyMod );
	LoadRegistryKeyMod	( AWHK_REG_FINE_KEY_MOD	, &fineKeyMod );

	// We MUST have a move key mod (otherwise what's the point?)
	if ( moveKeyMod )
		cfg->MoveKeyMod = moveKeyMod;

	// The two other modifiers must be different
	if ( fineKeyMod != moveKeyMod && fineKeyMod != soloKeyMod )
		cfg->FineKeyMod = fineKeyMod;
	if ( soloKeyMod != moveKeyMod && soloKeyMod != fineKeyMod )
		cfg->SoloKeyMod = soloKeyMod;

	return TRUE;
}

BOOL SaveConfiguration( const AWHK_APP_CONFIG* cfg )
{
	// TODO
	return FALSE;
}
