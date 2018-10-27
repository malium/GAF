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

#ifndef GAF_INPUTSYSTEM_H
#define GAF_INPUTSYSTEM_H 1

#include "GAF/Base/TaskDispatcher.h"
#include "GAF/EventManager.h"

namespace gaf
{
	class InputSystem : public TaskDispatcher
	{
	public:
		using KeyboardEventListener = std::function<void(const KeyboardState& keyboardState, const std::chrono::high_resolution_clock::time_point& eventTime)>;
		using MouseEventListener = std::function<void(const MouseState& mouseState, const std::chrono::high_resolution_clock::time_point& eventTime)>;

		static constexpr auto OnKeyboardFocusLostEvent = "OnKeyboardFocusLost";
		static EventID EventIDOnKeyboardFocusLost;
		static constexpr auto OnKeyboardFocusGainEvent = "OnKeyboardFocusGain";
		static EventID EventIDOnKeyboardFocusGain;
		static constexpr auto OnInputSystemReady = "OnInputSystemReady";
		static EventID EventIDOnInputSystemReady;

		using KeyboardListenerID = size_t;
		using MouseListenerID = size_t;
	private:
		void KeyboardEventSender(KeyboardState keyState, std::chrono::high_resolution_clock::time_point eventTime);
		void MouseEventSender(MouseState mouseState, std::chrono::high_resolution_clock::time_point eventTime);
		MouseState m_MouseState;
		std::shared_mutex m_MouseMutex;
		KeyboardState m_KeyboardState;
		std::shared_mutex m_KeyboardMutex;

		std::atomic<KeyboardListenerID> m_KeyboardListenerIDGen;
		std::map<KeyboardListenerID, KeyboardEventListener> m_KeyboardListeners;
		std::shared_mutex m_KeyboardListenersMutex;

		std::atomic<MouseListenerID> m_MouseListenerIDGen;
		std::map<MouseListenerID, MouseEventListener> m_MouseListeners;
		std::shared_mutex m_MouseListenersMutex;

		Window* m_AttachedWindow;

#if PLATFORM_WINDOWS
		HHOOK m_WindowEvents;
		HHOOK m_MouseEvents;
		HHOOK m_KeyboardEvents;
		friend static LRESULT CALLBACK WindowProc(_In_ int32 nCode, _In_ WPARAM wParam, _In_ LPARAM lParam);
		friend static LRESULT CALLBACK KeyboardProc(_In_ int32 nCode, _In_ WPARAM wParam, _In_ LPARAM lParam);
		friend static LRESULT CALLBACK MouseProc(_In_ int32 nCode, _In_ WPARAM wParam, _In_ LPARAM lParam);
#endif
	public:
		InputSystem();
		InputSystem(const InputSystem& other);
		InputSystem(InputSystem&& other)noexcept;
		InputSystem& operator=(const InputSystem& other);
		InputSystem& operator=(InputSystem&& other)noexcept;

		~InputSystem();

		InputState GetMouseButtonState(EMouseButtons::Type button);
		MouseState::MousePositionInfo GetMousePosition();
		MouseState::MousePositionInfo GetMouseDerivedPosition();
		int16 GetMouseWheelDelta(EMouseWheels::Type wheel);

		InputState GetKeyState(EKey::Type key);

		MouseState GetMouseState();

		KeyboardState GetKeyboardState();

		KeyboardListenerID RegisterKeyboardListener(const KeyboardEventListener& listener);
		bool UnregisterKeyboardListener(KeyboardListenerID listener);
		bool IsKeyboardListenerRegister(KeyboardListenerID listener);

		MouseListenerID RegisterMouseListener(const MouseEventListener& listener);
		bool UnregisterMouseListener(MouseListenerID listener);
		bool IsMouseListenerRegister(MouseListenerID listener);

		bool IsAttached()const;
		void AttachWindow(Window* wnd);
		void Detach();

		Window* GetAttachedWindow()const;
	};
}

#endif /* GAF_INPUTSYSTEM_H */
