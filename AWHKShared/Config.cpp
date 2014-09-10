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

#include "stdafx.h"
#include "Config.h"
#include "RegistryKeys.h"

AWHK_APP_CONFIG::AWHK_APP_CONFIG()
	: AllowSnapToOthers( TRUE )
    , EnableFineSnap( TRUE )
	, MaxEdgeSearchSize( 128 )
	, GridX( 8 )
	, GridY( 4 )
	, FineX( 32 )
	, FineY( 16 )
{
    HelpKey.Trigger = VK_F1;
    HelpKey.Modifiers = MOD_ALT;

    ConfigKey.Trigger = VK_F2;
    ConfigKey.Modifiers = MOD_ALT;

	ResizeKeys.LeftKey.Trigger = VK_LEFT;
	ResizeKeys.RightKey.Trigger = VK_RIGHT;
	ResizeKeys.UpKey.Trigger = VK_UP;
    ResizeKeys.DownKey.Trigger = VK_DOWN;

	ResizeKeys.LeftKey.Modifiers = MOD_ALT;
	ResizeKeys.RightKey.Modifiers = MOD_ALT;
	ResizeKeys.UpKey.Modifiers = MOD_ALT;
    ResizeKeys.DownKey.Modifiers = MOD_ALT;

	MoveKeys.LeftKey.Trigger = 'A';
	MoveKeys.RightKey.Trigger = 'D';
	MoveKeys.UpKey.Trigger = 'W';
	MoveKeys.DownKey.Trigger = 'S';

	MoveKeys.LeftKey.Modifiers = MOD_ALT;
	MoveKeys.RightKey.Modifiers = MOD_ALT;
	MoveKeys.UpKey.Modifiers = MOD_ALT;
	MoveKeys.DownKey.Modifiers = MOD_ALT;
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
	LoadRegistryBool	( AWHK_REG_ALLOW_SNAP	    , &cfg->AllowSnapToOthers );
	LoadRegistryBool    ( AWHK_REG_FINE_SNAP		, &cfg->EnableFineSnap );
	LoadConfigGridValue	( AWHK_REG_EDGE_SEARCH	    , &cfg->MaxEdgeSearchSize );

	LoadConfigGridValue	( AWHK_REG_GRID_X		    , &cfg->GridX );
	LoadConfigGridValue	( AWHK_REG_GRID_Y		    , &cfg->GridY );
	LoadConfigGridValue	( AWHK_REG_FINE_X		    , &cfg->FineX );
	LoadConfigGridValue	( AWHK_REG_FINE_Y		    , &cfg->FineY );

	LoadRegistryVKey	( AWKH_REG_HELP_KEY		    , &cfg->HelpKey.Trigger );
	LoadRegistryKeyMod	( AWKH_REG_HELP_KEY_MOD	    , &cfg->HelpKey.Modifiers );
	LoadRegistryVKey	( AWKH_REG_CFG_KEY		    , &cfg->ConfigKey.Trigger );
	LoadRegistryKeyMod	( AWKH_REG_CFG_KEY_MOD	    , &cfg->ConfigKey.Modifiers );

	LoadRegistryVKey	( AWHK_REG_LEFT_KEY		    , &cfg->ResizeKeys.LeftKey.Trigger );
	LoadRegistryKeyMod	( AWHK_REG_LEFT_KEY_MOD     , &cfg->ResizeKeys.LeftKey.Modifiers );
	LoadRegistryVKey	( AWHK_REG_RIGHT_KEY	    , &cfg->ResizeKeys.RightKey.Trigger );
	LoadRegistryKeyMod	( AWHK_REG_RIGHT_KEY_MOD    , &cfg->ResizeKeys.RightKey.Modifiers );
	LoadRegistryVKey	( AWHK_REG_UP_KEY		    , &cfg->ResizeKeys.UpKey.Trigger );
	LoadRegistryKeyMod	( AWHK_REG_UP_KEY_MOD	    , &cfg->ResizeKeys.UpKey.Modifiers );
	LoadRegistryVKey	( AWHK_REG_DOWN_KEY		    , &cfg->ResizeKeys.DownKey.Trigger );
	LoadRegistryKeyMod	( AWHK_REG_DOWN_KEY_MOD	    , &cfg->ResizeKeys.DownKey.Modifiers );

	LoadRegistryVKey	( AWHK_REG_LEFT_KEY_2	    , &cfg->MoveKeys.LeftKey.Trigger );
	LoadRegistryKeyMod	( AWHK_REG_LEFT_KEY_2_MOD   , &cfg->MoveKeys.LeftKey.Modifiers );
	LoadRegistryVKey	( AWHK_REG_RIGHT_KEY_2	    , &cfg->MoveKeys.RightKey.Trigger );
	LoadRegistryKeyMod	( AWHK_REG_RIGHT_KEY_2_MOD  , &cfg->MoveKeys.RightKey.Modifiers );
	LoadRegistryVKey	( AWHK_REG_UP_KEY_2		    , &cfg->MoveKeys.UpKey.Trigger );
	LoadRegistryKeyMod	( AWHK_REG_UP_KEY_2_MOD	    , &cfg->MoveKeys.UpKey.Modifiers );
	LoadRegistryVKey	( AWHK_REG_DOWN_KEY_2	    , &cfg->MoveKeys.DownKey.Trigger );
	LoadRegistryKeyMod	( AWHK_REG_DOWN_KEY_2_MOD   , &cfg->MoveKeys.DownKey.Modifiers );

    // We assume validation was done by the tool. We will simply fail to bind if
    // the configuration is damaged.

	return TRUE;
}

