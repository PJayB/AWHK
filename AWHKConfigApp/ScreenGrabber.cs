using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing; 
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using System.Windows.Interop;
using System.Windows.Media.Imaging;
using System.Windows;

namespace AWHKConfigApp
{
    // shameless copy-n-paste from: http://stackoverflow.com/questions/25500137/get-a-screenshot-of-desktop-in-full-screen-3d-application
    public class ScreenGrabber
    {
        public ScreenGrabber()
        {
        }

        public BitmapSource Capture()
        {
            // get the desktop window handle without the task bar
            // if you only use GetDesktopWindow() as the handle then you get and empty image
            IntPtr desktopHwnd = FindWindowEx(GetDesktopWindow(), IntPtr.Zero, "Progman", "Program Manager");

            // get the hwnd of this window
            IntPtr thisHwnd = new WindowInteropHelper(Application.Current.MainWindow).Handle;

            // get the entire desktop dimensions
            var desktopRect = new RECT();
            GetWindowRect(desktopHwnd, ref desktopRect);

            // which monitor is this window on?
            RECT monitorRect = desktopRect;
            IntPtr hThisMonitor = MonitorFromWindow(thisHwnd, (UInt32) MonitorDefault.ToPrimary);
            if (hThisMonitor != IntPtr.Zero)
            {
                // get the primary monitor dimensions
                MONITORINFO pmInfo = new MONITORINFO();
                pmInfo.cbSize = 40; // Hack?
                if (GetMonitorInfo(hThisMonitor, ref pmInfo))
                {
                    monitorRect = pmInfo.rcMonitor;
                }
            }

            // saving the screenshot to a bitmap
            var bmp = new Bitmap(desktopRect.Width, desktopRect.Height);
            Graphics memoryGraphics = Graphics.FromImage(bmp);
            IntPtr dc = memoryGraphics.GetHdc();
            PrintWindow(desktopHwnd, dc, 0);
            memoryGraphics.ReleaseHdc(dc);

            return Imaging.CreateBitmapSourceFromHBitmap(
               bmp.GetHbitmap(), 
               IntPtr.Zero, 
               new Int32Rect(
                   monitorRect.Left-desktopRect.Left, 
                   monitorRect.Top-desktopRect.Top, 
                   monitorRect.Width, 
                   monitorRect.Height),
               BitmapSizeOptions.FromWidthAndHeight(monitorRect.Width, monitorRect.Height));
        }

        struct RECT
        {
            public Int32 Left;
            public Int32 Top;
            public Int32 Right;
            public Int32 Bottom;

            public Int32 Width { get { return Right - Left; } }
            public Int32 Height { get { return Bottom - Top; } }
        };

        struct MONITORINFO
        {
            public UInt32 cbSize;
            public RECT rcMonitor;
            public RECT rcWork;
            public UInt32 dwFlags;
        };

        enum MonitorDefault : uint
        {
            ToNull = 0x0,
            ToPrimary = 0x1,
            ToNearest = 0x2
        };

        [DllImport("User32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool PrintWindow(IntPtr hwnd, IntPtr hdc, uint nFlags);

        [DllImport("user32.dll")]
        static extern bool GetWindowRect(IntPtr handle, ref RECT rect);

        [DllImport("user32.dll", EntryPoint = "GetDesktopWindow")]
        static extern IntPtr GetDesktopWindow();

        [DllImport("user32.dll", CharSet = CharSet.Unicode)]
        static extern IntPtr FindWindowEx(IntPtr parentHandle, IntPtr childAfter, string lclassName, string windowTitle);

        [DllImport("user32.dll", CharSet = CharSet.Unicode)]
        static extern bool GetMonitorInfo(IntPtr hMonitor, ref MONITORINFO pMonitorInfo);

        [DllImport("user32.dll", CharSet = CharSet.Unicode)]
        static extern IntPtr MonitorFromWindow(IntPtr hWnd, UInt32 dwFlags);
    }
}
