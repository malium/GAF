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

#ifndef GAF_I_DISPLAYADAPTER_H
#define GAF_I_DISPLAYADAPTER_H 1

namespace gaf
{
	class IDisplayAdapter
	{
	public:
		virtual MonitorVec GetMonitors()const = 0;
		virtual VideoModeVec GetVideoModes()const = 0;
		virtual IMonitor* GetMonitorWithSize(Resolution_t size)const = 0;
		virtual const std::string& GetName()const = 0;
		virtual const std::string& GetAdapterString()const = 0;
		virtual const std::string& GetAdpaterID()const = 0;
		virtual const std::string& GetAdapterKey()const = 0;
		virtual bool Equals(IDisplayAdapter* other)const = 0;
		virtual bool IsDefault()const = 0;
		virtual bool HasAnyMonitorConnected()const = 0;
		virtual ~IDisplayAdapter() = default;
};
	typedef std::vector<IDisplayAdapter*> DisplayAdapterVec;
}

#endif /* GAF_I_DISPLAYADAPTER_H */