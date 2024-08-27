/*

    Copyright (c) StellarDX Astronomy. All rights reserved.

    Module Name:

        IGDef.h

    Abstract:

        Master include file for Definitions and Settings of InfoGen.

*/

#pragma once

#ifndef _INFOGEN_DEF
#define _INFOGEN_DEF

#if defined (WIN32) | defined (WIN64)
//  巨硬
#include <Windows.h>
#define IGFORCEU8 system("chcp 65001")
#define _IGEXPORT __declspec(dllexport)
#define _IGIMPORT __declspec(dllimport)
#define IGCALL __stdcall
#undef ERROR // Conflict with CSE logger
#elif defined(__GNUC__)
//  GCC
#include <dlfcn.h>
#define IGFORCEU8
#define _IGEXPORT __attribute__((visibility("default")))
#define _IGIMPORT
#define IGCALL
#define HMODULE void*
#define LoadLibrary dlopen
#define GetProcAddress dlsym
#define FreeLibrary dlclose
typedef char *LPSTR, *PSTR;
typedef const char *LPCSTR, *PCSTR;
#else
#error InfoGen is not supported on this platform
#endif

#ifdef IGEXPORTDEF
#define IGEXPORT _IGEXPORT
#else
#define IGEXPORT _IGIMPORT
#endif

#ifndef _EXTERN_C
#define _EXTERN_C extern "C" {
#define _END_EXTERN_C }
#endif

using MainFuncType = void(*)(int argc, char const* const* argv);

// 颜色设置
#define _WARN_COLOR  "\033[38;2;252;200;16m"
#define _ERROR_COLOR "\033[38;2;221;26;32m"
#define _FATAL_COLOR "\033[38;2;254;226;223;48;2;167;23;50m"
#define _TITLE_COLOR "\033[38;2;166;126;183m"/*"\033[38;2;108;33;109m"*/
#define _TXT_COLOR   "\033[38;2;63;148;228m"
#define _NOTE_COLOR  "\033[38;2;113;195;157m"

#define _WARN(Str)  _WARN_COLOR Str "\033[39m"
#define _ERROR(Str) _ERROR_COLOR Str "\033[39m"
#define _FATAL(Str) _FATAL_COLOR Str "\033[39;49m"
#define _TITLE(Str) _TITLE_COLOR Str "\033[0m"
#define _TXT(Str)   _TXT_COLOR Str "\033[0m"
#define _NOTE(Str)  _NOTE_COLOR Str "\033[0m"

#define _DATA_DIR(Str) "InfoGen_Data/" Str

#endif
