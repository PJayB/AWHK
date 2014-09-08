using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace AWHKConfigApp
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private AWHKConfigShared.Configuration _config;
        private AWHKConfigShared.ServiceController _svcController;

        public MainWindow()
        {
            InitializeComponent();
        }

        private void SaveSettings()
        {
            // TODO: copy in the UI selections into _config.

            // Save the settings:
            try
            {
                _config.Save();
                _svcController.ReloadConfiguration();
            }
            catch (AWHKConfigShared.ConfigurationIoException)
            {
                // Failed to save settings.
                MessageBox.Show(
                    "Failed to save the settings. Ensure you have privileges to alter the Registry.",
                    "AWHK", MessageBoxButton.OK, MessageBoxImage.Exclamation);
            }
            catch (AWHKConfigShared.ServiceNotRunningException)
            {
                // Service not running.
            }
        }

        private void Window_Loaded_1(object sender, RoutedEventArgs e)
        {
            _svcController = new AWHKConfigShared.ServiceController();
            _config = new AWHKConfigShared.Configuration();

            // Load the configuration from the registry
            try
            {
                _config.Load();
            }
            catch (AWHKConfigShared.ConfigurationIoException)
            {
                // May not be set up yet.
            }

            // TODO: Update UI based on _config settings.

            // Disable the Unload button if the service isn't running
            btnUnload.IsEnabled = _svcController.IsLoaded;
        }

        private void btnUnload_Click(object sender, RoutedEventArgs e)
        {
            // todo
            // throw up a "Are you sure?" box:
            // if yes:
                // unload the awhk service (_svcController.Unload())
                    // you need to catch a AWHKConfigShared.ServiceNotRunningException 
                    // if you catch one, print a message saying "Already unloaded" 
                // otherwise print a message saying "It has been unloaded"
                    // and disable the button
            // if no:
                // do nothing
        }

        private void btnOkay_Click(object sender, RoutedEventArgs e)
        {
            SaveSettings();
            this.Close();
        }

        private void btnApply_Click(object sender, RoutedEventArgs e)
        {
            SaveSettings();
        }

        private void btnCancel_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private void HotKeyBox_LostFocus_1(object sender, RoutedEventArgs e)
        {
            try
            {
                _svcController.Resume();
            }
            catch (AWHKConfigShared.ServiceNotRunningException)
            {
                // We can ignore this.
            }
        }

        private void HotKeyBox_GotFocus_1(object sender, RoutedEventArgs e)
        {
            try
            {
                _svcController.Suspend();
            }
            catch (AWHKConfigShared.ServiceNotRunningException)
            {
                // We can ignore this.
            }
        }

        private void Window_GotFocus_1(object sender, RoutedEventArgs e)
        {
            // Update the status of the service each time we get focus (just in case
            // the user starts or stops it while we're loaded)
            btnUnload.IsEnabled = _svcController.IsLoaded;
        }

    }
}
