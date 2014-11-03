using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Markup;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace HotKeyCustomControlLibrary
{
    public static class ModifierKeyNameDataSource
    {
        public static IEnumerable<ModifierKeys> Values
        {
            get
            {
                return Enum.GetValues(typeof(ModifierKeys)).Cast<ModifierKeys>();
            }
        }
    }

    public static class ModifierKeySymbols
    {
        // TODO: localization?
        public const string WindowsKey = "\xE154";
        public const string ShiftKey = "\x21D1";
        public const string ControlKey = "Ctrl";
        public const string AltKey = "Alt";
        public const string NoKeyName = "None";

        public static string CreateSymbolString(ModifierKeys keys)
        {
            string[] symbols = new string[4];
            int currentSymbol = 0;

            if ((keys & ModifierKeys.Windows) != 0) symbols[currentSymbol++] = ModifierKeySymbols.WindowsKey;
            if ((keys & ModifierKeys.Shift) != 0) symbols[currentSymbol++] = ModifierKeySymbols.ShiftKey;
            if ((keys & ModifierKeys.Control) != 0) symbols[currentSymbol++] = ModifierKeySymbols.ControlKey;
            if ((keys & ModifierKeys.Alt) != 0) symbols[currentSymbol++] = ModifierKeySymbols.AltKey;

            if (currentSymbol == 0)
            {
                return NoKeyName;
            }

            StringBuilder sb = new StringBuilder();
            for (int i = 0; i < currentSymbol; ++i)
            {
                if (i != 0) { sb.Append(" + "); }
                sb.Append(symbols[i]);
            }
            return sb.ToString();
        }

        public static string CreateSymbolString(ModifierKeys keys, Key trigger)
        {
            string[] symbols = new string[5];
            int currentSymbol = 0;

            if ((keys & ModifierKeys.Windows) != 0) symbols[currentSymbol++] = ModifierKeySymbols.WindowsKey;
            if ((keys & ModifierKeys.Shift) != 0) symbols[currentSymbol++] = ModifierKeySymbols.ShiftKey;
            if ((keys & ModifierKeys.Control) != 0) symbols[currentSymbol++] = ModifierKeySymbols.ControlKey;
            if ((keys & ModifierKeys.Alt) != 0) symbols[currentSymbol++] = ModifierKeySymbols.AltKey;
            if (trigger != Key.None) symbols[currentSymbol++] = trigger.ToString();

            if (currentSymbol == 0)
            {
                return NoKeyName;
            }

            StringBuilder sb = new StringBuilder();
            for (int i = 0; i < currentSymbol; ++i)
            {
                if (i != 0) { sb.Append(" + "); }
                sb.Append(symbols[i]);
            }
            return sb.ToString();
        }

        public static ModifierKeys GetKeysFromSymbols(string value)
        {
            string[] symbols = value.Split(new char[] { ' ', '+' });

            ModifierKeys keys = ModifierKeys.None;
            foreach (string sym in symbols)
            {
                switch (sym)
                {
                    case ModifierKeySymbols.AltKey: keys |= ModifierKeys.Alt; break;
                    case ModifierKeySymbols.ControlKey: keys |= ModifierKeys.Control; break;
                    case ModifierKeySymbols.ShiftKey: keys |= ModifierKeys.Shift; break;
                    case ModifierKeySymbols.WindowsKey: keys |= ModifierKeys.Windows; break;
                }
            }

            return keys;
        }
    }

    public static class ModifierKeyState
    {
        public static ModifierKeys Set(ModifierKeys source, ModifierKeys value, bool enabled)
        {
            if (enabled) return source | value;
            else return source & (~value);
        }

        public static ModifierKeys? Set(ModifierKeys? source, ModifierKeys value, bool enabled)
        {
            if (enabled) return source.GetValueOrDefault() | value;
            else if (!source.HasValue) return source;
            else return source.GetValueOrDefault() & (~value);
        }
    }

    public class ModifierKeysIntConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return (int)value;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return (ModifierKeys)value;
        }
    }

    public class ModifierKeysSymbolConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return ModifierKeySymbols.CreateSymbolString((ModifierKeys)value);
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is string)
            {
                return ModifierKeySymbols.GetKeysFromSymbols((string)value);
            }
            else if (value is ModifierKeys || value is Int32)
            {
                return (ModifierKeys)value;
            }
            else
            {
                throw new NotImplementedException();
            }
        }
    }

    public class HotKeyComboSymbolConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            HotKeyCombo keyView = (HotKeyCombo)value;
            return ModifierKeySymbols.CreateSymbolString(
                keyView.Modifiers, 
                keyView.Trigger);
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }


    public struct HotKeyCombo
    {
        public HotKeyCombo(ModifierKeys mods, Key trigger)
        {
            Trigger = trigger;
            Modifiers = mods;
        }

        public Key Trigger;
        public ModifierKeys Modifiers;
    }

    public class HotKeyBox : Control
    {
        private TextBox displayBox;
        private bool hasFocus;
        private Key? previewTrigger;

        public HotKeyCombo? KeyCombo
        {
            get { return base.GetValue(KeyComboProperty) as HotKeyCombo?; }
            set { base.SetValue(KeyComboProperty, value); }
        }

        public ModifierKeys? DisabledModifiers
        {
            get { return base.GetValue(DisabledModifiersProperty) as ModifierKeys?; }
            set { base.SetValue(DisabledModifiersProperty, value); }
        }

        public ModifierKeys? Modifiers
        {
            get { return KeyCombo.HasValue ? KeyCombo.Value.Modifiers : default(ModifierKeys?); }
            set
            {
                if (KeyCombo != null || value.HasValue)
                {
                    KeyCombo = new HotKeyCombo(
                        value.GetValueOrDefault(),
                        Trigger.GetValueOrDefault());
                }
            }
        }

        public Key? Trigger
        {
            get { return KeyCombo != null ? KeyCombo.Value.Trigger : default(Key?); }
            set
            {
                if (KeyCombo != null || value.HasValue)
                {
                    KeyCombo = new HotKeyCombo(
                        Modifiers.GetValueOrDefault(),
                        value.GetValueOrDefault());
                }
            }
        }
        public bool HasAlt
        {
            get { return (Modifiers.GetValueOrDefault() & ModifierKeys.Alt) != 0; }
            set { Modifiers = ModifierKeyState.Set(Modifiers, ModifierKeys.Alt, value); }
        }

        public bool HasControl
        {
            get { return (Modifiers.GetValueOrDefault() & ModifierKeys.Control) != 0; }
            set { Modifiers = ModifierKeyState.Set(Modifiers, ModifierKeys.Control, value); }
        }

        public bool HasShift
        {
            get { return (Modifiers.GetValueOrDefault() & ModifierKeys.Shift) != 0; }
            set { Modifiers = ModifierKeyState.Set(Modifiers, ModifierKeys.Shift, value); }
        }

        public bool HasWindows
        {
            get { return (Modifiers.GetValueOrDefault() & ModifierKeys.Windows) != 0; }
            set { Modifiers = ModifierKeyState.Set(Modifiers, ModifierKeys.Windows, value); }
        }

        public bool DisableAlt
        {
            get { return (DisabledModifiers.GetValueOrDefault() & ModifierKeys.Alt) != 0; }
            set { DisabledModifiers = ModifierKeyState.Set(DisabledModifiers, ModifierKeys.Alt, value); }
        }

        public bool DisableControl
        {
            get { return (DisabledModifiers.GetValueOrDefault() & ModifierKeys.Control) != 0; }
            set { DisabledModifiers = ModifierKeyState.Set(DisabledModifiers, ModifierKeys.Control, value); }
        }

        public bool DisableShift
        {
            get { return (DisabledModifiers.GetValueOrDefault() & ModifierKeys.Shift) != 0; }
            set { DisabledModifiers = ModifierKeyState.Set(DisabledModifiers, ModifierKeys.Shift, value); }
        }

        public bool DisableWindows
        {
            get { return (DisabledModifiers.GetValueOrDefault() & ModifierKeys.Windows) != 0; }
            set { DisabledModifiers = ModifierKeyState.Set(DisabledModifiers, ModifierKeys.Windows, value); }
        }

        public event RoutedEventHandler HotKeyCommitted
        {
            add { AddHandler(CommitEvent, value); }
            remove { RemoveHandler(CommitEvent, value); }
        }

        public int VirtualKey
        {
            get { return KeyCombo.HasValue ? KeyInterop.VirtualKeyFromKey(KeyCombo.Value.Trigger) : 0; }
        }

        [TypeConverter(typeof(HotKeyComboSymbolConverter))]
        public static readonly DependencyProperty KeyComboProperty =
            DependencyProperty.Register("KeyCombo", typeof(HotKeyCombo), typeof(HotKeyBox));

        public static readonly DependencyProperty DisabledModifiersProperty =
            DependencyProperty.Register("DisabledModifiers", typeof(ModifierKeys?), typeof(HotKeyBox));

        public static readonly RoutedEvent CommitEvent = EventManager.RegisterRoutedEvent(
            "HotKeyCommitted", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(HotKeyBox));

        static HotKeyBox()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(HotKeyBox), new FrameworkPropertyMetadata(typeof(HotKeyBox)));
        }

        public HotKeyBox()
        {
        }

        public override void OnApplyTemplate()
        {
            base.OnApplyTemplate();

            displayBox = GetTemplateChild("txtDisplayBox") as TextBox;
            if (displayBox != null)
            {
                displayBox.LostKeyboardFocus += new System.Windows.Input.KeyboardFocusChangedEventHandler(OnLostKeyboardFocus);
            }

            this.PreviewKeyDown += new KeyEventHandler(OnPreviewKeyDown);
            this.PreviewKeyUp += new KeyEventHandler(OnPreviewKeyUp);
            this.GotKeyboardFocus += new KeyboardFocusChangedEventHandler(OnKeyboardGotFocus);

            UpdateDisplay();
        }

        private void OnKeyboardGotFocus(object sender, KeyboardFocusChangedEventArgs e)
        {
            Keyboard.Focus(displayBox);
            hasFocus = true;
            previewTrigger = null;
        }

        private void OnLostKeyboardFocus(object sender, System.Windows.Input.KeyboardFocusChangedEventArgs e)
        {
            hasFocus = false;
            previewTrigger = null;
        }

        private bool IsAnyModKeyDown()
        {
            return Keyboard.IsKeyDown(Key.LeftShift) || Keyboard.IsKeyDown(Key.RightShift) ||
                   Keyboard.IsKeyDown(Key.LeftCtrl) || Keyboard.IsKeyDown(Key.RightCtrl) ||
                   Keyboard.IsKeyDown(Key.LeftAlt) || Keyboard.IsKeyDown(Key.RightAlt) ||
                   Keyboard.IsKeyDown(Key.LWin) || Keyboard.IsKeyDown(Key.RWin);
        }

        private bool IsShiftKeyDown()
        {
            return Keyboard.IsKeyDown(Key.LeftShift) || Keyboard.IsKeyDown(Key.RightShift);
        }

        private bool IsControlKeyDown()
        {
            return Keyboard.IsKeyDown(Key.LeftCtrl) || Keyboard.IsKeyDown(Key.RightCtrl);
        }

        private bool IsAltKeyDown()
        {
            return Keyboard.IsKeyDown(Key.LeftAlt) || Keyboard.IsKeyDown(Key.RightAlt);
        }

        private bool IsWindowsKeyDown()
        {
            return Keyboard.IsKeyDown(Key.LWin) || Keyboard.IsKeyDown(Key.RWin);
        }

        private void CommitHotKey(Key k)
        {
            ModifierKeys mods = ModifierKeys.None;
          
            if (ShiftIsAllowed() && IsShiftKeyDown())
                mods |= ModifierKeys.Shift;
            if (ControlIsAllowed() && IsControlKeyDown())
                mods |= ModifierKeys.Control;
            if (AltIsAllowed() && IsAltKeyDown())
                mods |= ModifierKeys.Alt;
            if (WindowsIsAllowed() && IsWindowsKeyDown())
                mods |= ModifierKeys.Windows;

            Modifiers = mods;
            Trigger = k;

            // Raise the event
            RaiseEvent(new RoutedEventArgs(CommitEvent));
        }

        private void OnPreviewKeyDown(object sender, KeyEventArgs e)
        {
            Key key = (e.Key == Key.System ? e.SystemKey : e.Key);

            // Don't handle tabs
            if (key == Key.Tab)
                return;

            bool isShift = (key == Key.LeftShift || key == Key.RightShift);
            bool isCtrl = (key == Key.LeftCtrl || key == Key.RightCtrl);
            bool isAlt = (key == Key.LeftAlt || key == Key.RightAlt);
            bool isSys = (key == Key.LWin || key == Key.RWin);

            if (key == Key.Escape)
            {
                Clear();
            }
            else if (
                (isShift && ShiftIsAllowed()) || 
                (isCtrl && ControlIsAllowed()) || 
                (isAlt && AltIsAllowed()) ||
                (isSys && WindowsIsAllowed()))
            {
                if (Trigger.HasValue && !Keyboard.IsKeyDown(Trigger.Value))
                    Trigger = null;
            }
            else if (!isShift && !isCtrl && !isAlt)
            {
                CommitHotKey(key);
            }

            e.Handled = true;
            UpdateDisplay();
        }

        private void OnPreviewKeyUp(object sender, KeyEventArgs e)
        {
            UpdateDisplay();
        }

        private bool ShiftIsAllowed() 
        {
            return (DisabledModifiers.GetValueOrDefault() & ModifierKeys.Shift) == 0;
        }

        private bool ControlIsAllowed()
        {
            return (DisabledModifiers.GetValueOrDefault() & ModifierKeys.Control) == 0;
        }

        private bool AltIsAllowed()
        {
            return (DisabledModifiers.GetValueOrDefault() & ModifierKeys.Alt) == 0;
        }

        private bool WindowsIsAllowed()
        {
            return (DisabledModifiers.GetValueOrDefault() & ModifierKeys.Windows) == 0;
        }

        private void UpdateDisplayWithStoredValues()
        {
            if (Trigger.HasValue)
            {
                displayBox.Text = ModifierKeySymbols.CreateSymbolString(
                    Modifiers.GetValueOrDefault(),
                    Trigger.GetValueOrDefault());
            }
            else
            {
                displayBox.Text = ModifierKeySymbols.CreateSymbolString(
                    ModifierKeys.None,
                    Key.None);
            }
        }

        private void UpdateDisplayWithPreviewValues()
        {
            ModifierKeys showKeys = Modifiers.GetValueOrDefault() & ~DisabledModifiers.GetValueOrDefault();
            displayBox.Text = ModifierKeySymbols.CreateSymbolString(
                showKeys,
                Trigger.GetValueOrDefault());
        }

        private void UpdateDisplay()
        {
            if (hasFocus && (IsAnyModKeyDown() || previewTrigger.HasValue))
            {
                UpdateDisplayWithPreviewValues();
            }
            else
            {
                UpdateDisplayWithStoredValues();
            }

            displayBox.SelectionStart = displayBox.Text.Length;
            displayBox.SelectionLength = 0;
        }

        public void Clear()
        {
            KeyCombo = new HotKeyCombo();
            UpdateDisplay();
        }
    }
}
