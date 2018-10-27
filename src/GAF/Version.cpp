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

#include "GAF/Version.h"
#include "GAF/PropertiesManager.h"

using namespace gaf;

#if GREAPER_DEBUG
#define SUFIX0 "Debug ("
#elif defined(GREAPER_FRELEASE)
#define SUFIX0 "Public Relase"
#else
#if ((GREAPER_DEBUG_COMMANDS + GREAPER_DEBUG_ALLOCATION + GREAPER_DEBUG_EVENTS + GREAPER_DEBUG_FILESYS + GREAPER_DEBUG_INPUT + GREAPER_DEBUG_PROPERTIES + GREAPER_DEBUG_TASKMAN + GREAPER_DEBUG_WINDOW) > 0)
#define SUFIX0 "Private Relase ("
#else
#define SUFIX0 "Private Relase"
#endif
#endif

#if GREAPER_DEBUG_ALLOCATION
#define SUFIX1 " MEMLEAK_TEST "
#else
#define SUFIX1
#endif

#if GREAPER_DEBUG_COMMANDS
#define SUFIX2 " CMD_DBG "
#else
#define SUFIX2
#endif

#if GREAPER_DEBUG_EVENTS
#define SUFIX3 " EVT_DBG "
#else
#define SUFIX3
#endif

#if GREAPER_DEBUG_FILESYS
#define SUFIX4 " FS_DBG "
#else
#define SUFIX4
#endif

#if GREAPER_DEBUG_INPUT
#define SUFIX5 " INP_DBG "
#else
#define SUFIX5
#endif

#if GREAPER_DEBUG_PROPERTIES
#define SUFIX6 " PROP_DBG "
#else
#define SUFIX6
#endif

#if GREAPER_DEBUG_TASKMAN
#define SUFIX7 " TASK_DBG "
#else
#define SUFIX7
#endif

#if GREAPER_DEBUG_WINDOW
#define SUFIX8 " WND_DBG "
#else
#define SUFIX8
#endif

#if GREAPER_DEBUG
#define SUFIX9 ")"
#else
#if ((GREAPER_DEBUG_COMMANDS + GREAPER_DEBUG_ALLOCATION + GREAPER_DEBUG_EVENTS + GREAPER_DEBUG_FILESYS + GREAPER_DEBUG_INPUT + GREAPER_DEBUG_PROPERTIES + GREAPER_DEBUG_TASKMAN + GREAPER_DEBUG_WINDOW) > 0)
#define SUFIX9 ")"
#else
#define SUFIX9
#endif
#endif

#ifndef GREAPER_FRELEASE
#define SUFIX SUFIX0 SUFIX1 SUFIX2 SUFIX3 SUFIX4 SUFIX5 SUFIX6 SUFIX7 SUFIX8 SUFIX9
#else
#define SUFIX SUFIX0
#endif

static const Version gGAFVersion = {
	"GAF"
	,GREAPER_APPLICATION_FRAMEWORK_MAJOR
	,GREAPER_APPLICATION_FRAMEWORK_MINOR
	,GREAPER_APPLICATION_FRAMEWORK_PATCH
	,GREAPER_APPLICATION_FRAMEWORK_REV
	,SUFIX
};

static StaticProperty gafVersionMajor("GAF_VERSION_MAJOR", true, GREAPER_APPLICATION_FRAMEWORK_MAJOR, 1.f, -1.f, {});
static StaticProperty gafVersionMinor("GAF_VERSION_MINOR", true, GREAPER_APPLICATION_FRAMEWORK_MINOR, 1.f, -1.f, {});
static StaticProperty gafVersionPatch("GAF_VERSION_PATCH", true, GREAPER_APPLICATION_FRAMEWORK_PATCH, 1.f, -1.f, {});
static StaticProperty gafVersionRev("GAF_VERSION_REV", true, GREAPER_APPLICATION_FRAMEWORK_REV, 1.f, -1.f, {});
static StaticProperty gafVersion("GAF_VERSION", true, std::to_string(GREAPER_APPLICATION_FRAMEWORK_MAJOR) + "." + std::to_string(GREAPER_APPLICATION_FRAMEWORK_MINOR)
	+ "." + std::to_string(GREAPER_APPLICATION_FRAMEWORK_PATCH) + "." + std::to_string(GREAPER_APPLICATION_FRAMEWORK_REV), {}, {});
#ifndef GREAPER_FRELEASE
#if GREAPER_DEBUG
static StaticProperty gafCompilation("GAF_COMPILATION", true, "Debug", {}, {});
#else
static StaticProperty gafCompilation("GAF_COMPILATION", true, "PrivateRelease", {}, {});
#endif
#else
static StaticProperty gafCompilation("GAF_COMPILATION", true, "PublicRelease", {}, {});
#endif

Version gaf::GetVersion()
{
	return gGAFVersion;
}

Version::Version(const std::string versionPrefix, const uint16 major, const uint16 minor,
	const uint16 patch, const uint16 revision, const std::string versionSufix)
	:m_VersionPrefix(versionPrefix)
	,m_Major(major)
	,m_Minor(minor)
	,m_Patch(patch)
	,m_Revision(revision)
	,m_VersionSufix(versionSufix)
{

}

std::string Version::GetPrefix()const
{
	return m_VersionPrefix;
}

uint16 Version::GetMajor()const
{
	return m_Major;
}

uint16 Version::GetMinor()const
{
	return m_Minor;
}

uint16 Version::GetPatch()const
{
	return m_Patch;
}

uint16 Version::GetRevision()const
{
	return m_Revision;
}

std::string Version::GetSufix()const
{
	return m_VersionSufix;
}

std::string Version::GetVersionStringShort()const
{
	return std::to_string(m_Major) + "." + std::to_string(m_Minor) +
		"." + std::to_string(m_Patch) + "." + std::to_string(m_Revision);
}

std::string Version::GetVersionString()const
{
	return m_VersionPrefix + " " + GetVersionStringShort() + " "
		+ m_VersionSufix;
}