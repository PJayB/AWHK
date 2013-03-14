// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <iostream>
#include <map>
#include <string>
#include <strsafe.h>

namespace std {
  #if defined _UNICODE || defined UNICODE
    typedef wstring tstring;
  #else
    typedef string tstring;
  #endif
}

#include <Windows.h>