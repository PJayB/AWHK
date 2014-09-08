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

    public ref struct KeyBinding
    {
        bool ModAlt;
        bool ModControl;
        bool ModShift;
        UInt32 Key;
    };

    public ref struct CursorKeys
    {
        KeyBinding^ Left;
        KeyBinding^ Right;
        KeyBinding^ Up;
        KeyBinding^ Down;
    };

	public ref class Configuration
	{
    public:
    
		void Load();
        void Save();
    
        property bool AllowSnapToOthers;
        property int MaxEdgeSearchSize;
        property int GridX;
        property int GridY;
        property int FineX;
        property int FineY;
        property KeyBinding^ HelpKey;
        property KeyBinding^ ConfigKey;
        property CursorKeys^ ResizeKeys;
        property CursorKeys^ MoveKeys;
	};
    
    public ref class ServiceController
    {
    public:

        property bool AutoLogin {
            bool get() {
                return internalIsAutoLoginEnabled();
            }
            void set(bool v) {
                internalSetAutoLoginEnabled(v);
            }
        }

        property bool IsLoaded { 
            bool get() { 
                return internalIsLoaded(); 
            } 
        };

        void Suspend();
        void Resume();
        void ReloadConfiguration();
        void Unload();

    private:

        bool internalIsLoaded();
        bool internalIsAutoLoginEnabled();
        void internalSetAutoLoginEnabled(bool v);
    };
}
