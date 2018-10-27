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

#include "GAF/Application.h"
#include "GAF/LogManager.h"
#include "GAF/PropertiesManager.h"
#include "GAF/CommandSystem.h"
#include "GAF/Util/StringUtils.h"

using namespace gaf;

static StaticProperty gConsoleEnabled("ConsoleEnabled", false, false, [](IProperty* prop)
{
	const auto cmdMgr = InstanceCmd();
	const auto isEnabled = cmdMgr->IsConsoleEnabled();
	if (isEnabled != prop->GetBoolValue())
	{
		cmdMgr->EnableConsole(prop->GetBoolValue());
	}
});

struct StaticCmdElem
{
	StaticCommand* Cmd;
	StaticCmdElem* Next;
};
static StaticCmdElem* gCMDHead = nullptr;

CommandInfo gaf::ParseCommand(const std::string& cmdLine)
{
	CommandInfo info;
	
	auto args = StringUtils::SeparateBySpace(cmdLine);
	if (args.empty())
	{
		LogManager::LogMessage(LL_ERRO, "Trying to parse a command with no command name nor "
			"command arguments, '%s'.", cmdLine.c_str());
		return info;
	}
	info.CommandName = args.front();
	if (args.size() > 1)
	{
		args.erase(args.begin());
		info.Arguments = args;
	}
	return info;
}

#if GREAPER_DEBUG_ALLOCATION
static bool gCleanAtExit = false;
#endif

void CommandSystem::RegisterStaticCommand(const StaticCommand cmd)
{
	if (!gCMDHead)
	{
		gCMDHead = new StaticCmdElem();
		gCMDHead->Next = nullptr;
		gCMDHead->Cmd = new StaticCommand(std::move(cmd));
	}
	else
	{
		auto cur = gCMDHead;
		while (cur->Next)
		{
			cur = cur->Next;
		}
		cur->Next = new StaticCmdElem();
		cur->Next->Next = nullptr;
		gCMDHead->Cmd = new StaticCommand(std::move(cmd));
	}
#if GREAPER_DEBUG_ALLOCATION
	if (!gCleanAtExit)
	{
		atexit([]()
		{
			auto cur = gCMDHead;
			while (cur)
			{
				auto prev = cur;
				cur = cur->Next;
				SAFE_DELETE(prev->Cmd);
				SAFE_DELETE(prev);
			}
		});
		gCleanAtExit = true;
	}
#endif
}

CommandSystem::CommandSystem()
{
	LogManager::LogMessage(LL_INFO, "Starting CommandSystem...");

	m_StackMutex.lock();
	auto cur = gCMDHead;
	while (cur)
	{
		AddCommand(cur->Cmd->m_Name, Command(cur->Cmd->m_NumArguments,
			cur->Cmd->m_ExecuteFunc, cur->Cmd->m_UndoFunc));
		cur = cur->Next;
	}

	m_StackMutex.unlock();
}

CommandSystem::~CommandSystem()
{
	LogManager::LogMessage(LL_INFO, "Stopping CommandSystem...");
}

bool CommandSystem::HandleCommand(const CommandInfo& cmdInfo)
{
	if (cmdInfo.CommandName.empty())
	{
		LogManager::LogMessage(LL_WARN, "Trying to handle an unnamed command.");
		return false;
	}
	m_CommandMutex.lock_shared();
	const auto it = m_Commands.find(std::hash<std::string>{}(cmdInfo.CommandName));
	if (it == m_Commands.end())
	{
		m_CommandMutex.unlock_shared();
		LogManager::LogMessage(LL_WARN, "Trying to handled a non-added command: %s.", cmdInfo.CommandName.c_str());
		return false;
	}
	m_CommandMutex.unlock_shared();
	CompletedCommandInfo cci;
	cci.Info = std::move(cmdInfo);
#if GREAPER_DEBUG
	const auto args = StringUtils::ComposeString(cci.Info.Arguments, ", ");
	LogManager::LogMessage(LL_INFO, "Handling command: %s(%s).", cci.Info.CommandName.c_str(),
		args.c_str());
#else
	LogManager::LogMessage(LL_INFO, "Hnadling command: %s.", cci.Info.CommandName.c_str());
#endif
	cci.Start = std::chrono::high_resolution_clock::now();
	it->second.Execute(cci.Info.Arguments);
	cci.End = std::chrono::high_resolution_clock::now();
	m_StackMutex.lock();
	m_CommandStack.push(std::move(cci));
	m_StackMutex.unlock();
	return true;
}

