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

#ifndef GAF_TASK_H
#define GAF_TASK_H 1

namespace gaf
{
	struct Task_t
	{
		using TaskFn = std::function<void()>;

		std::string Name;
		TaskFn Function;
#if GREAPER_DEBUG
		const ANSICHAR* CallerFn;
		const ANSICHAR* FileName;
		int32 FileLine;
#endif

		void operator()()
		{
#if GREAPER_DEBUG_TASKMAN
			const auto begin = std::chrono::high_resolution_clock::now();
#endif
			Function();
#if GREAPER_DEBUG_TASKMAN
			const auto duration = std::chrono::high_resolution_clock::now() - begin;

			std::string out = "Task " + Name + " Took: " + std::to_string(duration.count() / (1000.0 * 1000.0)) + "ms.\n";
			OutputDebugStringA(out.c_str());
#endif
		}
	};

	namespace Impl
	{
		static void ConcatTaskFn(const std::vector<std::function<void()>>& tasks)
		{
			if (!tasks.empty())
			{
				for (auto it = tasks.begin(); it != tasks.end(); ++it)
					(*it)();
			}
		}
	}

#ifndef CreateTaskName
#define CreateTaskName(taskName)\
static const std::string taskName##_Name = #taskName
#endif

#ifndef CreateTask
#if !GREAPER_DEBUG
#define CreateTask(taskName, taskFn) gaf::Task_t{ taskName##_Name, taskFn }
#else
#define CreateTask(taskName, taskFn) gaf::Task_t{ taskName##_Name, taskFn, __FUNCTION__, __FILE__, __LINE__ }
#endif
#endif

#ifndef CreateConcatTask
#if !GREAPER_DEBUG
#define CreateConcatTask(taskName, ...) gaf::Task_t{ taskName##_Name, std::bind(gaf::Impl::ConcatTaskFn, std::vector<std::function<void()>>{ __VA_ARGS__ }) }
#else
#define CreateConcatTask(taskName, ...) gaf::Task_t{ taskName##_Name, std::bind(gaf::Impl::ConcatTaskFn, std::vector<std::function<void()>>{ __VA_ARGS__ }), __FUNCTION__, __FILE__, __LINE__ }
#endif
#endif
}

#endif /* GAF_TASK_H */
