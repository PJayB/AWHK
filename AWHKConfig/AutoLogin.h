#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

BOOL IsAutoLoginEnabled();
BOOL SetAutoLoginEnabled( BOOL bEnabled );
