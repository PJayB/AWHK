
/*
 * Virtual Keys, Standard Set
 */
KEYCODE(LBUTTON        , VK_LBUTTON        )
KEYCODE(RBUTTON        , VK_RBUTTON        )
KEYCODE(CANCEL         , VK_CANCEL         )
KEYCODE(MBUTTON        , VK_MBUTTON        )

#if(_WIN32_WINNT >= 0x0500)
KEYCODE(XBUTTON1       , VK_XBUTTON1       )
KEYCODE(XBUTTON2       , VK_XBUTTON2       )
#endif /* _WIN32_WINNT >= 0x0500 */

KEYCODE(BACK           , VK_BACK           )
KEYCODE(TAB            , VK_TAB            )

KEYCODE(CLEAR          , VK_CLEAR          )
KEYCODE(RETURN         , VK_RETURN         )

KEYCODE(SHIFT          , VK_SHIFT          )
KEYCODE(CONTROL        , VK_CONTROL        )
KEYCODE(MENU           , VK_MENU           )
KEYCODE(PAUSE          , VK_PAUSE          )
KEYCODE(CAPITAL        , VK_CAPITAL        )

KEYCODE(ESCAPE         , VK_ESCAPE         )

KEYCODE(CONVERT        , VK_CONVERT        )
KEYCODE(NONCONVERT     , VK_NONCONVERT     )
KEYCODE(ACCEPT         , VK_ACCEPT         )
KEYCODE(MODECHANGE     , VK_MODECHANGE     )

KEYCODE(SPACE          , VK_SPACE          )
KEYCODE(PRIOR          , VK_PRIOR          )
KEYCODE(NEXT           , VK_NEXT           )
KEYCODE(END            , VK_END            )
KEYCODE(HOME           , VK_HOME           )
KEYCODE(LEFT           , VK_LEFT           )
KEYCODE(UP             , VK_UP             )
KEYCODE(RIGHT          , VK_RIGHT          )
KEYCODE(DOWN           , VK_DOWN           )
KEYCODE(SELECT         , VK_SELECT         )
KEYCODE(PRINT          , VK_PRINT          )
KEYCODE(EXECUTE        , VK_EXECUTE        )
KEYCODE(SNAPSHOT       , VK_SNAPSHOT       )
KEYCODE(INSERT         , VK_INSERT         )
KEYCODE(DELETE         , VK_DELETE         )
KEYCODE(HELP           , VK_HELP           )

	  /*
	   * VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
	   * 0x3A - 0x40 : unassigned
	   * VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
	   */

KEYCODE(LWIN           , VK_LWIN           )
KEYCODE(RWIN           , VK_RWIN           )
KEYCODE(APPS           , VK_APPS           )

	   /*
		* 0x5E : reserved
		*/

KEYCODE(SLEEP          , VK_SLEEP          )

KEYCODE(NUMPAD0        , VK_NUMPAD0        )
KEYCODE(NUMPAD1        , VK_NUMPAD1        )
KEYCODE(NUMPAD2        , VK_NUMPAD2        )
KEYCODE(NUMPAD3        , VK_NUMPAD3        )
KEYCODE(NUMPAD4        , VK_NUMPAD4        )
KEYCODE(NUMPAD5        , VK_NUMPAD5        )
KEYCODE(NUMPAD6        , VK_NUMPAD6        )
KEYCODE(NUMPAD7        , VK_NUMPAD7        )
KEYCODE(NUMPAD8        , VK_NUMPAD8        )
KEYCODE(NUMPAD9        , VK_NUMPAD9        )
KEYCODE(MULTIPLY       , VK_MULTIPLY       )
KEYCODE(ADD            , VK_ADD            )
KEYCODE(SEPARATOR      , VK_SEPARATOR      )
KEYCODE(SUBTRACT       , VK_SUBTRACT       )
KEYCODE(DECIMAL        , VK_DECIMAL        )
KEYCODE(DIVIDE         , VK_DIVIDE         )
KEYCODE(F1             , VK_F1             )
KEYCODE(F2             , VK_F2             )
KEYCODE(F3             , VK_F3             )
KEYCODE(F4             , VK_F4             )
KEYCODE(F5             , VK_F5             )
KEYCODE(F6             , VK_F6             )
KEYCODE(F7             , VK_F7             )
KEYCODE(F8             , VK_F8             )
KEYCODE(F9             , VK_F9             )
KEYCODE(F10            , VK_F10            )
KEYCODE(F11            , VK_F11            )
KEYCODE(F12            , VK_F12            )
KEYCODE(F13            , VK_F13            )
KEYCODE(F14            , VK_F14            )
KEYCODE(F15            , VK_F15            )
KEYCODE(F16            , VK_F16            )
KEYCODE(F17            , VK_F17            )
KEYCODE(F18            , VK_F18            )
KEYCODE(F19            , VK_F19            )
KEYCODE(F20            , VK_F20            )
KEYCODE(F21            , VK_F21            )
KEYCODE(F22            , VK_F22            )
KEYCODE(F23            , VK_F23            )
KEYCODE(F24            , VK_F24            )

