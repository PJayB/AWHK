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

    DWORD ClrModifierSetToNative(ModifierKeys a)
    {
        return static_cast<DWORD>(a);
    }

    ModifierKeys NativeModifierSetToClr(DWORD mods)
    {
        return static_cast<ModifierKeys>(mods);
    }

    void ClrConfigToNative(Configuration^ a, AWHK_APP_CONFIG* b)
    {
        b->AllowSnapToOthers   = a->AllowSnapToOthers;
        b->MaxEdgeSearchSize   = a->MaxEdgeSearchSize;
        b->GridX               = a->GridX;
        b->GridY               = a->GridY;
        b->FineX               = a->FineX;
        b->FineY               = a->FineY;
        b->HelpKey             = a->HelpKey;
        b->HelpKeyMod          = ClrModifierSetToNative(a->HelpKeyMod);
        b->ConfigKey           = a->ConfigKey;
        b->ConfigKeyMod        = ClrModifierSetToNative(a->ConfigKeyMod);
        b->MoveKeys.LeftKey    = a->MoveLeft   ;
        b->MoveKeys.RightKey   = a->MoveRight  ;
        b->MoveKeys.UpKey      = a->MoveUp     ;
        b->MoveKeys.DownKey    = a->MoveDown   ;
        b->ResizeKeys.LeftKey  = a->ResizeLeft ;
        b->ResizeKeys.RightKey = a->ResizeRight;
        b->ResizeKeys.UpKey    = a->ResizeUp   ;
        b->ResizeKeys.DownKey  = a->ResizeDown ;
        b->FineKeyMod          = ClrModifierSetToNative(a->FineKeyMod);
        b->NextKeyMod          = ClrModifierSetToNative(a->GrabKeyMod);
        b->MoveKeyMod          = ClrModifierSetToNative(a->BaseKeyMod);
    }

    void NativeConfigToClr(const AWHK_APP_CONFIG* a, Configuration^ b)
    {
        b->AllowSnapToOthers   = (a->AllowSnapToOthers != FALSE);
        b->MaxEdgeSearchSize   = a->MaxEdgeSearchSize;
        b->GridX               = a->GridX;
        b->GridY               = a->GridY;
        b->FineX               = a->FineX;
        b->FineY               = a->FineY;
        b->HelpKey             = a->HelpKey;
        b->HelpKeyMod          = NativeModifierSetToClr(a->HelpKeyMod);
        b->ConfigKey           = a->ConfigKey;
        b->ConfigKeyMod        = NativeModifierSetToClr(a->ConfigKeyMod);
        b->MoveLeft            = a->MoveKeys.LeftKey   ;
        b->MoveRight           = a->MoveKeys.RightKey  ;
        b->MoveUp              = a->MoveKeys.UpKey     ;
        b->MoveDown            = a->MoveKeys.DownKey   ;
        b->ResizeLeft          = a->ResizeKeys.LeftKey ;
        b->ResizeRight         = a->ResizeKeys.RightKey;
        b->ResizeUp            = a->ResizeKeys.UpKey   ;
        b->ResizeDown          = a->ResizeKeys.DownKey ;
        b->FineKeyMod          = NativeModifierSetToClr(a->FineKeyMod);
        b->GrabKeyMod          = NativeModifierSetToClr(a->NextKeyMod);
        b->BaseKeyMod          = NativeModifierSetToClr(a->MoveKeyMod);
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
