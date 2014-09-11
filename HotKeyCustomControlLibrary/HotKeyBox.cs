using System;
using System.Collections.Generic;
using System.ComponentModel;
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

    public class HotKeyBox : Control
    {
        private TextBox displayBox;
        private bool hasFocus;
        private Key? previewTrigger;

        public Key? Trigger
        {
            get { return base.GetValue(TriggerProperty) as Key?; }
            set { base.SetValue(TriggerProperty, value); }
        }

        public ModifierKeys? Modifiers
        {
            get { return base.GetValue(ModifiersProperty) as ModifierKeys?; }
            set { base.SetValue(ModifiersProperty, value); }
        }

        public ModifierKeys? DisabledModifiers
        {
            get { return base.GetValue(DisabledModifiersProperty) as ModifierKeys?; }
            set { base.SetValue(DisabledModifiersProperty, value); }
        }

        public bool HasAlt
        {
            get { return (Modifiers.GetValueOrDefault() & ModifierKeys.Alt) != 0; }
            set { Modifiers = Modifiers.GetValueOrDefault() | ModifierKeys.Alt; }
        }

        public bool HasControl
        {
            get { return (Modifiers.GetValueOrDefault() & ModifierKeys.Control) != 0; }
            set { Modifiers = Modifiers.GetValueOrDefault() | ModifierKeys.Control; }
        }

        public bool HasShift
        {
            get { return (Modifiers.GetValueOrDefault() & ModifierKeys.Shift) != 0; }
            set { Modifiers = Modifiers.GetValueOrDefault() | ModifierKeys.Shift; }
        }

        public bool HasWindows
        {
            get { return (Modifiers.GetValueOrDefault() & ModifierKeys.Windows) != 0; }
            set { Modifiers = Modifiers.GetValueOrDefault() | ModifierKeys.Windows; }
        }

        public bool DisableAlt
        {
            get { return (DisabledModifiers.GetValueOrDefault() & ModifierKeys.Alt) != 0; }
            set { DisabledModifiers = DisabledModifiers.GetValueOrDefault() | ModifierKeys.Alt; }
        }

        public bool DisableControl
        {
            get { return (DisabledModifiers.GetValueOrDefault() & ModifierKeys.Control) != 0; }
            set { DisabledModifiers = DisabledModifiers.GetValueOrDefault() | ModifierKeys.Control; }
        }

        public bool DisableShift
        {
            get { return (DisabledModifiers.GetValueOrDefault() & ModifierKeys.Shift) != 0; }
            set { DisabledModifiers = DisabledModifiers.GetValueOrDefault() | ModifierKeys.Shift; }
        }

        public bool DisableWindows
        {
            get { return (DisabledModifiers.GetValueOrDefault() & ModifierKeys.Windows) != 0; }
            set { DisabledModifiers = DisabledModifiers.GetValueOrDefault() | ModifierKeys.Windows; }
        }

        public event RoutedEventHandler HotKeyCommitted
        {
            add { AddHandler(CommitEvent, value); }
            remove { RemoveHandler(CommitEvent, value); }
        }

        public string DisplayText
        {
            get;
            private set;
        }

        public int VirtualKey
        {
            get { return Trigger.HasValue ? KeyInterop.VirtualKeyFromKey(Trigger.Value) : 0; }
        }

        public static readonly DependencyProperty TriggerProperty =
            DependencyProperty.Register("Trigger", typeof(Key?), typeof(HotKeyBox));
        public static readonly DependencyProperty ModifiersProperty =
            DependencyProperty.Register("Modifiers", typeof(ModifierKeys?), typeof(HotKeyBox));
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

        private static readonly string WinKeyName = ModifierKeys.Windows.ToString();
        private static readonly string AltKeyName = ModifierKeys.Alt.ToString();
        private static readonly string ControlKeyName = ModifierKeys.Control.ToString();
        private static readonly string ShiftKeyName = ModifierKeys.Shift.ToString();
        private static readonly string NoKeyName = ModifierKeys.None.ToString();

        private void UpdateDisplayWithStoredValues()
        {
            if (Trigger == null)
            {
                displayBox.Text = NoKeyName;
            }
            else
            {
                StringBuilder sb = new StringBuilder();
                if (HasWindows) sb.Append(WinKeyName + " + ");
                if (HasShift) sb.Append(ShiftKeyName + " + ");
                if (HasControl) sb.Append(ControlKeyName + " + ");
                if (HasAlt) sb.Append(AltKeyName + " + ");
                sb.Append(Trigger.Value.ToString());
                displayBox.Text = sb.ToString();
            }
        }

        private void UpdateDisplayWithPreviewValues()
        {
            StringBuilder sb = new StringBuilder();
            if (IsWindowsKeyDown() && WindowsIsAllowed()) sb.Append(WinKeyName + " + ");
            if (IsShiftKeyDown() && ShiftIsAllowed()) sb.Append(ShiftKeyName + " + ");
            if (IsControlKeyDown() && ControlIsAllowed()) sb.Append(ControlKeyName + " + ");
            if (IsAltKeyDown() && AltIsAllowed()) sb.Append(AltKeyName + " + ");
            if (Trigger.HasValue) sb.Append(Trigger.Value.ToString());
            displayBox.Text = sb.ToString();
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
            Trigger = null;
            Modifiers = null;
            UpdateDisplay();
        }
    }
}