void CommandSystem::UndoCommand()
{
	m_StackMutex.lock();
	if (m_CommandStack.empty())
	{
		m_StackMutex.unlock();
		return;
	}
	m_StackMutex.unlock();
	
	decltype(m_Commands)::iterator it;
	const auto hash = std::hash<std::string>{}(m_CommandStack.top().Info.CommandName);
	
	m_CommandMutex.lock_shared();
	for (it = m_Commands.find(hash); it == m_Commands.end(); it = m_Commands.find(hash))
	{
		m_StackMutex.lock();
		if (m_CommandStack.empty())
		{
			m_CommandMutex.unlock_shared();
			m_StackMutex.unlock();
			return;
		}
		m_CommandStack.pop();
		m_StackMutex.unlock();
	}
	m_CommandMutex.unlock_shared();
	
	m_StackMutex.lock();
#if GREAPER_DEBUG
	const auto args = StringUtils::ComposeString(m_CommandStack.top().Info.Arguments, ", ");
	LogManager::LogMessage(LL_INFO, "Undoing command: %s(%s).", m_CommandStack.top().Info.CommandName.c_str(),
		args.c_str());
#else
	LogManager::LogMessage(LL_INFO, "Undoing command: %s.", m_CommandStack.top().Info.CommandName.c_str());
#endif

	it->second.Undo(m_CommandStack.top().Info.Arguments);
	m_CommandStack.pop();
	m_StackMutex.unlock();
}

bool CommandSystem::RemoveCommand(const std::string & cmdName)
{
	if (cmdName.empty())
	{
		LogManager::LogMessage(LL_WARN, "Trying to remove an unnamed command.");
		return false;
	}
	m_CommandMutex.lock_shared();
	const auto it = m_Commands.find(std::hash<std::string>{}(cmdName));
	if (it == m_Commands.end())
	{
		m_CommandMutex.unlock_shared();
		LogManager::LogMessage(LL_WARN, "Trying to remove a Command '%s' which wasn't added.", cmdName.c_str());
		return false;
	}
	m_CommandMutex.unlock_shared();
	m_CommandMutex.lock();
	m_Commands.erase(it);
	m_CommandMutex.unlock();
	LogManager::LogMessage(LL_INFO, "Command: %s, was successfully removed.");
	return true;
}

bool CommandSystem::AddCommand(const std::string& cmdName, const Command & cmd)
{
	if (cmdName.empty())
	{
		LogManager::LogMessage(LL_WARN, "Trying to add an unnamed command.");
		return false;
	}
	const auto hash = std::hash<std::string>{}(cmdName);

	m_CommandMutex.lock_shared();
	const auto it = m_Commands.find(hash);
	if (it == m_Commands.end())
	{
		m_CommandMutex.unlock_shared();
		LogManager::LogMessage(LL_WARN, "Trying to add a Command '%s' which was already added.", cmdName.c_str());
		return false;
	}
	m_CommandMutex.unlock_shared();
	m_CommandMutex.lock();
	m_Commands.insert_or_assign(hash, cmd);
	m_CommandMutex.unlock();
	LogManager::LogMessage(LL_INFO, "Command: %s, was successfully added.");
	return true;
}

CommandSystem::CommandStack::size_type gaf::CommandSystem::GetStackedCommandsNum() 
{
	m_StackMutex.lock();
	const auto num = m_CommandStack.size();
	m_StackMutex.unlock();
	return num;
}

bool CommandSystem::IsCmdPresent(const std::string & cmdName)
{
	m_CommandMutex.lock_shared();
	const auto it = m_Commands.find(std::hash<std::string>{}(cmdName));
	if (it == m_Commands.end())
	{
		m_CommandMutex.unlock_shared();
		return false;
	}
	m_CommandMutex.unlock_shared();
	return true;
}

bool CommandSystem::GetCommandArgumentNum(const std::string & name, uint32 & num)
{
	m_CommandMutex.lock_shared();
	const auto it = m_Commands.find(std::hash<std::string>{}(name));
	if (it == m_Commands.end())
	{
		m_CommandMutex.unlock_shared();
		return false;
	}
	m_CommandMutex.unlock_shared();
	num = it->second.GetNumArguments();
	return true;	
}

bool CommandSystem::EnableConsole(const bool enable)
{
	NOT_IMPLEMENTED();
	return false;
}

Console* CommandSystem::GetConsole() const
{
	return m_Console;
}

bool CommandSystem::IsConsoleEnabled() const
{
	return m_Console;
}

CommandSystem & gaf::CommandSystem::Instance()
{
	return *InstanceApp()->GetCommandSystem();
}

CommandSystem* gaf::CommandSystem::InstancePtr()
{
	return InstanceApp()->GetCommandSystem();
}