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

#include "GAF/TaskManager.h"
#include "GAF/HWDetector.h"

using namespace gaf;

TaskManager::Dispatcher::DHandler::DHandler()
	:Handler(&Tasks)
{

}

TaskManager::Dispatcher::DHandler::DHandler(const DHandler& other)
	:Tasks(other.Tasks)
	,Handler(other.Handler)
{
	
}

TaskManager::Dispatcher::DHandler::DHandler(DHandler&& other)noexcept
	:Tasks(std::move(other.Tasks))
	,Handler(std::move(other.Handler))
{

}

TaskManager::Dispatcher::DHandler& TaskManager::Dispatcher::DHandler::operator=(const DHandler& other)
{
	if (this != &other)
	{
		Tasks = other.Tasks;
		Handler = other.Handler;
	}
	return *this;
}


TaskManager::Dispatcher::DHandler& TaskManager::Dispatcher::DHandler::operator=(DHandler&& other)noexcept
{
	if (this != &other)
	{
		Tasks = std::move(other.Tasks);
		Handler = other.Handler;
	}
	return *this;
}

TaskManager::Dispatcher::Dispatcher(const Dispatcher& other)
{
	HandlerMutex.lock();
	Handlers = other.Handlers;
	HandlerMutex.unlock();
}

TaskManager::Dispatcher::Dispatcher(Dispatcher&& other)noexcept
{
	other.HandlerMutex.lock_shared();
	HandlerMutex.lock();
	Handlers = std::move(other.Handlers);
	HandlerMutex.unlock();
	other.HandlerMutex.unlock_shared();
}

TaskManager::Dispatcher& TaskManager::Dispatcher::operator=(const Dispatcher& other)
{
	if (this != &other)
	{
		HandlerMutex.lock();
		Handlers = other.Handlers;
		HandlerMutex.unlock();
	}
	return *this;
}

TaskManager::Dispatcher& TaskManager::Dispatcher::operator=(Dispatcher&& other)noexcept
{
	if (this != &other)
	{
		other.HandlerMutex.lock_shared();
		HandlerMutex.lock();
		Handlers = std::move(other.Handlers);
		HandlerMutex.unlock();
		other.HandlerMutex.unlock_shared();
	}
	return *this;
}

/****************************************************************
*						TASKMANAGER								*
****************************************************************/

void TaskManager::SendTask(TaskDispatcher * dispatcher, const Task_t & task, const uint32 handler)
{
	Assertion::WhenNullptr(dispatcher, "Trying to send a Task from a nullptr dispatcher.");
	m_DispatchersLock.lock_shared();
	const auto it = m_Dispatchers.find(std::hash<TaskDispatcher*>{}(dispatcher));
	Assertion::WhenEqual(it, m_Dispatchers.end(), "Trying to send a task from non registered dispatcher.");
	const auto sz = it->second.Handlers.size();
	m_DispatchersLock.unlock_shared();
	if (handler == static_cast<uint32>(-1))
	{
		uint32 less = 500;
		uint32 lessId = static_cast<uint32>(-1);
		it->second.HandlerMutex.lock_shared();
		for (auto itt = it->second.Handlers.begin(); itt != it->second.Handlers.end(); ++itt)
		{
			const auto tmp = itt->Tasks.Size();
			if (tmp == 0)
			{
				less = 0;
				lessId = (uint32)std::distance(it->second.Handlers.begin(), itt);
				break;
			}
			if (tmp < less)
			{
				less = (uint32)tmp;
				lessId = (uint32)std::distance(it->second.Handlers.begin(), itt);
			}
		}
		
		it->second.Handlers[lessId].Tasks.PushBack(task);
		it->second.HandlerMutex.unlock_shared();
	}
	else
	{
		Assertion::WhenGreaterEqual(handler, sz, "Trying to send a task to a non-existant handler.");
		it->second.HandlerMutex.lock_shared();
		it->second.Handlers[handler].Tasks.PushBack(task);
		it->second.HandlerMutex.unlock_shared();
	}
}

uint32 TaskManager::GetPendingTask(TaskDispatcher * dispatcher, const uint32 handler)
{
	if(!dispatcher)
		return static_cast<uint32>(-1);
	const auto hash = std::hash<TaskDispatcher*>{}(dispatcher);
	m_DispatchersLock.lock_shared();
	const auto it = m_Dispatchers.find(hash);
	if (it == m_Dispatchers.end())
	{
		m_DispatchersLock.unlock_shared();
		return static_cast<uint32>(-1);
	}

	it->second.HandlerMutex.lock_shared();
	if (it->second.Handlers.size() < handler)
	{
		it->second.HandlerMutex.unlock_shared();
		return static_cast<uint32>(-1);
	}
	
	const auto tasks = (uint32)it->second.Handlers[handler].Tasks.Size();
	it->second.HandlerMutex.unlock_shared();
	return tasks;
}

