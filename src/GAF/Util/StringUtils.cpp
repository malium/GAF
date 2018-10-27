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

#include "GAF/Util/StringUtils.h"
#include <codecvt>

using namespace gaf;

std::wstring StringUtils::s2ws(const std::string& str)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	return conv.from_bytes(str);
}

std::string StringUtils::ws2s(const std::wstring& str)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	return conv.to_bytes(str);
}

SIZET StringUtils::Contains(const std::string & str, const ANSICHAR token)
{
	SIZET count = 0;
	for (auto it = str.begin(); it != str.end(); ++it)
	{
		if (*it == token)
			++count;
	}
	return count;
}

SIZET StringUtils::Contains(const std::wstring & wstr, WIDECHAR token)
{
	SIZET count = 0;
	for (auto it = wstr.begin(); it != wstr.end(); ++it)
	{
		if (*it == token)
			++count;
	}
	return count;
}

SIZET StringUtils::Contains(const std::string & str, const std::string & token)
{
	SIZET count = 0;
	for (auto it = str.begin(); it != str.end(); ++it)
	{
		auto itt = it;
		bool match = true;
		for (auto ittt = token.begin(); ittt != token.end(); ++ittt)
		{
			if ((*itt) != (*ittt))
			{
				match = false;
				break;
			}
			++itt;
		}
		if (match)
			++count;
	}
	return count;
}

SIZET StringUtils::Contains(const std::wstring & wstr, const std::wstring & token)
{
	SIZET count = 0;
	for (auto it = wstr.begin(); it != wstr.end(); ++it)
	{
		auto itt = it;
		bool match = true;
		for (auto ittt = token.begin(); ittt != token.end(); ++ittt)
		{
			if ((*itt) != (*ittt))
			{
				match = false;
				break;
			}
			++itt;
		}
		if (match)
			++count;
	}
	return count;
}

StringUtils::StringVec StringUtils::SeparateBySpace(const std::string & str)
{
	return Tokenize(str, ' ');
}

StringUtils::StringVecW StringUtils::SeparateBySpace(const std::wstring & wstr)
{
	return Tokenize(wstr, L' ');
}

std::string StringUtils::ComposeString(const StringVec & vec, const ANSICHAR separator)
{
	std::string rtn;
	for (SIZET i = 0; i < vec.size(); ++i)
	{
		rtn += vec[i];
		if (i != (vec.size() - 1))
			rtn += separator;
	}

	return rtn;
}

std::wstring StringUtils::ComposeString(const StringVecW& vec, const WIDECHAR separator)
{
	std::wstring rtn;
	for (SIZET i = 0; i < vec.size(); ++i)
	{
		rtn += vec[i];
		if (i != (vec.size() - 1))
			rtn += separator;
	}

	return rtn;
}

std::string StringUtils::ComposeString(const StringVec & vec, const std::string & separator)
{
	std::string rtn;
	for (SIZET i = 0; i < vec.size(); ++i)
	{
		rtn += vec[i];
		if (i != (vec.size() - 1))
			rtn += separator;
	}
	return rtn;
}

std::wstring StringUtils::ComposeString(const StringVecW & vec, const std::wstring & separator)
{
	std::wstring rtn;
	for (SIZET i = 0; i < vec.size(); ++i)
	{
		rtn += vec[i];
		if (i != (vec.size() - 1))
			rtn += separator;
	}
	return rtn;
}

StringUtils::StringVec StringUtils::Tokenize(const std::string & str, const ANSICHAR token)
{
	StringVec rtn;
	auto lastToken = str.begin();
	for (auto it = str.begin(); it != str.end(); ++it)
	{
		if ((*it) == token)
		{
			rtn.emplace_back(str.substr(std::distance(str.begin(), lastToken), std::distance(lastToken, it)));
			lastToken = it;
		}
	}
	return rtn;
}

StringUtils::StringVecW StringUtils::Tokenize(const std::wstring & str, const WIDECHAR token)
{
	StringVecW rtn;
	auto lastToken = str.begin();
	for (auto it = str.begin(); it != str.end(); ++it)
	{
		if ((*it) == token)
		{
			rtn.emplace_back(str.substr(std::distance(str.begin(), lastToken), std::distance(lastToken, it)));
			lastToken = it;
		}
	}
	return rtn;
}

StringUtils::StringVec StringUtils::Tokenize(const std::string & str, const std::string& token)
{
	StringVec rtn;
	auto lastToken = str.begin();
	const auto tokenSize = token.size();
	for (auto it = str.begin(); it != str.end(); ++it)
	{
		if ((size_t)std::distance(it, str.end()) < tokenSize)
		{
			rtn.emplace_back(str.substr(std::distance(str.begin(), lastToken)));
			break;
		}
		auto itt = it;
		bool match = true;
		for (auto ittt = token.begin(); ittt != token.end(); ++ittt)
		{
			if ((*itt) != (*ittt))
			{
				match = false;
				break;
			}
			++itt;
		}
		if (match)
		{
			rtn.emplace_back(str.substr(std::distance(str.begin(), lastToken), tokenSize));
			it += tokenSize;
		}
	}
	return rtn;
}

StringUtils::StringVecW StringUtils::Tokenize(const std::wstring & str, const std::wstring& token)
{
	StringVecW rtn;
	auto lastToken = str.begin();
	const auto tokenSize = token.size();
	for (auto it = str.begin(); it != str.end(); ++it)
	{
		if ((size_t)std::distance(it, str.end()) < tokenSize)
		{
			rtn.emplace_back(str.substr(std::distance(str.begin(), lastToken)));
			break;
		}
		auto itt = it;
		bool match = true;
		for (auto ittt = token.begin(); ittt != token.end(); ++ittt)
		{
			if ((*itt) != (*ittt))
			{
				match = false;
				break;
			}
			++itt;
		}
		if (match)
		{
			rtn.emplace_back(str.substr(std::distance(str.begin(), lastToken), tokenSize));
			it += tokenSize;
		}
	}
	return rtn;
}

std::string StringUtils::Replace(const std::string & str, const ANSICHAR toReplace, const ANSICHAR replacement)
{
	return ComposeString(Tokenize(str, toReplace), replacement);
}

std::wstring StringUtils::Replace(const std::wstring & str, const WIDECHAR toReplace, const WIDECHAR replacement)
{
	return ComposeString(Tokenize(str, toReplace), replacement);
}

std::string StringUtils::Replace(const std::string & str, const std::string & toReplace, const std::string & replacement)
{
	return ComposeString(Tokenize(str, toReplace), replacement);
}

std::wstring StringUtils::Replace(const std::wstring & str, const std::wstring & toReplace, const std::wstring & replacement)
{
	return ComposeString(Tokenize(str, toReplace), replacement);
}
