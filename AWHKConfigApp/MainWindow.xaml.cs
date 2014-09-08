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

            btnUnload.IsEnabled = _svcController.IsLoaded;
        }

        private void btnUnload_Click(object sender, RoutedEventArgs e)
        {
            // todo
            // unload the awhk service (_svcController.Unload())
                // you need to catch a AWHKConfigShared.ServiceNotRunningException 
                // if you catch one, print a message saying "Already unloaded" 
            // otherwise print a message saying "It has been unloaded"
                // and disable the button
        }

        private void btnOkay_Click(object sender, RoutedEventArgs e)
        {
            //todo
            //1. save the configuration (_config.Save())
            //2. tell awhk to reload its settings (_svcController.ReloadConfiguration())
                // you need to catch a AWHKConfigShared.ServiceNotRunningException but you can ignore it
            //3. close the application
            this.Close();
        }

        private void btnApply_Click(object sender, RoutedEventArgs e)
        {
            //todo
            //1. save the configuration (_config.Save())
            //2. tell awhk to reload its settings (_svcController.ReloadConfiguration())
                // you need to catch a AWHKConfigShared.ServiceNotRunningException
                // If you catch one on those, print a message
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
            btnUnload.IsEnabled = _svcController.IsLoaded;
        }

    }
}
