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
#include "GAF/FileSystem.h"
#include "GAF/LogManager.h"
#include "GAF/Version.h"
#include "GAF/Util/StringUtils.h"
#include "GAF/PropertiesManager.h"

using namespace gaf;

std::vector<LogManager::MessageInfo> LogManager::m_Messages{};
std::shared_mutex LogManager::m_MessageMutex{};
File* LogManager::m_DefaultLogFile = nullptr;

static StaticProperty gDefaultLogProperty("DEFAULT_LOG_ENABLED", false, false, [](IProperty* prop)
{
	const auto logMgr = InstanceLog();
	const auto isEnabled = logMgr->IsDefaultLogEnabled();
	if (isEnabled != prop->GetBoolValue())
	{
		logMgr->EnableDefaultLog(prop->GetBoolValue());
	}
});

const ANSICHAR* gaf::GetLogLevelStr(const LogLevel ll)
{
	static const ANSICHAR* levels[] =
	{
		"VERBOSE",
		" INFORM",
		"WARNING",
		" ERROR ",
		"CRITICL",
		" FATAL "
	};
	return levels[static_cast<size_t>(ll)];
}

void LogManager::DefaultLoggingFn(const std::string& msg, const DayTime& time, const LogLevel level)
{
	if (m_DefaultLogFile)
	{
		std::string log = '[' + std::string(GetLogLevelStr(level)) + "][" + time.ToString() + "]: " + msg + FileSystem::LineTerminator;
		SIZET written, length = log.length();
		auto rtn = m_DefaultLogFile->StoreContents((void*)log.data(), length, written);
#if GREAPER_DEBUG
		Assertion::WhenInequal(rtn, FileSysError_t::NoError, "ERROR - Something went wrong while logging a message with the FileSystem.");
		Assertion::WhenInequal(length, written, "ERROR - Couldn't log the totality of the message, maybe a file error?");
#endif
	}
}

void LogManager::LogTask(const MessageInfo & msg)
{
	if (!InstanceLog())
		return;
	m_HandlersMutex.lock_shared();
	if (m_LogHandlers.empty())
	{
		m_HandlersMutex.unlock_shared();
		return;
	}
	for (auto it = m_LogHandlers.begin(); it != m_LogHandlers.end(); ++it)
	{
		if (!it->first || !it->second)
			continue;
		it->second(msg.Message, msg.Time, msg.Level);
	}
	m_HandlersMutex.unlock_shared();
}

LogManager::LogManager()
	:m_DefaultLogHandler(NullLogHandlerID)
{
	LogMessage(LL_INFO, "Starting LogManager...");
	m_MessageMutex.lock();
	const auto ver = gaf::GetVersion();
	MessageInfo info;
	info.Message = ver.GetVersionString();
	info.Level = LL_INFO;
	info.Time = m_Messages.front().Time;
	m_Messages.insert(m_Messages.begin(), std::move(info));
	m_MessageMutex.unlock();
}

LogManager::~LogManager()
{
	LogMessage(LL_INFO, "Stopping LogManager...");
}

LogManager& LogManager::Instance()
{
	return *InstanceApp()->GetLogManager();
}

LogManager* LogManager::InstancePtr()
{
	return InstanceApp()->GetLogManager();
}

void LogManager::EnableDefaultLog(const bool enable)
{
	if (enable == IsDefaultLogEnabled())
		return;
	if (enable)
	{
		if (!m_DefaultLogFile)
		{
			const auto root = InstanceFS()->GetRootDirectory();
			Directory* logsDir = root->ContainsDir(L"Logs");
			if (!logsDir)
			{
				const auto err = root->AddDir(L"Logs", logsDir);
				if (err != FileSysError_t::NoError || !logsDir)
				{
					LogMessage(LL_ERRO, "Trying to create the logging files directory but something happened.");
					return;
				}
			}
			/* Remove old ones */
			while (logsDir->GetNumFiles() >= MaxLogFiles)
			{
				/* delete the oldest one */
				DayTime oldestone;
				DayTime current;
				File* oldestFile = nullptr;
				logsDir->LockFileListRead();
				for (auto it = logsDir->GetFileListBegin(); it != logsDir->GetFileListEnd(); ++it)
				{
					const auto fsErr = (*it)->GetCreationTime(current);
					if (fsErr != FileSysError_t::NoError)
					{
						oldestFile = nullptr;
						LogMessage(LL_ERRO, "Trying to clean-up the logging files directory but something happened.");
						break;
					}
					if (current > oldestone)
					{
						oldestone = current;
						oldestFile = *it;
					}
				}
				logsDir->UnlockFileListRead();
				if(oldestFile)
					oldestFile->Erase();
			}
			const auto name = StringUtils::s2ws(DayTime::GetCurrentDayTime().ToString() + ".log");
			auto err = FileSystem::CreateExternalFile(FileSystem::GetExeDirectoryW() + PATH_SEPARATOR_WIDE L"Logs" PATH_SEPARATOR_WIDE, name, m_DefaultLogFile);
			if (err != FileSysError_t::NoError || !m_DefaultLogFile)
			{
				LogMessage(LL_ERRO, "Trying to create the logging file but something happened.");
				return;
			}
			m_DefaultLogFile->Open();
		}
		m_DefaultLogHandler.store(AddLogHandler(
			std::bind(&LogManager::DefaultLoggingFn, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), true), 
			std::memory_order_release);
		gaf::InstanceProp()->GetProperty("DEFAULT_LOG_ENABLED")->SetBoolValue(true);
		LogMessage(LL_VERB, "Default LogHandler was added.");
	}
	else
	{
		if (m_DefaultLogFile)
		{
			m_DefaultLogFile->Close();
			FileSystem::DeleteExternalFile(m_DefaultLogFile);
		}
		RemoveLogHandler(m_DefaultLogHandler);
		m_DefaultLogHandler.store(NullLogHandlerID, std::memory_order_release);
		gaf::InstanceProp()->GetProperty("DEFAULT_LOG_ENABLED")->SetBoolValue(false);
		LogMessage(LL_VERB, "Default LogHandler was removed.");
	}
}

