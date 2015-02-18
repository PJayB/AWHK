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
{
}

BOOL LoadConfigGridValue( LPCWSTR strName, DWORD* value )
{
	DWORD gridVal = 0;
	if ( LoadRegistryDword( strName, &gridVal ) )
	{
		*value = max( gridVal, 2 );
		*value = min( *value, 0xFF );
        return TRUE;
	}
    return FALSE;
}

BOOL LoadKeyCombo( LPCWSTR strName, AWHK_KEY_COMBO* combo )
{
    return LoadRegistryKeyCombo( strName, &combo->Trigger, &combo->Modifiers );
}

BOOL SaveKeyCombo( LPCWSTR strName, const AWHK_KEY_COMBO* combo )
{
    return StoreRegistryKeyCombo( strName, combo->Trigger, combo->Modifiers );
}

BOOL LoadConfiguration( AWHK_APP_CONFIG* cfg )
{
	LoadRegistryBool	( AWHK_REG_ALLOW_SNAP	    , &cfg->AllowSnapToOthers );
	LoadConfigGridValue	( AWHK_REG_EDGE_SEARCH	    , &cfg->MaxEdgeSearchSize );

	LoadConfigGridValue	( AWHK_REG_GRID_X		    , &cfg->GridX );
	LoadConfigGridValue	( AWHK_REG_GRID_Y		    , &cfg->GridY );
	LoadConfigGridValue	( AWHK_REG_FINE_X		    , &cfg->FineX );
	LoadConfigGridValue	( AWHK_REG_FINE_Y		    , &cfg->FineY );

	LoadKeyCombo	    ( AWKH_REG_HELP_COMBO	    , &cfg->HelpCombo );
	LoadKeyCombo	    ( AWKH_REG_CFG_COMBO	    , &cfg->ConfigCombo );
    
	LoadRegistryVKey	( AWHK_REG_RESIZE_LEFT		, &cfg->ResizeKeys.LeftKey );
	LoadRegistryVKey	( AWHK_REG_RESIZE_RIGHT	    , &cfg->ResizeKeys.RightKey );
	LoadRegistryVKey	( AWHK_REG_RESIZE_UP	    , &cfg->ResizeKeys.UpKey );
	LoadRegistryVKey	( AWHK_REG_RESIZE_DOWN		, &cfg->ResizeKeys.DownKey );

	LoadRegistryVKey	( AWHK_REG_MOVE_LEFT	    , &cfg->MoveKeys.LeftKey );
	LoadRegistryVKey	( AWHK_REG_MOVE_RIGHT	    , &cfg->MoveKeys.RightKey );
	LoadRegistryVKey	( AWHK_REG_MOVE_UP		    , &cfg->MoveKeys.UpKey );
	LoadRegistryVKey	( AWHK_REG_MOVE_DOWN	    , &cfg->MoveKeys.DownKey );

    LoadKeyCombo        ( AWHK_REG_MEDIA_PREV       , &cfg->MediaPrev );
    LoadKeyCombo        ( AWHK_REG_MEDIA_NEXT       , &cfg->MediaPrev );
    LoadKeyCombo        ( AWHK_REG_MEDIA_PLAY       , &cfg->MediaPlay );
    LoadKeyCombo        ( AWHK_REG_MEDIA_PAUSE      , &cfg->MediaPause );
    LoadKeyCombo        ( AWHK_REG_MEDIA_PLAY_PAUSE , &cfg->MediaPlayPause );
    LoadKeyCombo        ( AWHK_REG_MEDIA_VOLUME_UP  , &cfg->MediaVolumeUp );
    LoadKeyCombo        ( AWHK_REG_MEDIA_VOLUME_DOWN, &cfg->MediaVolumeDown );

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
	if ( !fineKeyMod || ( fineKeyMod != moveKeyMod && fineKeyMod != nextKeyMod ) )
		cfg->FineKeyMod = fineKeyMod;
	if ( !nextKeyMod || ( nextKeyMod != moveKeyMod && nextKeyMod != fineKeyMod ) )
		cfg->NextKeyMod = nextKeyMod;

    return TRUE;
}

BOOL SaveConfiguration( const AWHK_APP_CONFIG* cfg )
{
	StoreRegistryDword  ( AWHK_REG_ALLOW_SNAP		, cfg->AllowSnapToOthers );
	StoreRegistryDword  ( AWHK_REG_EDGE_SEARCH	    , cfg->MaxEdgeSearchSize );

	StoreRegistryDword  ( AWHK_REG_GRID_X			, cfg->GridX );
	StoreRegistryDword  ( AWHK_REG_GRID_Y			, cfg->GridY );
	StoreRegistryDword  ( AWHK_REG_FINE_X			, cfg->FineX );
	StoreRegistryDword  ( AWHK_REG_FINE_Y			, cfg->FineY );

	SaveKeyCombo	    ( AWKH_REG_HELP_COMBO	    , &cfg->HelpCombo );
	SaveKeyCombo	    ( AWKH_REG_CFG_COMBO	    , &cfg->ConfigCombo );

	StoreRegistryDword	( AWHK_REG_RESIZE_LEFT		, cfg->ResizeKeys.LeftKey );
	StoreRegistryDword	( AWHK_REG_RESIZE_RIGHT	    , cfg->ResizeKeys.RightKey );
	StoreRegistryDword	( AWHK_REG_RESIZE_UP	    , cfg->ResizeKeys.UpKey );
	StoreRegistryDword	( AWHK_REG_RESIZE_DOWN		, cfg->ResizeKeys.DownKey );

    StoreRegistryDword	( AWHK_REG_MOVE_LEFT	    , cfg->MoveKeys.LeftKey );
	StoreRegistryDword	( AWHK_REG_MOVE_RIGHT	    , cfg->MoveKeys.RightKey );
	StoreRegistryDword	( AWHK_REG_MOVE_UP		    , cfg->MoveKeys.UpKey );
	StoreRegistryDword	( AWHK_REG_MOVE_DOWN	    , cfg->MoveKeys.DownKey );

    SaveKeyCombo        ( AWHK_REG_MEDIA_PREV       , &cfg->MediaPrev );
    SaveKeyCombo        ( AWHK_REG_MEDIA_NEXT       , &cfg->MediaPrev );
    SaveKeyCombo        ( AWHK_REG_MEDIA_PLAY       , &cfg->MediaPlay );
    SaveKeyCombo        ( AWHK_REG_MEDIA_PAUSE      , &cfg->MediaPause );
    SaveKeyCombo        ( AWHK_REG_MEDIA_PLAY_PAUSE , &cfg->MediaPlayPause );
    SaveKeyCombo        ( AWHK_REG_MEDIA_VOLUME_UP  , &cfg->MediaVolumeUp );
    SaveKeyCombo        ( AWHK_REG_MEDIA_VOLUME_DOWN, &cfg->MediaVolumeDown );
	
	StoreRegistryDword	( AWHK_REG_MOVE_KEY_MOD     , cfg->MoveKeyMod );
	StoreRegistryDword	( AWHK_REG_NEXT_KEY_MOD     , cfg->NextKeyMod );
	StoreRegistryDword	( AWHK_REG_FINE_KEY_MOD     , cfg->FineKeyMod );

    return TRUE;
}
