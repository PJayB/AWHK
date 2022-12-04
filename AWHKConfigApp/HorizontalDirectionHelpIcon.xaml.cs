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
    /// <summary>
    /// Interaction logic for DirectionHelpIcon.xaml
    /// </summary>
    public partial class HorizontalDirectionHelpIcon : DirectionHelpIcon
    {
        private static readonly Dictionary<DirectionIcon, string> PinSymbolMap = new Dictionary<DirectionIcon, string>() 
        {
            { DirectionIcon.None, "\0" },
            { DirectionIcon.Left, "\xE176" },
            { DirectionIcon.Right, "\xE140" }
        };

        public HorizontalDirectionHelpIcon()
        {
            InitializeComponent();
        }

        public string PinSymbol
        {
            get { return PinSymbolMap[Direction.GetValueOrDefault()]; }
        }

        public string PinInstruction
        {
            get { return "Pin " + Direction.ToString(); }
        }

        public string PinCombo
        {
            get 
            {
                Key key = Key.None;
                switch (Direction)
                {
                    case DirectionIcon.Left: key = Key.Left; break;
                    case DirectionIcon.Right: key = Key.Right; break;
                }
                return ModifierKeySymbols.CreateSymbolString(ModifierKeys.Windows, key);
            }
        }

    }
}
