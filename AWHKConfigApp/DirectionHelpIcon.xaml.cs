using HotKeyCustomControlLibrary;
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
    public enum DirectionIcon
    {
        None,
        Left,
        Right,
        Up,
        Down
    };

    /// <summary>
    /// Interaction logic for DirectionHelpIcon.xaml
    /// </summary>
    public partial class DirectionHelpIcon : UserControl
    {
        private static readonly Dictionary<DirectionIcon, char> ExpandSymbolMap = new Dictionary<DirectionIcon, char>() 
        {
            { DirectionIcon.None, '\0' },
            { DirectionIcon.Left, '\xE150' },
            { DirectionIcon.Right, '\xE0D6' },
            { DirectionIcon.Up, '\xE11C' },
            { DirectionIcon.Down, '\xE118' }
        };

        private static readonly Dictionary<DirectionIcon, char> MoveSymbolMap = new Dictionary<DirectionIcon, char>() 
        {
            { DirectionIcon.None, '\0' },
            { DirectionIcon.Left, '\xE2B6' },
            { DirectionIcon.Right, '\xE2B7' },
            { DirectionIcon.Up, '\xE1FE' },
            { DirectionIcon.Down, '\xE1FC' }
        };

        public DirectionHelpIcon()
        {
            InitializeComponent();
        }

        public DirectionIcon? Direction
        {
            get { return base.GetValue(DirectionIconProperty) as DirectionIcon?; }
            set { base.SetValue(DirectionIconProperty, value); }
        }

        public char ExpandSymbol
        {
            get { return ExpandSymbolMap[Direction.GetValueOrDefault()]; }
        }

        public string ExpandInstruction
        {
            get { return "Resize " + Direction.ToString(); }
        }

        public string ExpandCombo
        {
            get 
            { 
                ConfigurationView config = (DataContext as ConfigurationView);
                if (config == null)
                    return "<ERROR>";
                return ModifierKeySymbols.CreateSymbolString(config.ResizeLeft.Modifiers, config.ResizeLeft.Trigger);
            }
        }

        public char MoveSymbol
        {
            get { return MoveSymbolMap[Direction.GetValueOrDefault()]; }
        }

        public string MoveInstruction
        {
            get { return "Move " + Direction.ToString(); }
        }

        public string MoveCombo
        {
            get
            {
                ConfigurationView config = (DataContext as ConfigurationView);
                if (config == null)
                    return "<ERROR>";
                return ModifierKeySymbols.CreateSymbolString(config.MoveLeft.Modifiers, config.MoveLeft.Trigger);
            }
        }

        public static readonly DependencyProperty DirectionIconProperty =
            DependencyProperty.Register("DirectionIcon", typeof(DirectionIcon), typeof(DirectionHelpIcon));


    }
}
