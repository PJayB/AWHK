#pragma once

typedef void (* ASYNC_FORM_CLOSED_PROC)( 
	LPVOID pUserData );

BOOL ShowSettingsDialogAsync(
	ASYNC_FORM_CLOSED_PROC pfCallback,
	LPVOID pUserData );

