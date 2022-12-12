/*
Copyright (c) 2022 Pete Lewis

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "stdafx.h"
#include "MediaKeys.h"

// All this does is reissue hotkey events pretending the user hit their media keys (even if they
// don't have any.

BOOL IssueHotkey( DWORD vkey )
{
    INPUT inputs[2];
    ZeroMemory( &inputs, sizeof(inputs) );

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = (WORD) vkey;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = (WORD) vkey;
    inputs[1].ki.dwFlags |= KEYEVENTF_KEYUP;

    if ( SendInput( _countof(inputs), inputs, sizeof(inputs[0]) ) != _countof(inputs) )
        return FALSE;

    return TRUE;
}

BOOL MediaPlayPause()   { return IssueHotkey( VK_MEDIA_PLAY_PAUSE ); }
BOOL MediaStop()        { return IssueHotkey( VK_MEDIA_STOP ); }
BOOL MediaNextTrack()   { return IssueHotkey( VK_MEDIA_NEXT_TRACK ); }
BOOL MediaPrevTrack()   { return IssueHotkey( VK_MEDIA_PREV_TRACK ); }
BOOL MediaMute()        { return IssueHotkey( VK_VOLUME_MUTE ); }
BOOL MediaVolumeUp()    { return IssueHotkey( VK_VOLUME_UP ); }
BOOL MediaVolumeDown()  { return IssueHotkey( VK_VOLUME_DOWN ); }
