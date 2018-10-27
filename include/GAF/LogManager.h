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

#ifndef GAF_LOGMANAGER_H
#define GAF_LOGMANAGER_H 1

#include "GAF/GAFPrerequisites.h"
#include "GAF/Util/DayTime.h"
#include "GAF/FileSystem.h"

namespace gaf
{
	enum LogLevel
	{
		LL_VERB,
		LL_INFO,
		LL_WARN,
		LL_ERRO,
		LL_CRIT,
		LL_FATL
	};

	const ANSICHAR* GetLogLevelStr(LogLevel ll);
	
	class LogManager
	{
	public:
		using LogHandlerID = uint32;
		static constexpr LogHandlerID NullLogHandlerID = static_cast<LogHandlerID>(-1);
		using LogHandler = std::function<void(const std::string& msg, const DayTime& time, LogLevel level)>;
		static constexpr SIZET MaxLogFiles = 20;
	private:
		struct MessageInfo
		{
			std::string Message;
			DayTime Time;
			LogLevel Level;
		};
		static std::vector<MessageInfo> m_Messages;
		static std::shared_mutex m_MessageMutex;
		std::vector<std::pair<bool, LogHandler>> m_LogHandlers;
		std::shared_mutex m_HandlersMutex;
		std::atomic<LogHandlerID> m_DefaultLogHandler;
		static File* m_DefaultLogFile;
		void DefaultLoggingFn(const std::string& msg, const DayTime& time, const LogLevel level);
		
		void LogTask(const MessageInfo& msg);

		LogManager();
		~LogManager();
	public:

		static LogManager& Instance();
		static LogManager* InstancePtr();

		void EnableDefaultLog(bool enable);
		bool IsDefaultLogEnabled()const;

		SIZET GetNumLogMessages();
		SIZET GetNumLogHandlers();

		LogHandlerID AddLogHandler(const LogHandler& handler, bool pushAllPreviousMessages = false);
		void RemoveLogHandler(LogHandlerID handler);
		bool IsLogHandlerAdded(LogHandlerID handler);

		static void LogMessage(LogLevel ll, PRINTF_FORMAT_STRING const ANSICHAR* message, ...);
		friend class Application;
		friend class Assertion;
	};

	static LogManager* InstanceLog() { return LogManager::InstancePtr(); }
}

#endif /* GAF_LOGMANAGER_H */