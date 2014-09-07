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

        // from: http://stackoverflow.com/questions/2136431/how-do-i-read-custom-keyboard-shortcut-from-user-in-wpf
        private static void HotKeyTextBoxPreviewKeyDown(TextBox textBox, KeyEventArgs e)
        {
            if (textBox == null)
                return;

            // The text box grabs all input.
            e.Handled = true;

            // Fetch the actual shortcut key.
            Key key = e.Key == Key.System ? e.SystemKey : e.Key;

            // Ignore key down events on modifier keys
            // TODO: do we really want to do this?
            if ( key == Key.LeftShift || key == Key.RightCtrl
                || key == Key.LeftCtrl || key == Key.RightCtrl
                || key == Key.LeftAlt || key == Key.RightAlt
                || key == Key.LWin || key == Key.RWin)
            {
                return;
            }

            // Build the shortcut key name.
            StringBuilder shortcutText = new StringBuilder();
            if ((Keyboard.Modifiers & ModifierKeys.Control) != 0)
            {
                shortcutText.Append("Ctrl+");
            }
            if ((Keyboard.Modifiers & ModifierKeys.Shift) != 0)
            {
                shortcutText.Append("Shift+");
            }
            if ((Keyboard.Modifiers & ModifierKeys.Alt) != 0)
            {
                shortcutText.Append("Alt+");
            }
            shortcutText.Append(key.ToString());

            textBox.Text = shortcutText.ToString();
        }

        private void TextBox_PreviewKeyDown_1(object sender, KeyEventArgs e)
        {
            HotKeyTextBoxPreviewKeyDown(sender as TextBox, e);
        }
    }
}
