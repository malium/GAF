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

#ifndef GAF_HWDETECTOR_H
#define GAF_HWDETECTOR_H 1

#include "GAF/GAFPrerequisites.h"
#include "GAF/Base/CPUInfo.h"

namespace gaf
{
	class HWDetector
	{
		CPUInfo m_Info;
		HWDetector();
		~HWDetector() = default;
		static HWDetector* m_Instance;
	public:

		static HWDetector* Instance();
		void Stop();

		uint32 GetNumberLogicalCores()const;
		uint32 GetNumberPhysicalCores()const;
		CPUInfo GetCPUInfo()const;
		uint64 GetRAMAmount()const;
		std::string GetOSNameAndVer();
	};
	static HWDetector* InstanceHW() { return HWDetector::Instance(); }
}

#endif /* GAF_HWDETECTOR_H */