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

#ifndef GAF_COMMAND_SYSTEM_H
#define GAF_COMMAND_SYSTEM_H 1

#include "GAF/GAFPrerequisites.h"
#include "GAF/Base/Command.h"
#include "GAF/Base/Console.h"

namespace gaf
{
	struct CommandInfo
	{
		std::vector<std::string> Arguments;
		std::string CommandName;
	};

	CommandInfo ParseCommand(const std::string& cmdLine);

	class CommandSystem
	{
	public:
		using TimePoint = std::chrono::high_resolution_clock::time_point;
		struct CompletedCommandInfo
		{
			CommandInfo Info;
			TimePoint Start;
			TimePoint End;
		};
		using CommandStack = std::stack<CompletedCommandInfo>;
	private:
		/*
			Adds a Command to the command list, but before the CommandSystem is initialized,
			so when its initialized the command will be automatically inserted.
		*/
		static void RegisterStaticCommand(StaticCommand cmd);

		std::map<size_t, Command> m_Commands;
		std::shared_mutex m_CommandMutex;

		CommandStack m_CommandStack;
		std::mutex m_StackMutex;

		Console* m_Console;

		CommandSystem();
		~CommandSystem();
	public:

		/*
			Sends a CommandInfo to be handled by the command which has that name.
			Return:
				true - The command existed and was handled correctly.
				false - The command name was not found or another thing went wrong,
						check the log in order to get more info.
		*/
		bool HandleCommand(const CommandInfo& cmdInfo);
		/*
			Undo's the last command don't overuse the CommandSystem, let just the user
			use it, because commands can enter the stack in a multithreaded way so
			there's no garantee that the last command is the one you send it.
		*/
		void UndoCommand();

		/*
			Removes a command from the command list, so trying to handle a command
			with that name will not be possible anymore, this will also remove all
			the handled commands from the stack because they cannot be Undo anymore.
			Return:
				true - The command was successfully removed.
				false - That command name was not found.
		*/
		bool RemoveCommand(const std::string& cmdName);
		/*
			Adds a command to the command list, where every time a CommandInfo with
			this command name is trying to be handled, this Command will be used.
			Return:
				true - The command addition went ok.
				false - The command was already existant or the command name was bad.
		*/
		bool AddCommand(const std::string& cmdName, const Command& cmd);

		/*
			Returns the number of handled commands.
		*/
		CommandStack::size_type GetStackedCommandsNum();

		/*
			Returns true when that command name is inside the command list
			otherwise false.
		*/
		bool IsCmdPresent(const std::string& cmdName);

		/*
			Returns the Command with that name
			Return:
				true - The command was successfully found and the number returned
				false - There was no command with that name.
		*/
		bool GetCommandArgumentNum(const std::string& name, uint32& num);

		bool EnableConsole(bool enable);
		Console* GetConsole()const;
		bool IsConsoleEnabled()const;

		static CommandSystem& Instance();
		static CommandSystem* InstancePtr();
		
		friend class Application;
		friend class StaticCommand;
	};

	static CommandSystem* InstanceCmd() { return CommandSystem::InstancePtr(); }
}

#endif /* GAF_COMMAND_SYSTEM_H */