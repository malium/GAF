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

#ifndef GAF_VERSION_H
#define GAF_VERSION_H 1

#include "GAF/GAFPrerequisites.h"

namespace gaf
{
	class Version
	{
		const std::string m_VersionPrefix;
		const uint16 m_Major;
		const uint16 m_Minor;
		const uint16 m_Patch;
		const uint16 m_Revision;
		const std::string m_VersionSufix;
	public:
		/*
			Prefix Major.Minor.Patch.Revision Sufix
			Example:
			GAF 0.1.25.16 Debug (MemDebug | TaskDebug | EventDebug | PropertyDebug | InputDebug)
		*/
		Version(std::string versionPrefix = "Unknown", uint16 major = 0,
			uint16 minor = 0, uint16 patch = 0, uint16 revision = 0, 
			std::string versionSufix = "");

		Version(const Version&) = default;
		Version(Version&&)noexcept = default;
		Version& operator=(const Version&) = default;
		Version& operator=(Version&&)noexcept = default;

		std::string GetPrefix()const;
		uint16 GetMajor()const;
		uint16 GetMinor()const;
		uint16 GetPatch()const;
		uint16 GetRevision()const;
		std::string GetSufix()const;

		/* It doesn't not include any string attached to it, just the numbers */
		std::string GetVersionStringShort()const;
		std::string GetVersionString()const;
	};

	Version GetVersion();
}

#endif /* GAF_VERSION_H */