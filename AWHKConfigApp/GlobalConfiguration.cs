using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Windows.Input;
using HotKeyCustomControlLibrary;
using System.Windows.Data;
using System.Globalization;
using AWHKConfigShared;

namespace AWHKConfigApp
{
    // Translate an AWHKShared exception to a write error
    class ConfigurationWriteException : Exception 
    {
        public ConfigurationWriteException(ConfigurationIoException inner) 
            : base(
                "Failed to save the configuration settings. Make sure you have permissions to write to the system registry.",
                inner)
        {
        }
    }

    // Translate an AWHKShared exception to a write error
    class ConfigurationReadException : Exception 
    {
        public ConfigurationReadException(ConfigurationIoException inner)
            : base(
                "Failed to read the configuration settings. Make sure you have permissions to read from the system registry.",
                inner)
        {
        }
    }

    public class ModifierKeyView
    {
        public ModifierKeys Value { get; set; }

        public bool HasShift
        {
            get { return (Value & ModifierKeys.Shift) != 0; }
            set { Value = ModifierKeyState.Set(Value, ModifierKeys.Shift, value); }
        }

        public bool HasControl
        {
            get { return (Value & ModifierKeys.Control) != 0; }
            set { Value = ModifierKeyState.Set(Value, ModifierKeys.Control, value); }
        }

        public bool HasAlt
        {
            get { return (Value & ModifierKeys.Alt) != 0; }
            set { Value = ModifierKeyState.Set(Value, ModifierKeys.Alt, value); }
        }

        public bool HasWindows
        {
            get { return (Value & ModifierKeys.Windows) != 0; }
            set { Value = ModifierKeyState.Set(Value, ModifierKeys.Windows, value); }
        }
    }    

    static class ConfigurationProxy
    {
        public static bool? GetBool(string propertyName)
        {
            try
            {
                return AWHKConfigShared.Configuration.LoadBool(propertyName);
            }
            catch (ConfigurationIoException)
            {
                return default(bool?);
            } 
        }

        public static void SetBool(string propertyName, bool v)
        {
            AWHKConfigShared.Configuration.Store(propertyName, v);
        }

        public static int? GetInt(string propertyName)
        {
            try
            {
                return (Int32)AWHKConfigShared.Configuration.LoadInt(propertyName);
            }
            catch (ConfigurationIoException)
            {
                return default(int?);
            }
        }

        public static void SetInt(string propertyName, Int32 v)
        {
            AWHKConfigShared.Configuration.Store(propertyName, v);
        }

        // Uses magic to get any key based on the property name
        public static Key? GetKey(string propertyName)
        {
            try
            {
                return AWHKConfigShared.Configuration.LoadKey(propertyName);
            }
            catch (ConfigurationIoException)
            {
                return default(Key?);
            } 
        }

        public static void SetKey(string propertyName, Key v)
        {
            AWHKConfigShared.Configuration.Store(propertyName, v);
        }

        public static ModifierKeys? GetModifiers(string propertyName)
        {
            try
            {
                return AWHKConfigShared.Configuration.LoadModKeys(propertyName);
            }
            catch (ConfigurationIoException)
            {
                return default(ModifierKeys?);
            }
        }

        public static void SetModifiers(string propertyName, ModifierKeys v)
        {
            AWHKConfigShared.Configuration.Store(propertyName, v);
        }

        public static HotKeyCombo GetKeyCombo(string propertyName)
        {
            try
            {
                KeyCombo kc = AWHKConfigShared.Configuration.LoadKeyCombo(propertyName);
                return new HotKeyCombo(kc.Modifiers, kc.Key);
            }
            catch (ConfigurationIoException)
            {
                return new HotKeyCombo();
            }
        }

        public static void SetKeyCombo(string propertyName, HotKeyCombo v)
        {
            if (propertyName == "")
                throw new MissingFieldException();

            KeyCombo kc = new KeyCombo(v.Modifiers, v.Trigger);
            AWHKConfigShared.Configuration.Store(propertyName, kc);
        }
    }

    public class ConfigurationView : INotifyPropertyChanged
    {
        #region Lazy auto-set auto-get magic
        Dictionary<string, HotKeyCombo> _hotKeyMappings = new Dictionary<string, HotKeyCombo>();
        Dictionary<string, ModifierKeys?> _modifierKeyMappings = new Dictionary<string, ModifierKeys?>();
        Dictionary<string, Key?> _keyMappings = new Dictionary<string, Key?>();
        Dictionary<string, bool?> _boolMappings = new Dictionary<string, bool?>();
        Dictionary<string, int?> _intMappings = new Dictionary<string, int?>();

