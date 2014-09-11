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

    public class KeyBindingView : INotifyPropertyChanged
    {
        private AWHKConfigShared.Configuration _config;
        private System.Reflection.PropertyInfo _property;

        public ModifierKeys Modifiers 
        {
            get 
            {
                return (ModifierKeys) GetBindingSourceCopy().Modifiers; 
            }
            set
            {
                AWHKConfigShared.KeyBinding sourceBinding = GetBindingSourceCopy();
                sourceBinding.Modifiers = (AWHKConfigShared.ModifierKeys) value;
                SetBindingSource(sourceBinding);
                NotifyPropertyChanged();
            }
        }

        public Key Trigger 
        {
            get
            {
                return KeyInterop.KeyFromVirtualKey(GetBindingSourceCopy().Trigger);
            }
            set
            {
                AWHKConfigShared.KeyBinding sourceBinding = GetBindingSourceCopy();
                sourceBinding.Trigger = KeyInterop.VirtualKeyFromKey(value);
                SetBindingSource(sourceBinding);
                NotifyPropertyChanged();
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public KeyBindingView(AWHKConfigShared.Configuration config, string propertyName)
        {
            _config = config;

            _property = _config.GetType().GetProperty(propertyName);
            if (_property == null)
            {
                throw new MissingMemberException("Property '" + propertyName + "' doesn't exist.");
            }
        }

        private AWHKConfigShared.KeyBinding GetBindingSourceCopy()
        {
            return (AWHKConfigShared.KeyBinding)_property.GetValue(_config);
        }

        private void SetBindingSource(AWHKConfigShared.KeyBinding binding)
        {
            _property.SetValue(_config, binding);
        }

        // This method is called by the Set accessor of each property. 
        // The CallerMemberName attribute that is applied to the optional propertyName 
        // parameter causes the property name of the caller to be substituted as an argument. 
        private void NotifyPropertyChanged([CallerMemberName] String propertyName = "")
        {
            // Update the XAML dependency chain
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        public AWHKConfigShared.KeyBinding ToAwhk()
        {
            return new AWHKConfigShared.KeyBinding()
            {
                Modifiers = (AWHKConfigShared.ModifierKeys) this.Modifiers,
                Trigger = KeyInterop.VirtualKeyFromKey(this.Trigger)
            };
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

        // General settings
        public bool RunOnStartUp
        {
            get { return _config.AutoLogin; }
            set { _config.AutoLogin = value; NotifyPropertyChanged(); }
        }

        // Resize/move hotkeys
        public KeyBindingView HelpKey { get; private set; }
        public KeyBindingView ConfigKey { get; private set; } 
        public KeyBindingView ResizeLeft { get; private set; }
        public KeyBindingView ResizeRight { get; private set; }
        public KeyBindingView ResizeUp { get; private set; }
        public KeyBindingView ResizeDown { get; private set; }
        public KeyBindingView MoveLeft { get; private set; }
        public KeyBindingView MoveRight { get; private set; }
        public KeyBindingView MoveUp { get; private set; }
        public KeyBindingView MoveDown { get; private set; }

        public ModifierKeys BaseModifierKeys
        {
            get
            {
                return ResizeLeft.Modifiers;
            }
            set
            {
                ResizeLeft.Modifiers = value;
                ResizeRight.Modifiers = value;
                ResizeUp.Modifiers = value;
                ResizeDown.Modifiers = value;
                MoveLeft.Modifiers = value;
                MoveRight.Modifiers = value;
                MoveUp.Modifiers = value;
                MoveDown.Modifiers = value;
            }
        }

        // TODO
        public ModifierKeys FineSnapModifierKeys
        {
            get
            {
                return ResizeLeft.Modifiers;
            }
            set
            {
                ResizeLeft.Modifiers = value;
                ResizeRight.Modifiers = value;
                ResizeUp.Modifiers = value;
                ResizeDown.Modifiers = value;
                MoveLeft.Modifiers = value;
                MoveRight.Modifiers = value;
                MoveUp.Modifiers = value;
                MoveDown.Modifiers = value;
            }
        }

        // TODO
        public ModifierKeys GrabModifierKeys
        {
            get
            {
                return ResizeLeft.Modifiers;
            }
            set
            {
                ResizeLeft.Modifiers = value;
                ResizeRight.Modifiers = value;
                ResizeUp.Modifiers = value;
                ResizeDown.Modifiers = value;
                MoveLeft.Modifiers = value;
                MoveRight.Modifiers = value;
                MoveUp.Modifiers = value;
                MoveDown.Modifiers = value;
            }
        }

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

            HelpKey = new KeyBindingView(_config, "HelpKey");
            ConfigKey = new KeyBindingView(_config, "ConfigKey");
            ResizeLeft = new KeyBindingView(_config, "ResizeLeft");
            ResizeRight = new KeyBindingView(_config, "ResizeRight");
            ResizeUp = new KeyBindingView(_config, "ResizeUp");
            ResizeDown = new KeyBindingView(_config, "ResizeDown");
            MoveLeft = new KeyBindingView(_config, "MoveLeft");
            MoveRight = new KeyBindingView(_config, "MoveRight");
            MoveUp = new KeyBindingView(_config, "MoveUp");
            MoveDown = new KeyBindingView(_config, "MoveDown");
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
