// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#ifdef _WIN32 
    #include "targetver.h"
    #define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
    // Windows Header Files
    #include <windows.h>
    #include <tchar.h>
#else
#   define CP_UTF8 65001
#   define GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS 0x00000004
#   define HANDLE void *
#   define HINSTANCE void *
#   define SOCKET unsigned int
#   define WSAEWOULDBLOCK 10035
    typedef unsigned short WCHAR;
#endif
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>