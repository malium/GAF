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

#ifndef GAF_ASSERTION_H
#define GAF_ASSERTION_H 1

#include "GAF/Dialog.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>

namespace gaf
{
	class Assertion
	{
		static void LogAssertion(const ANSICHAR* msg);
		static void Assert(const ANSICHAR*const message, va_list arglist)
		{
#if GREAPER_DEBUG_ASSERTIONS
			TRIGGER_BREAKPOINT();
#endif
			std::string msg;
			msg.resize(strlen(message) + 2048);
			vsnprintf(&msg[0], msg.size(), message, arglist);
			Dialog::Create("Greaper Assertion", msg.c_str());

			LogAssertion(msg.c_str());
			exit(EXIT_FAILURE);
		}
	public:
		static void Always(PRINTF_FORMAT_STRING const ANSICHAR*const message, ...)
		{
			va_list arglist;
			va_start(arglist, message);
			Assert(message, arglist);
			va_end(arglist);
		}
		static void WhenTrue(bool expression, PRINTF_FORMAT_STRING const ANSICHAR*const message, ...)
		{
			if (expression)
			{
				va_list arglist;
				va_start(arglist, message);
				Assert(message, arglist);
				va_end(arglist);
			}
		}
		template<typename T, typename Y>
		static void WhenInequal(const T left, const Y right, PRINTF_FORMAT_STRING const ANSICHAR*const message, ...)
		{
			if (left != right)
			{
				va_list arglist;
				va_start(arglist, message);
				Assert(message, arglist);
				va_end(arglist);
			}
		}
		template<typename T, typename Y>
		static void WhenEqual(const T left, const Y right, PRINTF_FORMAT_STRING const ANSICHAR*const message, ...)
		{
			if (left == right)
			{
				va_list arglist;
				va_start(arglist, message);
				Assert(message, arglist);
				va_end(arglist);
			}
		}
		template<typename T, typename Y>
		static void WhenGreater(const T left, const Y right, PRINTF_FORMAT_STRING const ANSICHAR*const message, ...)
		{
			if (left > (T)right)
			{
				va_list arglist;
				va_start(arglist, message);
				Assert(message, arglist);
				va_end(arglist);
			}
		}
		template<typename T, typename Y>
		static void WhenLess(const T left, const Y right, PRINTF_FORMAT_STRING const ANSICHAR*const message, ...)
		{
			if (left < (T)right)
			{
				va_list arglist;
				va_start(arglist, message);
				Assert(message, arglist);
				va_end(arglist);
			}
		}
		template<typename T, typename Y>
		static void WhenGreaterEqual(const T left, const Y right, PRINTF_FORMAT_STRING const ANSICHAR*const message, ...)
		{
			if (left >= (T)right)
			{
				va_list arglist;
				va_start(arglist, message);
				Assert(message, arglist);
				va_end(arglist);
			}
		}
		template<typename T, typename Y>
		static void WhenLessEqual(const T left, const Y right, PRINTF_FORMAT_STRING const ANSICHAR*const message, ...)
		{
			if (left <= (T)right)
			{
				va_list arglist;
				va_start(arglist, message);
				Assert(message, arglist);
				va_end(arglist);
			}
		}
		template<typename T>
		static void WhenNullptr(const T ptr, PRINTF_FORMAT_STRING const ANSICHAR*const message, ...)
		{
			if (!ptr)
			{
				va_list arglist;
				va_start(arglist, message);
				Assert(message, arglist);
				va_end(arglist);
			}
		}
		template<typename T>
		static void WhenWithin(const T min, const T max, const T value, PRINTF_FORMAT_STRING const ANSICHAR*const message, ...)
		{
			if (value > min && value <= max)
			{
				va_list arglist;
				va_start(arglist, message);
				Assert(message, arglist);
				va_end(arglist);
			}
		}
		template<typename T>
		static void WhenWithinInclusive(const T min, const T max, const T value, PRINTF_FORMAT_STRING const ANSICHAR*const message, ...)
		{
			if (value >= min && value <= max)
			{
				va_list arglist;
				va_start(arglist, message);
				Assert(message, arglist);
				va_end(arglist);
			}
		}
		template<typename T>
		static void WhenNotWithin(const T min, const T max, const T value, PRINTF_FORMAT_STRING const ANSICHAR*const message, ...)
		{
			if (value < min || value > max)
			{
				va_list arglist;
				va_start(arglist, message);
				Assert(message, arglist);
				va_end(arglist);
			}
		}
	};
}

#endif /* GAF_ASSERTION_H */