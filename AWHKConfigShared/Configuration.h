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

    using namespace System::Windows::Input;

    public ref class ServiceNotRunningException : public Exception
    {
    };

    public ref class ConfigurationIoException : public Exception
    {
    };

    public value struct KeyCombo
    {
    public:
        KeyCombo(ModifierKeys mods, Key trigger)
        {
            Key = trigger;
            Modifiers = mods;
        }

        ModifierKeys Modifiers;
        Key Key;
    };
    
	public ref class Configuration sealed
	{
    public:

		static bool         DefaultBool( System::String^ name );
		static Int32        DefaultInt( System::String^ name );
	    static Key          DefaultKey( System::String^ name );
		static ModifierKeys DefaultModKeys( System::String^ name );
        static KeyCombo     DefaultKeyCombo( System::String^ name );

		static bool         LoadBool( System::String^ name );
		static Int32        LoadInt( System::String^ name );
	    static Key          LoadKey( System::String^ name );
		static ModifierKeys LoadModKeys( System::String^ name );
        static KeyCombo     LoadKeyCombo( System::String^ name );
        
        static void         Store( System::String^ name, bool v );
        static void         Store( System::String^ name, Int32 v );
        static void         Store( System::String^ name, Key v );
        static void         Store( System::String^ name, ModifierKeys v );
        static void         Store( System::String^ name, KeyCombo v );

        static bool         Test( System::IntPtr hWnd, int id, KeyCombo v );

        static property bool AutoLogin
        {
            bool get();
            void set( bool v );
        };
	};
}
