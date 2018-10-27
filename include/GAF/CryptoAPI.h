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

#ifndef GAF_CRYPTOAPI_H
#define GAF_CRYPTOAPI_H 1

#include "GAF\GAFPrerequisites.h"

namespace gaf
{
	class Crypto
	{
	public:
		using CryptoFn = std::function<void(void* buffer, SIZET sz)>;
	private:
		CryptoFn m_Crypter;
		CryptoFn m_Decrypter;
	public:
		Crypto() = delete;
		Crypto(const CryptoFn& crypter, const CryptoFn& decrypter);
		void Crypt(void* buffer, SIZET bufferSize);
		void Decrypt(void* buffer, SIZET bufferSize);
		virtual ~Crypto() = default;
	};

	typedef std::function<SIZET(SIZET oldVal)> RandomGeneratorFn;
	SIZET GenerateSeed();
	Crypto* CreateDefaultCrypto();
	Crypto* CreateDefaultCrypto(SIZET seed);
	Crypto* CreateDefaultCrypto(const RandomGeneratorFn& rnd);
	Crypto* CreateDefaultCrypto(const RandomGeneratorFn& rnd, SIZET seed);
	void ResetDefaultCryptoSeed(Crypto* defaultCrypto, SIZET seed);
	SIZET GetDefaultCryptoSeed(Crypto* defaultCrypto);
	void ReleaseDefaultCrypto(Crypto* crypt);
}

#endif /* GAF_CRYPTOAPI_H */