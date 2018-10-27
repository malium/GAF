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

void TaskDispatcher::SendTask(const Task_t & task, const uint32 handler)
{
	m_Manager->SendTask(this, task, handler);
}

TaskDispatcher::TaskDispatcher(const std::string & name, TaskManager * manager)
	:m_Name(name.empty() ? std::to_string((PTRUINT)this) : name)
	, m_Manager(manager)
{
	Assertion::WhenNullptr(manager, "Trying to attach a TaskDispatcher named:%s, with a nullptr manager.");
}

TaskDispatcher::~TaskDispatcher()
{
	m_Manager->UnregisterTaskDispatcher(this);
}

