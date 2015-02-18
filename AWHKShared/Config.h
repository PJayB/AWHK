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

struct AWHK_CURSOR_KEYS
{
	DWORD				LeftKey;
	DWORD				RightKey;
	DWORD				UpKey;
	DWORD				DownKey;
};

struct AWHK_KEY_COMBO
{
    DWORD               Trigger;
    DWORD               Modifiers;
};

struct AWHK_APP_CONFIG
{
	AWHK_APP_CONFIG();

	BOOL				AllowSnapToOthers;
	DWORD				MaxEdgeSearchSize;

	DWORD				GridX;
	DWORD				GridY;
	DWORD				FineX;
	DWORD				FineY;

	AWHK_KEY_COMBO      HelpCombo;
	AWHK_KEY_COMBO      ConfigCombo;

	DWORD				MoveKeyMod;
	DWORD				NextKeyMod;
	DWORD				FineKeyMod;

	AWHK_CURSOR_KEYS	ResizeKeys;
	AWHK_CURSOR_KEYS	MoveKeys;

    AWHK_KEY_COMBO      MediaPrev;
    AWHK_KEY_COMBO      MediaNext;
    AWHK_KEY_COMBO      MediaPlay;
    AWHK_KEY_COMBO      MediaPause;
    AWHK_KEY_COMBO      MediaPlayPause;
    AWHK_KEY_COMBO      MediaVolumeUp;
    AWHK_KEY_COMBO      MediaVolumeDown;
};

BOOL LoadConfiguration( AWHK_APP_CONFIG* cfg );
BOOL SaveConfiguration( const AWHK_APP_CONFIG* cfg );



