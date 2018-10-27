/***********************************************************************************
* Copyright 2018 Marcos S�nchez Torrent                                            *
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

#ifndef GAF_I_MONITOR_H
#define GAF_I_MONITOR_H 1

namespace gaf
{
	typedef
#if PLATFORM_WINDOWS
		HMONITOR
#else
		void*
#endif
		MonitorHandle;

	class IDisplayAdapter;

	class IMonitor
	{
	public:
		virtual Resolution_t GetSize()const = 0;
		virtual Resolution_t GetWorkArea()const = 0;
		virtual MonitorHandle GetOSHandle()const = 0;
		virtual Position_t GetDesktopPosition()const = 0;
		virtual IDisplayAdapter* GetAdapter()const = 0;
		virtual const std::string& GetName()const = 0;
		virtual const std::string& GetMonitorString()const = 0;
		virtual const std::string& GetMonitorID()const = 0;
		virtual const std::string& GetMonitorKey()const = 0;
		virtual bool Equals(IMonitor* other)const = 0;
		virtual bool IsPrimary()const = 0;
		virtual ~IMonitor() = default;
	};
	typedef std::vector<IMonitor*> MonitorVec;
}

#endif /* GAF_I_MONITOR_H */