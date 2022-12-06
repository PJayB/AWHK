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
#include <Shlobj.h>

#include "WindowSnap.h"
#include "MediaKeys.h"
#include "Config.h"
#include "AutoLogin.h"

#define AWHK_MAX_HOTKEYS		64
#define AWHK_APP_SEM			L"AWHK_APP_OPEN_SEM" 
#define AWHK_CONFIG_FILE_NAME	L"AWHK.ini"

typedef enum
{
	EXIT_SUCCESS_ = EXIT_SUCCESS,
	EXIT_FAILURE_ = EXIT_FAILURE,
	EXIT_ALREADY_RUNNING,
	EXIT_COM_INIT_FAILURE,
	EXIT_BAD_CONFIG,
} EXIT_CODES;

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
    UINT                MsgSuspend;
    UINT                MsgResume;

	AWHK_REGISTRATION   Registration;
	PWSTR				ConfigFile;

	BOOL				Suspended;
} AWHK_APP_STATE;

void RegisterHotKeys(const AWHK_APP_CONFIG* cfg, AWHK_REGISTRATION* pKeyStatus);
void UnregisterHotkeys( AWHK_REGISTRATION* pReg );

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
	// todo: improve this
	return (INT) ShellExecute(
		NULL,
		L"open",
		L"index.html",
		NULL,
		NULL,
		SW_SHOWNORMAL ) > 32;
}

LPCWSTR SafeKeyModString( DWORD keyMod )
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

void AppendRegistrationFailures( const AWHK_REGISTRATION* pKeyState, LPWSTR pMsg, size_t msgLen )
{
	WCHAR strHR[256] = {0};
	LPCWSTR strEnd = pMsg + msgLen;
	LPWSTR strCursor = pMsg;

	strCursor += swprintf_s(
		pMsg,
		msgLen,
		L"Some keys failed to register. Please check the following keys in the settings:\n\n" );

	for ( LONG k = 0; k < pKeyState->AttemptCount; ++k )
	{
		const AWHK_REGISTER_STATUS* pStatus = &pKeyState->StatusCodes[k];

		if ( FAILED( pStatus->Reason ) )
		{
			DWORD dwVKey = AWHK_GET_TRIGGER_KEY( pStatus->dwCombo );
			DWORD dwMod = AWHK_GET_MODIFIER_KEYS( pStatus->dwCombo );

			strCursor += swprintf_s(
				strCursor, 
				strEnd - strCursor,
				L"%s%s%s%s%s: %s.\n",
				SafeKeyModString( dwMod & MOD_ALT ),
				SafeKeyModString( dwMod & MOD_SHIFT ),
				SafeKeyModString( dwMod & MOD_CONTROL ),
				SafeKeyModString( dwMod & MOD_WIN ),
				KeyToString(dwVKey),
				HResultToString( pStatus->Reason, strHR, _countof(strHR) ) );
		}
	}
}

typedef enum
{
	CONFIG_LOADED = 0,
	CONFIG_ABORT = IDABORT,
	CONFIG_RETRY = IDRETRY,
	CONFIG_IGNORE = IDIGNORE,
} CONFIG_LOAD_RESULT;

