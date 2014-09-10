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

        private void FilterFineSnapModifierKeys()
        {
            ConfigurationView configView = DataContext as ConfigurationView;

            // Get the current selection
            ModifierKeys currentFineSnapKey = configView.FineSnapModifier;

            // Compile a new list of possibles
            ModifierKeys unavailableKeys = ModifierKeys.None;

            if (configView.AllowResizeKeys)
            {
                unavailableKeys |= hkResizeLeft.Modifiers;
                unavailableKeys |= hkResizeRight.Modifiers;
                unavailableKeys |= hkResizeDown.Modifiers;
                unavailableKeys |= hkResizeUp.Modifiers;
            }

            if (configView.AllowMoveKeys)
            {
                unavailableKeys |= hkMoveLeft.Modifiers;
                unavailableKeys |= hkMoveRight.Modifiers;
                unavailableKeys |= hkMoveDown.Modifiers;
                unavailableKeys |= hkMoveUp.Modifiers;
            }

            List<ModifierKeys> eligibleModifierKeys = new List<ModifierKeys>();
            foreach (var key in HotKeyCustomControlLibrary.ModifierKeyNameDataSource.Values)
            {
                // (I know these do the same thing but I'm being explicit)
                if (key == ModifierKeys.None || (unavailableKeys & key) == 0)
                {
                    eligibleModifierKeys.Add(key);
                }
                else if (key == currentFineSnapKey)
                {
                    // Damn, we overwrote our fine snap key when updating other hotkeys.
                    // We'll have to reset it.
                    cmbFineSnapModifier.SelectedItem = null;
                }
            }
            cmbFineSnapModifier.ItemsSource = eligibleModifierKeys;
        }

        private void Page_Loaded(object sender, RoutedEventArgs e)
        {
            FilterFineSnapModifierKeys();
        }

        private void HotKeyCommitted(object sender, RoutedEventArgs e)
        {
            FilterFineSnapModifierKeys();
        }

        private void chkEnableResizeKeys_CheckedChanged(object sender, RoutedEventArgs e)
        {
            FilterFineSnapModifierKeys();
        }

        private void chkEnableMoveKeys_CheckedChanged(object sender, RoutedEventArgs e)
        {
            FilterFineSnapModifierKeys();
        }

    }
}
