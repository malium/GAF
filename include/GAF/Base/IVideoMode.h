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

#ifndef GAF_I_VIDEOMODE_H
#define GAF_I_VIDEOMODE_H 1

namespace gaf
{
	class IDisplayAdapter;
	class IVideoMode
	{
	public:
		virtual Resolution_t GetResolution()const = 0;
		virtual uint8 GetPixelDepth()const = 0;
		virtual IDisplayAdapter* GetAdapter()const = 0;
		virtual bool Equals(IVideoMode* other)const = 0;
		virtual uint16 GetFrequency()const = 0;
		virtual ~IVideoMode() = default;
	};
	typedef std::vector<IVideoMode*> VideoModeVec;
}

#endif /* GAF_I_VIDEOMODE_H */