CONFIG_LOAD_RESULT AttemptConfigurationAndBind(LPCWSTR pConfigFile, AWHK_APP_CONFIG* pCfg, AWHK_REGISTRATION* pRegistration, BOOL ignoreMissingConfig)
{
	BOOL success = TRUE;
	PARSING_ERROR* errors = NULL;

	WCHAR errorText[2048] = {0};
	size_t errorCur = 0;

	// Attempt to load it from the file
	if (!LoadConfiguration(pConfigFile, pCfg, &errors) && !ignoreMissingConfig)
	{
		errorCur += swprintf(errorText + errorCur,
			_countof(errorText) - errorCur,
			L"Couldn't read the configuration file, '%s'\n\n",
			pConfigFile);

		success = FALSE;
	}

	if (errors)
	{
		success = FALSE;

		errorCur += swprintf(errorText + errorCur,
			_countof(errorText) - errorCur,
			L"There were syntax errors in configuration file '%s':\n\n",
			pConfigFile);
		
		for (const PARSING_ERROR* e = errors; e; e = e->pNext)
		{
			errorCur += swprintf(errorText + errorCur,
				_countof(errorText) - errorCur,
				L"On line %u: %s\n",
				e->LineNumber,
				e->ErrorText);
		}

		errorCur += swprintf(errorText + errorCur,
			_countof(errorText) - errorCur,
			L"\n");
	}

	FreeParsingErrors(errors);

	// Apply the configuration. Register the new hotkeys.
	if (success)
	{
		RegisterHotKeys(pCfg, pRegistration);
		if (pRegistration->ErrorCount > 0)
		{
			AppendRegistrationFailures(pRegistration, errorText + errorCur, 
				_countof(errorText) - errorCur);
			success = FALSE;
		}
	}

	if (success)
	{
		return CONFIG_LOADED;
	}
	else
	{
		errorCur += swprintf(errorText + errorCur,
			_countof(errorText) - errorCur,
			L"Abort: quit " APPLICATION_TITLE L".\n"
			L"Retry: attempt to load the configuration file again.\n"
			L"Ignore: continue with the currently loaded configuration.\n");

		return MessageBox(NULL, errorText, APPLICATION_TITLE, MB_ICONERROR | MB_ABORTRETRYIGNORE | MB_DEFBUTTON2);
	}
}

BOOL LoadConfigurationAndBind(LPCWSTR pConfigFile, AWHK_APP_CONFIG* pCfg, AWHK_REGISTRATION* pRegistration, BOOL ignoreMissingConfig)
{
	AWHK_APP_CONFIG newCfg;
	AWHK_REGISTRATION newReg = *pRegistration;

	CONFIG_LOAD_RESULT result;
	do
	{
		// Unregister any existing hotkeys
		UnregisterHotkeys(&newReg);

		// Create a new configuration
		InitConfiguration(&newCfg);

		// Get the current state of autologin
		newCfg.LaunchOnStartUp = IsAutoLoginEnabled();

		// Create a new registration
		ZeroMemory(&newReg, sizeof(newReg));

		// Try and load the config and if there are errors, the user might want to retry
		result = AttemptConfigurationAndBind(pConfigFile, &newCfg, &newReg, ignoreMissingConfig);
	}
	while (result == CONFIG_RETRY);

	// Configuration is committed.
	switch (result)
	{
	case CONFIG_LOADED:
		*pCfg = newCfg;
		*pRegistration = newReg;

		// Set autologin state
		SetAutoLoginEnabled(pCfg->LaunchOnStartUp);
		return TRUE;
	case CONFIG_ABORT:
		// Quit the app
		return FALSE;
	case CONFIG_IGNORE:
		// Ignore everything. Just rebind the old config.
		RegisterHotKeys(pCfg, pRegistration);
		// todo: show an error here or something
		return pRegistration->ErrorCount == 0;
	default:
		assert(0);
		return FALSE;
	}
}

