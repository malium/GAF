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

#ifndef GAF_TASKHANDLER_H
#define GAF_TASKHANDLER_H 1

#include "GAF/MPMCQueue.h"

namespace gaf
{
	/*
		Internal class that enables the handling of tasks
	*/
	class TaskHandler
	{
		static constexpr auto WaitTimeMS = std::chrono::milliseconds(1);
		/* A link to the task queue */
		MPMCQueue<Task_t>* m_TaskQueue;
		/* The thread obj */
		std::thread m_WorkerThread;
		/* The thread function */
		void Run();
		/* Flag that tells the thread to stop */
		AtomicFlag m_Stop;
	public:
		TaskHandler()
			:m_TaskQueue(nullptr)
			, m_Stop(true)
		{

		}
		/* Only TaskManager can create or destroy TaskHandlers */
		TaskHandler(MPMCQueue<Task_t>* taskQueue);

		/*
			Starts the thread and begins the tasks handling
			Only TaskManager can do this
		*/
		void Start();

		/*
			Finishes the current task and stops the thread
			Only TaskManager can do this
		*/
		void Stop();

		TaskHandler(TaskHandler&& other)noexcept = default;
		TaskHandler& operator=(TaskHandler&& other)noexcept = default;
		TaskHandler(const TaskHandler& other);
		TaskHandler& operator=(const TaskHandler& other);
		~TaskHandler();
		friend class TaskManager;
	};
}

#endif /* GAF_TASKHANDLER_H */
