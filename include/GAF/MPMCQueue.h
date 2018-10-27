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

#ifndef GAF_MPMCQUEUE_H
#define GAF_MPMCQUEUE_H 1

#include "GAF/GAFPrerequisites.h"

namespace gaf
{
	template<typename T>
	class MPMCQueue
	{
		std::queue<T> m_Data;
		std::shared_mutex m_Mutex;
	public:
		MPMCQueue() = default;
		~MPMCQueue() = default;
		MPMCQueue(const MPMCQueue& other)
		{
			m_Mutex.lock();
			m_Data = other.m_Data;
			m_Mutex.unlock();
		}
		MPMCQueue(MPMCQueue&& other)noexcept
		{
			m_Mutex.lock();
			other.m_Mutex.lock_shared();
			m_Data = std::move(other.m_Data);
			other.m_Mutex.unlock_shared();
			m_Mutex.unlock();
		}
		MPMCQueue& operator=(const MPMCQueue& other)
		{
			if (this != &other)
			{
				m_Mutex.lock();
				m_Data = other.m_Data;
				m_Mutex.unlock();
			}
			return *this;
		}
		MPMCQueue& operator=(MPMCQueue&& other)noexcept
		{
			if (this != &other)
			{
				m_Mutex.lock();
				other.m_Mutex.lock_shared();
				m_Data = std::move(other.m_Data);
				other.m_Mutex.unlock_shared();
				m_Mutex.unlock();
			}
			return *this;
		}
		MPMCQueue(std::initializer_list<T> list)
		{
			m_Mutex.lock();
			for (auto it = list.begin(); it != list.end(); ++it)
				m_Data.push(*it);
			m_Mutex.unlock();
		}

		MPMCQueue& operator=(std::initializer_list<T> list)
		{
			m_Mutex.lock();
			for (auto it = list.begin(); it != list.end(); ++it)
				m_Data.push(*it);
			m_Mutex.unlock();
			return *this;
		}

		void PushBack(const T& val)
		{
			m_Mutex.lock();
			m_Data.push(val);
			m_Mutex.unlock();
		}
		void PushBack(T&& val)
		{
			m_Mutex.lock();
			m_Data.push(val);
			m_Mutex.unlock();
		}

		template<typename... Args>
		void EmplaceBack(Args&&... args)
		{
			m_Mutex.lock();
			m_Data.emplace(std::forward<Args>(args));
			m_Mutex.unlock();
		}

		bool PopFront(T& val)
		{
			m_Mutex.lock();
			if (!m_Data.empty())
			{
				val = m_Data.front();
				m_Data.pop();
				m_Mutex.unlock();
				return true;
			}
			m_Mutex.unlock();
			return false;
		}

		T Front()
		{
			m_Mutex.lock_shared();
			auto val = m_Data.front();
			m_Mutex.unlock_shared();
			return val;
		}

		void PopFront()
		{
			m_Mutex.lock();
			m_Data.pop();
			m_Mutex.unlock();
		}

		bool Empty()
		{
			m_Mutex.lock_shared();
			const auto empty = m_Data.empty();
			m_Mutex.unlock_shared();
			return empty;
		}

		SIZET Size()
		{
			m_Mutex.lock_shared();
			const auto sz = m_Data.size();
			m_Mutex.unlock_shared();
			return sz;
		}

		void swap(MPMCQueue& other)noexcept
		{
			m_Mutex.lock();
			other.m_Mutex.lock();
			m_Data.swap(other.m_Data);
			other.m_Mutex.unlock();
			m_Mutex.unlock();
		}
	};

	template<typename T>
	void swap(MPMCQueue<T>& a, MPMCQueue<T>& b)
	{
		a.swap(b);
	}
}

#endif /* GAF_MPMCQUEUE_H */