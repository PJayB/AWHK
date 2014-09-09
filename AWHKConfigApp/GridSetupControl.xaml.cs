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
    public class GridView : Control
    {
        public BitmapSource BackgroundImage
        {
            get { return base.GetValue(BackgroundImageProperty) as BitmapSource; }
            set { base.SetValue(BackgroundImageProperty, value); }
        }

        public static readonly DependencyProperty BackgroundImageProperty =
            DependencyProperty.Register("BackgroundImage", typeof(BitmapSource), typeof(GridSetupControl));

        public GridView()
        {
        }

        protected override void OnRender(DrawingContext dc)
        {
            base.OnRender(dc);

            Pen borderPen = new Pen(BorderBrush, 1.0);
            Rect drawRect = new Rect(0, 0, ActualWidth, ActualHeight);

            if (BackgroundImage != null)
            {
                // Get the border rect from our dimensions
                double imageAspect = BackgroundImage.Width / (double)BackgroundImage.Height;
                double actualAspect = ActualWidth / ActualHeight;
                if (actualAspect < 1)
                {
                    // Too thin. Vertically position.
                    double newImageHeight = ActualWidth / imageAspect;
                    drawRect.Y = (drawRect.Height - newImageHeight) * 0.5;
                    drawRect.Height = newImageHeight;
                }
                else
                {
                    // Too short. Horizontally position.
                    double newImageWidth = ActualHeight * imageAspect;
                    drawRect.X = (drawRect.Width - newImageWidth) * 0.5;
                    drawRect.Width = newImageWidth;
                }

                dc.DrawImage(BackgroundImage, drawRect);
            }

            dc.DrawRectangle(null, borderPen, new Rect(0, 0, ActualWidth, ActualHeight));
        }
    }

    /// <summary>
    /// Interaction logic for GridSetupControl.xaml
    /// </summary>
    public partial class GridSetupControl : UserControl
    {
        public List<int> Cols
        {
            get { return base.GetValue(ColsProperty) as List<int>; }
            set { base.SetValue(ColsProperty, value); }
        }

        public List<int> Rows
        {
            get { return base.GetValue(RowsProperty) as List<int>; }
            set { base.SetValue(RowsProperty, value); }
        }

        public int? NumCols
        {
            get { return base.GetValue(NumColsProperty) as int?; }
            set { base.SetValue(NumColsProperty, value); }
        }

        public int? NumRows
        {
            get { return base.GetValue(NumRowsProperty) as int?; }
            set { base.SetValue(NumRowsProperty, value); }
        }

        public static readonly DependencyProperty ColsProperty =
            DependencyProperty.Register("Cols", typeof(List<int>), typeof(GridSetupControl));
        public static readonly DependencyProperty RowsProperty =
            DependencyProperty.Register("Rows", typeof(List<int>), typeof(GridSetupControl));
        public static readonly DependencyProperty NumColsProperty =
            DependencyProperty.Register("NumCols", typeof(int?), typeof(GridSetupControl));
        public static readonly DependencyProperty NumRowsProperty =
            DependencyProperty.Register("NumRows", typeof(int?), typeof(GridSetupControl));
        
        public GridSetupControl()
        {
            InitializeComponent();
            border.BackgroundImage = new ScreenGrabber().Capture();
        }
    }
}
