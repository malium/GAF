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

#ifndef GAF_DIALOG_H
#define GAF_DIALOG_H 1

namespace gaf
{
	class Dialog
	{
	public:
		static void Create(const ANSICHAR*const title, const ANSICHAR*const content)
		{
#if PLATFORM_WINDOWS
			MessageBoxA(nullptr, content, title, MB_TASKMODAL | MB_ICONASTERISK | MB_OK);
#else
			std::cerr << "[" << title << "]" << content << LINE_TERMINATOR;
#endif
		}

		static void Create(const WIDECHAR*const title, const WIDECHAR*const content)
		{
#if PLATFORM_WINDOWS
			MessageBoxW(nullptr, content, title, MB_TASKMODAL | MB_ICONASTERISK | MB_OK);
#else
			std::wcerr << "[" << title << "]" << content << LINE_TERMINATOR_WIDE;
#endif
		}
	};
}

#endif /* GAF_DIALOG_H */