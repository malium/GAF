/***********************************************************************************
* Copyright 2018 Marcos Sánchez Torrent                                            *
*                                                                                  *
* Licensed under the Apache License, Version 2.0 (the "License");                  *
* you may not use this file except in compliance with the License.                 *
* You may obtain a copy of the License at                                          *
*                                                                                  *
* http://www.apache.org/licenses/LICENSE-2.0                                       *
*                                                                                  *
* Unless required by applicable law or agreed to in writing, software              *
* distributed under the License is distributed on an "AS IS" BASIS,                *
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.         *
* See the License for the specific language governing permissions and              *
* limitations under the License.                                                   *
***********************************************************************************/

#pragma once

#ifdef GREAPER_APPLICATION_FRAMEWORK_VERSION
#error "Version already defined!"
#else
#define GREAPER_APPLICATION_FRAMEWORK_MAJOR 0
#define GREAPER_APPLICATION_FRAMEWORK_MINOR 0
#define GREAPER_APPLICATION_FRAMEWORK_PATCH 1
#define GREAPER_APPLICATION_FRAMEWORK_REV	2
#endif


/***********************************************************************************
*                            LANGUAJE DETECTION                                    *
***********************************************************************************/

#if __cplusplus < 201103L
#if defined(_MSC_VER)
#if _MSC_VER < 1900
#error "Unsopported C++ version!"
#endif
#else
#error "Unsopported C++ version!"
#endif
#endif

#if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_) && !defined(_ARM_) && defined(_M_AMD64)
#define ARCHITECTURE_X64 1
#else
#define ARCHITECTURE_X64 0
#endif

#if !ARCHITECTURE_X64
#error "Unsopported Architecture!"
#endif

/***********************************************************************************
*                             COMPILER DETECTION                                   *
***********************************************************************************/

#ifndef COMPILER_CLANG
#if defined(__clang__)
#define COMPILER_CLANG 1
#define COMPILER_VERSION ((__clang_major__ * 10) + __clang_minor__)
#else
#define COMPILER_CLANG 0
#endif
#endif

#ifndef COMPILER_GCC
#if defined(__GNUC__) && !defined(COMPILER_VERSION)
#define COMPILER_GCC 1
#define COMPILER_VERSION __GNUC__
#else
#define COMPILER_GCC 0
#endif
#endif

#ifndef COMPILER_ICC
#if (defined(__INTEL_COMPILER) || defined(__ICC) || defined(__ECC) || defined(__ICL)) && !defined(COMPILER_VERSION)
#define COMPILER_ICC 1
#define COMPILER_VERSION __INTEL_COMPILER
#else
#define COMPILER_ICC 0
#endif
#endif

#ifndef COMPILER_MSVC
#if defined(_MSC_VER) && !defined(COMPILER_VERSION)
#define COMPILER_MSVC 1
#define COMPILER_VERSION _MSC_VER
#else
#define COMPILER_MSVC 0
#endif
#endif

#ifndef COMPILER_VERSION
#error "Unknown compiler!"
#endif

/***********************************************************************************
*                             PLATFORM DETECTION                                   *
***********************************************************************************/

#ifndef PLATFORM_WINDOWS
#if (defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__))
#define PLATFORM_WINDOWS 1
#else
#define PLATFORM_WINDOWS 0
#endif
#endif

#ifndef PLATFORM_LINUX
#if (defined(linux) || defined(__linux__) || defined(__linux))
#define PLATFORM_LINUX 1
#else
#define PLATFORM_LINUX 0
#endif
#endif

#ifndef PLATFORM_MAC
#if (defined(macintosh) || defined(Macintosh) || (defined(__APPLE__) && defined(__MACH__)))
#include "TargetConditionals.h"
#if TARGET_OS_MAC
#define PLATFORM_MAC 1
#else
#define PLATFORM_MAC 0
#endif
#else
#define PLATFORM_MAC 0
#endif
#endif

/* DEBUG MACRO */
#ifndef GREAPER_DEBUG
#if defined(_DEBUG) || defined(DEBUG)
#define GREAPER_DEBUG 1
#else
#define GREAPER_DEBUG 0
#endif
#endif

