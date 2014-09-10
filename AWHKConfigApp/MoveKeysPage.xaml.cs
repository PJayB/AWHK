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
    /// Interaction logic for MoveKeysPage.xaml
    /// </summary>
    public partial class MoveKeysPage : Page
    {
        public MoveKeysPage()
        {
            InitializeComponent();
        }

        private void Hyperlink_Click(object sender, RoutedEventArgs e)
        {
            MessageBox.Show((sender as Hyperlink).TargetName);
        }

        private void Page_Loaded(object sender, RoutedEventArgs e)
        {

        }

        private void HotKeyCommitted(object sender, RoutedEventArgs e)
        {
            //ConfigurationView config = DataContext as ConfigurationView;
            //
            //HotKeyCustomControlLibrary.HotKeyBox hk = sender as HotKeyCustomControlLibrary.HotKeyBox;
            //MessageBox.Show(String.Format("{0} {1} -> {2} {3}", 
            //    hk.Modifiers.ToString(), (int) hk.Trigger,
            //    config.ConfigKey.Modifiers, (int) config.ConfigKey.Trigger));
        }
    }
}
