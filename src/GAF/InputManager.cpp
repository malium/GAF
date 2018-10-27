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
#include "GAF/InputManager.h"

using namespace gaf;

EventID InputSystem::EventIDOnKeyboardFocusLost = EventManager::NullEventID;
EventID InputSystem::EventIDOnKeyboardFocusGain = EventManager::NullEventID;
EventID InputSystem::EventIDOnInputSystemReady = EventManager::NullEventID;

InputManager::InputManager()
	:m_NextID(0)
{
	LogManager::LogMessage(LL_INFO, "Starting InputManager...");
	const auto eventMgr = InstanceEvent();
	InputSystem::EventIDOnKeyboardFocusLost = eventMgr->RegisterEvent(InputSystem::OnKeyboardFocusLostEvent);
	InputSystem::EventIDOnKeyboardFocusGain = eventMgr->RegisterEvent(InputSystem::OnKeyboardFocusGainEvent);
	InputSystem::EventIDOnInputSystemReady = eventMgr->RegisterEvent(InputSystem::OnInputSystemReady);
}

InputManager::~InputManager()
{
	LogManager::LogMessage(LL_INFO, "Stopping InputManager...");
	const auto eventMgr = InstanceEvent();
	eventMgr->UnregisterEvent(InputSystem::EventIDOnKeyboardFocusLost);
	eventMgr->UnregisterEvent(InputSystem::EventIDOnKeyboardFocusGain);
	eventMgr->UnregisterEvent(InputSystem::EventIDOnInputSystemReady);
	InputSystem::EventIDOnKeyboardFocusLost = EventManager::NullEventID;
	InputSystem::EventIDOnKeyboardFocusGain = EventManager::NullEventID;
	InputSystem::EventIDOnInputSystemReady = EventManager::NullEventID;
}

InputManager& InputManager::Instance()
{
	return *(InstanceApp()->GetInputManager());
}

InputManager* InputManager::InstancePtr()
{
	return InstanceApp()->GetInputManager();
}

InputManager::InputSystemID InputManager::CreateSystem(Window* window)
{
	if (window)
	{
		m_InputSystemsMutex.lock_shared();
		for (auto it = m_InputSystems.begin(); it != m_InputSystems.end(); ++it)
		{
			if (it->second.GetAttachedWindow() == window)
			{
				m_InputSystemsMutex.unlock_shared();
				return it->first;
			}
		}
		m_InputSystemsMutex.unlock_shared();
	}

	const auto id = m_NextID.load();
	++m_NextID;

	m_InputSystemsMutex.lock();
	m_InputSystems.insert_or_assign(id, InputSystem());
	if(window)
		m_InputSystems.at(id).AttachWindow(window);
	m_InputSystemsMutex.unlock();

	return id;
}

bool InputManager::DestroySystem(const InputSystemID inputSystem)
{
	m_InputSystemsMutex.lock_shared();
	const auto it = m_InputSystems.find(inputSystem);
	if (it != m_InputSystems.end())
	{
		m_InputSystemsMutex.unlock_shared();
		m_InputSystemsMutex.lock();
		m_InputSystems.erase(it);
		m_InputSystemsMutex.unlock();
		return true;
	}
	else
	{
		m_InputSystemsMutex.unlock_shared();
		return false;
	}
}

InputSystem* InputManager::GetInputSystem(const InputSystemID inputSystem)
{
	m_InputSystemsMutex.lock_shared();
	const auto it = m_InputSystems.find(inputSystem);
	if (it != m_InputSystems.end())
	{
		const auto sys = &m_InputSystems[inputSystem];
		m_InputSystemsMutex.unlock_shared();
		return sys;
	}
	else
	{
		m_InputSystemsMutex.unlock_shared();
		return nullptr;
	}
}

InputSystem* InputManager::GetInputSystem(Window * attachedWindow)
{
	InputSystem* rtn = nullptr;
	m_InputSystemsMutex.lock_shared();
	for (auto it = m_InputSystems.begin(); it != m_InputSystems.end(); ++it)
		if (it->second.GetAttachedWindow() == attachedWindow)
			return &(it->second);
	m_InputSystemsMutex.unlock_shared();
	return rtn;
}







