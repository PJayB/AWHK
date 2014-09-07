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

namespace HotKeyCustomControlLibrary
{
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

        public string DisplayText
        {
            get;
            private set;
        }

        public static readonly DependencyProperty TriggerProperty =
            DependencyProperty.Register("Trigger", typeof(Key?), typeof(HotKeyBox));
        public static readonly DependencyProperty HasShiftProperty =
            DependencyProperty.Register("HasShift", typeof(bool?), typeof(HotKeyBox));
        public static readonly DependencyProperty HasControlProperty =
            DependencyProperty.Register("HasControl", typeof(bool?), typeof(HotKeyBox));
        public static readonly DependencyProperty HasAltProperty =
            DependencyProperty.Register("HasAlt", typeof(bool?), typeof(HotKeyBox));

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
            return  Keyboard.IsKeyDown(Key.LeftShift) || Keyboard.IsKeyDown(Key.RightShift) ||
                    Keyboard.IsKeyDown(Key.LeftCtrl) || Keyboard.IsKeyDown(Key.RightCtrl) ||
                    Keyboard.IsKeyDown(Key.LeftAlt) || Keyboard.IsKeyDown(Key.RightAlt);
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

        private void CommitHotKey(Key k)
        {
            HasShift = Keyboard.IsKeyDown(Key.LeftShift) || Keyboard.IsKeyDown(Key.RightShift);
            HasControl = Keyboard.IsKeyDown(Key.LeftCtrl) || Keyboard.IsKeyDown(Key.RightCtrl);
            HasAlt = Keyboard.IsKeyDown(Key.LeftAlt) || Keyboard.IsKeyDown(Key.RightAlt);
            Trigger = k;
        }

        private void OnPreviewKeyDown(object sender, KeyEventArgs e)
        {
            Key key = (e.Key == Key.System ? e.SystemKey : e.Key);

            // Don't handle tabs
            if (key == Key.Tab)
                return;

            if (key == Key.Escape)
            {
                Clear();
            }
            else if (key == Key.LeftShift || key == Key.RightShift ||
                     key == Key.LeftCtrl || key == Key.RightCtrl ||
                     key == Key.LeftAlt || key == Key.RightAlt)
            {
                if (Trigger.HasValue && !Keyboard.IsKeyDown(Trigger.Value))
                    Trigger = null;
            }
            else
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

        private void UpdateDisplayWithStoredValues()
        {
            if (Trigger == null)
            {
                displayBox.Text = "(None)";
            }
            else
            {
                StringBuilder sb = new StringBuilder();
                if (HasShift.HasValue && HasShift.Value) sb.Append("Shift + ");
                if (HasControl.HasValue && HasControl.Value) sb.Append("Ctrl + ");
                if (HasAlt.HasValue && HasAlt.Value) sb.Append("Alt + ");
                sb.Append(Trigger.Value.ToString());
                displayBox.Text = sb.ToString();
            }
        }

        private void UpdateDisplayWithPreviewValues()
        {
            StringBuilder sb = new StringBuilder();
            if (IsShiftKeyDown()) sb.Append("Shift + ");
            if (IsControlKeyDown()) sb.Append("Ctrl + ");
            if (IsAltKeyDown()) sb.Append("Alt + ");
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
            UpdateDisplay();
        }
    }
}
