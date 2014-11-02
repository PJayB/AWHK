/*
	Copyright (C) 2014 Peter J. B. Lewis

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

using namespace System;

namespace AWHKConfigShared {

    public ref class ServiceNotRunningException : public Exception
    {
    };

    public ref class ConfigurationIoException : public Exception
    {
    };

    [Flags] public enum class ModifierKeys
    {
        None = 0,
        Shift = MOD_SHIFT,
        Control = MOD_CONTROL,
        Alt = MOD_ALT,
        Windows = MOD_WIN
    };

    public struct KeyCombo
    {
        ModifierKeys Modifiers;
        Int32 VKey;
    };
    
	public ref class Configuration sealed
	{
    public:

        Configuration();
    
		bool LoadBool( System::String^ name );
		Int32 LoadInt( System::String^ name );
		Int32 LoadVKey( System::String^ name );
		ModifierKeys LoadModKeys( System::String^ name );
        KeyCombo LoadKeyCombo( System::String^ name );
        
        void Store( System::String^ name, bool v );
        void Store( System::String^ name, Int32 v );
        void Store( System::String^ name, ModifierKeys v );
        void Store( System::String^ name, ModifierKeys mods, Int32 trigger );

        property bool AutoLogin
        {
            bool get();
            void set( bool v );
        };
	};
}