TaskManager::TaskManager()
	:m_StartingNum(InstanceHW()->GetNumberLogicalCores())
{
	m_HandlersLock.lock();
	m_TaskHandlers.resize(m_StartingNum, TaskHandler{ &m_Tasks });
	m_HandlersLock.unlock();
}

TaskManager::~TaskManager()
{

}

void TaskManager::RegisterTaskDispatcher(TaskDispatcher * dispatcher, uint32 handlers)
{
	Assertion::WhenNullptr(dispatcher, "Trying to register a nullptr dispatcher.");
	const auto hash = std::hash<TaskDispatcher*>{}(dispatcher);
	m_DispatchersLock.lock_shared();
	if (m_Dispatchers.find(hash) != m_Dispatchers.end())
	{
		m_DispatchersLock.unlock_shared();
		return;
	}
	m_DispatchersLock.unlock_shared();

	if (handlers == 0)
		handlers = 1;

	m_DispatchersLock.lock();
	m_Dispatchers.emplace(std::make_pair(hash, Dispatcher()));
	m_Dispatchers[hash].HandlerMutex.lock();
	m_Dispatchers[hash].Handlers.resize(handlers);
	m_Dispatchers[hash].HandlerMutex.unlock();
	m_DispatchersLock.unlock();
}

void TaskManager::UnregisterTaskDispatcher(TaskDispatcher * dispatcher)
{
	Assertion::WhenNullptr(dispatcher, "Trying to unregister a nullptr dispatcher.");
	m_DispatchersLock.lock_shared();
	const auto it = m_Dispatchers.find(std::hash<TaskDispatcher*>{}(dispatcher));
	if (it == m_Dispatchers.end())
	{
		m_DispatchersLock.unlock_shared();
		return;
	}
	m_DispatchersLock.unlock_shared();

	m_DispatchersLock.lock();
	m_Dispatchers.erase(it);
	m_DispatchersLock.unlock();
}

uint32 TaskManager::GetNumberOfHandlers(TaskDispatcher * dispatcher)
{
	if (!dispatcher)
		return 0;
	
	m_DispatchersLock.lock_shared();
	const auto it = m_Dispatchers.find(std::hash<TaskDispatcher*>{}(dispatcher));
	if (it == m_Dispatchers.end())
	{
		m_DispatchersLock.unlock_shared();
		return 0;
	}

	auto ret = (uint32)it->second.Handlers.size();
	m_DispatchersLock.unlock_shared();
	return ret;
}

void TaskManager::ChangeNumberOfHandlers(TaskDispatcher* dispatcher, uint32 handlers)
{
	Assertion::WhenNullptr(dispatcher, "Trying to change the number of handlers of a nullptr dispatcher.");
	m_DispatchersLock.lock_shared();
	const auto it = m_Dispatchers.find(std::hash<TaskDispatcher*>{}(dispatcher));
	m_DispatchersLock.unlock_shared();
	Assertion::WhenEqual(it, m_Dispatchers.end(), "Trying to change the number of handlers of a non-registered dispatcher.");
	if (handlers == 0)
		handlers = 1;
	

	it->second.HandlerMutex.lock_shared();
	const auto oldSize = (uint32)it->second.Handlers.size();
	it->second.HandlerMutex.unlock_shared();
	
	it->second.HandlerMutex.lock();
	it->second.Handlers.reserve(handlers);

	for (auto i = oldSize; i < handlers; ++i)
	{
		Dispatcher::DHandler hnd;
		hnd.Handler.Start();
		it->second.Handlers.emplace_back(hnd);
	}
	it->second.HandlerMutex.unlock();
}

SIZET TaskManager::GetNumberTaskHandlers()
{
	std::shared_lock<std::shared_mutex> lock(m_HandlersLock);
	return m_TaskHandlers.size();
}

void TaskManager::SendTask(Task_t task)
{
	m_Tasks.PushBack(std::move(task));
	const auto queue = m_Tasks.Size();
	if (queue > 32)
		SetNumberTaskHandlers(GetNumberTaskHandlers() + 1);
	else if (queue < m_StartingNum)
		SetNumberTaskHandlers(GetNumberTaskHandlers() - 1);
}

void gaf::TaskManager::SetNumberTaskHandlers(const SIZET num)
{
	if (num < m_StartingNum)
		return;
	const auto numTaskHnd = GetNumberTaskHandlers();
	if (num == numTaskHnd)
		return;
	if (num > numTaskHnd)
	{
		m_HandlersLock.lock();
		for (SIZET i = numTaskHnd; i < num; ++i)
		{
			TaskHandler hnd{ &m_Tasks };
			hnd.Start();
			m_TaskHandlers.emplace_back(hnd);
		}
		m_HandlersLock.unlock();
	}
	else
	{
		m_HandlersLock.lock();
		m_TaskHandlers.resize(num);
		m_HandlersLock.unlock();
	}
}