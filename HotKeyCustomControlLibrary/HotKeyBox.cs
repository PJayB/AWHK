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

        public bool? AllowShift
        {
            get { return base.GetValue(AllowShiftProperty) as bool?; }
            set { base.SetValue(AllowShiftProperty, value); }
        }

        public bool? AllowControl
        {
            get { return base.GetValue(AllowControlProperty) as bool?; }
            set { base.SetValue(AllowControlProperty, value); }
        }

        public bool? AllowAlt
        {
            get { return base.GetValue(AllowAltProperty) as bool?; }
            set { base.SetValue(AllowAltProperty, value); }
        }

        public bool? AllowSystem
        {
            get { return base.GetValue(AllowSystemProperty) as bool?; }
            set { base.SetValue(AllowSystemProperty, value); }
        }

        public Key? Trigger
        {
            get { return base.GetValue(TriggerProperty) as Key?; }
            set { base.SetValue(TriggerProperty, value); }
        }

        public bool? HasShift
        {
            get { return base.GetValue(HasShiftProperty) as bool?; }
            set { base.SetValue(HasShiftProperty, value); }
        }

        public bool? HasControl
        {
            get { return base.GetValue(HasControlProperty) as bool?; }
            set { base.SetValue(HasControlProperty, value); }
        }

        public bool? HasAlt
        {
            get { return base.GetValue(HasAltProperty) as bool?; }
            set { base.SetValue(HasAltProperty, value); }
        }

        public bool? HasSystem
        {
            get { return base.GetValue(HasSystemProperty) as bool?; }
            set { base.SetValue(HasSystemProperty, value); }
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

        public ModifierKeys Modifiers
        {
            get
            {
                ModifierKeys keys = ModifierKeys.None;
                if (HasAlt.GetValueOrDefault()) keys |= ModifierKeys.Alt;
                if (HasControl.GetValueOrDefault()) keys |= ModifierKeys.Control;
                if (HasShift.GetValueOrDefault()) keys |= ModifierKeys.Shift;
                if (HasSystem.GetValueOrDefault()) keys |= ModifierKeys.Windows;
                return keys;
            }
            set
            {
                HasAlt = null;
                HasControl = null;
                HasShift = null;
                HasSystem = null;

                if ((value & ModifierKeys.Alt) != 0) HasAlt = true;
                if ((value & ModifierKeys.Control) != 0) HasControl = true;
                if ((value & ModifierKeys.Shift) != 0) HasShift = true;
                if ((value & ModifierKeys.Windows) != 0) HasSystem = true;
            }
        }

        public static readonly DependencyProperty TriggerProperty =
            DependencyProperty.Register("Trigger", typeof(Key?), typeof(HotKeyBox));
        public static readonly DependencyProperty HasShiftProperty =
            DependencyProperty.Register("HasShift", typeof(bool?), typeof(HotKeyBox));
        public static readonly DependencyProperty HasControlProperty =
            DependencyProperty.Register("HasControl", typeof(bool?), typeof(HotKeyBox));
        public static readonly DependencyProperty HasAltProperty =
            DependencyProperty.Register("HasAlt", typeof(bool?), typeof(HotKeyBox));
        public static readonly DependencyProperty HasSystemProperty =
            DependencyProperty.Register("HasSystem", typeof(bool?), typeof(HotKeyBox));
        public static readonly DependencyProperty AllowShiftProperty =
            DependencyProperty.Register("AllowShift", typeof(bool?), typeof(HotKeyBox));
        public static readonly DependencyProperty AllowControlProperty =
            DependencyProperty.Register("AllowControl", typeof(bool?), typeof(HotKeyBox));
        public static readonly DependencyProperty AllowAltProperty =
            DependencyProperty.Register("AllowAlt", typeof(bool?), typeof(HotKeyBox));
        public static readonly DependencyProperty AllowSystemProperty =
            DependencyProperty.Register("AllowSystem", typeof(bool?), typeof(HotKeyBox));

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
                //displayBox.GotKeyboardFocus += new System.Windows.Input.KeyboardFocusChangedEventHandler(OnGotKeyboardFocus);
                //displayBox.PreviewKeyDown += new Preview(OnFilterBoxTextChanged);
                // TODO
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

        private bool IsSystemKeyDown()
        {
            return Keyboard.IsKeyDown(Key.LWin) || Keyboard.IsKeyDown(Key.RWin);
        }

        private void CommitHotKey(Key k)
        {
            HasShift = null;
            HasControl = null;
            HasAlt = null;
            HasSystem = null;
          
            if (ShiftIsAllowed()) 
                HasShift = IsShiftKeyDown();
            if (ControlIsAllowed()) 
                HasControl = IsControlKeyDown();
            if (AltIsAllowed()) 
                HasAlt = IsAltKeyDown();
            if (SystemIsAllowed())
                HasSystem = IsSystemKeyDown();

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
                (isSys && SystemIsAllowed()))
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
            return !AllowShift.HasValue || AllowShift.GetValueOrDefault();
        }

        private bool ControlIsAllowed()
        {
            return !AllowControl.HasValue || AllowControl.GetValueOrDefault();
        }

        private bool AltIsAllowed()
        {
            return !AllowAlt.HasValue || AllowAlt.GetValueOrDefault();
        }

        private bool SystemIsAllowed()
        {
            return !AllowSystem.HasValue || AllowSystem.GetValueOrDefault();
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
                if (HasSystem.GetValueOrDefault()) sb.Append(WinKeyName + " + ");
                if (HasShift.GetValueOrDefault()) sb.Append(ShiftKeyName + " + ");
                if (HasControl.GetValueOrDefault()) sb.Append(ControlKeyName + " + ");
                if (HasAlt.GetValueOrDefault()) sb.Append(AltKeyName + " + ");
                sb.Append(Trigger.Value.ToString());
                displayBox.Text = sb.ToString();
            }
        }

        private void UpdateDisplayWithPreviewValues()
        {
            StringBuilder sb = new StringBuilder();
            if (IsSystemKeyDown() && SystemIsAllowed()) sb.Append(WinKeyName + " + ");
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
            HasAlt = null;
            HasControl = null;
            HasShift = null;
            HasSystem = null;
            UpdateDisplay();
        }
    }
}
