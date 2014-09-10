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
        private static readonly Dictionary<DirectionIcon, char> IconSymbolMap = new Dictionary<DirectionIcon, char>() 
        {
            { DirectionIcon.None, '\0' },
            { DirectionIcon.Left, '\xE29D' },
            { DirectionIcon.Right, '\xE29C' },
            { DirectionIcon.Up, '\xE11C' },
            { DirectionIcon.Down, '\xE118' }
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

        public char Symbol
        {
            get { return IconSymbolMap[Direction.GetValueOrDefault()]; }
        }

        public static readonly DependencyProperty DirectionIconProperty =
            DependencyProperty.Register("DirectionIcon", typeof(DirectionIcon), typeof(DirectionHelpIcon));


    }
}