// Get the path to the INI file.
PWSTR GetConfigFilePath()
{
	PWSTR result = NULL;

	// Get the user data directory
	PWSTR userDataDir = NULL;
	if (SUCCEEDED(SHGetKnownFolderPath(&FOLDERID_LocalAppData, 0, NULL, &userDataDir)))
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

BOOL PostEditConfigFile( const AWHK_APP_STATE* pState )
{
	return PostThreadMessage(
		pState->dwMainThreadID,
		pState->MsgEditConfigFile,
		0, 0 );
}

BOOL PostReloadConfig(const AWHK_APP_STATE* pState)
{
	if (!pState->Suspended)
	{
		if (MessageBox(NULL, L"Reload the configuration file?", APPLICATION_TITLE, MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			return PostThreadMessage(pState->dwMainThreadID, pState->MsgSuspend, 0, 0)
				&& PostThreadMessage(pState->dwMainThreadID, pState->MsgResume, 0, 0);
		}
		return TRUE;
	}
	return FALSE;
}

BOOL PostQuitPrompt()
{
	if (MessageBox(NULL,
		L"Are you sure you want to quit " APPLICATION_TITLE L"?",
		APPLICATION_TITLE,
		MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2) == IDYES)
	{
		PostQuitMessage(EXIT_SUCCESS);
	}
	return TRUE;
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
	// If the app is suspended, ignore hotkeys
	if (state->Suspended)
	{
		return FALSE;
	}

	// Deal with system keys
	if (key.dwBits == cfg->HelpCombo.dwBits)
		return ShowWebHelp();
	else if (key.dwBits == cfg->EditConfigCombo.dwBits)
		return PostEditConfigFile( state );
	else if (key.dwBits == cfg->ReloadConfigCombo.dwBits)
		return PostReloadConfig(state);
	else if (key.dwBits == cfg->QuitCombo.dwBits)
		return PostQuitPrompt();
	else if (key.dwBits == cfg->MediaPlayPause.dwBits)
        return MediaPlayPause();
    else if (key.dwBits == cfg->MediaStop.dwBits)
        return MediaStop();
    else if (key.dwBits == cfg->MediaNext.dwBits)
        return MediaNextTrack();
    else if (key.dwBits == cfg->MediaPrev.dwBits)
        return MediaPrevTrack();
    else if (key.dwBits == cfg->MediaVolumeMute.dwBits)
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
	RegisterKeyComboAtIndex( &cfg->ReloadConfigCombo, pKeyStatus );
	RegisterKeyComboAtIndex( &cfg->EditConfigCombo,   pKeyStatus );
	RegisterKeyComboAtIndex( &cfg->QuitCombo,		  pKeyStatus );
	RegisterKeyComboAtIndex( &cfg->MediaPlayPause,    pKeyStatus );
	RegisterKeyComboAtIndex( &cfg->MediaStop,         pKeyStatus );
	RegisterKeyComboAtIndex( &cfg->MediaNext,         pKeyStatus );
	RegisterKeyComboAtIndex( &cfg->MediaPrev,         pKeyStatus );
	RegisterKeyComboAtIndex( &cfg->MediaVolumeMute,   pKeyStatus );
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

        if (msg.message == appState->MsgSuspend && !appState->Suspended)
        {
            UnregisterHotkeys( &appState->Registration );
			appState->Suspended = TRUE;
            continue;
        }
        else if (msg.message == appState->MsgResume && appState->Suspended)
        {
			appState->Suspended = FALSE;

			// Reload the settings
			if (!LoadConfigurationAndBind(appState->ConfigFile, appCfg, &appState->Registration, TRUE))
			{
				return EXIT_BAD_CONFIG;
			}

			continue;
        }

		switch ( msg.message )
		{
		case WM_QUIT:
			{
				return EXIT_SUCCESS;
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

	return EXIT_SUCCESS;
}

void Shutdown(AWHK_APP_STATE* appState)
{
	UnregisterHotkeys( &appState->Registration );
	CloseHandle( appState->hAppOpenSemaphore );
	HeapFree(GetProcessHeap(), 0, appState->ConfigFile);
}

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	UNUSED(hPrevInstance);
	UNUSED(lpCmdLine);
	UNUSED(nCmdShow);

	if ( AppAlreadyOpenCheck() )
		return EXIT_ALREADY_RUNNING;

	if (FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)))
		return EXIT_COM_INIT_FAILURE;

	AWHK_APP_STATE appState;
    ZeroMemory( &appState, sizeof(appState) );
	appState.hInstance = hInstance;
	appState.dwMainThreadID = GetCurrentThreadId();
	appState.MsgEditConfigFile = RegisterWindowMessage( L"AWHKEditConfigMsg" );
	appState.MsgSuspend = RegisterWindowMessage( L"AWHKSuspendMsg" );
	appState.MsgResume = RegisterWindowMessage( L"AWHKResumeMsg" );
	appState.ConfigFile = GetConfigFilePath();
	appState.Suspended = FALSE;

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
	InitConfiguration(&appCfg);

	// Attempt to load the config
	if (!LoadConfigurationAndBind(appState.ConfigFile, &appCfg, &appState.Registration, TRUE))
	{
		Shutdown(&appState);
		return EXIT_BAD_CONFIG;
	}

	int ret = MessageLoop( &appState, &appCfg );

	Shutdown(&appState);

	return ret;
}

