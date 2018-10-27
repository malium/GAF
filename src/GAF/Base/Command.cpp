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

#include "GAF/CommandSystem.h"

using namespace gaf;

Command::Command(const uint32 numArgs, const CommandFunc& executeFn, const CommandFunc& undoFn)
	:m_NumArguments(numArgs)
	, m_ExecuteFunc(executeFn)
	, m_UndoFunc(undoFn)
{

}

uint32 Command::GetNumArguments() const
{
	return m_NumArguments;
}

void Command::Execute(const std::vector<std::string>& args)
{
	m_ExecuteFunc(args);
}

void Command::Undo(const std::vector<std::string>& args)
{
	m_UndoFunc(args);
}

StaticCommand::StaticCommand(const std::string name, uint32 numArgs, const Command::CommandFunc& executeFn, const Command::CommandFunc& undoFn)
	:m_NumArguments(numArgs)
	,m_ExecuteFunc(executeFn)
	,m_UndoFunc(undoFn)
	,m_Name(std::move(name))
{
	CommandSystem::RegisterStaticCommand(*this);
}