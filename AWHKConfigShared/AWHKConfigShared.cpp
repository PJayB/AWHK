// This is the main DLL file.

#include "stdafx.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "../AWHKShared/IPC.h"
#include "../AWHKShared/RegistryKeys.h"
#include "../AWHKShared/SupportFile.h"
#include "../AWHKShared/Config.h"
#include "../AWHKShared/AutoLogin.h"

#include "AWHKConfigShared.h"

namespace AWHKConfigShared {

    void ClrModifierSetToNative(ModifierSet a, DWORD* mods)
    {
        *mods = 0;
        if (a.Shift) {
            *mods |= MOD_SHIFT;
        }
        if (a.Control) {
            *mods |= MOD_CONTROL;
        }
        if (a.Alt) {
            *mods |= MOD_ALT;
        }
        if (a.Win) {
            *mods |= MOD_WIN;
        }
    }

    ModifierSet NativeModifierSetToClr(DWORD mods)
    {
        ModifierSet b;
        if (mods & MOD_SHIFT) {
            b.Shift = true;
        }
        if (mods & MOD_CONTROL) {
            b.Control = true;
        }
        if (mods & MOD_ALT) {
            b.Alt = true;
        }
        if (mods & MOD_WIN) {
            b.Win = true;
        }
        return b;
    }

    void ClrKeyBindingToNative(KeyBinding a, DWORD* key, DWORD* mods)
    {
        *key = a.Key;
        ClrModifierSetToNative(a.Mods, mods);
    }

    KeyBinding NativeKeyBindingToClr(DWORD key, DWORD mods)
    {
        KeyBinding b;
        b.Key = key;
        b.Mods = NativeModifierSetToClr(mods);
        return b;
    }

    void ClrCursorKeysToNative(CursorKeys a, AWHK_CURSOR_KEYS* b)
    {
        b->LeftKey = a.Left;
        b->RightKey = a.Right;
        b->UpKey = a.Up;
        b->DownKey = a.Down;
    }

    CursorKeys NativeCursorKeysToClr(const AWHK_CURSOR_KEYS* a)
    {
        CursorKeys b;
        b.Left = a->LeftKey;
        b.Right = a->RightKey;
        b.Up = a->UpKey;
        b.Down = a->DownKey;
        return b;
    }

    void ClrConfigToNative(Configuration^ a, AWHK_APP_CONFIG* b)
    {
        b->EnableFineSnap = a->EnableFineSnap;
        b->EnableResizeKeys = a->EnableResizeKeys;
        b->EnableMoveKeys = a->EnableMoveKeys;
        b->AllowSnapToOthers = a->AllowSnapToOthers;
        b->MaxEdgeSearchSize = a->MaxEdgeSearchSize;
        b->GridX = a->GridX;
        b->GridY = a->GridY;
        b->FineX = a->FineX;
        b->FineY = a->FineY;
        ClrKeyBindingToNative(a->HelpKey, &b->HelpKey, &b->HelpKeyMod);
        ClrKeyBindingToNative(a->ConfigKey, &b->ConfigKey, &b->ConfigKeyMod);
        ClrCursorKeysToNative(a->MoveKeys, &b->MoveKeys);
        ClrCursorKeysToNative(a->ResizeKeys, &b->ResizeKeys);
        ClrModifierSetToNative(a->MoveKeyMod, &b->MoveKeyMod);
        ClrModifierSetToNative(a->NextKeyMod, &b->NextKeyMod);
        ClrModifierSetToNative(a->FineKeyMod, &b->FineKeyMod);
    }

    void NativeConfigToClr(const AWHK_APP_CONFIG* a, Configuration^ b)
    {
        b->EnableFineSnap = (a->EnableFineSnap != FALSE);
        b->EnableResizeKeys = (a->EnableResizeKeys != FALSE);
        b->EnableMoveKeys = (a->EnableMoveKeys != FALSE);
        b->AllowSnapToOthers = (a->AllowSnapToOthers != FALSE);
        b->MaxEdgeSearchSize = a->MaxEdgeSearchSize;
        b->GridX = a->GridX;
        b->GridY = a->GridY;
        b->FineX = a->FineX;
        b->FineY = a->FineY;
        b->HelpKey = NativeKeyBindingToClr(a->HelpKey, a->HelpKeyMod);
        b->ConfigKey = NativeKeyBindingToClr(a->ConfigKey, a->ConfigKeyMod);
        b->MoveKeys = NativeCursorKeysToClr(&a->MoveKeys);
        b->ResizeKeys = NativeCursorKeysToClr(&a->ResizeKeys);
        b->MoveKeyMod = NativeModifierSetToClr(a->MoveKeyMod);
        b->NextKeyMod = NativeModifierSetToClr(a->NextKeyMod);
        b->FineKeyMod = NativeModifierSetToClr(a->FineKeyMod);
    }

    Configuration::Configuration()
    {
        AWHK_APP_CONFIG cfg;
        NativeConfigToClr(&cfg, this);
    }

    void Configuration::Load()
    {
        AutoLogin = (::IsAutoLoginEnabled() != FALSE);

        AWHK_APP_CONFIG cfg;
        if (LoadConfiguration(&cfg))
        {
            NativeConfigToClr(&cfg, this);
        }
        else
        {
            throw gcnew ConfigurationIoException();
        }
    }

    void Configuration::Save()
    {
        ::SetAutoLoginEnabled(AutoLogin);

        AWHK_APP_CONFIG cfg;
        ClrConfigToNative(this, &cfg);
        if (!SaveConfiguration(&cfg))
            throw gcnew ConfigurationIoException();
    }

    void ServiceController::ReloadConfiguration()
    {
        IPC ipc;
        if ( !OpenIPC( &ipc ) )
        {
            throw gcnew ServiceNotRunningException();
        }

        WriteMessageIPC( &ipc, IPC_MSG_QUIT );
        CloseIPC( &ipc );
    }

    void ServiceController::Unload()
    {
        IPC ipc;
        if ( !OpenIPC( &ipc ) )
        {
            throw gcnew ServiceNotRunningException();
        }

        WriteMessageIPC( &ipc, IPC_MSG_RELOAD_CONFIG );
        CloseIPC( &ipc );
    }

    void ServiceController::Suspend()
    {
        IPC ipc;
        if ( !OpenIPC( &ipc ) )
        {
            throw gcnew ServiceNotRunningException();
        }

        WriteMessageIPC( &ipc, IPC_MSG_SUSPEND );
        CloseIPC( &ipc );
    }

    void ServiceController::Resume()
    {
        IPC ipc;
        if ( !OpenIPC( &ipc ) )
        {
            throw gcnew ServiceNotRunningException();
        }

        WriteMessageIPC( &ipc, IPC_MSG_RESUME );
        CloseIPC( &ipc );
    }

    bool ServiceController::internalIsLoaded()
    {
        IPC ipc;
        if ( OpenIPC( &ipc ) )
        {
            CloseIPC( &ipc );
            return true;
        }
        return false;
    }
}
