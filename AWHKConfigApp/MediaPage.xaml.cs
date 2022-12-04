﻿using System;
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
    /// Interaction logic for MediaPage.xaml
    /// </summary>
    public partial class MediaPage : Page
    {
        public MediaPage()
        {
            InitializeComponent();
        }

        private void HotKeyBox_LostFocus(object sender, RoutedEventArgs e)
        {
            try
            {
                new AWHKConfigShared.ServiceController().Resume();
            }
            catch (AWHKConfigShared.ServiceNotRunningException)
            {
                // We can ignore this.
            }
        }

        private void HotKeyBox_GotFocus(object sender, RoutedEventArgs e)
        {
            try
            {
                new AWHKConfigShared.ServiceController().Suspend();
            }
            catch (AWHKConfigShared.ServiceNotRunningException)
            {
                // We can ignore this.
            }
        }
    }
}