/* MSVC Compiler warnings disable */
#if COMPILER_MSVC
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#endif
// Add compiler warnings to disable


#endif

/* Standard C function */
#ifndef CDECL
#if COMPILER_CLANG || COMPILER_GCC
#define CDECL __attribute__((cdecl))
#elif COMPILER_ICC || COMPILER_MSVC
#define CDECL __cdecl
#else
#define CDECL
#endif
#endif

/* Standard calling convention */
#ifndef STDCALL
#if COMPILER_CLANG || COMPILER_GCC
#define STDCALL __attribute__((stdcall))
#elif COMPILER_ICC || COMPILER_MSVC
#define STDCALL __stdcall
#else
#define STDCALL
#endif
#endif

/* Thread local variable declaration */
#ifndef GREAPER_THLOCAL
#if COMPILER_MSVC || (COMPILER_ICC && PLATFORM_WINDOWS)
#define GREAPER_THLOCAL __declspec(thread)
#else
#define GREAPER_THLOCAL __thread
#endif
#endif

#include "GAF/PHAL/Config.h"
#include "GAF/PHAL/BasicTypes.h"

#if PLATFORM_WINDOWS
#include "GAF/PHAL/Windows/WinDefines.h"
#include "GAF/PHAL/Windows/WinIncludes.h"
#include "GAF/PHAL/Windows/WinTypes.h"
#include "GAF/PHAL/Windows/WinUtil.h"
#elif PLATFORM_MAC
#include "GAF/PHAL/Mac/MacDefines.h"
#include "GAF/PHAL/Mac/MacIncludes.h"
#include "GAF/PHAL/Mac/MacTypes.h"
#include "GAF/PHAL/Mac/MacUtil.h"
#elif PLATFORM_LINUX
#include "GAF/PHAL/Linux/LinuxDefines.h"
#include "GAF/PHAL/Linux/LinuxIncludes.h"
#include "GAF/PHAL/Linux/LinuxTypes.h"
#include "GAF/PHAL/Linux/LinuxUtil.h"
#else
#error "Unknown Platform!"
#endif

typedef PlatformTypes::uint8			uint8;
typedef PlatformTypes::uint16			uint16;
typedef PlatformTypes::uint32			uint32;
typedef PlatformTypes::uint64			uint64;
typedef PlatformTypes::int8				int8;
typedef PlatformTypes::int16			int16;
typedef PlatformTypes::int32			int32;
typedef PlatformTypes::int64			int64;

typedef PlatformTypes::ANSICHAR			ANSICHAR;
typedef PlatformTypes::WIDECHAR			WIDECHAR;
typedef PlatformTypes::CHAR8			UTF8CHAR;
typedef PlatformTypes::CHAR16			UTF16CHAR;
typedef PlatformTypes::CHAR32			UTF32CHAR;

typedef PlatformTypes::PTRUINT			PTRUINT;
typedef PlatformTypes::PTRINT			PTRINT;

typedef PlatformTypes::SIZET			SIZET;
typedef PlatformTypes::SSIZET			SSIZET;

/***********************************************************************************
*                                    ENDIANESS                                     *
***********************************************************************************/

#ifndef PLATFORM_ENDIANESS
#define PLATFORM_LITTLE_ENDIAN 1
#define PLATFORM_BIG_ENDIAN 2
#define PLATFORM_ENDIANESS PLATFORM_LITTLE_ENDIAN
#endif

/* Source Anotations */
#if COMPILER_MSVC
#include <sal.h>
#define IN_BYTECOUNT(x)		_In_bytecount_(x)
#define INOUT_Z_CAP(x)		_Inout_z_cap_(x)
#define OUT_Z_CAP(x)		_Out_z_cap_(x)
#define OUT_CAP(x)			_Out_cap_(x)
#define OUT_BYTECAP(x)		_Out_bytecap_(x)
#define OUT_Z_BYTECAP(x)	_Out_z_bytecap_(x)
#define PRINTF_FORMAT_STRING _Printf_format_string_
#define SCANF_FORMAT_STRING _Scanf_format_string_impl_
#else
#define IN_BYTECOUNT(x)
#define INOUT_Z_CAP(x)
#define OUT_Z_CAP(x)
#define OUT_CAP(x)
#define OUT_BYTECAP(x)
#define OUT_Z_BYTECAP(x)
#define PRINTF_FORMAT_STRING
#define SCANF_FORMAT_STRING
#endif

