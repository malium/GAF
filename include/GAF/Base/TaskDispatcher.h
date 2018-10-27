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

#ifndef GAF_TASKDISPATCHER_H
#define GAF_TASKDISPATCHER_H 1

#include "GAF/Base/Task.h"

namespace gaf
{
	class TaskDispatcher
	{
		const std::string m_Name;
		TaskManager* m_Manager;

	protected:
		void SendTask(const Task_t& task, uint32 handler = static_cast<uint32>(-1));
	public:
		TaskDispatcher() = delete;
		TaskDispatcher(const std::string& name, TaskManager* manager);
		virtual ~TaskDispatcher();
	};
}

#endif /* GAF_TASKDISPATCHER_H */
