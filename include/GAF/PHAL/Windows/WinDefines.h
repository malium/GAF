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

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "Imm32.lib")
#pragma comment(lib, "imagehlp.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "version.lib")
#pragma comment(lib, "uuid.lib")

/* Function type macros */
#define FORCEINLINE __forceinline									/* Force code to be inline */
#define FORCENOINLINE __declspec(noinline)							/* Force code to NOT be inline */
#define FUNCTION_NO_RETURN_START __declspec(noreturn)				/* Indicate that the function never returns. */

#ifndef TRIGGER_BREAKPOINT
extern void CDECL __debugbreak();
#define TRIGGER_BREAKPOINT() __debugbreak()
#endif

/* String constants */
#ifndef LINE_TERMINATOR
#define LINE_TERMINATOR "\r\n"
#endif
#ifndef LINE_TERMINATOR_WIDE
#define LINE_TERMINATOR_WIDE L"\r\n"
#endif
#ifndef PATH_SEPARATOR
#define PATH_SEPARATOR "\\"
#endif
#ifndef PATH_SEPARATOR_WIDE
#define PATH_SEPARATOR_WIDE L"\\"
#endif

#if GREAPER_DEBUG_ALLOCATION
#ifdef GREAPER_MEMORY_DEBUG_BEGIN
#undef GREAPER_MEMORY_DEBUG_BEGIN
#endif
#define GREAPER_MEMORY_DEBUG_BEGIN()\
_CrtMemState state;\
_CrtMemCheckpoint(&state)

#ifdef GREAPER_MEMORY_DEBUG_END
#undef GREAPER_MEMORY_DEBUG_END
#endif
#define GREAPER_MEMORY_DEBUG_END()\
_CrtMemDumpAllObjectsSince(&state)
#endif

#ifndef DLLIMPORT
#define DLLIMPORT __declspec(dllimport)
#endif
#ifndef DLLEXPORT
#define DLLEXPORT __declspec(dllexport)
#endif