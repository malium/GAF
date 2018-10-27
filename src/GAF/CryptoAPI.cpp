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

#include "GAF/LogManager.h"
#include "GAF/CryptoAPI.h"
#include "GAF/Util/DayTime.h"

using namespace gaf;

Crypto::Crypto(const CryptoFn& crypter, const CryptoFn& decrypter)
	:m_Crypter(crypter)
	,m_Decrypter(decrypter)
{

}

void Crypto::Crypt(void* buffer, const SIZET bufferSize)
{
	m_Crypter(buffer, bufferSize);
}

void Crypto::Decrypt(void* buffer, const SIZET bufferSize)
{
	m_Decrypter(buffer, bufferSize);
}

class DefaultCrypto : gaf::Crypto
{
	uint16 m_CryptArray[256];
	uint16 m_DecryptArray[256];
	RandomGeneratorFn m_RanGen;
	SIZET m_InitialSeed;
	SIZET m_CurrentSeed;
	void Impl_Encrypt(void* buffer, SIZET size);
	void Impl_Decrypt(void* buffer, SIZET size);
	void GenerateArrays();
public:
	SIZET GetInitialSeed()const;
	void SetInitialSeed(SIZET seed);

	DefaultCrypto();
	DefaultCrypto(SIZET seed);
	DefaultCrypto(const RandomGeneratorFn& rnd);
	DefaultCrypto(const RandomGeneratorFn& rnd, SIZET seed);
	~DefaultCrypto() = default;
};

static SIZET DefaultRandomGenerator(const SIZET oldVal)
{
	std::mt19937_64 engine{ oldVal };
	return engine();
}

static SIZET GetTime()
{
	time_t now;
	struct tm y2k = { };
	y2k.tm_year = 100;
	y2k.tm_mday = 1;
	time(&now);

	union
	{
		time_t Time_t;
		SIZET Seed_t;
		double DiffTime_t;
	}Conv;
	Conv.DiffTime_t = difftime(now, mktime(&y2k));
	return Conv.Seed_t;
}

SIZET gaf::GenerateSeed()
{
	return GetTime();
}

Crypto* gaf::CreateDefaultCrypto()
{
	auto crypt = new DefaultCrypto();
	return reinterpret_cast<Crypto*>(crypt);
}

Crypto* gaf::CreateDefaultCrypto(const SIZET seed)
{
	auto crypt = new DefaultCrypto(seed);
	return reinterpret_cast<Crypto*>(crypt);
}

Crypto* gaf::CreateDefaultCrypto(const RandomGeneratorFn & rnd)
{
	auto crypt = new DefaultCrypto(rnd);
	return reinterpret_cast<Crypto*>(crypt);
}

Crypto* gaf::CreateDefaultCrypto(const RandomGeneratorFn & rnd, const SIZET seed)
{
	auto crypt = new DefaultCrypto(rnd, seed);
	return reinterpret_cast<Crypto*>(crypt);
}

void gaf::ResetDefaultCryptoSeed(Crypto * defaultCrypto, const SIZET seed)
{
	Assertion::WhenNullptr(defaultCrypto, "Trying to change the default crypto seed but the Crypto object was nullptr.");
	reinterpret_cast<DefaultCrypto*>(defaultCrypto)->SetInitialSeed(seed);
}

SIZET gaf::GetDefaultCryptoSeed(Crypto * defaultCrypto)
{
	Assertion::WhenNullptr(defaultCrypto, "Trying to get the default crypto seed but the Crypto object was nullptr.");
	return reinterpret_cast<DefaultCrypto*>(defaultCrypto)->GetInitialSeed();
}

void gaf::ReleaseDefaultCrypto(Crypto * crypt)
{
	Assertion::WhenNullptr(crypt, "Trying to release the Crypto object  but was nullptr.");
	auto obj =  reinterpret_cast<DefaultCrypto*>(crypt);
	delete obj;
}

