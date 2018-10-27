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

/* Usefull utilities */
namespace gaf
{
	/* Checks if a value is within range, inclusive on max and min */
	template<typename T>
	FORCEINLINE constexpr bool IsWithin(const T& value, const T& min, const T& max)
	{
		return value > min && value <= max;
	}
	/* Checks if a value is within range, inclusive on max and min */
	template<class T>
	FORCEINLINE bool IsWithinInclusive(const T& value, const T& min, const T& max)
	{
		return value >= min && value <= max;
	}
	/* Checks if a number is a power of two */
	template<typename T>
	FORCEINLINE constexpr bool IsPowerOfTwo(const T value)
	{
		return !(value == 0) && !(value &(value - 1));
	}
	/* Rounds number up/down to the next multiple, multiple must be power of two */
	template<typename T>
	FORCEINLINE constexpr T RoundUp(const T number, const T multiple)
	{
		static_assert(!IsPowerOfTwo(multiple), "Multiple must be power of two.");
		const T result = (number + multiple - 1) & ~(multiple - 1);
		static_assert(number <= result && result < number + multiple, "");
		return result;
	}
	/* Returns the highest of 3 values */
	template<typename T>
	FORCEINLINE constexpr T Max3(const T a, const T b, const T c)
	{
		return (a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c);
	}
	/* Returns the lowest of 3 values */
	template<typename T>
	FORCEINLINE constexpr T Min3(const T a, const T b, const T c)
	{
		return (a < b) ? ((a < c) ? a : c) : ((b < c) ? b : c);
	}
	/* Returns a higher value */
	template<class T>
	FORCEINLINE constexpr T Max(const T a, const T b)
	{
		return (a >= b) ? a : b;
	}
	/* Returns the lower value */
	template<class T>
	FORCEINLINE constexpr T Min(const T a, const T b)
	{
		return (a <= b) ? a : b;
	}
	/* Clamps the value between a minimum and a maximum */
	template<typename T>
	FORCEINLINE constexpr T Clamp(const T a, const T min, const T max)
	{
		return Max(min, Min(max, a));
	}
	/* Computes the absolute value */
	template<class T>
	FORCEINLINE constexpr T Abs(const T a)
	{
		return (a >= (T)0) ? a : -a;
	}
	/* Returns a * a */
	template<typename T>
	FORCEINLINE T Square(const T a)
	{
		return a * a;
	}
	/* Returns a * a * a */
	template<typename T>
	FORCEINLINE T Cube(const T a)
	{
		return a * a * a;
	}
	/* Returns 1, 0 or -1 depending on relation of T to 0 */
	template<class T>
	FORCEINLINE constexpr T Sign(const T a)
	{
		return ((a > (T)0) ? (T)1 : ((a) < (T)0) ? (T)-1 : (T)0);
	}

	int32 FloatToInt(float f);

	int8 FloatToChar(float f);

	int16 FloatToShort(float f);

	uint32 FloatToUnsigned(float f);

	uint8 FloatToByte(float f);

	uint16 FloatToUShort(float f);

	FORCEINLINE constexpr int8 ClampChar(const int32 i)
	{
		return static_cast<int8>(Clamp(i, -128, 127));
	}
	FORCEINLINE constexpr int16 ClampShort(const int32 i)
	{
		return static_cast<int16>(Clamp(i, -32768, 32767));
	}
}

/* Other usefull definitions */
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#endif
#ifndef TEXT
#define TEXT_PASTE(x) L ## x
#define TEXT(x) TEXT_PASTE(x)
#endif
#ifndef STRINGIFY
#define STRINGIFY(x) #x
#endif
#ifndef WSTRINGIFY
#define WSTRINGIFY(x) TEXT(#x)
#endif
#ifndef SAFE_DELETE
#define SAFE_DELETE(x) {if(x){delete x; x = nullptr;}}
#endif
#ifndef SAFE_FREE
#define SAFE_FREE(x) { if(x){free(x); x = nullptr;} }
#endif

#ifndef NOT_IMPLEMENTED
#define NOT_IMPLEMENTED() Assertion::Always(__FUNCTION__ " Not implemented.")
#endif