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

using namespace gaf;

void TaskHandler::Run()
{
	std::this_thread::sleep_for(WaitTimeMS);
	bool stop = false;
	while (!stop)
	{
		if (m_Stop)
		{
			stop = true;
		}
		else if (!m_TaskQueue)
		{
			m_Stop.store(true);
			stop = true;
		}
		else
		{
			Task_t task;
			bool hasValue = false;
			if (m_TaskQueue && !m_Stop)
				hasValue = m_TaskQueue->PopFront(task);
			if (!hasValue)
				std::this_thread::sleep_for(WaitTimeMS);
			else
				task();
		}
	}
}

TaskHandler::TaskHandler(MPMCQueue<Task_t>* taskQueue)
	:m_TaskQueue(taskQueue)
	, m_Stop(true)
{
	Start();
}

TaskHandler::~TaskHandler()
{
	Stop();
}

void TaskHandler::Start()
{
	if (m_Stop.load())
	{
		m_Stop.store(false);
		m_WorkerThread = /* new */ std::thread(&TaskHandler::Run, this);
	}
}

void TaskHandler::Stop()
{
	if (!m_Stop.load())
	{
		m_Stop.store(true);
		if (m_WorkerThread.joinable())
			m_WorkerThread.join();
	}
}

TaskHandler::TaskHandler(const TaskHandler& other)
	:m_TaskQueue(other.m_TaskQueue)
	, m_Stop(true)
{
	if (!other.m_Stop)
		Start();
}

TaskHandler& TaskHandler::operator=(const TaskHandler& other)
{
	if (this != &other)
	{
		Stop();
		m_TaskQueue = other.m_TaskQueue;
		if (!other.m_Stop.load())
			Start();
	}
	return *this;
}