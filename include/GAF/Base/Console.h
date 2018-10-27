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

#ifndef GAF_CONSOLE_H
#define GAF_CONSOLE_H 1

#include "GAF/LogManager.h"

namespace gaf
{
	class Console
	{
	public:
		using ReadInputLineFn = std::function<void(const std::string& msg, Console* console)>;
		using ReadInputFn = std::function<void(ANSICHAR chr, Console* console)>;
		using ReadInputID = size_t;
		using ReadInputLineID = size_t;

	private:
#if PLATFORM_WINDOWS
		HANDLE m_ConsoleHandle;
		HANDLE m_ReadHandle;
		HANDLE m_WriteHandle;
#endif
		std::thread m_ConsoleReadThread;
		void ConsoleReadFn();
		void LogWrite(const std::string& msg, const DayTime& dt, LogLevel ll);
		LogManager::LogHandlerID m_LogRedirect;
		
		std::map<ReadInputID, ReadInputFn> m_ReadInputs;
		std::shared_mutex m_ReadInputMutex;
		std::map<ReadInputLineID, ReadInputLineFn> m_ReadInputLines;
		std::shared_mutex m_ReadInputLineMutex;
	protected:
		Console();
		~Console();
	public:

		bool IsConsoleCreated();
		void CreateConsole();

		bool IsLogRedirectToConsole()const;
		void SetLogRedirection(bool redirect);

		ReadInputID AddInputReadFn(const ReadInputFn& readFn);
		bool IsInputReadRegistered(ReadInputID id);
		bool RemoveInputRead(ReadInputID id);

		ReadInputLineID AddInputReadLineFn(const ReadInputLineFn& readFn);
		bool IsInputReadLineRegistered(ReadInputLineID id);
		bool RemoveInputReadLine(ReadInputLineID id);
	};
}

#endif /* GAF_CONSOLE_H */
