// This is the main DLL file.

#include "stdafx.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "../AWHKShared/RegistryKeys.h"
#include "../AWHKShared/SupportFile.h"
#include "../AWHKShared/Config.h"
#include "../AWHKShared/AutoLogin.h"

#include "Configuration.h"

#include <vcclr.h>

namespace AWHKConfigShared {

    Configuration::Configuration()
    {
    }

    bool Configuration::AutoLogin::get()
    {
        return ::IsAutoLoginEnabled() != FALSE;
    }

    void Configuration::AutoLogin::set( bool v )
    {
        ::SetAutoLoginEnabled( v );
    }

    bool Configuration::LoadBool( System::String^ name )
    {
        BOOL v = FALSE;
        pin_ptr<const wchar_t> nameC = PtrToStringChars( name );
        if (LoadRegistryBool( nameC, &v ))
        {
            return v != FALSE;
        }
        throw gcnew ConfigurationIoException();
    }

	Int32 Configuration::LoadInt( System::String^ name )
    {
        DWORD v = 0;
        pin_ptr<const wchar_t> nameC = PtrToStringChars( name );
        if (LoadRegistryDword( nameC, &v ))
        {
            return (Int32) v;
        }
        throw gcnew ConfigurationIoException();
    }

	Int32 Configuration::LoadVKey( System::String^ name )
    {
        DWORD v = 0;
        pin_ptr<const wchar_t> nameC = PtrToStringChars( name );
        if (LoadRegistryVKey( nameC, &v ))
        {
            return (Int32) v;
        }
        throw gcnew ConfigurationIoException();
    }

	ModifierKeys Configuration::LoadModKeys( System::String^ name )
    {
        DWORD v = 0;
        pin_ptr<const wchar_t> nameC = PtrToStringChars( name );
        if (LoadRegistryKeyMod( nameC, &v ))
        {
            return (ModifierKeys) v;
        }
        throw gcnew ConfigurationIoException();
    }

	KeyCombo Configuration::LoadKeyCombo( System::String^ name )
    {
        DWORD mods = 0;
        DWORD trigger = 0;

        pin_ptr<const wchar_t> nameC = PtrToStringChars( name );
        if (!LoadRegistryKeyCombo( nameC, &trigger, &mods ))
        {
            throw gcnew ConfigurationIoException();
        }

        KeyCombo kc;
        kc.Modifiers = (ModifierKeys) mods;
        kc.VKey = (Int32) trigger;

        return kc;
    }
        
    void Configuration::Store( System::String^ name, bool v )
    {
        pin_ptr<const wchar_t> nameC = PtrToStringChars( name );
        if (!StoreRegistryDword( nameC, (DWORD) v ))
        {
            throw gcnew ConfigurationIoException();
        }
    }

    void Configuration::Store( System::String^ name, Int32 v )
    {
        pin_ptr<const wchar_t> nameC = PtrToStringChars( name );
        if (!StoreRegistryDword( nameC, v ))
        {
            throw gcnew ConfigurationIoException();
        }
    }

    void Configuration::Store( System::String^ name, ModifierKeys v )
    {
        pin_ptr<const wchar_t> nameC = PtrToStringChars( name );
        if (!StoreRegistryDword( nameC, (DWORD) v ))
        {
            throw gcnew ConfigurationIoException();
        }
    }
    
    void Configuration::Store( System::String^ name, ModifierKeys mods, Int32 trigger )
    {
        pin_ptr<const wchar_t> nameC = PtrToStringChars( name );
        if (!StoreRegistryKeyCombo( nameC, (DWORD) trigger, (DWORD) mods ))
        {
            throw gcnew ConfigurationIoException();
        }
    }
}
