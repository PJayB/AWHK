CONFIG_VALUE(LaunchOnStartUp, BOOL, FALSE)
CONFIG_VALUE(AllowSnapToOthers, BOOL, FALSE)
CONFIG_VALUE(MaxEdgeSearchSize, DWORD, 128)
CONFIG_VALUE(GridX, DWORD, 8)
CONFIG_VALUE(GridY, DWORD, 4)
CONFIG_VALUE(FineX, DWORD, 32)
CONFIG_VALUE(FineY, DWORD, 16)
CONFIG_VALUE(HelpCombo, AWHK_KEY_COMBO, CreateKeyComboFromModAndKey(MOD_ALT, VK_F1))
CONFIG_VALUE(ReloadConfigCombo, AWHK_KEY_COMBO, CreateKeyComboFromModAndKey(MOD_ALT, VK_F2))
CONFIG_VALUE(EditConfigCombo, AWHK_KEY_COMBO, CreateKeyComboFromModAndKey(MOD_SHIFT | MOD_ALT, VK_F2))
CONFIG_VALUE(QuitCombo, AWHK_KEY_COMBO, CreateKeyComboFromModAndKey(MOD_ALT | MOD_SHIFT, VK_F4))
CONFIG_VALUE(MoveKeyMod, MODKEY, MOD_ALT)
CONFIG_VALUE(NextKeyMod, MODKEY, MOD_CONTROL)
CONFIG_VALUE(FineKeyMod, MODKEY, MOD_SHIFT)
CONFIG_VALUE(ResizeKeys, AWHK_CURSOR_KEYS, CreateCursorKeys(VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN))
CONFIG_VALUE(MoveKeys, AWHK_CURSOR_KEYS, CreateCursorKeys('A', 'D', 'W', 'S'))
CONFIG_VALUE(MediaPrev, AWHK_KEY_COMBO, CreateKeyComboFromModAndKey(MOD_ALT | MOD_SHIFT, VK_F11))
CONFIG_VALUE(MediaNext, AWHK_KEY_COMBO, CreateKeyComboFromModAndKey(MOD_ALT | MOD_SHIFT, VK_F12))
CONFIG_VALUE(MediaStop, AWHK_KEY_COMBO, NullKeyCombo())
CONFIG_VALUE(MediaPlayPause, AWHK_KEY_COMBO, CreateKeyComboFromModAndKey(MOD_ALT, VK_F9))
CONFIG_VALUE(MediaVolumeMute, AWHK_KEY_COMBO, CreateKeyComboFromModAndKey(MOD_ALT, VK_F10))
CONFIG_VALUE(MediaVolumeDown, AWHK_KEY_COMBO, CreateKeyComboFromModAndKey(MOD_ALT, VK_F11))
CONFIG_VALUE(MediaVolumeUp, AWHK_KEY_COMBO, CreateKeyComboFromModAndKey(MOD_ALT, VK_F12))
