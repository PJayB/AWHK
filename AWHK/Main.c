/*
Copyright (c) 2022 Pete Lewis

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// WindowEnum.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <assert.h>
#include <Shlobj.h>

#include "WindowSnap.h"
#include "MediaKeys.h"
#include "Config.h"

#define AWHK_MAX_HOTKEYS		64
#define AWHK_APP_SEM			L"AWHK_APP_OPEN_SEM" 
#define AWHK_CONFIG_FILE_NAME	L"AWHK.ini"

typedef struct
{
    DWORD           dwCombo;
    HRESULT         Reason;
} AWHK_REGISTER_STATUS;

typedef struct
{
    LONG            RegisteredCount;
    LONG            AttemptCount;
    LONG            ErrorCount;
    AWHK_REGISTER_STATUS StatusCodes[AWHK_MAX_HOTKEYS];
} AWHK_REGISTRATION;

typedef struct
{
	HINSTANCE		    hInstance;
	DWORD			    dwMainThreadID;
    HANDLE              hAppOpenSemaphore;

	UINT			    MsgEditConfigFile;
	UINT			    MsgReloadConfig;
    UINT                MsgSuspend;
    UINT                MsgResume;

	AWHK_REGISTRATION   Registration;
	PWSTR				ConfigFile;
} AWHK_APP_STATE;

DIRECTION DirectionFromVKey( 
	const AWHK_CURSOR_KEYS* cfg, 
	USHORT vKey )
{
	if ( vKey == cfg->LeftKey ) 
		return DIR_LEFT;
	if ( vKey == cfg->RightKey )
		return DIR_RIGHT;
	if ( vKey == cfg->UpKey )
		return DIR_UP;
	if ( vKey == cfg->DownKey )
		return DIR_DOWN;

	return DIR_UNKNOWN;
}

BOOL IsAppAlreadyOpen()
{
	HANDLE hSyncSem = OpenSemaphore(
		SYNCHRONIZE | SEMAPHORE_MODIFY_STATE,	
		FALSE,
		AWHK_APP_SEM );
	if ( hSyncSem )
	{
		CloseHandle( hSyncSem );
		return TRUE;
	}

    return FALSE;
}

BOOL AppAlreadyOpenCheck()
{
	// Check if the app is already running
	if ( IsAppAlreadyOpen() )
	{
#ifdef _DEBUG
		if ( IsDebuggerPresent() )
		{
			// Kill the old app and allow this to run
			//return FALSE;
		}
#endif

		MessageBox( 
			NULL,
			L"AWHK is already running.",
			APPLICATION_TITLE,
			MB_OK | MB_ICONSTOP );

		return TRUE;
	}

	return FALSE;
}

BOOL ShowWebHelp()
{
	return (INT) ShellExecute(
		NULL,
		L"open",
		L"http://gibbering.net/AWHK/",
		NULL,
		NULL,
		SW_SHOWNORMAL ) > 32;
}

// Get the path to the INI file.
PWSTR GetConfigFilePath()
{
	PWSTR result = NULL;

	// Get the user data directory
	PWSTR userDataDir = NULL;
	if (SUCCEEDED(SHGetKnownFolderPath(&FOLDERID_LocalAppDataLow, 0, NULL, &userDataDir)))
	{
		size_t resultSize = wcslen(userDataDir) + wcslen(AWHK_CONFIG_FILE_NAME) + 2;
		result = (PWSTR)HeapAlloc(GetProcessHeap(), 0, resultSize * sizeof(WCHAR));
		if (result)
		{
			swprintf_s(result, resultSize, L"%s\\%s", userDataDir, AWHK_CONFIG_FILE_NAME);
		}
	}

	CoTaskMemFree(userDataDir);
	return result;
}

BOOL EditConfigFile( const AWHK_APP_STATE* pState )
{
	return PostThreadMessage(
		pState->dwMainThreadID,
		pState->MsgEditConfigFile,
		0, 0 );
}

void ConfigureWindowSnapParams( 
	const AWHK_APP_CONFIG* cfg,
	USHORT modifiers,
	WINDOW_SNAP_PARAMS* params )
{
	params->Flags = 0;

	if ( cfg->AllowSnapToOthers )
		params->Flags |= WINDOW_SNAP_TO_GRID | WINDOW_SNAP_TO_OTHERS;
	else
		params->Flags |= WINDOW_SNAP_TO_GRID;

	if ( ( modifiers & cfg->NextKeyMod ) == cfg->NextKeyMod )
		params->Flags |= WINDOW_MODIFY_ADJACENT;
	if ( ( modifiers & cfg->FineKeyMod ) == cfg->FineKeyMod )
		params->Flags |= WINDOW_SNAP_FINE_GRID;

	params->MaxEdgeSearchSize = cfg->MaxEdgeSearchSize * 2;
	params->GridDivisorX = cfg->GridX;
	params->GridDivisorY = cfg->GridY;
	params->FineDivisorX = cfg->FineX;
	params->FineDivisorY = cfg->FineY;
}

BOOL HandleHotKey(
	const AWHK_APP_STATE* state,
	const AWHK_APP_CONFIG* cfg,
	AWHK_KEY_COMBO key )
{
	// Deal with system keys
	if (key.dwBits == cfg->HelpCombo.dwBits)
		return ShowWebHelp();
	else if (key.dwBits == cfg->EditConfigCombo.dwBits)
		return EditConfigFile( state );
    else if (key.dwBits == cfg->MediaPlayPause.dwBits)
        return MediaPlayPause();
    else if (key.dwBits == cfg->MediaStop.dwBits)
        return MediaStop();
    else if (key.dwBits == cfg->MediaNext.dwBits)
        return MediaNextTrack();
    else if (key.dwBits == cfg->MediaPrev.dwBits)
        return MediaPrevTrack();
    else if (key.dwBits == cfg->MediaMute.dwBits)
        return MediaMute();
    else if (key.dwBits == cfg->MediaVolumeDown.dwBits)
        return MediaVolumeDown();
    else if (key.dwBits == cfg->MediaVolumeUp.dwBits)
        return MediaVolumeUp();
    

	WINDOW_SNAP_PARAMS params;
	InitWindowSnapParams(&params);

	ConfigureWindowSnapParams(
		cfg,
		key.Modifiers,
		&params );

	// Which set of arrow keys are we using?
	DIRECTION arrowKeys = DirectionFromVKey( &cfg->ResizeKeys, key.Trigger );
	if ( arrowKeys == DIR_UNKNOWN )
	{
		arrowKeys = DirectionFromVKey( &cfg->MoveKeys, key.Trigger );
		if ( arrowKeys == DIR_UNKNOWN )
			return FALSE;

		params.Flags |= WINDOW_MOVE_ONLY;
	}

	return ForegroundWindowSnap( 
		arrowKeys, // direction
		&params // single window only?
		);
}

BOOL FileExists(LPCTSTR szPath)
{
	DWORD dwAttrib = GetFileAttributes(szPath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

BOOL ValidateConfigFileExists(LPCWSTR pConfigFile, const AWHK_APP_CONFIG* pCfg)
{
	if (!pConfigFile || !*pConfigFile)
	{
		return FALSE;
	}

	if (!FileExists(pConfigFile))
	{
		// Create the file
		// todo
		return SaveConfiguration(pConfigFile, pCfg);
	}

	return TRUE;
}

void OpenConfigEditor(LPCWSTR pConfigFile, const AWHK_APP_CONFIG* pCfg)
{
	if (!ValidateConfigFileExists(pConfigFile, pCfg))
	{
		MessageBox(NULL,
			APPLICATION_TITLE L" was unable to write settings to your user profile.",
			APPLICATION_TITLE,
			MB_ICONERROR | MB_OK);
		return;
	}

	if (FAILED(ShellExecute(NULL, L"edit", pConfigFile, NULL, NULL, SW_SHOWDEFAULT)))
    {
		TCHAR msg[256];
		swprintf_s(
			msg, _countof(msg),
			APPLICATION_TITLE L" was unable to invoke a text editor. You can edit the config file at: <path>"": %s",
			pConfigFile);
        MessageBox( NULL, 
			msg,
			APPLICATION_TITLE,
			MB_ICONERROR | MB_OK );
    }
}

void RegisterModifierAndKeyAtIndex( DWORD keyMod, DWORD vKey, AWHK_REGISTRATION* pKeyStatus )
{
	assert( pKeyStatus->AttemptCount < AWHK_MAX_HOTKEYS );

    AWHK_REGISTER_STATUS* pResult = &pKeyStatus->StatusCodes[pKeyStatus->AttemptCount++];

	pResult->dwCombo = AWHK_MAKE_HOTKEY( keyMod, vKey );
    if ( keyMod == 0 && vKey == 0 )
    {
        // Not bound
        pResult->Reason = S_FALSE;
        return;
    }

    SetLastError( ERROR_SUCCESS );
	if ( RegisterHotKey( NULL, pKeyStatus->RegisteredCount, keyMod, vKey ) )
    {
        pKeyStatus->RegisteredCount++;
		pResult->Reason = S_OK;
    }
    else
    {
        pKeyStatus->ErrorCount++;
        pResult->Reason = HRESULT_FROM_WIN32( GetLastError() );
    }
}

void RegisterKeyComboAtIndex( const AWHK_KEY_COMBO* pCombo, AWHK_REGISTRATION* pKeyStatus )
{
	RegisterModifierAndKeyAtIndex( pCombo->Modifiers, pCombo->Trigger, pKeyStatus );
}

void RegisterArrowKeys( 
	const AWHK_APP_CONFIG* cfg,
	const AWHK_CURSOR_KEYS* pArrowKeys,
	AWHK_REGISTRATION* pKeyStatus )
{
	DWORD dwMoveKeyMod = cfg->MoveKeyMod;
	DWORD dwFineKeyMod = cfg->MoveKeyMod | cfg->FineKeyMod;
	DWORD dwSoloKeyMod = cfg->MoveKeyMod | cfg->NextKeyMod;
	DWORD dwAllKeyMods = cfg->MoveKeyMod | cfg->FineKeyMod | cfg->NextKeyMod;

	RegisterModifierAndKeyAtIndex( dwMoveKeyMod, pArrowKeys->LeftKey, pKeyStatus );
	RegisterModifierAndKeyAtIndex( dwMoveKeyMod, pArrowKeys->RightKey, pKeyStatus );
	RegisterModifierAndKeyAtIndex( dwMoveKeyMod, pArrowKeys->UpKey, pKeyStatus );
	RegisterModifierAndKeyAtIndex( dwMoveKeyMod, pArrowKeys->DownKey, pKeyStatus );

	if ( cfg->FineKeyMod )
	{
		RegisterModifierAndKeyAtIndex( dwFineKeyMod, pArrowKeys->LeftKey, pKeyStatus );
		RegisterModifierAndKeyAtIndex( dwFineKeyMod, pArrowKeys->RightKey, pKeyStatus );
		RegisterModifierAndKeyAtIndex( dwFineKeyMod, pArrowKeys->UpKey, pKeyStatus );
		RegisterModifierAndKeyAtIndex( dwFineKeyMod, pArrowKeys->DownKey, pKeyStatus );
	}

	if ( cfg->NextKeyMod )
	{
		RegisterModifierAndKeyAtIndex( dwSoloKeyMod, pArrowKeys->LeftKey, pKeyStatus );
		RegisterModifierAndKeyAtIndex( dwSoloKeyMod, pArrowKeys->RightKey, pKeyStatus );
		RegisterModifierAndKeyAtIndex( dwSoloKeyMod, pArrowKeys->UpKey, pKeyStatus );
		RegisterModifierAndKeyAtIndex( dwSoloKeyMod, pArrowKeys->DownKey, pKeyStatus );
	}

	if ( cfg->FineKeyMod | cfg->NextKeyMod )
	{
		RegisterModifierAndKeyAtIndex( dwAllKeyMods, pArrowKeys->LeftKey, pKeyStatus );
		RegisterModifierAndKeyAtIndex( dwAllKeyMods, pArrowKeys->RightKey, pKeyStatus );
		RegisterModifierAndKeyAtIndex( dwAllKeyMods, pArrowKeys->UpKey, pKeyStatus );
		RegisterModifierAndKeyAtIndex( dwAllKeyMods, pArrowKeys->DownKey, pKeyStatus );
	}
}

void RegisterExtraHotKeys( 
	const AWHK_APP_CONFIG* cfg,
	AWHK_REGISTRATION* pKeyStatus )
{
	RegisterKeyComboAtIndex( &cfg->HelpCombo,         pKeyStatus );
	RegisterKeyComboAtIndex( &cfg->EditConfigCombo,   pKeyStatus );
	RegisterKeyComboAtIndex( &cfg->MediaPlayPause,    pKeyStatus );
	RegisterKeyComboAtIndex( &cfg->MediaStop,         pKeyStatus );
	RegisterKeyComboAtIndex( &cfg->MediaNext,         pKeyStatus );
	RegisterKeyComboAtIndex( &cfg->MediaPrev,         pKeyStatus );
	RegisterKeyComboAtIndex( &cfg->MediaMute,         pKeyStatus );
	RegisterKeyComboAtIndex( &cfg->MediaVolumeUp,     pKeyStatus );
	RegisterKeyComboAtIndex( &cfg->MediaVolumeDown,   pKeyStatus );
}

void RegisterHotKeys( 
	const AWHK_APP_CONFIG* cfg,
	AWHK_REGISTRATION* pKeyStatus )
{
	RegisterExtraHotKeys( cfg, pKeyStatus );

	if ( !cfg->MoveKeyMod )
		return;

	RegisterArrowKeys( cfg, &cfg->ResizeKeys, pKeyStatus );
	RegisterArrowKeys( cfg, &cfg->MoveKeys, pKeyStatus );
}

LPCWSTR GetKeyModString( DWORD keyMod )
{
#define KEYMOD(x)	case MOD_##x: return TEXT(#x) TEXT(" ")
	switch (keyMod)
	{
	KEYMOD(ALT);
	KEYMOD(CONTROL);
	KEYMOD(SHIFT);
	KEYMOD(WIN);
	default: 
		return L"";
	}
#undef KEYMOD
};

LPCWSTR HResultToString( HRESULT hr, LPWSTR buf, DWORD bufSize )
{
    INT len = (INT) FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		buf,
		bufSize,
		NULL);
    if (len == 0)
    {
        swprintf_s( buf, bufSize, L"Unknown error." );
        return buf;
    }

    // Strip newlines and periods from the end of the string.
    for ( INT i = len-1; i > 0 && (buf[i] == L'\n' || buf[i] == L'\r' || buf[i] == L'.'); --i )
    {
        buf[i] = 0;
    }

    return buf;
}

void ShowRegistrationFailures( const AWHK_REGISTRATION* pKeyState )
{
    // TODO: pass this to the listeners instead
    if ( pKeyState->ErrorCount == 0 )
        return;

    WCHAR strHR[256] = {0};
	WCHAR strMsg[2048] = {0};
	LPCWSTR strEnd = strMsg + _countof(strMsg);
	LPWSTR strCursor = strMsg;

	strCursor += swprintf_s(
		strMsg,
		_countof(strMsg),
		L"Some keys failed to register. Please check the following keys in the settings:\n\n" );

	for ( LONG k = 0; k < pKeyState->AttemptCount; ++k )
	{
		const AWHK_REGISTER_STATUS* pStatus = &pKeyState->StatusCodes[k];

        if ( FAILED( pStatus->Reason ) )
		{
			DWORD dwVKey = AWHK_GET_TRIGGER_KEY( pStatus->dwCombo );
			DWORD dwMod = AWHK_GET_MODIFIER_KEYS( pStatus->dwCombo );
			UINT uScanCode = MapVirtualKey( dwVKey, 0 );

            WCHAR strVKey[64] = {0};
            GetKeyNameText( 
                uScanCode << 16, 
                strVKey, _countof(strVKey) );
            if ( wcslen(strVKey) == 0 )
			{
				swprintf_s(
					strVKey,
					_countof(strVKey),
					L"0x%X",
					dwVKey );
			}

			strCursor += swprintf_s(
				strCursor, 
				strEnd - strCursor,
				L"%s%s%s%s%s: %s.\n",
				GetKeyModString( dwMod & MOD_ALT ),
				GetKeyModString( dwMod & MOD_SHIFT ),
				GetKeyModString( dwMod & MOD_CONTROL ),
				GetKeyModString( dwMod & MOD_WIN ),
				strVKey,
                HResultToString( pStatus->Reason, strHR, _countof(strHR) ) );
		}
	}

	MessageBox( NULL, 
		strMsg,
		APPLICATION_TITLE,
		MB_ICONERROR | MB_OK );
}

void RegisterHotKeysAndWarn( AWHK_APP_STATE* appState, const AWHK_APP_CONFIG* cfg )
{
    ZeroMemory( &appState->Registration, sizeof(appState->Registration) );

	RegisterHotKeys( cfg, &appState->Registration );

    if ( appState->Registration.ErrorCount > 0 )
    {
        ShowRegistrationFailures( &appState->Registration );
    }
}

void UnregisterHotkeys( AWHK_REGISTRATION* pReg )
{
    LONG count = pReg->RegisteredCount;
	while ( count-- > 0 )
	{
		UnregisterHotKey( NULL, count );
	}

    ZeroMemory( pReg, sizeof(*pReg) );
}

int MessageLoop( AWHK_APP_STATE* appState, AWHK_APP_CONFIG* appCfg )
{
	MSG msg;

	while ( GetMessage( &msg, NULL, 0, 0 ) )
	{
		if (msg.message == appState->MsgEditConfigFile)
		{
			OpenConfigEditor(appState->ConfigFile, appCfg);
			continue;
		}

		if ( msg.message == appState->MsgReloadConfig )
		{
			UnregisterHotkeys( &appState->Registration );

			// Reload the settings
			LoadConfiguration(appState->ConfigFile, appCfg);
			RegisterHotKeysAndWarn(appState, appCfg);

			continue;
		}

        // The control panel can suspend and resume the functionality without quitting the application
        if ( msg.message == appState->MsgSuspend )
        {
            UnregisterHotkeys( &appState->Registration );
            continue;
        }
        if ( msg.message == appState->MsgResume )
        {
            // Silent reload here.
			RegisterHotKeysAndWarn( 
				appState,
				appCfg );

            continue;
        }

		switch ( msg.message )
		{
		case WM_QUIT:
			{
				return 0;
			}
			break;
		case WM_HOTKEY:
			{
				BOOL bOK = HandleHotKey(
					appState,
					appCfg,
					CreateKeyCombo( msg.lParam ) );
#ifdef _DEBUG
                assert( bOK );
#else
                (void) bOK;
#endif
				break;
			}
			break;
		default:
			{
				TranslateMessage( &msg ); 
				DispatchMessage( &msg );
			}
			break;
		}
	}

	return 0;
}

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	if ( AppAlreadyOpenCheck() )
		return -1;

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	AWHK_APP_STATE appState;
    ZeroMemory( &appState, sizeof(appState) );
	appState.hInstance = hInstance;
	appState.dwMainThreadID = GetCurrentThreadId();
	appState.MsgEditConfigFile = RegisterWindowMessage( L"AWHKEditConfigMsg" );
	appState.MsgReloadConfig = RegisterWindowMessage( L"AWHKReloadConfigMsg" );
	appState.MsgSuspend = RegisterWindowMessage( L"AWHKSuspendMsg" );
	appState.MsgResume = RegisterWindowMessage( L"AWHKResumeMsg" );
	appState.ConfigFile = GetConfigFilePath();

    // Open the application semaphore
	SECURITY_ATTRIBUTES sa;
	sa.bInheritHandle = FALSE;
	sa.lpSecurityDescriptor = NULL;
	sa.nLength = sizeof( sa );

    appState.hAppOpenSemaphore = CreateSemaphore(
		&sa,
		0,
		MAXINT32,
		AWHK_APP_SEM );

	AWHK_APP_CONFIG appCfg;
    ZeroMemory( &appCfg, sizeof(appCfg) );
	LoadConfiguration(appState.ConfigFile, &appCfg);

	RegisterHotKeysAndWarn(
        &appState,
		&appCfg );

	int ret = MessageLoop( &appState, &appCfg );

	UnregisterHotkeys( &appState.Registration );
    CloseHandle( appState.hAppOpenSemaphore );
	HeapFree(GetProcessHeap(), 0, appState.ConfigFile);

	return ret;
}