#include <cstdarg>

/* Print and ScanF VARARGS */
#if COMPILER_GCC
#define PRINTF_VARARG_FUNC(fmtargnumber) __attribute__ (( format( __printf__, fmtargnumber, fmtargnumber+1 )))
#define SCANF_VARARG_FUNC(fmtargnumber) __attribute__ (( format( __scanf__, fmtargnumber, fmtargnumber+1 )))
#else
#define PRINTF_VARARG_FUNC(fmtargnumber)
#define SCANF_VARARG_FUNC(fmtargnumber)
#endif

/* MEMORY DEBUG */
#ifndef GREAPER_MEMORY_DEBUG_BEGIN
#define GREAPER_MEMORY_DEBUG_BEGIN()
#endif

#ifndef GREAPER_MEMORY_DEBUG_END
#define GREAPER_MEMORY_DEBUG_END()
#endif

/* Full function display */
#define FUNCTION_FULL __PRETTY_FUNCTION__

/* Dynamic Link library macro */
#ifndef DLLIMPORT
#define DLLIMPORT
#endif
#ifndef DLLEXPORT
#define DLLEXPORT
#endif

/* Force code to be inlined */
#ifndef FORCEINLINE
#define FORCEINLINE
#endif
/* Force code to NOT be inlined */
#ifndef FORCENOINLINE
#define FORCENOINLINE
#endif
/* Indicate that the function never returns. */
#ifndef FUNCTION_NO_RETURN_START
#define FUNCTION_NO_RETURN_START
#endif
/* Wrap a function signature in this to indicate that the function never returns. */
#ifndef FUNCTION_NO_RETURN_END
#define FUNCTION_NO_RETURN_END
#endif

/* CPU cache line size, important to use in order to avoid cache misses */
#ifndef CACHE_LINE_SIZE
#if ARCHITECTURE_X64
#define CACHE_LINE_SIZE 128
#else
#define CACHE_LINE_SIZE 64
#endif
#endif

/* String constants */
#ifndef LINE_TERMINATOR
#define LINE_TERMINATOR "\n"
#endif
#ifndef LINE_TERMINATOR_WIDE
#define LINE_TERMINATOR_WIDE L"\n"
#endif
#ifndef PATH_SEPARATOR
#define PATH_SEPARATOR "/"
#endif
#ifndef PATH_SEPARATOR_WIDE
#define PATH_SEPARATOR_WIDE L"/"
#endif

/* Alignment */
#ifndef GCC_PACK
#if (COMPILER_CLANG || COMPILER_GCC) && !COMPILER_MSVC
#define GCC_PACK(n) __attribute__((packed,aligned(n)))
#else
#define GCC_PACK(n)
#endif
#endif

#ifndef MS_ALIGN
#if COMPILER_MSVC || (COMPILER_CLANG && PLATFORM_WINDOWS)
#define MS_ALIGN(n) __declspec(align(n))
#else
#define MS_ALIGN(n)
#endif
#endif

#ifndef GCC_ALIGN
#if (COMPILER_CLANG || COMPILER_GCC) && !COMPILER_MSVC
#define GCC_ALIGN(n) __attribute__((aligned(n)))
#else
#define GCC_ALIGN(n)
#endif
#endif

/* Debug breakpoint */
#ifndef TRIGGER_BREAKPOINT
#define TRIGGER_BREAKPOINT() __asm__ __volatile__ ( "int $3\n\t" )
#endif

#include "GAF/PHAL/Utilities.h"

/* Standard includes */
#include <algorithm>
#include <array>
#include <atomic>
#include <bitset>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <fstream>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <iomanip>
#include <limits>
#include <locale>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <random>
#include <shared_mutex>
#include <sstream>
#include <stack>
#include <string>
#include <strstream>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>