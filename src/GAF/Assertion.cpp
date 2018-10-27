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

#include "GAF/GAFPrerequisites.h"
#include "GAF/LogManager.h"
#include "GAF/FileSystem.h"

using namespace gaf;

void Assertion::LogAssertion(const ANSICHAR* msg)
{
	std::ofstream errorFile("error.log", std::ios::out | std::ios::trunc);
	errorFile << "ERROR - Something went wrong! Assertion message:" << FileSystem::LineTerminator << msg <<
		FileSystem::LineTerminator << "Pushing all log file into here:" << FileSystem::LineTerminator;
	LogManager::m_MessageMutex.lock_shared();
	const auto& messages = LogManager::m_Messages;
	for (auto it = messages.begin(); it != messages.end(); ++it)
	{
		errorFile << '[' << GetLogLevelStr(it->Level) << "][" << it->Time.ToString() << "]: " << it->Message << FileSystem::LineTerminator;
	}
	LogManager::m_MessageMutex.unlock_shared();
	errorFile << " --- END LOG FILE --- ";
	errorFile.close();
}