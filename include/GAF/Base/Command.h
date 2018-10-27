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

#ifndef GAF_COMMAND_H
#define GAF_COMMAND_H 1

namespace gaf
{
	class Command
	{
	public:
		using CommandFunc = std::function<void(const std::vector<std::string>& args)>;
	protected:
		uint32 m_NumArguments;
		CommandFunc m_ExecuteFunc;
		CommandFunc m_UndoFunc;
	public:
		Command() = delete;
		Command(uint32 numArgs, const CommandFunc& executeFn, const CommandFunc& undoFn);
		~Command() = default;

		static constexpr uint32 UnlimitedArguments = static_cast<uint32>(-1);

		uint32 GetNumArguments()const;

		void Execute(const std::vector<std::string>& arg);
		void Undo(const std::vector<std::string>& arg);
	};

	class StaticCommand
	{
	public:
		uint32 m_NumArguments;
		Command::CommandFunc m_ExecuteFunc;
		Command::CommandFunc m_UndoFunc;
		std::string m_Name;
		StaticCommand() = delete;
		StaticCommand(std::string name, uint32 numArgs, const Command::CommandFunc& executeFn, const Command::CommandFunc& undoFn);
		~StaticCommand() = default;
	};
}

#endif /* GAF_COMMAND_H */