        HotKeyCombo ConvertHotKeyCombo(KeyCombo kc)
        {
            return new HotKeyCombo(kc.Modifiers, kc.Key);
        }
        KeyCombo ConvertHotKeyCombo(HotKeyCombo kc)
        {
            return new KeyCombo(kc.Modifiers, kc.Trigger);
        }

        void SetHotKeyCombo(HotKeyCombo? value, [CallerMemberName] string propertyName = "") 
        {
            System.Diagnostics.Debug.Assert(propertyName != "");

            if (value.HasValue)
                _hotKeyMappings[propertyName] = ConvertHotKeyCombo(
                    Configuration.DefaultKeyCombo(propertyName));
            else
                _hotKeyMappings[propertyName] = value.Value;

            NotifyPropertyChanged(propertyName);
        }

        HotKeyCombo? GetHotKeyCombo([CallerMemberName] string propertyName = "")
        {
            System.Diagnostics.Debug.Assert(propertyName != "");

            if (_hotKeyMappings.ContainsKey(propertyName))
                return _hotKeyMappings[propertyName];
            else
                return default(HotKeyCombo);
        }

        void SetModifierKeys(ModifierKeys? value, [CallerMemberName] string propertyName = "")
        {
            System.Diagnostics.Debug.Assert(propertyName != "");

            if (!value.HasValue)
                _modifierKeyMappings[propertyName] = Configuration.DefaultModKeys(propertyName);
            else
                _modifierKeyMappings[propertyName] = value;

            NotifyPropertyChanged(propertyName);
        }

        ModifierKeys? GetModifierKeys([CallerMemberName] string propertyName = "")
        {
            System.Diagnostics.Debug.Assert(propertyName != "");

            if (_modifierKeyMappings.ContainsKey(propertyName))
                return _modifierKeyMappings[propertyName];
            else
                return default(ModifierKeys?);
        }

        void SetKey(Key? value, [CallerMemberName] string propertyName = "")
        {
            System.Diagnostics.Debug.Assert(propertyName != "");

            if (!value.HasValue)
                _keyMappings[propertyName] = Configuration.DefaultKey(propertyName);
            else
                _keyMappings[propertyName] = value;

            NotifyPropertyChanged(propertyName);
        }

        Key? GetKey([CallerMemberName] string propertyName = "")
        {
            System.Diagnostics.Debug.Assert(propertyName != "");

            if (_keyMappings.ContainsKey(propertyName))
                return _keyMappings[propertyName];
            else
                return default(Key?);
        }

        void SetBool(bool? value, [CallerMemberName] string propertyName = "")
        {
            System.Diagnostics.Debug.Assert(propertyName != "");

            if (!value.HasValue)
                _boolMappings[propertyName] = Configuration.DefaultBool(propertyName);
            else
                _boolMappings[propertyName] = value;

            NotifyPropertyChanged(propertyName);
        }

        bool? GetBool([CallerMemberName] string propertyName = "")
        {
            System.Diagnostics.Debug.Assert(propertyName != "");

            if (_boolMappings.ContainsKey(propertyName))
                return _boolMappings[propertyName];
            else
                return default(bool?);
        }

        void SetInt(int? value, [CallerMemberName] string propertyName = "")
        {
            System.Diagnostics.Debug.Assert(propertyName != "");

            if (!value.HasValue)
                _intMappings[propertyName] = Configuration.DefaultInt(propertyName);
            else
                _intMappings[propertyName] = value;

            NotifyPropertyChanged(propertyName);
        }

        int? GetInt([CallerMemberName] string propertyName = "")
        {
            System.Diagnostics.Debug.Assert(propertyName != "");

            if (_intMappings.ContainsKey(propertyName))
                return _intMappings[propertyName];
            else
                return default(int?);
        }
        #endregion

