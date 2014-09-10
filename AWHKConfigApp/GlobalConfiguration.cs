using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Windows.Input;

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

    public class KeyBindingView : INotifyPropertyChanged
    {
        private ModifierKeys _modifiers;
        private Key _trigger;

        public ModifierKeys Modifiers 
        {
            get { return _modifiers; }
            set
            {
                _modifiers = value;
                NotifyPropertyChanged();
            }
        }

        public Key Trigger 
        {
            get { return _trigger; }
            set
            {
                _trigger = value;
                NotifyPropertyChanged();
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

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

        public static KeyBindingView FromAwhk(AWHKConfigShared.KeyBinding key)
        {
            return new KeyBindingView()
            {
                Modifiers = (ModifierKeys) key.Modifiers,
                Trigger = KeyInterop.KeyFromVirtualKey(key.Trigger)
            };
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

        // General settings
        public bool RunOnStartUp
        {
            get { return _config.AutoLogin; }
            set { _config.AutoLogin = value; NotifyPropertyChanged(); }
        }

        // Resize/move hotkeys
        public KeyBindingView HelpKey
        {
            get { return KeyBindingView.FromAwhk(_config.HelpKey); }
            set { _config.HelpKey = value.ToAwhk(); }
        }
        public KeyBindingView ConfigKey
        {
            get { return KeyBindingView.FromAwhk(_config.ConfigKey); }
            set { _config.ConfigKey = value.ToAwhk(); }
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
            AWHKConfigShared.Configuration config = new AWHKConfigShared.Configuration();

            // TODO: translate the settings
            
            // Save the settings:
            try
            {
                config.Save();
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
