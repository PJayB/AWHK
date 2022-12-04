using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using AWHKConfigShared;
using HotKeyCustomControlLibrary;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Interop;

namespace AWHKConfigApp
{
    class HotkeyValidator
    {
        private static ServiceController _svc = new ServiceController();

        private static void Suspend()
        {
            try
            {
                _svc.Resume();
            }
            catch (AWHKConfigShared.ServiceNotRunningException)
            {
                // We can ignore this.
            }
        }

        private static void Resume()
        {
            try
            {
                _svc.Suspend();
            }
            catch (AWHKConfigShared.ServiceNotRunningException)
            {
                // We can ignore this.
            }
        }

        public static void GotFocus(HotKeyBox hkBox)
        {
            Suspend();
        }

        public static void LostFocus(HotKeyBox hkBox)
        {
            Resume();
        }

        public static void Committed(HotKeyBox hkBox)
        {
            if (!hkBox.Trigger.HasValue ||
                 !hkBox.Modifiers.HasValue)
            {
                hkBox.Error = false;
                return;
            }

            KeyCombo keys = new KeyCombo()
            {
                Key = hkBox.Trigger.Value,
                Modifiers = hkBox.Modifiers.Value
            };

            Window win = Window.GetWindow(hkBox);
            WindowInteropHelper wih = new WindowInteropHelper(win);

            hkBox.Error = !Configuration.Test(wih.Handle, 0, keys);
        }
    }
}
