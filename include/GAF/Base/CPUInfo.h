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

#ifndef GAF_CPUINFO_H
#define GAF_CPUINFO_H 1

namespace gaf
{
	struct CPUFeatures
	{
		int AES : 1;			/* AES instruction set */
		int erms : 1;			/* Enhanced REP MOVSB/STOSB */
		int f16c : 1;			/* F16C (half-precision) FP support */
		int fma3 : 1;			/* Fused multiply-add (FMA3) */
		int fma4 : 1;			/* Fused multiply-add (FMA4) */
		int vpclmulqdq : 1;		/* CLMUL instruction set (VEX-256/EVEX) */
		int bmi1 : 1;			/* Bit Manipulation Instruction Set 1 */
		int bmi2 : 1;			/* Bit Manipulation Instruction Set 2 */
		int sha : 1;			/* Intel SHA extensions */
		int vaes : 1;			/* Vector AES instruction set (VEX-256/EVEX) */
		int htt : 1;			/* Hyper-Threading */

		int sse3 : 1;			/* SSE3 instructions */
		int ssse3 : 1;			/* Supplemental SSE3 instructions */
		int sse4_1 : 1;			/* SSE4.1 instructions */
		int sse4_2 : 1;			/* SSE4.2 instructions */
		int sse4_a : 1;			/* SSE4a instructions */

		int avx : 1;			/* Advanced Vector Extensions */
		int avx2 : 1;			/* Advanced Vector Extensions 2 */

		int avx512f : 1;		/* AVX-512 Foundation */
		int avx512cd : 1;		/* AVX-512 Conflict Detection Instructions */
		int avx512er : 1;		/* AVX-512 Exponential and Reciprocal Instructions */
		int avx512pf : 1;		/* AVX-512 Prefetch Instructions */
		int avx512bw : 1;		/* AVX-512 Byte and Word Instructions */
		int avx512dq : 1;		/* AVX-512 Doubleword and Quadword Instructions */
		int avx512vl : 1;		/* AVX-512 Vector Length Extensions */
		int avx512ifma : 1;		/* AVX-512 Integer Fused Multiply-Add Instructions */
		int avx512vbmi : 1;		/* AVX-512 Vector Bit Manipulation Instructions */
		int avx512vbmi2 : 1;	/* AVX-512 Vector Bit Manipulation Instructions 2 */
		int avx512vnni : 1;		/* AVX-512 Vector Neural Network Instructions */
		int avx512bitalg : 1;	/* AVX-512 BITALG instructions */
		int avx512vpopcntdq : 1;/* AVX-512 Vector Population Count Double and Quad-word */

		int smx : 1;			/* Safer Mode Extensions (LaGrande) */
		int sgx : 1;			/* Software Guard Extensions */
		int cx16 : 1;			/* CMPXCHG16B instruction */
	};

	namespace ECPUVendor
	{
		enum Type
		{
			INTEL,
			AMD,
			UNKNOWN
		};
	}

	struct CPUInfo
	{
		CPUFeatures Features;
		ECPUVendor::Type Vendor;
	};
}

#endif /* GAF_CPUINFO_H */
