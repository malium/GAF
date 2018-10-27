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

#ifndef GAF_STRING_UTILS_H
#define GAF_STRING_UTILS_H 1

#include "GAF/GAFPrerequisites.h"

namespace gaf
{
	namespace StringUtils
	{
		typedef std::vector<std::string> StringVec;
		typedef std::vector<std::wstring> StringVecW;

		std::wstring s2ws(const std::string& str);
		std::string ws2s(const std::wstring& str);

		SIZET Contains(const std::string& str, ANSICHAR token);
		SIZET Contains(const std::wstring& wstr, WIDECHAR token);
		SIZET Contains(const std::string& str, const std::string& token);
		SIZET Contains(const std::wstring& wstr, const std::wstring& token);

		StringVec SeparateBySpace(const std::string& str);
		StringVecW SeparateBySpace(const std::wstring& wstr);

		std::string ComposeString(const StringVec& vec, ANSICHAR separator);
		std::wstring ComposeString(const StringVecW& vec, WIDECHAR separator);
		std::string ComposeString(const StringVec& vec, const std::string& separator);
		std::wstring ComposeString(const StringVecW& vec, const std::wstring& separator);

		StringVec Tokenize(const std::string& str, ANSICHAR token);
		StringVecW Tokenize(const std::wstring& str, WIDECHAR token);
		StringVec Tokenize(const std::string& str, const std::string& token);
		StringVecW Tokenize(const std::wstring& str, const std::wstring& token);

		std::string Replace(const std::string& str, ANSICHAR toReplace, ANSICHAR replacement);
		std::wstring Replace(const std::wstring& str, WIDECHAR toReplace, WIDECHAR replacement);
		std::string Replace(const std::string& str, const std::string& toReplace, const std::string& replacement);
		std::wstring Replace(const std::wstring& str, const std::wstring& toReplace, const std::wstring& replacement);
	}
}

#endif /* GAF_STRING_UTILS_H */