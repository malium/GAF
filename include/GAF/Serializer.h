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

#ifndef GAF_SERIALIZER_H
#define GAF_SERIALIZER_H 1

#include "GAF/GAFPrerequisites.h"

namespace gaf
{
	class BinarySerializer
	{
		int8* m_Buffer;
		SIZET m_Capacity;
		int8* m_Pointer;
	public:
		BinarySerializer();
		BinarySerializer(const BinarySerializer& other);
		BinarySerializer(BinarySerializer&& other)noexcept;
		BinarySerializer& operator=(const BinarySerializer& other);
		BinarySerializer& operator=(BinarySerializer&& other)noexcept;

		SIZET Capacity()const;
		void Reserve(SIZET cap);
		SIZET Size()const;
		
		void Get(void* buffer, SIZET size);
		void Set(void* buffer, SIZET size);
		void Clear();

		void* GetBuffer()const;

		void swap(BinarySerializer& other)noexcept;

		/* Serialize */
		template<typename T>
		friend BinarySerializer& operator<<(BinarySerializer& buffer, const T& object);
		template<typename T>
		friend BinarySerializer& operator>>(const T& object, BinarySerializer& buffer)
		{
			return buffer << object;
		}

		/* Deserialize */
		template<typename T>
		friend T& operator<<(T& object, BinarySerializer& buffer);
		template<typename T>
		friend T& operator>>(BinarySerializer& buffer, T& object)
		{
			return object << buffer;
		}
	};

	FORCEINLINE void swap(BinarySerializer& a, BinarySerializer& b)noexcept
	{
		a.swap(b);
	}

	template<typename T>
	class SerializableType
	{
	public:
		virtual ~SerializableType() = default;

		virtual void Serialize(BinarySerializer& buffer)
		{
			buffer.Set((T*)this, sizeof(T));
		}
		virtual void Deserialize(BinarySerializer& buffer)
		{
			buffer.Get((T*)this, sizeof(T));
		}
	};

	template<typename T>
	BinarySerializer & operator<<(BinarySerializer & buffer, const T & object)
	{
		if (std::is_base_of<SerializableType<T>, T>::value)
			object.Serialize(buffer);
		else
			buffer.Set(&object, sizeof(T));
		return buffer;
	}

	template<typename T>
	T & operator<<(T& object, BinarySerializer & buffer)
	{
		if (std::is_base_of<SerializableType<T>, T>::value)
			object.Deserialize(buffer);
		else
			buffer.Get(&object, sizeof(T));
		return buffer;
	}
}

#endif /* GAF_SERIALIZER_H */