        // These are for conveniently displaying the move and resize ke
        public HotKeyCombo ResizeLeftView { get { return new HotKeyCombo(MoveMod.GetValueOrDefault(), ResizeLeft.GetValueOrDefault()); } }
        public HotKeyCombo ResizeRightView { get { return new HotKeyCombo(MoveMod.GetValueOrDefault(), ResizeRight.GetValueOrDefault()); } }
        public HotKeyCombo ResizeUpView { get { return new HotKeyCombo(MoveMod.GetValueOrDefault(), ResizeUp.GetValueOrDefault()); } }
        public HotKeyCombo ResizeDownView { get { return new HotKeyCombo(MoveMod.GetValueOrDefault(), ResizeDown.GetValueOrDefault()); } }
        public HotKeyCombo MoveLeftView { get { return new HotKeyCombo(MoveMod.GetValueOrDefault(), MoveLeft.GetValueOrDefault()); } }
        public HotKeyCombo MoveRightView { get { return new HotKeyCombo(MoveMod.GetValueOrDefault(), MoveRight.GetValueOrDefault()); } }
        public HotKeyCombo MoveUpView { get { return new HotKeyCombo(MoveMod.GetValueOrDefault(), MoveUp.GetValueOrDefault()); } }
        public HotKeyCombo MoveDownView { get { return new HotKeyCombo(MoveMod.GetValueOrDefault(), MoveDown.GetValueOrDefault()); } }
        
        // Explicit hotkeys
        public HotKeyCombo? HelpKey { get { return GetHotKeyCombo(); } set { SetHotKeyCombo(value); } }
        public HotKeyCombo? ConfigKey { get { return GetHotKeyCombo(); } set { SetHotKeyCombo(value); } }
        public HotKeyCombo? SessionSave0 { get { return GetHotKeyCombo(); } set { SetHotKeyCombo(value); } }
        public HotKeyCombo? SessionSave1 { get { return GetHotKeyCombo(); } set { SetHotKeyCombo(value); } }
        public HotKeyCombo? SessionSave2 { get { return GetHotKeyCombo(); } set { SetHotKeyCombo(value); } }
        public HotKeyCombo? SessionSave3 { get { return GetHotKeyCombo(); } set { SetHotKeyCombo(value); } }
        public HotKeyCombo? SessionSave4 { get { return GetHotKeyCombo(); } set { SetHotKeyCombo(value); } }
        public HotKeyCombo? SessionSave5 { get { return GetHotKeyCombo(); } set { SetHotKeyCombo(value); } }
        public HotKeyCombo? SessionSave6 { get { return GetHotKeyCombo(); } set { SetHotKeyCombo(value); } }
        public HotKeyCombo? SessionSave7 { get { return GetHotKeyCombo(); } set { SetHotKeyCombo(value); } }
        public HotKeyCombo? SessionSave8 { get { return GetHotKeyCombo(); } set { SetHotKeyCombo(value); } }
        public HotKeyCombo? SessionSave9 { get { return GetHotKeyCombo(); } set { SetHotKeyCombo(value); } }
        public HotKeyCombo? SessionLoad0 { get { return GetHotKeyCombo(); } set { SetHotKeyCombo(value); } }
        public HotKeyCombo? SessionLoad1 { get { return GetHotKeyCombo(); } set { SetHotKeyCombo(value); } }
        public HotKeyCombo? SessionLoad2 { get { return GetHotKeyCombo(); } set { SetHotKeyCombo(value); } }
        public HotKeyCombo? SessionLoad3 { get { return GetHotKeyCombo(); } set { SetHotKeyCombo(value); } }
        public HotKeyCombo? SessionLoad4 { get { return GetHotKeyCombo(); } set { SetHotKeyCombo(value); } }
        public HotKeyCombo? SessionLoad5 { get { return GetHotKeyCombo(); } set { SetHotKeyCombo(value); } }
        public HotKeyCombo? SessionLoad6 { get { return GetHotKeyCombo(); } set { SetHotKeyCombo(value); } }
        public HotKeyCombo? SessionLoad7 { get { return GetHotKeyCombo(); } set { SetHotKeyCombo(value); } }
        public HotKeyCombo? SessionLoad8 { get { return GetHotKeyCombo(); } set { SetHotKeyCombo(value); } }
        public HotKeyCombo? SessionLoad9 { get { return GetHotKeyCombo(); } set { SetHotKeyCombo(value); } }
        public HotKeyCombo? MediaPrev { get { return GetHotKeyCombo(); } set { SetHotKeyCombo(value); } }
        public HotKeyCombo? MediaNext { get { return GetHotKeyCombo(); } set { SetHotKeyCombo(value); } }
        public HotKeyCombo? MediaPlay { get { return GetHotKeyCombo(); } set { SetHotKeyCombo(value); } }
        public HotKeyCombo? MediaPause { get { return GetHotKeyCombo(); } set { SetHotKeyCombo(value); } }
        public HotKeyCombo? MediaPlayPause { get { return GetHotKeyCombo(); } set { SetHotKeyCombo(value); } }
        public HotKeyCombo? MediaVolumeUp { get { return GetHotKeyCombo(); } set { SetHotKeyCombo(value); } }
        public HotKeyCombo? MediaVolumeDown { get { return GetHotKeyCombo(); } set { SetHotKeyCombo(value); } }
        public Key? ResizeLeft { get { return GetKey(); } set { SetKey(value); } }
        public Key? ResizeRight { get { return GetKey(); } set { SetKey(value); } }
        public Key? ResizeUp { get { return GetKey(); } set { SetKey(value); } }
        public Key? ResizeDown { get { return GetKey(); } set { SetKey(value); } }
        public Key? MoveLeft { get { return GetKey(); } set { SetKey(value); } }
        public Key? MoveRight { get { return GetKey(); } set { SetKey(value); } }
        public Key? MoveUp { get { return GetKey(); } set { SetKey(value); } }
        public Key? MoveDown { get { return GetKey(); } set { SetKey(value); } }
        public ModifierKeys? MoveMod { get { return GetModifierKeys(); } set { SetModifierKeys(value); } }
        public ModifierKeys? NextMod { get { return GetModifierKeys(); } set { SetModifierKeys(value); } }
        public ModifierKeys? FineMod { get { return GetModifierKeys(); } set { SetModifierKeys(value); } }
        public bool? AllowSnapToOthers { get { return GetBool(); } set { SetBool(value); } }
        public int? GridX { get { return GetInt(); } set { SetInt(value); } }
        public int? GridY { get { return GetInt(); } set { SetInt(value); } }
        public int? FineX { get { return GetInt(); } set { SetInt(value); } }
        public int? FineY { get { return GetInt(); } set { SetInt(value); } }
        public bool RunOnStartUp { get; set; }

