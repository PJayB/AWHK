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

            // get the desktop dimensions
            // if you don't get the correct values then set it manually
            var rect = new Rectangle();
            GetWindowRect(desktopHwnd, ref rect);

            // saving the screenshot to a bitmap
            var bmp = new Bitmap(rect.Width, rect.Height);
            Graphics memoryGraphics = Graphics.FromImage(bmp);
            IntPtr dc = memoryGraphics.GetHdc();
            PrintWindow(desktopHwnd, dc, 0);
            memoryGraphics.ReleaseHdc(dc);

            return Imaging.CreateBitmapSourceFromHBitmap(
               bmp.GetHbitmap(), 
               IntPtr.Zero, 
               System.Windows.Int32Rect.Empty,
               BitmapSizeOptions.FromWidthAndHeight(rect.Width, rect.Height));
        }

        [DllImport("User32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool PrintWindow(IntPtr hwnd, IntPtr hdc, uint nFlags);

        [DllImport("user32.dll")]
        static extern bool GetWindowRect(IntPtr handle, ref Rectangle rect);

        [DllImport("user32.dll", EntryPoint = "GetDesktopWindow")]
        static extern IntPtr GetDesktopWindow();

        [DllImport("user32.dll", CharSet = CharSet.Unicode)]
        static extern IntPtr FindWindowEx(IntPtr parentHandle, IntPtr childAfter, string lclassName, string windowTitle);
    }
}