bool LogManager::IsDefaultLogEnabled() const
{
	const auto id = m_DefaultLogHandler.load(std::memory_order_acquire);
	return id != NullLogHandlerID;
}

SIZET LogManager::GetNumLogMessages()
{
	m_MessageMutex.lock_shared();
	const auto rtn = m_Messages.size();
	m_MessageMutex.unlock_shared();
	return rtn;
}

SIZET LogManager::GetNumLogHandlers()
{
	m_HandlersMutex.lock_shared();
	const auto rtn = m_LogHandlers.size();
	m_HandlersMutex.unlock_shared();
	return rtn;
}

LogManager::LogHandlerID LogManager::AddLogHandler(const LogHandler & handler, const bool pushAllPreviousMessages)
{
	LogHandlerID id = NullLogHandlerID;
	decltype(m_LogHandlers)::iterator it;
	m_HandlersMutex.lock_shared();
	for (it = m_LogHandlers.begin(); it != m_LogHandlers.end(); ++it)
	{
		if (!it->first)
		{
			it->second = handler;
			it->first = true;
			id = (LogHandlerID)std::distance(m_LogHandlers.begin(), it);
			break;
		}
	}
	m_HandlersMutex.unlock_shared();
	if (it == m_LogHandlers.end()) /* There wasn't no unused handler */
	{
		m_HandlersMutex.lock();
		m_LogHandlers.emplace_back(std::make_pair(true, handler));
		id = (LogHandlerID)(m_LogHandlers.size() - 1);
		m_HandlersMutex.unlock();
	}
	if (pushAllPreviousMessages)
	{
		m_MessageMutex.lock_shared();
		m_HandlersMutex.lock_shared();
		for (auto itt = m_Messages.begin(); itt != m_Messages.end(); ++itt)
		{
			m_LogHandlers[id].second(itt->Message, itt->Time, itt->Level);			
		}
		m_HandlersMutex.unlock_shared();
		m_MessageMutex.unlock_shared();
	}
	LogMessage(LL_INFO, "Added a new LogHandler with id: %d.", id);
	return id;
}

void LogManager::RemoveLogHandler(const LogHandlerID handler)
{
	if (handler >= GetNumLogHandlers())
	{
		LogMessage(LL_WARN, "Trying to remove a LogHandler which is not added, id: %d.", handler);
		return;
	}
	m_HandlersMutex.lock_shared();
	if (!m_LogHandlers[handler].first)
	{
		m_HandlersMutex.unlock_shared();
		LogMessage(LL_VERB, "Removing an already removed LogHandler, id: %d.", handler);
	}
	else
	{
		m_LogHandlers[handler].first = false;
		m_HandlersMutex.unlock_shared();
		LogMessage(LL_INFO, "Removed a LogHandler with id: %d.", handler);
	}
}

bool LogManager::IsLogHandlerAdded(const LogHandlerID handler)
{
	if (handler >= GetNumLogHandlers())
		return false;
	m_HandlersMutex.lock_shared();
	const auto rtn = m_LogHandlers[handler].first;
	m_HandlersMutex.unlock_shared();
	return rtn;
}

CreateTaskName(LogDispatchTask);

void LogManager::LogMessage(const LogLevel ll, PRINTF_FORMAT_STRING const ANSICHAR * message, ...)
{
	MessageInfo info;
	info.Level = ll;
	va_list ap;
	va_start(ap, message);
	info.Message.resize(strlen(message) + 2048);
	const auto err = vsnprintf(&info.Message[0], info.Message.size(), message, ap);
	Assertion::WhenLess(err, 0, "Error when converting a VA_ARGS to string.");
	info.Message.resize(err);
	info.Message.reserve(err + 1);
	m_MessageMutex.lock();
	m_Messages.emplace_back(info);
	m_MessageMutex.unlock();
	const auto app = InstanceApp();
	if (app)
	{
		const auto logMgr = InstanceLog();
		if (logMgr)
		{
			logMgr->m_HandlersMutex.lock_shared();
			if (!logMgr->m_LogHandlers.empty())
			{
				logMgr->m_HandlersMutex.unlock_shared();
				app->SendTask(CreateTask(LogDispatchTask, std::bind(&LogTask, logMgr, info)));
			}
			else
			{
				logMgr->m_HandlersMutex.unlock_shared();
			}
		}
	}
}



