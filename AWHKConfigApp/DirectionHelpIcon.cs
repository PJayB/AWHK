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
    public class DirectionHelpIcon : UserControl
    {
        public enum DirectionIcon
        {
            None,
            Left,
            Right,
            Up,
            Down
        };

        protected static readonly Dictionary<DirectionIcon, string> ExpandSymbolMap = new Dictionary<DirectionIcon, string>() 
        {
            { DirectionIcon.None, "\0" },
            { DirectionIcon.Left, "\xE29D" },
            { DirectionIcon.Right, "\xE29C" },
            { DirectionIcon.Up, "\xE11C" },
            { DirectionIcon.Down, "\xE118" }
        };

        private static readonly Dictionary<DirectionIcon, string> MoveSymbolMap = new Dictionary<DirectionIcon, string>() 
        {
            { DirectionIcon.None, "\0" },
            { DirectionIcon.Left, "\xE2B6" },
            { DirectionIcon.Right, "\xE2B7" },
            { DirectionIcon.Up, "\xE1FE" },
            { DirectionIcon.Down, "\xE1FC" }
        };

        public DirectionIcon? Direction
        {
            get { return base.GetValue(DirectionIconProperty) as DirectionIcon?; }
            set { base.SetValue(DirectionIconProperty, value); }
        }

        public string ExpandSymbol
        {
            get { return ExpandSymbolMap[Direction.GetValueOrDefault()]; }
        }

        public string ExpandInstruction
        {
            get { return "Resize " + Direction.ToString(); }
        }

        public string ExpandCombo
        {
            get { return base.GetValue(ExpandComboProperty) as string; }
            set { base.SetValue(ExpandComboProperty, value); }
        }

        public string MoveSymbol
        {
            get { return MoveSymbolMap[Direction.GetValueOrDefault()]; }
        }

        public string MoveInstruction
        {
            get { return "Move " + Direction.ToString(); }
        }

        public string MoveCombo
        {
            get { return base.GetValue(MoveComboProperty) as string; }
            set { base.SetValue(MoveComboProperty, value); }
        }

        public static readonly DependencyProperty DirectionIconProperty =
            DependencyProperty.Register("DirectionIcon", typeof(DirectionIcon), typeof(DirectionHelpIcon));
        public static readonly DependencyProperty ExpandComboProperty =
            DependencyProperty.Register("ExpandCombo", typeof(string), typeof(DirectionHelpIcon));
        public static readonly DependencyProperty MoveComboProperty =
            DependencyProperty.Register("MoveCombo", typeof(string), typeof(DirectionHelpIcon));
    }
}
