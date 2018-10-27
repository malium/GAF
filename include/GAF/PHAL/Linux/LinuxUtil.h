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

namespace gaf
{
	/* Counts the number of leading zeros in the bit representation of the value */
	unsigned int CountLeadingZeros(unsigned int value);

	/* Counts the number of leading zeros in the bit representation of the 64-bit value */
	unsigned long long CountLeadingZeros64(unsigned long long value);

	/* Counts the number of trailing zeros in the bit representation of the value */
	unsigned int CountTrailingZeros(unsigned int value);

	/* Counts the number of trailing zeros in the bit representation of the 64-bit value */
	unsigned long long CountTrailingZeros64(unsigned long long value);

	/* Rounds the given number up to the next higher power of two */
	unsigned int RoundUpToPowerOf2(unsigned int val);
}