BOOL SaveConfiguration( const AWHK_APP_CONFIG* cfg )
{
	StoreRegistryDword( AWHK_REG_ALLOW_SNAP		, cfg->AllowSnapToOthers );
	StoreRegistryDword( AWHK_REG_FINE_SNAP		, cfg->EnableFineSnap );
	StoreRegistryDword( AWHK_REG_EDGE_SEARCH	, cfg->MaxEdgeSearchSize );

	StoreRegistryDword( AWHK_REG_GRID_X			, cfg->GridX );
	StoreRegistryDword( AWHK_REG_GRID_Y			, cfg->GridY );
	StoreRegistryDword( AWHK_REG_FINE_X			, cfg->FineX );
	StoreRegistryDword( AWHK_REG_FINE_Y			, cfg->FineY );

	StoreRegistryDword( AWKH_REG_HELP_KEY		, cfg->HelpKey.Trigger );
	StoreRegistryDword( AWKH_REG_HELP_KEY_MOD	, cfg->HelpKey.Modifiers );
	StoreRegistryDword( AWKH_REG_CFG_KEY		, cfg->ConfigKey.Trigger );
	StoreRegistryDword( AWKH_REG_CFG_KEY_MOD	, cfg->ConfigKey.Modifiers );

	StoreRegistryDword( AWHK_REG_LEFT_KEY		, cfg->ResizeKeys.LeftKey.Trigger );
	StoreRegistryDword( AWHK_REG_LEFT_KEY_MOD	, cfg->ResizeKeys.LeftKey.Modifiers );
	StoreRegistryDword( AWHK_REG_RIGHT_KEY		, cfg->ResizeKeys.RightKey.Trigger );
	StoreRegistryDword( AWHK_REG_RIGHT_KEY_MOD	, cfg->ResizeKeys.RightKey.Modifiers );
	StoreRegistryDword( AWHK_REG_UP_KEY			, cfg->ResizeKeys.UpKey.Trigger );
	StoreRegistryDword( AWHK_REG_UP_KEY_MOD		, cfg->ResizeKeys.UpKey.Modifiers );
	StoreRegistryDword( AWHK_REG_DOWN_KEY		, cfg->ResizeKeys.DownKey.Trigger );
	StoreRegistryDword( AWHK_REG_DOWN_KEY_MOD	, cfg->ResizeKeys.DownKey.Modifiers );

	StoreRegistryDword( AWHK_REG_LEFT_KEY_2		, cfg->MoveKeys.LeftKey.Trigger );
	StoreRegistryDword( AWHK_REG_LEFT_KEY_2_MOD	, cfg->MoveKeys.LeftKey.Modifiers );
	StoreRegistryDword( AWHK_REG_RIGHT_KEY_2	, cfg->MoveKeys.RightKey.Trigger );
	StoreRegistryDword( AWHK_REG_RIGHT_KEY_2_MOD, cfg->MoveKeys.RightKey.Modifiers );
	StoreRegistryDword( AWHK_REG_UP_KEY_2		, cfg->MoveKeys.UpKey.Trigger );
	StoreRegistryDword( AWHK_REG_UP_KEY_2_MOD	, cfg->MoveKeys.UpKey.Modifiers );
	StoreRegistryDword( AWHK_REG_DOWN_KEY_2		, cfg->MoveKeys.DownKey.Trigger );
	StoreRegistryDword( AWHK_REG_DOWN_KEY_2_MOD	, cfg->MoveKeys.DownKey.Modifiers );
	
	return TRUE;
}
