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

#pragma once

#ifdef _DEBUG
#   include <assert.h>
#endif

#define AWHK_MAKE_HOTKEY( mods, trigger ) MAKELONG( mods, trigger )
#define AWHK_GET_TRIGGER_KEY( packed ) HIWORD( packed )
#define AWHK_GET_MODIFIER_KEYS( packed) LOWORD( packed )

#pragma warning(push)
#pragma warning(disable : 4201) // nonstandard extension: anonymous struct
typedef union
{
    LONG dwBits;
    struct { 
        USHORT Modifiers;
        USHORT Trigger;
    };
} AWHK_KEY_COMBO;
#pragma warning(pop)

static inline AWHK_KEY_COMBO CreateKeyCombo(DWORD bits)
{
    AWHK_KEY_COMBO kc = { (LONG)bits };
#ifdef _DEBUG
    assert(kc.Trigger == AWHK_GET_TRIGGER_KEY(bits));
    assert(kc.Modifiers == AWHK_GET_MODIFIER_KEYS(bits));
#endif
    return kc;
}

static inline AWHK_KEY_COMBO CreateKeyComboFromModAndKey(USHORT modifiers, USHORT trigger)
{
    AWHK_KEY_COMBO kc = { AWHK_MAKE_HOTKEY(modifiers, trigger) };
#ifdef _DEBUG
    assert(kc.Trigger == trigger);
    assert(kc.Modifiers == modifiers);
#endif
    return kc;
}
