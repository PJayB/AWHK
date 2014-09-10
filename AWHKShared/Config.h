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

struct AWHK_KEY_BINDING
{
    DWORD               Modifiers;
    DWORD               Trigger;
};

struct AWHK_CURSOR_KEYS
{
	AWHK_KEY_BINDING    LeftKey;
	AWHK_KEY_BINDING    RightKey;
	AWHK_KEY_BINDING    UpKey;
	AWHK_KEY_BINDING    DownKey;
};

struct AWHK_APP_CONFIG
{
	AWHK_APP_CONFIG();

	BOOL				AllowSnapToOthers;
    BOOL                EnableFineSnap;

	DWORD				MaxEdgeSearchSize;

	DWORD				GridX;
	DWORD				GridY;
	DWORD				FineX;
	DWORD				FineY;

	AWHK_KEY_BINDING	HelpKey;
	AWHK_KEY_BINDING	ConfigKey;

	AWHK_CURSOR_KEYS	ResizeKeys;
	AWHK_CURSOR_KEYS	MoveKeys;
};

BOOL LoadConfiguration( AWHK_APP_CONFIG* cfg );
BOOL SaveConfiguration( const AWHK_APP_CONFIG* cfg );

