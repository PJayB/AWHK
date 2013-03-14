#include "stdafx.h"
#include "SupportModule.h"

typedef void (* SUPPORTLIB_SHOW_DIALOG_PROC)();

BOOL GetSupportLibraryFilename( LPWSTR strModuleFile, SIZE_T nNumChars );


class SupportModuleImpl
{
public:

	HMODULE hLib;

	SupportModuleImpl()
		: hLib(nullptr)
	{
		TCHAR strLibName[MAX_PATH];
		if ( !GetSupportLibraryFilename( strLibName, MAX_PATH ) )
			return;

		// Load the library
		hLib = ::LoadLibrary( strLibName );
	}

	~SupportModuleImpl()
	{
		if ( hLib )
			::FreeLibrary( hLib );
	}
};

SupportModule::SupportModule()
	: pImpl( new SupportModuleImpl() )
{

}


SupportModule::~SupportModule()
{
	delete pImpl;
}

BOOL SupportModule::ShowSettingsDialog()
{
	if ( !pImpl || !pImpl->hLib )
		return FALSE;

	SUPPORTLIB_SHOW_DIALOG_PROC pfShowSettingsDlg = (SUPPORTLIB_SHOW_DIALOG_PROC)
		::GetProcAddress(
			pImpl->hLib,
			"ShowSettingsDialog" );

	if ( !pfShowSettingsDlg )
		return FALSE;

	pfShowSettingsDlg();

	return TRUE;
}




BOOL GetSupportLibraryFilename( LPWSTR strModuleFile, SIZE_T nNumChars )
{
	SIZE_T nModuleFileLen = ::GetModuleFileName( 
		nullptr, 
		strModuleFile, 
		nNumChars );
	if ( ::GetLastError() == ERROR_INSUFFICIENT_BUFFER )
		return FALSE;

	// Find the last \\ .
	LPWSTR strFilename;
	for ( strFilename = strModuleFile + nModuleFileLen; strFilename >= strModuleFile; --strFilename )
	{
		if ( *strFilename == '\\' ) 
		{
			strFilename++;
			break;
		}
	}

	SIZE_T nRemainingSize = nNumChars - ( strFilename - strModuleFile );

	// Concatenate our DLL name
	StringCbCopy( strFilename, nRemainingSize, L"SupportLibrary.dll" );

	return TRUE;
}

