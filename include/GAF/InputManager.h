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

#ifndef GAF_INPUTMANAGER_H
#define GAF_INPUTMANAGER_H 1

#include "GAF/GAFPrerequisites.h"
#include "GAF/Base/Mouse.h"
#include "GAF/Base/Keyboard.h"
#include "GAF/Base/InputSystem.h"

namespace gaf
{
	class InputManager
	{
	public:
		using InputSystemID = uint32;
		static constexpr InputSystemID NullInputSystemID = static_cast<InputSystemID>(-1);
	private:
		InputManager();
		~InputManager();

		std::map<InputSystemID, InputSystem> m_InputSystems;
		std::shared_mutex m_InputSystemsMutex;
		std::atomic<InputSystemID> m_NextID;

#if PLATFORM_WINDOWS
		friend static LRESULT CALLBACK WindowProc(_In_ int32 nCode, _In_ WPARAM wParam, _In_ LPARAM lParam);
		friend static LRESULT CALLBACK KeyboardProc(_In_ int32 nCode, _In_ WPARAM wParam, _In_ LPARAM lParam);
		friend static LRESULT CALLBACK MouseProc(_In_ int32 nCode, _In_ WPARAM wParam, _In_ LPARAM lParam);
#endif
	public:

		InputSystemID CreateSystem(Window* window = nullptr);
		bool DestroySystem(InputSystemID inputSystem);

		void SendKeyEventWindow(Window* window, EKey::Type key, InputState state);
		void SendMouseMoveEventWindow(Window* window, int32 x, int32 y);
		void SendMouseButtonEventWindow(Window* window, EMouseButtons::Type button, InputState state);
		void SendMouseWheelEventWindow(Window* window, EMouseWheels::Type wheel, int16 delta);

		void SetMousePosition(int32 x, int32 y)const;
		MouseState::MousePositionInfo GetMousePosition()const;

		InputState IsButtonDown(EMouseButtons::Type button)const;
		InputState IsKeyDown(EKey::Type key)const;

		InputSystem* GetInputSystem(InputSystemID inputSystem);
		InputSystem* GetInputSystem(Window* attachedWindow);

		static InputManager& Instance();
		static InputManager* InstancePtr();

		friend class Application;
	};

	static InputManager* InstanceInput() { return InputManager::InstancePtr(); }
}

#endif /* GAF_INPUTMANAGER_H */