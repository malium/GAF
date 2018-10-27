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

#include "GAF/Serializer.h"
#include "GAF/LogManager.h"

using namespace gaf;

BinarySerializer::BinarySerializer()
	:m_Buffer(nullptr)
	, m_Capacity(0)
	, m_Pointer(nullptr)
{

}

BinarySerializer::BinarySerializer(const BinarySerializer& other)
	:m_Buffer(nullptr)
	,m_Capacity(other.m_Capacity)
	,m_Pointer(nullptr)
{
	if (m_Capacity != 0)
	{
		m_Buffer = (int8*)malloc(m_Capacity);
		m_Buffer = (int8*)memcpy(m_Buffer, other.m_Buffer, m_Capacity);
		m_Pointer = m_Buffer + (other.Size());
	}
}

BinarySerializer::BinarySerializer(BinarySerializer && other) noexcept
	:m_Buffer(other.m_Buffer)
	,m_Capacity(other.m_Capacity)
	,m_Pointer(other.m_Pointer)
{
	other.m_Buffer = nullptr;
	other.m_Capacity = 0;
	other.m_Pointer = nullptr;
}

BinarySerializer & BinarySerializer::operator=(const BinarySerializer & other)
{
	if (this != &other)
	{
		if (other.m_Capacity == 0)
		{
			SAFE_FREE(m_Buffer);
			m_Capacity = 0;
			m_Pointer = nullptr;
		}
		else
		{
			if (m_Capacity == 0)
				m_Buffer = (int8*)malloc(other.m_Capacity);
			else
				m_Buffer = (int8*)realloc(m_Buffer, other.m_Capacity);
			m_Capacity = other.m_Capacity;
			m_Buffer = (int8*)memcpy(m_Buffer, other.m_Buffer, m_Capacity);
			m_Pointer = m_Buffer + (other.Size());
		}
	}
	return *this;
}

BinarySerializer & gaf::BinarySerializer::operator=(BinarySerializer && other) noexcept
{
	if (this != &other)
	{
		if (other.m_Capacity == 0)
		{
			SAFE_FREE(m_Buffer);
			m_Capacity = 0;
			m_Pointer = nullptr;
		}
		else
		{
			if (m_Capacity != 0)
			{
				SAFE_FREE(m_Buffer);
			}
			m_Buffer = other.m_Buffer;
			m_Capacity = other.m_Capacity;
			m_Pointer = other.m_Pointer;
			other.m_Buffer = nullptr;
			other.m_Capacity = 0;
			other.m_Pointer = nullptr;
		}
	}
	return *this;
}

SIZET BinarySerializer::Capacity() const
{
	return m_Capacity;
}

void BinarySerializer::Reserve(const SIZET cap)
{
	if (m_Capacity == cap)
		return;
	if (cap == 0)
	{
		SAFE_FREE(m_Buffer);
		m_Capacity = 0;
		m_Pointer = nullptr;
		return;
	}
	const auto size = Size();
	if (!m_Buffer)
		m_Buffer = (int8*)malloc(cap);
	else
		m_Buffer = (int8*)realloc(m_Buffer, cap);	
	if (cap < size)
	{
		m_Pointer = m_Buffer + cap;
	}
}

SIZET BinarySerializer::Size() const
{
	if (m_Capacity == 0)
		return 0;
	return m_Pointer - m_Buffer;
}

void BinarySerializer::Get(void * buffer, const SIZET size)
{
	Assertion::WhenNullptr(buffer, "Trying to deserialize to a nullptr buffer.");
	if (size == 0)
		return;
	if (size > Size())
	{
		LogManager::LogMessage(LL_ERRO, "Trying to deserialize %lld bytes, but this buffers is not so big.", size);
		return;
	}
	memcpy(buffer, m_Pointer - size, size);
	m_Pointer -= size;
}

void BinarySerializer::Set(void * buffer, const SIZET size)
{
	Assertion::WhenNullptr(buffer, "Trying to serialize to a nullptr buffer.");
	if (size == 0)
		return;

	const auto oldSize = Size();
	const auto availableSize = m_Capacity - oldSize;
	if (availableSize < size)
	{
		if (!m_Buffer)
			m_Buffer = (int8*)malloc(size);
		else
			m_Buffer = (int8*)realloc(m_Buffer, availableSize + size);
		m_Capacity = availableSize + size;
	}
	m_Pointer = m_Buffer + oldSize;
	memcpy(m_Pointer, buffer, size);
	m_Pointer += size;
}

void BinarySerializer::Clear()
{
	m_Pointer = m_Buffer;
}

void * BinarySerializer::GetBuffer() const
{
	return m_Buffer;
}

void BinarySerializer::swap(BinarySerializer & other) noexcept
{
	const auto tmpBuffer = m_Buffer;
	const auto tmpCap = m_Capacity;
	const auto tmpPtr = m_Pointer;
	m_Buffer = other.m_Buffer;
	m_Capacity = other.m_Capacity;
	m_Pointer = other.m_Pointer;
	other.m_Buffer = tmpBuffer;
	other.m_Capacity = tmpCap;
	other.m_Pointer = tmpPtr;
}