        public event PropertyChangedEventHandler PropertyChanged;

        // constructor creates default settings
        public ConfigurationView()
        {
        }

        // This method is called by the Set accessor of each property. 
        // The CallerMemberName attribute that is applied to the optional propertyName 
        // parameter causes the property name of the caller to be substituted as an argument. 
        private void NotifyPropertyChanged(String propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        // May throw an exception if properties fail validation
        public void Save()
        {
            // Save the settings:
            try
            {
                Configuration.AutoLogin = RunOnStartUp;

                foreach (var kvp in _hotKeyMappings)
                    ConfigurationProxy.SetKeyCombo(kvp.Key, kvp.Value);
                foreach (var kvp in _keyMappings)
                    ConfigurationProxy.SetKey(kvp.Key, kvp.Value.GetValueOrDefault());
                foreach (var kvp in _modifierKeyMappings)
                    ConfigurationProxy.SetModifiers(kvp.Key, kvp.Value.GetValueOrDefault());
                foreach (var kvp in _boolMappings)
                    ConfigurationProxy.SetBool(kvp.Key, kvp.Value.GetValueOrDefault());
                foreach (var kvp in _intMappings)
                    ConfigurationProxy.SetInt(kvp.Key, kvp.Value.GetValueOrDefault());
            }
            catch (ConfigurationIoException ex)
            {
                // Failed to save settings.
                throw new ConfigurationWriteException(ex);
            }

            // Force a refresh of all the settings
            NotifyPropertyChanged(String.Empty);
        }

        // Resets the configuration
        public void Load()
        {
            // Throw away our intermediate settings by clearing down the dictionaries
            _hotKeyMappings.Clear();
            _keyMappings.Clear();
            _modifierKeyMappings.Clear();
            _boolMappings.Clear();
            _intMappings.Clear();
            
            // Re-cache all the settings

            RunOnStartUp = Configuration.AutoLogin;

            foreach (var prop in GetType().GetProperties())
            {
                if (!prop.CanWrite)
                    continue;

                try
                {
                    if (prop.PropertyType == typeof(HotKeyCombo?))
                        prop.SetValue(this, ConfigurationProxy.GetKeyCombo(prop.Name));
                    else if (prop.PropertyType == typeof(ModifierKeys?))
                        prop.SetValue(this, ConfigurationProxy.GetModifiers(prop.Name));
                    else if (prop.PropertyType == typeof(Key?))
                        prop.SetValue(this, ConfigurationProxy.GetKey(prop.Name));
                    else if (prop.PropertyType == typeof(bool?))
                        prop.SetValue(this, ConfigurationProxy.GetBool(prop.Name));
                    else if (prop.PropertyType == typeof(int?))
                        prop.SetValue(this, ConfigurationProxy.GetInt(prop.Name));
                }
                catch (ConfigurationIoException)
                {
                    // Failed to read a setting - just ignore.
                }
            }

            // Notify all settings have changed
            NotifyPropertyChanged(String.Empty);
        }
    }
}
