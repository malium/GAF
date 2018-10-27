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

#ifndef GAF_TASKMANAGER_H
#define GAF_TASKMANAGER_H 1

#include "GAF/GAFPrerequisites.h"
#include "GAF/MPMCQueue.h"
#include "GAF/Base/Task.h"
#include "GAF/Base/TaskHandler.h"
#include "GAF/Base/TaskDispatcher.h"

namespace gaf
{
	/*
		Class that gives to the overloaded class a set of functions that
		enable task dispatching and handling and makes small operations
		easier and paralelized
	*/
	class TaskManager
	{
		MPMCQueue<Task_t> m_Tasks;
		std::vector<TaskHandler> m_TaskHandlers;
		std::shared_mutex m_HandlersLock;
		
		struct Dispatcher
		{
			struct DHandler
			{
				MPMCQueue<Task_t> Tasks;
				TaskHandler Handler;
				DHandler();
				~DHandler() {};
				DHandler(const DHandler& other);
				DHandler(DHandler&& other)noexcept;
				DHandler& operator=(const DHandler& other);
				DHandler& operator=(DHandler&& other)noexcept;
			};

			std::shared_mutex HandlerMutex;
			std::vector<DHandler> Handlers;

			Dispatcher() {};
			~Dispatcher() {};
			Dispatcher(const Dispatcher& other);
			Dispatcher(Dispatcher&& other)noexcept;
			Dispatcher& operator=(const Dispatcher& other);
			Dispatcher& operator=(Dispatcher&& other)noexcept;
		};

		std::map<size_t, Dispatcher> m_Dispatchers;
		std::shared_mutex m_DispatchersLock;

		/* Hardware concurrency */
		SIZET m_StartingNum;

	protected:
		void SendTask(TaskDispatcher* dispatcher, const Task_t& task, uint32 handler = static_cast<uint32>(-1));
		uint32 GetPendingTask(TaskDispatcher* dispatcher, uint32 handler);
		friend class TaskDispatcher;
	public:
		TaskManager();
		virtual ~TaskManager();
		
		void RegisterTaskDispatcher(TaskDispatcher* dispatcher, uint32 handlers);

		void UnregisterTaskDispatcher(TaskDispatcher* dispatcher);

		uint32 GetNumberOfHandlers(TaskDispatcher* dispatcher);

		void ChangeNumberOfHandlers(TaskDispatcher* dispatcher, uint32 handlers);

		/*
			Returns the number of TaskHandlers currently available
		*/
		SIZET GetNumberTaskHandlers();

		/*
			Sends a task to the TaskHandlers in order to be executed
			as soon as possible
		*/
		void SendTask(Task_t task);

		/*
			Changes the number of TaskHandlers even though if there
			are too many tasks queued the TaskManager will add more
			This is just if you want preallocating all the future
			needed TaskHandlers.
		*/
		void SetNumberTaskHandlers(SIZET num);
	};
}

#endif /* GAF_TASKMANAGER_H */