void DefaultCrypto::Impl_Encrypt(void* buffer, const SIZET size)
{
	if (size == 0)
		return;
	if (!buffer)
	{
		LogManager::LogMessage(LL_ERRO, "Trying to encrypt a buffer but it was nullptr.");
		return;
	}
	auto byteBuffer = (uint8*)buffer;
	for (SIZET i = 0; i < size; ++i)
	{
		byteBuffer[i] = (uint8)m_CryptArray[byteBuffer[i]];
	}
}

void DefaultCrypto::Impl_Decrypt(void* buffer, const SIZET size)
{
	if (size == 0)
		return;
	if (!buffer)
	{
		LogManager::LogMessage(LL_ERRO, "Trying to decrypt a buffer but it was nullptr.");
		return;
	}
	auto byteBuffer = (uint8*)buffer;
	for (SIZET i = 0; i < size; ++i)
	{
		byteBuffer[i] = (uint8)m_DecryptArray[byteBuffer[i]];
	}
}

#define UpdateVal(x) { m_CurrentSeed = m_RanGen(m_CurrentSeed); x = (uint8)(m_CurrentSeed % 256); }
static constexpr uint16 DefaultVal = 1024;
void DefaultCrypto::GenerateArrays()
{
	m_CurrentSeed = m_RanGen(m_InitialSeed);
	wmemset(&((wchar_t*)m_CryptArray)[0], DefaultVal, ARRAY_SIZE(m_CryptArray));
	wmemset(&((wchar_t*)m_DecryptArray)[0], DefaultVal, ARRAY_SIZE(m_DecryptArray));
	for (uint32 i = 0; i < 256; ++i)
	{
		bool done = false;
		/* Find a good random match */
		while (!done)
		{
			for (uint32 j = 0; j < 4; ++j)
			{
				uint8 val;
				UpdateVal(val);
				if (m_DecryptArray[val] == DefaultVal)
				{
					m_CryptArray[i] = val;
					m_DecryptArray[val] = i;
					done = true;
					break;
				}
			}
			/* If after 4 random tries, find a valid value */
			if (!done)
			{
				for (uint32 j = 0; j < 256; ++j)
				{
					if (m_DecryptArray[j] == DefaultVal)
					{
						m_CryptArray[i] = j;
						m_DecryptArray[j] = i;
						done = true;
						break;
					}
				}
			}
		}
	}
}

SIZET DefaultCrypto::GetInitialSeed() const
{
	return m_InitialSeed;
}

void DefaultCrypto::SetInitialSeed(const SIZET seed)
{
	m_InitialSeed = seed;
	GenerateArrays();
}

DefaultCrypto::DefaultCrypto()
	:Crypto(std::bind(&DefaultCrypto::Impl_Encrypt, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&DefaultCrypto::Impl_Decrypt, this, std::placeholders::_1, std::placeholders::_2))
	, m_InitialSeed(GetTime())
	, m_CurrentSeed(0)
	, m_RanGen(DefaultRandomGenerator)
{
	GenerateArrays();
}

DefaultCrypto::DefaultCrypto(const SIZET seed)
	:Crypto(std::bind(&DefaultCrypto::Impl_Encrypt, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&DefaultCrypto::Impl_Decrypt, this, std::placeholders::_1, std::placeholders::_2))
	, m_InitialSeed(seed)
	, m_CurrentSeed(0)
	, m_RanGen(DefaultRandomGenerator)
{
	GenerateArrays();
}

DefaultCrypto::DefaultCrypto(const RandomGeneratorFn & rnd)
	:Crypto(std::bind(&DefaultCrypto::Impl_Encrypt, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&DefaultCrypto::Impl_Decrypt, this, std::placeholders::_1, std::placeholders::_2))
	, m_InitialSeed(GetTime())
	, m_CurrentSeed(0)
	, m_RanGen(rnd)
{
	GenerateArrays();
}

DefaultCrypto::DefaultCrypto(const RandomGeneratorFn & rnd, const SIZET seed)
	:Crypto(std::bind(&DefaultCrypto::Impl_Encrypt, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&DefaultCrypto::Impl_Decrypt, this, std::placeholders::_1, std::placeholders::_2))
	, m_InitialSeed(seed)
	, m_CurrentSeed(0)
	, m_RanGen(rnd)
{
	GenerateArrays();
}
