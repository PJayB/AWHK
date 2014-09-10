using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using AWHKConfigShared;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Windows.Input;

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

    class ConfigurationView : INotifyPropertyChanged
    {
        private AWHKConfigShared.Configuration _config;

        private static ModifierKeys ConvertModifierSetToKeys(ModifierSet set)
        {
            ModifierKeys keys = ModifierKeys.None;
            if (set.Alt) keys |= ModifierKeys.Alt;
            if (set.Control) keys |= ModifierKeys.Control;
            if (set.Shift) keys |= ModifierKeys.Shift;
            if (set.Win) keys |= ModifierKeys.Windows;
            return keys;
        }

        private static ModifierSet ConvertModifierKeysToSet(ModifierKeys keys)
        {
            ModifierSet set = new ModifierSet();
            set.Alt = (keys & ModifierKeys.Alt) != 0;
            set.Control = (keys & ModifierKeys.Control) != 0;
            set.Shift = (keys & ModifierKeys.Shift) != 0;
            set.Win = (keys & ModifierKeys.Windows) != 0;
            return set;
        }

        // General settings
        public bool RunOnStartUp
        {
            get { return _config.AutoLogin; }
            set { _config.AutoLogin = value; NotifyPropertyChanged(); }
        }

        // Resize/move hotkeys
        public ModifierKeys FineSnapModifier
        {
            get { return ConvertModifierSetToKeys(_config.FineKeyMod); }
            set { _config.FineKeyMod = ConvertModifierKeysToSet(value); NotifyPropertyChanged(); }
        }
        public bool AllowResizeKeys
        {
            get { return _config.EnableResizeKeys; }
            set { _config.EnableResizeKeys = value; NotifyPropertyChanged(); }
        }
        public bool AllowMoveKeys
        {
            get { return _config.EnableMoveKeys; }
            set { _config.EnableMoveKeys = value; NotifyPropertyChanged(); }
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
            _config = new Configuration();
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
            Configuration config = new Configuration();

            // TODO: translate the settings
            config.AutoLogin = RunOnStartUp;
            
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
