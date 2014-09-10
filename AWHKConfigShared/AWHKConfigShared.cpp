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

    void ClrModifierSetToNative(ModifierKeys a, DWORD* mods)
    {
        *mods = static_cast<DWORD>(a);
    }

    ModifierKeys NativeModifierSetToClr(DWORD mods)
    {
        return static_cast<ModifierKeys>(mods);
    }

    void ClrKeyBindingToNative(KeyBinding a, AWHK_KEY_BINDING* b)
    {
        b->Trigger = a.Trigger;
        ClrModifierSetToNative(a.Modifiers, &b->Modifiers);
    }

    KeyBinding NativeKeyBindingToClr(const AWHK_KEY_BINDING* a)
    {
        KeyBinding b;
        b.Trigger = a->Trigger;
        b.Modifiers = NativeModifierSetToClr(a->Modifiers);
        return b;
    }

    void ClrCursorKeysToNative(CursorKeys a, AWHK_CURSOR_KEYS* b)
    {
        ClrKeyBindingToNative(a.Left, &b->LeftKey);
        ClrKeyBindingToNative(a.Right, &b->RightKey);
        ClrKeyBindingToNative(a.Up, &b->UpKey);
        ClrKeyBindingToNative(a.Down, &b->DownKey);
    }

    CursorKeys NativeCursorKeysToClr(const AWHK_CURSOR_KEYS* a)
    {
        CursorKeys b;
        b.Left = NativeKeyBindingToClr(&a->LeftKey);
        b.Right = NativeKeyBindingToClr(&a->RightKey);
        b.Up = NativeKeyBindingToClr(&a->UpKey);
        b.Down = NativeKeyBindingToClr(&a->DownKey);
        return b;
    }

    void ClrConfigToNative(Configuration^ a, AWHK_APP_CONFIG* b)
    {
        b->EnableFineSnap = a->EnableFineSnap;
        b->AllowSnapToOthers = a->AllowSnapToOthers;
        b->MaxEdgeSearchSize = a->MaxEdgeSearchSize;
        b->GridX = a->GridX;
        b->GridY = a->GridY;
        b->FineX = a->FineX;
        b->FineY = a->FineY;
        ClrKeyBindingToNative(a->HelpKey, &b->HelpKey);
        ClrKeyBindingToNative(a->ConfigKey, &b->ConfigKey);
        ClrCursorKeysToNative(a->MoveKeys, &b->MoveKeys);
        ClrCursorKeysToNative(a->ResizeKeys, &b->ResizeKeys);
    }

    void NativeConfigToClr(const AWHK_APP_CONFIG* a, Configuration^ b)
    {
        b->EnableFineSnap = (a->EnableFineSnap != FALSE);
        b->AllowSnapToOthers = (a->AllowSnapToOthers != FALSE);
        b->MaxEdgeSearchSize = a->MaxEdgeSearchSize;
        b->GridX = a->GridX;
        b->GridY = a->GridY;
        b->FineX = a->FineX;
        b->FineY = a->FineY;
        b->HelpKey = NativeKeyBindingToClr(&a->HelpKey);
        b->ConfigKey = NativeKeyBindingToClr(&a->ConfigKey);
        b->MoveKeys = NativeCursorKeysToClr(&a->MoveKeys);
        b->ResizeKeys = NativeCursorKeysToClr(&a->ResizeKeys);
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
