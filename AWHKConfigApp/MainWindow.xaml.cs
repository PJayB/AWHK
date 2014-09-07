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
        public MainWindow()
        {
            InitializeComponent();
        }

        private void btnUnload_Click(object sender, RoutedEventArgs e)
        {
            //todo 
            //unload the hawk service
        }

        private void btnOkay_Click(object sender, RoutedEventArgs e)
        {
            //todo
            //save the configuration
            //close the application
            this.Close();
        }

        private void btnApply_Click(object sender, RoutedEventArgs e)
        {
            //todo
            //save the configuration
        }

        private void btnCancel_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
    }
}
