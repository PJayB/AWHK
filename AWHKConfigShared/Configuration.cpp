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

#using <WindowsBase.dll>

using namespace System::Windows::Input;

namespace AWHKConfigShared {

    static_assert( (INT) ModifierKeys::Alt == MOD_ALT, "Modifier Keys don't match." );
    static_assert( (INT) ModifierKeys::Control == MOD_CONTROL, "Modifier Keys don't match." );
    static_assert( (INT) ModifierKeys::Shift == MOD_SHIFT, "Modifier Keys don't match." );
    static_assert( (INT) ModifierKeys::Windows == MOD_WIN, "Modifier Keys don't match." );

    bool Configuration::AutoLogin::get()
    {
        return ::IsAutoLoginEnabled() != FALSE;
    }

    void Configuration::AutoLogin::set( bool v )
    {
        ::SetAutoLoginEnabled( v );
    }

    bool Configuration::DefaultBool( System::String^ name )
    {
        BOOL v = FALSE;
        pin_ptr<const wchar_t> nameC = PtrToStringChars( name );
        if (DefaultRegistryBool( nameC, &v ))
        {
            return v != FALSE;
        }
        throw gcnew ConfigurationIoException();
    }

	Int32 Configuration::DefaultInt( System::String^ name )
    {
        DWORD v = 0;
        pin_ptr<const wchar_t> nameC = PtrToStringChars( name );
        if (DefaultRegistryDword( nameC, &v ))
        {
            return (Int32) v;
        }
        throw gcnew ConfigurationIoException();
    }

	System::Windows::Input::Key Configuration::DefaultKey( System::String^ name )
    {
        DWORD v = 0;
        pin_ptr<const wchar_t> nameC = PtrToStringChars( name );
        if (DefaultRegistryVKey( nameC, &v ))
        {
            return KeyInterop::KeyFromVirtualKey( v );
        }
        throw gcnew ConfigurationIoException();
    }

	System::Windows::Input::ModifierKeys Configuration::DefaultModKeys( System::String^ name )
    {
        DWORD v = 0;
        pin_ptr<const wchar_t> nameC = PtrToStringChars( name );
        if (DefaultRegistryKeyMod( nameC, &v ))
        {
            return (ModifierKeys) v;
        }
        throw gcnew ConfigurationIoException();
    }

	KeyCombo Configuration::DefaultKeyCombo( System::String^ name )
    {
        USHORT mods = 0;
        USHORT trigger = 0;

        pin_ptr<const wchar_t> nameC = PtrToStringChars( name );
        if (!DefaultRegistryKeyCombo( nameC, &trigger, &mods ))
        {
            throw gcnew ConfigurationIoException();
        }

        KeyCombo kc;
        kc.Modifiers = (ModifierKeys) mods;
        kc.Key = KeyInterop::KeyFromVirtualKey( trigger );

        return kc;
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

	System::Windows::Input::Key Configuration::LoadKey( System::String^ name )
    {
        DWORD v = 0;
        pin_ptr<const wchar_t> nameC = PtrToStringChars( name );
        if (LoadRegistryVKey( nameC, &v ))
        {
            return KeyInterop::KeyFromVirtualKey( v );
        }
        throw gcnew ConfigurationIoException();
    }

	System::Windows::Input::ModifierKeys Configuration::LoadModKeys( System::String^ name )
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
        USHORT mods = 0;
        USHORT trigger = 0;

        pin_ptr<const wchar_t> nameC = PtrToStringChars( name );
        if (!LoadRegistryKeyCombo( nameC, &trigger, &mods ))
        {
            throw gcnew ConfigurationIoException();
        }

        KeyCombo kc;
        kc.Modifiers = (ModifierKeys) mods;
        kc.Key = KeyInterop::KeyFromVirtualKey( trigger );

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
    
    void Configuration::Store( System::String^ name, Key v )
    {
        pin_ptr<const wchar_t> nameC = PtrToStringChars( name );
        if (!StoreRegistryDword( nameC, (DWORD) KeyInterop::VirtualKeyFromKey( v ) ))
        {
            throw gcnew ConfigurationIoException();
        }
    }
    
    void Configuration::Store( System::String^ name, KeyCombo v )
    {
        pin_ptr<const wchar_t> nameC = PtrToStringChars( name );
        if (!StoreRegistryKeyCombo( nameC, (USHORT) KeyInterop::VirtualKeyFromKey( v.Key ), (USHORT) v.Modifiers ))
        {
            throw gcnew ConfigurationIoException();
        }
    }
}
