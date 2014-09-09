using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
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
        public Brush DisabledOverlayBrush
        {
            get { return base.GetValue(DisabledOverlayBrushProperty) as Brush; }
            set { base.SetValue(DisabledOverlayBrushProperty, value); InvalidateVisual(); }
        }

        public BitmapSource BackgroundImage
        {
            get { return base.GetValue(BackgroundImageProperty) as BitmapSource; }
            set { base.SetValue(BackgroundImageProperty, value); InvalidateVisual(); }
        }

        public int? NumCols
        {
            get { return base.GetValue(NumColsProperty) as int?; }
            set { base.SetValue(NumColsProperty, value); InvalidateVisual(); }
        }

        public int? NumRows
        {
            get { return base.GetValue(NumRowsProperty) as int?; }
            set { base.SetValue(NumRowsProperty, value); InvalidateVisual(); }
        }

        public static readonly DependencyProperty DisabledOverlayBrushProperty =
            DependencyProperty.Register("DisabledOverlayBrush", typeof(Brush), typeof(GridView),
                new FrameworkPropertyMetadata(new SolidColorBrush(
                    new Color() { R = 192, G = 192, B = 192, A = 128 }
                    )
                )
            );
        public static readonly DependencyProperty BackgroundImageProperty =
            DependencyProperty.Register("BackgroundImage", typeof(BitmapSource), typeof(GridView));
        public static readonly DependencyProperty NumColsProperty =
            DependencyProperty.Register("NumCols", typeof(int?), typeof(GridView));
        public static readonly DependencyProperty NumRowsProperty =
            DependencyProperty.Register("NumRows", typeof(int?), typeof(GridView));

        public GridView()
        {
            this.IsEnabledChanged += GridView_IsEnabledChanged;
        }

        void GridView_IsEnabledChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            InvalidateVisual();
        }

        protected override void OnRender(DrawingContext dc)
        {
            base.OnRender(dc);

            Pen borderPen = new Pen(BorderBrush, 1.0);
            Pen gridLinePen = new Pen(BorderBrush, 1.0) { DashStyle = DashStyles.Dash };

            Rect drawRect = new Rect(0, 0, ActualWidth, ActualHeight);

            if (BackgroundImage != null)
            {
                double xscale = ActualWidth / (double) BackgroundImage.Width;
                double yscale = ActualHeight / (double) BackgroundImage.Height;

                drawRect.Width = Math.Min(ActualWidth, BackgroundImage.Width * yscale);
                drawRect.Height = Math.Min(ActualHeight, BackgroundImage.Height * xscale);

                // Center it
                if (drawRect.Width < ActualWidth)
                {
                    drawRect.X = (ActualWidth - drawRect.Width) * 0.5;
                }
                if (drawRect.Height < ActualHeight)
                {
                    drawRect.Y = (ActualHeight - drawRect.Height) * 0.5;
                }

                dc.DrawImage(BackgroundImage, drawRect);

                // Draw a translucent rect over it if it's disabled
                if (!IsEnabled)
                {
                    dc.DrawRectangle(DisabledOverlayBrush, null, drawRect);
                }
            }

            // Draw horizontal lines
            if (NumRows.HasValue && NumRows.Value > 1)
            {
                for (int i = 1; i < NumRows.Value; ++i)
                {
                    double y = drawRect.Y + i * drawRect.Height / NumRows.Value;
                    dc.DrawLine(
                        gridLinePen,
                        new Point(drawRect.Left, y),
                        new Point(drawRect.Right, y));
                }
            }

            // Draw vertical lines
            if (NumCols.HasValue && NumCols.Value > 1)
            {
                for (int i = 1; i < NumCols.Value; ++i)
                {
                    double x = drawRect.X + i * drawRect.Width / NumCols.Value;
                    dc.DrawLine(
                        gridLinePen,
                        new Point(x, drawRect.Top),
                        new Point(x, drawRect.Bottom));
                }
            }

            dc.DrawRectangle(null, borderPen, drawRect);
        }
    }

    /// <summary>
    /// Interaction logic for GridSetupControl.xaml
    /// </summary>
    public partial class GridSetupControl : UserControl
    {
        public ObservableCollection<int> Cols
        {
            get { return base.GetValue(ColsProperty) as ObservableCollection<int>; }
            set { base.SetValue(ColsProperty, value); }
        }

        public ObservableCollection<int> Rows
        {
            get { return base.GetValue(RowsProperty) as ObservableCollection<int>; }
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

        private static readonly DependencyPropertyKey ColsPropertyKey =
            DependencyProperty.RegisterReadOnly(
                "Cols",
                typeof(ObservableCollection<int>), 
                typeof(GridSetupControl),
                new FrameworkPropertyMetadata(new ObservableCollection<int>()));
        public static readonly DependencyProperty ColsProperty = ColsPropertyKey.DependencyProperty;
        
        private static readonly DependencyPropertyKey RowsPropertyKey =
            DependencyProperty.RegisterReadOnly(
                "Rows",
                typeof(ObservableCollection<int>), 
                typeof(GridSetupControl),
                new FrameworkPropertyMetadata(new ObservableCollection<int>()));
        public static readonly DependencyProperty RowsProperty = RowsPropertyKey.DependencyProperty;

        public static readonly DependencyProperty NumColsProperty =
            DependencyProperty.Register("NumCols", typeof(int?), typeof(GridSetupControl));
        public static readonly DependencyProperty NumRowsProperty =
            DependencyProperty.Register("NumRows", typeof(int?), typeof(GridSetupControl));
        
        public GridSetupControl()
        {
            InitializeComponent();

            SetValue(ColsPropertyKey, new ObservableCollection<int>());
            SetValue(RowsPropertyKey, new ObservableCollection<int>()); 

            border.BackgroundImage = new ScreenGrabber().Capture();
        }
    }
}
