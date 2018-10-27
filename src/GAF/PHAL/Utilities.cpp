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

#include "GAF/PHAL/Platform.h"
#include <intrin.h>

using namespace gaf;

namespace
{
	constexpr __m128 SIMD_SP_zero = { 0.f, 0.f, 0.f, 0.f };
	constexpr __m128 SIMD_SP_255 = { 255.f, 255.f, 255.f, 255.f };
	constexpr __m128 SIMD_SP_min_char = { -128.f, -128.f, -128.f, -128.f };
	constexpr __m128 SIMD_SP_max_char = { 127.f, 127.f, 127.f, 127.f };
	constexpr __m128 SIMD_SP_min_short = { -32768.0f, -32768.0f, -32768.0f, -32768.0f };
	constexpr __m128 SIMD_SP_max_short = { 32767.0f, 32767.0f, 32767.0f, 32767.0f };
	constexpr __m128 SIMD_SP_max_ushort = { 65535.f, 65535.f, 65535.f, 65535.f };
}

int32 gaf::FloatToInt(const float f)
{
	const auto x = _mm_load_ss(&f);
	return _mm_cvtt_ss2si(x);
}

int8 gaf::FloatToChar(const float f)
{
	__m128 x = _mm_load_ss(&f);
	x = _mm_max_ss(x, SIMD_SP_min_char);
	x = _mm_min_ss(x, SIMD_SP_max_char);
	return static_cast<int8>(_mm_cvtt_ss2si(x));
}

int16 gaf::FloatToShort(const float f)
{
	__m128 x = _mm_load_ss(&f);
	x = _mm_max_ss(x, SIMD_SP_min_short);
	x = _mm_min_ss(x, SIMD_SP_max_short);
	return static_cast<int16>(_mm_cvtt_ss2si(x));
}

uint32 gaf::FloatToUnsigned(const float f)
{
	return (uint32)FloatToInt(f);
}

uint8 gaf::FloatToByte(const float f)
{
	__m128 x = _mm_load_ss(&f);
	x = _mm_max_ss(x, SIMD_SP_zero);
	x = _mm_min_ss(x, SIMD_SP_255);
	return static_cast<uint8>(_mm_cvtt_ss2si(x));
}

uint16 gaf::FloatToUShort(const float f)
{
	__m128 x = _mm_load_ss(&f);
	x = _mm_max_ss(x, SIMD_SP_zero);
	x = _mm_min_ss(x, SIMD_SP_max_ushort);
	return static_cast<uint16>(_mm_cvtt_ss2si(x));
}

unsigned int gaf::CountLeadingZeros(const unsigned int value)
{
#if PLATFORM_WINDOWS 
	unsigned long log2 = 0;
	return _BitScanReverse(&log2, value) != 0 ? 31 - log2 : 32;
#else
	if (value == 0)
		return 32;
	return __builtin_clz(value);
#endif
}

static uint64 FloorLog2_64(uint64 value)
{
	uint64 pos = 0;
	if (value >= 1ull << 32) { value >>= 32; pos += 32; }
	if (value >= 1ull << 16) { value >>= 16; pos += 16; }
	if (value >= 1ull << 8) { value >>= 8; pos += 8; }
	if (value >= 1ull << 4) { value >>= 4; pos += 4; }
	if (value >= 1ull << 2) { value >>= 2; pos += 2; }
	if (value >= 1ull << 1) { pos += 1; }
	return (value == 0) ? 0 : pos;
}

unsigned long long gaf::CountLeadingZeros64(const unsigned long long value)
{
#if PLATFORM_WINDOWS && ARCHITECTURE_X64
	unsigned long log2 = 0;
	return _BitScanReverse64(&log2, value) != 0 ? 63 - log2 : 64;
#else
	if (value == 0)
		return 64;
	return 63 - FloorLog2_64(value);
#endif
}

unsigned int gaf::CountTrailingZeros(const unsigned int value)
{
#if PLATFORM_WINDOWS
	if (value == 0)
		return 32;
	unsigned long bitIndex = 0;
	_BitScanForward(&bitIndex, value);
	return bitIndex;
#else
	if (value == 0)
		return 32;
	return __builtin_ctz(value);
#endif
}

unsigned long long gaf::CountTrailingZeros64(unsigned long long value)
{
	if (value == 0)
		return 32;
	uint32 result = 0;
	while ((value & 1) == 0)
	{
		value >>= 1;
		++result;
	}
	return result;
}

static uint32 CeilLogTwo(const uint32 arg)
{
	const auto bitMask = static_cast<int32>(CountLeadingZeros(arg) << 26) >> 31;
	return (32 - CountLeadingZeros(arg - 1)) & (~bitMask);
}

unsigned int gaf::RoundUpToPowerOf2(const unsigned int val)
{
	return 1 << CeilLogTwo(val);
}