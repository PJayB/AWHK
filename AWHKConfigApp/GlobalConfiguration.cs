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

namespace AWHKConfigApp
{
    // Translate an AWHKShared exception to a write error
    class ConfigurationWriteException : Exception 
    {
        public ConfigurationWriteException(AWHKConfigShared.ConfigurationIoException inner) 
            : base(
                "Failed to save the configuration settings. Make sure you have permissions to write to the system registry.",
                inner)
        {
        }
    }

    // Translate an AWHKShared exception to a write error
    class ConfigurationReadException : Exception 
    {
        public ConfigurationReadException(AWHKConfigShared.ConfigurationIoException inner)
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

    public class ModifierKeysSymbolConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return ModifierKeySymbols.CreateSymbolString((ModifierKeys)value);
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is string)
            {
                return ModifierKeySymbols.GetKeysFromSymbols((string)value);
            }
            else if (value is ModifierKeys || value is Int32)
            {
                return (ModifierKeys)value;
            }
            else
            {
                throw new NotImplementedException();
            }
        }
    }

    public class ModifierKeysIntConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return (int)value;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return (ModifierKeys)value;
        }
    }

    public class KeyBindingViewSymbolConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            KeyBindingView keyView = value as KeyBindingView;
            return ModifierKeySymbols.CreateSymbolString(keyView.Modifiers, keyView.Trigger);
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }

    public class KeyBindingView
    {
        private AWHKConfigShared.Configuration _config;
        private System.Reflection.PropertyInfo _triggerProperty;
        private System.Reflection.PropertyInfo _modifiersProperty;

        public ModifierKeys Modifiers 
        {
            get 
            {
                return (ModifierKeys)_modifiersProperty.GetValue(_config);
            }
        }

        public Key Trigger 
        {
            get
            {
                return KeyInterop.KeyFromVirtualKey((int)_triggerProperty.GetValue(_config));
            }
        }

        public KeyBindingView(AWHKConfigShared.Configuration config, string triggerPropertyName, string modifierPropertyName)
        {
            _config = config;

            _triggerProperty = _config.GetType().GetProperty(triggerPropertyName);
            if (_triggerProperty == null)
            {
                throw new MissingMemberException("Property '" + triggerPropertyName + "' doesn't exist.");
            }

            _modifiersProperty = _config.GetType().GetProperty(modifierPropertyName);
            if (_modifiersProperty == null)
            {
                throw new MissingMemberException("Property '" + modifierPropertyName + "' doesn't exist.");
            }
        }
    }

    public class ConfigurationView : INotifyPropertyChanged
    {
        private AWHKConfigShared.Configuration _config;

        // Set any property
        public void SetConfig(string propertyName, object value)
        {
            var prop = this.GetType().GetProperty(propertyName);
            if (prop == null)
            {
                throw new MissingMemberException("Property '" + propertyName + "' doesn't exist.");
            }

            prop.SetValue(this, value, null);
        }

        // Does the configuration have a property?
        public bool HasConfig(string propertyName)
        {
            return this.GetType().GetProperty(propertyName) != null;
        }

        // Get any property
        public object GetConfig(string propertyName)
        {
            var prop = this.GetType().GetProperty(propertyName);
            if (prop == null)
            {
                throw new MissingMemberException("Property '" + propertyName + "' doesn't exist.");
            }

            return prop.GetValue(this, null);
        }

        // Uses magic to get any key based on the property name
        private Key GetTriggerKey([CallerMemberName] String propertyName = "")
        {
            // Get the property
            var prop = _config.GetType().GetProperty(propertyName);
            if (prop == null)
            {
                throw new MissingMethodException();
            }

            int vkey = (int)prop.GetValue(_config);
            return KeyInterop.KeyFromVirtualKey(vkey);
        }

        // Uses magic to get any modifiers based on the property name
        private ModifierKeys GetModifierKeys([CallerMemberName] String propertyName = "")
        {
            // Get the property
            var prop = _config.GetType().GetProperty(propertyName);
            if (prop == null)
            {
                throw new MissingMethodException();
            }

            return (ModifierKeys) prop.GetValue(_config);
        }

        // Uses magic to set any key based on the property name
        private void SetTriggerKey(Key key, [CallerMemberName] String propertyName = "")
        {
            // Get the property
            var prop = _config.GetType().GetProperty(propertyName);
            if (prop == null)
            {
                throw new MissingMethodException();
            }

            prop.SetValue(_config, KeyInterop.VirtualKeyFromKey(key));
            NotifyPropertyChanged(propertyName);
        }

        // Uses magic to set any modifiers based on the property name
        private void SetModifierKeys(ModifierKeys keys, [CallerMemberName] String propertyName = "")
        {
            // Get the property
            var prop = _config.GetType().GetProperty(propertyName);
            if (prop == null)
            {
                throw new MissingMethodException();
            }

            prop.SetValue(_config, (AWHKConfigShared.ModifierKeys) keys);
            NotifyPropertyChanged(propertyName);
        }

        // General settings
        public bool RunOnStartUp
        {
            get { return _config.AutoLogin; }
            set { _config.AutoLogin = value; NotifyPropertyChanged(); }
        }

        // Explicit hotkeys
        public Key HelpKey
        {
            get { return GetTriggerKey(); }
            set { SetTriggerKey(value); }
        }

        public ModifierKeys HelpKeyMod
        {
            get { return GetModifierKeys(); }
            set { SetModifierKeys(value); }
        }

        public Key ConfigKey
        {
            get { return GetTriggerKey(); }
            set { SetTriggerKey(value); }
        }
        
        public ModifierKeys ConfigKeyMod
        {
            get { return GetModifierKeys(); }
            set { SetModifierKeys(value); }
        }

        public KeyBindingView HelpKeyView { get; private set; }
        public KeyBindingView ConfigKeyView { get; private set; }

        // Trigger keys
        public Key ResizeLeft
        {
            get { return GetTriggerKey(); } 
            set { SetTriggerKey(value); }
        }
        public Key ResizeRight
        {
            get { return GetTriggerKey(); } 
            set { SetTriggerKey(value); }
        }
        public Key ResizeUp
        {
            get { return GetTriggerKey(); } 
            set { SetTriggerKey(value); }
        }
        public Key ResizeDown
        {
            get { return GetTriggerKey(); } 
            set { SetTriggerKey(value); }
        }
        public Key MoveLeft
        {
            get { return GetTriggerKey(); } 
            set { SetTriggerKey(value); }
        }
        public Key MoveRight
        {
            get { return GetTriggerKey(); } 
            set { SetTriggerKey(value); }
        }
        public Key MoveUp
        {
            get { return GetTriggerKey(); } 
            set { SetTriggerKey(value); }
        }
        public Key MoveDown
        {
            get { return GetTriggerKey(); }
            set { SetTriggerKey(value); }
        }

        // Modifier keys
        public ModifierKeys BaseModifierKeys
        {
            get { return (ModifierKeys) _config.BaseModifier; }
            set { _config.BaseModifier = (AWHKConfigShared.ModifierKeys)value; NotifyPropertyChanged(); }
        }

        public ModifierKeys FineSnapModifierKeys
        {
            get { return (ModifierKeys)_config.FineModifier; }
            set { _config.FineModifier = (AWHKConfigShared.ModifierKeys)value; NotifyPropertyChanged(); }
        }

        public ModifierKeys GrabModifierKeys
        {
            get { return (ModifierKeys)_config.GrabModifier; }
            set { _config.GrabModifier = (AWHKConfigShared.ModifierKeys)value; NotifyPropertyChanged(); }
        }

        // Properties for conveniently binding the key bindings

        
        // Grid settings
        public bool AllowSnapToOthers
        {
            get { return _config.AllowSnapToOthers; }
            set { _config.AllowSnapToOthers = value; NotifyPropertyChanged(); }
        }
        public bool EnableFineSnap
        {
            get { return _config.EnableFineSnap; }
            set { _config.EnableFineSnap = value; NotifyPropertyChanged(); }
        }
        public int CoarseGridCols
        {
            get { return _config.GridX; }
            set { _config.GridX = value; NotifyPropertyChanged(); }
        }
        public int CoarseGridRows
        {
            get { return _config.GridY; }
            set { _config.GridY = value; NotifyPropertyChanged(); }
        }
        public int FineGridCols
        {
            get { return _config.FineX; }
            set { _config.FineX = value; NotifyPropertyChanged(); }
        }
        public int FineGridRows
        {
            get { return _config.FineY; }
            set { _config.FineY = value; NotifyPropertyChanged(); }
        }

        // Session settings

        // Media hotkeys


        public event PropertyChangedEventHandler PropertyChanged;

        // constructor creates default settings
        public ConfigurationView()
        {
            _config = new AWHKConfigShared.Configuration();

            HelpKeyView = new KeyBindingView(_config, "HelpKey", "HelpKeyMod");
            ConfigKeyView = new KeyBindingView(_config, "ConfigKey", "ConfigKeyMod");
        }

        // This method is called by the Set accessor of each property. 
        // The CallerMemberName attribute that is applied to the optional propertyName 
        // parameter causes the property name of the caller to be substituted as an argument. 
        private void NotifyPropertyChanged([CallerMemberName] String propertyName = "")
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
                _config.Save();
            }
            catch (AWHKConfigShared.ConfigurationIoException ex)
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
            try
            {
                _config.Load();
            }
            catch (AWHKConfigShared.ConfigurationIoException ex)
            {
                // Failed to read settings.
                throw new ConfigurationReadException(ex);
            }

            // Notify all settings have changed
            NotifyPropertyChanged(String.Empty);
        }
    }
}