#if(_WIN32_WINNT >= 0x0604)

		/*
		 * 0x88 - 0x8F : UI navigation
		 */

KEYCODE(VIEW     , VK_NAVIGATION_VIEW     )
KEYCODE(MENU     , VK_NAVIGATION_MENU     )
KEYCODE(UP       , VK_NAVIGATION_UP       )
KEYCODE(DOWN     , VK_NAVIGATION_DOWN     )
KEYCODE(LEFT     , VK_NAVIGATION_LEFT     )
KEYCODE(RIGHT    , VK_NAVIGATION_RIGHT    )
KEYCODE(ACCEPT   , VK_NAVIGATION_ACCEPT   )
KEYCODE(CANCEL   , VK_NAVIGATION_CANCEL   )

#endif /* _WIN32_WINNT >= 0x0604 */

KEYCODE(NUMLOCK        , VK_NUMLOCK        )
KEYCODE(SCROLL         , VK_SCROLL         )

		   /*
			* 0x97 - 0x9F : unassigned
			*/

			/*
			 * VK_L* & VK_R* - left and right Alt, Ctrl and Shift virtual keys.
			 * Used only as parameters to GetAsyncKeyState() and GetKeyState().
			 * No other API or message will distinguish left and right keys in this way.
			 */
KEYCODE(LSHIFT         , VK_LSHIFT         )
KEYCODE(RSHIFT         , VK_RSHIFT         )
KEYCODE(LCONTROL       , VK_LCONTROL       )
KEYCODE(RCONTROL       , VK_RCONTROL       )
KEYCODE(LMENU          , VK_LMENU          )
KEYCODE(RMENU          , VK_RMENU          )

#if(_WIN32_WINNT >= 0x0500)
KEYCODE(BACK        , VK_BROWSER_BACK        )
KEYCODE(FORWARD     , VK_BROWSER_FORWARD     )
KEYCODE(REFRESH     , VK_BROWSER_REFRESH     )
KEYCODE(STOP        , VK_BROWSER_STOP        )
KEYCODE(SEARCH      , VK_BROWSER_SEARCH      )
KEYCODE(FAVORITES   , VK_BROWSER_FAVORITES   )
KEYCODE(HOME        , VK_BROWSER_HOME        )

KEYCODE(MUTE         , VK_VOLUME_MUTE         )
KEYCODE(VOLUMEDOWN   , VK_VOLUME_DOWN         )
KEYCODE(VOLUMEUP     , VK_VOLUME_UP           )
KEYCODE(MEDIANEXT    , VK_MEDIA_NEXT_TRACK    )
KEYCODE(MEDIAPREV    , VK_MEDIA_PREV_TRACK    )
KEYCODE(MEDIASTOP    , VK_MEDIA_STOP          )
KEYCODE(MEDIAPLAYPAUSE      , VK_MEDIA_PLAY_PAUSE    )
KEYCODE(MAIL         , VK_LAUNCH_MAIL         )
KEYCODE(MEDIASELECT  , VK_LAUNCH_MEDIA_SELECT )
KEYCODE(LAUNCH1      , VK_LAUNCH_APP1         )
KEYCODE(LAUNCH2      , VK_LAUNCH_APP2         )

#endif /* _WIN32_WINNT >= 0x0500 */
