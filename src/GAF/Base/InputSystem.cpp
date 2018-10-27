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

#include "GAF/TaskManager.h"
#include "GAF/Application.h"
#include "GAF/LogManager.h"
#include "GAF/WindowManager.h"
#include "GAF/InputManager.h"

using namespace gaf;

EKey::Type gKeyConversion[] =
{
	EKey::NUM_KEYS,
	EKey::NUM_KEYS,
	EKey::NUM_KEYS,
	EKey::NUM_KEYS,
	EKey::NUM_KEYS,
	EKey::NUM_KEYS,
	EKey::NUM_KEYS,
	EKey::NUM_KEYS,
	EKey::BACKSPACE,
	EKey::TAB,
	EKey::NUM_KEYS,
	EKey::NUM_KEYS,
	EKey::CLEAR,
	EKey::ENTER,
	EKey::NUM_KEYS,
	EKey::NUM_KEYS,
	EKey::SHIFT,
	EKey::CTRL,
	EKey::ALT,
	EKey::PAUSE,
	EKey::CAPSLOCK,
	EKey::NUM_KEYS,
	EKey::NUM_KEYS,
	EKey::NUM_KEYS,
	EKey::NUM_KEYS,
	EKey::NUM_KEYS,
	EKey::NUM_KEYS,
	EKey::ESCAPE,
	EKey::NUM_KEYS,
	EKey::NUM_KEYS,
	EKey::NUM_KEYS,
	EKey::NUM_KEYS,
	EKey::SPACE,
	EKey::PAGEUP,
	EKey::PAGEDOWN,
	EKey::END,
	EKey::HOME,
	EKey::LEFT_ARROW,
	EKey::UP_ARROW,
	EKey::RIGHT_ARROW,
	EKey::DOWN_ARROW,
	EKey::SELECT,
	EKey::PRINT,
	EKey::EXECUTE,
	EKey::PRINTSCREEN,
	EKey::INS,
	EKey::DEL,
	EKey::HELP,
	EKey::N0,
	EKey::N1,
	EKey::N2,
	EKey::N3,
	EKey::N4,
	EKey::N5,
	EKey::N6,
	EKey::N7,
	EKey::N8,
	EKey::N9,
	EKey::NUM_KEYS,
	EKey::NUM_KEYS,
	EKey::NUM_KEYS,
	EKey::NUM_KEYS,
	EKey::NUM_KEYS,
	EKey::NUM_KEYS,
	EKey::NUM_KEYS,
	EKey::A,
	EKey::B,
	EKey::C,
	EKey::D,
	EKey::E,
	EKey::F,
	EKey::G,
	EKey::H,
	EKey::I,
	EKey::J,
	EKey::K,
	EKey::L,
	EKey::M,
	EKey::N,
	EKey::O,
	EKey::P,
	EKey::Q,
	EKey::R,
	EKey::S,
	EKey::T,
	EKey::U,
	EKey::V,
	EKey::W,
	EKey::X,
	EKey::Y,
	EKey::Z,
	EKey::LSYS,
	EKey::RSYS,
	EKey::APPS,
	EKey::NUM_KEYS,
	EKey::SLEEP,
	EKey::NP0,
	EKey::NP1,
	EKey::NP2,
	EKey::NP3,
	EKey::NP4,
	EKey::NP5,
	EKey::NP6,
	EKey::NP7,
	EKey::NP8,
	EKey::NP9,
		EKey::NP_MULTIPLY,
		EKey::NP_ADD,
		EKey::NP_SEPARATOR,
		EKey::NP_SUBSTRACT,
		EKey::NP_DECIMAL,
		EKey::NP_DIVIDE,
		EKey::F1,
		EKey::F2,
		EKey::F3,
		EKey::F4,
		EKey::F5,
		EKey::F6,
		EKey::F7,
		EKey::F8,
		EKey::F9,
		EKey::F10,
		EKey::F11,
		EKey::F12,
		EKey::F13,
		EKey::F14,
		EKey::F15,
		EKey::F16,
		EKey::F17,
		EKey::F18,
		EKey::F19,
		EKey::F20,
		EKey::F21,
		EKey::F22,
		EKey::F23,
		EKey::F24,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NP_NUMLOCK,
		EKey::SCROLL,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::LSHIFT,
		EKey::RSHIFT,
		EKey::LCTRL,
		EKey::RCTRL,
		EKey::LALT,
		EKey::RALT,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::US_SEMICOLON,
		EKey::PLUS,
		EKey::COMMA,
		EKey::MINUS,
		EKey::PERIOD,
		EKey::US_FWSLASH,
		EKey::US_TILDE,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
		EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::US_LBRACKET,
			EKey::US_BACKSLASH,
			EKey::US_RBRACKET,
			EKey::US_SINGLEQUOTE,
			EKey::OEM_8,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::US_LESSTHAN,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
			EKey::NUM_KEYS,
};

#if PLATFORM_WINDOWS
GREAPER_THLOCAL InputSystem* m_CurrentInputSystem = nullptr;
CreateTaskName(MouseEventTask);
CreateTaskName(KeyboardEventTask);
LRESULT gaf::WindowProc(const int32 nCode, const WPARAM wParam, const LPARAM lParam)
{
	if (nCode < 0)
		return CallNextHookEx(nullptr, nCode, wParam, lParam);

	CWPSTRUCT* wndStruct = (CWPSTRUCT*)lParam;

	if (!m_CurrentInputSystem)
	{
		const auto inputSys = InstanceInput();
		inputSys->m_InputSystemsMutex.lock_shared();
		for (auto it = inputSys->m_InputSystems.begin();
			it != inputSys->m_InputSystems.end(); ++it)
		{
			if (it->second.GetAttachedWindow()->GetWindowHandle() == wndStruct->hwnd)
				m_CurrentInputSystem = &it->second;
		}
		inputSys->m_InputSystemsMutex.unlock_shared();
		if (!m_CurrentInputSystem)
			return CallNextHookEx(nullptr, nCode, wParam, lParam);
	}

	const auto loWParam = LOWORD(wndStruct->wParam), hiWParam = HIWORD(wndStruct->wParam);
	const auto loLParam = LOWORD(wndStruct->lParam), hiLParam = HIWORD(wndStruct->lParam);

	const auto time = std::chrono::high_resolution_clock::now();

	if (wndStruct->message == WM_MOUSEWHEEL)
	{
		m_CurrentInputSystem->m_MouseMutex.lock();
		m_CurrentInputSystem->m_MouseState.WheelInfo.Delta[EMouseWheels::VERTICAL] = *(int16*)&hiWParam;
		m_CurrentInputSystem->m_MouseState.MouseEvent = EMouseEvent::WHEEL_CHANGE;
		m_CurrentInputSystem->SendTask(CreateTask(MouseEventTask, std::bind(&InputSystem::MouseEventSender, m_CurrentInputSystem, m_CurrentInputSystem->m_MouseState, time)));
		m_CurrentInputSystem->m_MouseMutex.unlock();
	}
	else if (wndStruct->message == WM_KILLFOCUS)
	{
		m_CurrentInputSystem->m_KeyboardMutex.lock();
		m_CurrentInputSystem->m_KeyboardState = KeyboardState{};
		m_CurrentInputSystem->m_KeyboardState.LastKeyModified = EKey::NUM_KEYS;
		m_CurrentInputSystem->SendTask(CreateTask(KeyboardEventTask, std::bind(&InputSystem::KeyboardEventSender, m_CurrentInputSystem, m_CurrentInputSystem->m_KeyboardState, time)));
		m_CurrentInputSystem->m_KeyboardMutex.unlock();
		InstanceEvent()->DispatchEvent(InputSystem::EventIDOnKeyboardFocusLost, m_CurrentInputSystem);
	}
	else if (wndStruct->message == WM_SETFOCUS)
	{
		InstanceEvent()->DispatchEvent(InputSystem::EventIDOnKeyboardFocusGain, m_CurrentInputSystem);
	}
	else if (wndStruct->message == WM_XBUTTONDOWN)
	{
		const auto button = GET_XBUTTON_WPARAM(wndStruct->wParam);
		if (button == XBUTTON1)
		{
			m_CurrentInputSystem->m_MouseMutex.lock();
			if (m_CurrentInputSystem->m_MouseState.ButtonInfo.State[EMouseButtons::MB_XBUTTON1] == InputState::DOWN)
			{
				m_CurrentInputSystem->m_MouseMutex.unlock();
				return CallNextHookEx(nullptr, nCode, wParam, lParam);
			}
			m_CurrentInputSystem->m_MouseState.ButtonInfo.State[EMouseButtons::MB_XBUTTON1] = InputState::DOWN;
			m_CurrentInputSystem->m_MouseState.MouseEvent = EMouseEvent::BUTTON_STATE_CHANGE;
			m_CurrentInputSystem->m_MouseState.EventButton = EMouseButtons::MB_XBUTTON1;
			m_CurrentInputSystem->SendTask(CreateTask(MouseEventTask, std::bind(&InputSystem::MouseEventSender, m_CurrentInputSystem, m_CurrentInputSystem->m_MouseState, time)));
			m_CurrentInputSystem->m_KeyboardMutex.unlock();
		}
		else if(button == XBUTTON2)
		{
			m_CurrentInputSystem->m_MouseMutex.lock();
			if (m_CurrentInputSystem->m_MouseState.ButtonInfo.State[EMouseButtons::MB_XBUTTON2] == InputState::DOWN)
			{
				m_CurrentInputSystem->m_MouseMutex.unlock();
				return CallNextHookEx(nullptr, nCode, wParam, lParam);
			}
			m_CurrentInputSystem->m_MouseState.ButtonInfo.State[EMouseButtons::MB_XBUTTON2] = InputState::DOWN;
			m_CurrentInputSystem->m_MouseState.MouseEvent = EMouseEvent::BUTTON_STATE_CHANGE;
			m_CurrentInputSystem->m_MouseState.EventButton = EMouseButtons::MB_XBUTTON2;
			m_CurrentInputSystem->SendTask(CreateTask(MouseEventTask, std::bind(&InputSystem::MouseEventSender, m_CurrentInputSystem, m_CurrentInputSystem->m_MouseState, time)));
			m_CurrentInputSystem->m_KeyboardMutex.unlock();
		}
		else
		{
			printf("\nXBUTTON%d pressed!\n", button);
		}
	}
	else if (wndStruct->message == WM_XBUTTONUP)
	{
		const auto button = GET_XBUTTON_WPARAM(wndStruct->wParam);
		if (button == XBUTTON1)
		{
			m_CurrentInputSystem->m_MouseMutex.lock();
			if (m_CurrentInputSystem->m_MouseState.ButtonInfo.State[EMouseButtons::MB_XBUTTON1] == InputState::UP)
			{
				m_CurrentInputSystem->m_MouseMutex.unlock();
				return CallNextHookEx(nullptr, nCode, wParam, lParam);
			}
			m_CurrentInputSystem->m_MouseState.ButtonInfo.State[EMouseButtons::MB_XBUTTON1] = InputState::UP;
			m_CurrentInputSystem->m_MouseState.MouseEvent = EMouseEvent::BUTTON_STATE_CHANGE;
			m_CurrentInputSystem->m_MouseState.EventButton = EMouseButtons::MB_XBUTTON1;
			m_CurrentInputSystem->SendTask(CreateTask(MouseEventTask, std::bind(&InputSystem::MouseEventSender, m_CurrentInputSystem, m_CurrentInputSystem->m_MouseState, time)));
			m_CurrentInputSystem->m_KeyboardMutex.unlock();
		}
		else if (button == XBUTTON2)
		{
			m_CurrentInputSystem->m_MouseMutex.lock();
			if (m_CurrentInputSystem->m_MouseState.ButtonInfo.State[EMouseButtons::MB_XBUTTON2] == InputState::UP)
			{
				m_CurrentInputSystem->m_MouseMutex.unlock();
				return CallNextHookEx(nullptr, nCode, wParam, lParam);
			}
			m_CurrentInputSystem->m_MouseState.ButtonInfo.State[EMouseButtons::MB_XBUTTON2] = InputState::UP;
			m_CurrentInputSystem->m_MouseState.MouseEvent = EMouseEvent::BUTTON_STATE_CHANGE;
			m_CurrentInputSystem->m_MouseState.EventButton = EMouseButtons::MB_XBUTTON2;
			m_CurrentInputSystem->SendTask(CreateTask(MouseEventTask, std::bind(&InputSystem::MouseEventSender, m_CurrentInputSystem, m_CurrentInputSystem->m_MouseState, time)));
			m_CurrentInputSystem->m_KeyboardMutex.unlock();
		}
		else
		{
			printf("\nXBUTTON%d released!\n", button);
		}
	}
	return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

void InputSystem::KeyboardEventSender(const KeyboardState keyState, const std::chrono::high_resolution_clock::time_point eventTime)
{
	m_KeyboardListenersMutex.lock_shared();
	for (auto it = m_KeyboardListeners.begin(); it != m_KeyboardListeners.end(); ++it)
		it->second(keyState, eventTime);
	m_KeyboardListenersMutex.unlock_shared();
}

void InputSystem::MouseEventSender(const MouseState mouseState, const std::chrono::high_resolution_clock::time_point eventTime)
{
	m_MouseListenersMutex.lock_shared();
	for (auto it = m_MouseListeners.begin(); it != m_MouseListeners.end(); ++it)
		it->second(mouseState, eventTime);
	m_MouseListenersMutex.unlock_shared();
}

LRESULT gaf::KeyboardProc(const int32 nCode, const WPARAM wParam, const LPARAM lParam)
{
	if (nCode < 0)
		return CallNextHookEx(nullptr, nCode, wParam, lParam);

	if (!m_CurrentInputSystem)
	{
		const auto inputMgr = InstanceInput();
		inputMgr->m_InputSystemsMutex.lock_shared();
		for (auto it = inputMgr->m_InputSystems.begin();
			it != inputMgr->m_InputSystems.end(); ++it)
		{
			if (it->second.GetAttachedWindow()->GetWindowHandle() == GetActiveWindow())
				m_CurrentInputSystem = &it->second;
		}
		inputMgr->m_InputSystemsMutex.unlock_shared();
		if (!m_CurrentInputSystem)
			return CallNextHookEx(nullptr, nCode, wParam, lParam);
	}

	if (wParam < ARRAY_SIZE(gKeyConversion))
	{
		const auto time = std::chrono::high_resolution_clock::now();
		const auto key = gKeyConversion[wParam];
		if (key == EKey::NUM_KEYS)
			return CallNextHookEx(nullptr, nCode, wParam, lParam);
		auto& pState = m_CurrentInputSystem->m_KeyboardState.KeyInfo.State[key];
		InputState nState = ((lParam & (1ULL << 31)) == 0 ? InputState::DOWN : InputState::UP);
		if (nState != pState)
		{
			m_CurrentInputSystem->m_KeyboardMutex.lock();
			m_CurrentInputSystem->m_KeyboardState.KeyInfo.State[key] = nState;
			m_CurrentInputSystem->m_KeyboardState.LastKeyModified = key;
			if (key == EKey::LALT || key == EKey::RALT || key == EKey::ALT)
			{
				if (nState == InputState::DOWN)
					m_CurrentInputSystem->m_KeyboardState.ModifierKeys |= 1UL << KeyModifiers::ALT;
				else
					m_CurrentInputSystem->m_KeyboardState.ModifierKeys &= ~(1UL << KeyModifiers::ALT);
			}
			else if (key == EKey::LCTRL || key == EKey::RCTRL || key == EKey::CTRL)
			{
				if (nState == InputState::DOWN)
					m_CurrentInputSystem->m_KeyboardState.ModifierKeys |= 1UL << KeyModifiers::CTRL;
				else
					m_CurrentInputSystem->m_KeyboardState.ModifierKeys &= ~(1UL << KeyModifiers::CTRL);
			}
			else if (key == EKey::LSHIFT || key == EKey::RSHIFT || key == EKey::SHIFT)
			{
				if (nState == InputState::DOWN)
					m_CurrentInputSystem->m_KeyboardState.ModifierKeys |= 1UL << KeyModifiers::SHIFT;
				else
					m_CurrentInputSystem->m_KeyboardState.ModifierKeys &= ~(1UL << KeyModifiers::SHIFT);
			}
			m_CurrentInputSystem->SendTask(CreateTask(KeyboardEventTask, std::bind(&InputSystem::KeyboardEventSender, m_CurrentInputSystem, m_CurrentInputSystem->m_KeyboardState, time)));
			m_CurrentInputSystem->m_KeyboardMutex.unlock();
		}
	}

	return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

LRESULT gaf::MouseProc(const int32 nCode, const WPARAM wParam, const LPARAM lParam)
{
	if (nCode < 0)
		return CallNextHookEx(nullptr, nCode, wParam, lParam);

	MOUSEHOOKSTRUCT* mouseStruct = (MOUSEHOOKSTRUCT*)lParam;

	if (!m_CurrentInputSystem)
	{
		const auto inputMgr = InstanceInput();
		inputMgr->m_InputSystemsMutex.lock_shared();
		for (auto it = inputMgr->m_InputSystems.begin();
			it != inputMgr->m_InputSystems.end(); ++it)
		{
			if (it->second.GetAttachedWindow()->GetWindowHandle() == mouseStruct->hwnd)
				m_CurrentInputSystem = &it->second;
		}
		inputMgr->m_InputSystemsMutex.unlock_shared();
		if (!m_CurrentInputSystem)
			return CallNextHookEx(nullptr, nCode, wParam, lParam);
	}

	const auto time = std::chrono::high_resolution_clock::now();

	if (wParam == WM_MOUSEMOVE)
	{
		m_CurrentInputSystem->m_MouseMutex.lock();
		const auto deltaX = mouseStruct->pt.x - m_CurrentInputSystem->m_MouseState.Position.X;
		const auto deltaY = mouseStruct->pt.y - m_CurrentInputSystem->m_MouseState.Position.Y;
		if (deltaX == 0 && deltaY == 0)
		{ // No change
			m_CurrentInputSystem->m_MouseMutex.unlock();
			return CallNextHookEx(nullptr, nCode, wParam, lParam);
		}
		m_CurrentInputSystem->m_MouseState.PositionDelta.X = deltaX;
		m_CurrentInputSystem->m_MouseState.PositionDelta.Y = deltaY;
		m_CurrentInputSystem->m_MouseState.Position.X = mouseStruct->pt.x;
		m_CurrentInputSystem->m_MouseState.Position.Y = mouseStruct->pt.y;
		m_CurrentInputSystem->m_MouseState.DerivedPosition.X = mouseStruct->pt.x - m_CurrentInputSystem->m_AttachedWindow->GetPosition().first;
		m_CurrentInputSystem->m_MouseState.DerivedPosition.Y = mouseStruct->pt.y - m_CurrentInputSystem->m_AttachedWindow->GetPosition().second;
		m_CurrentInputSystem->m_MouseState.MouseEvent = EMouseEvent::POSITION_CHAGE;
		m_CurrentInputSystem->SendTask(CreateTask(MouseEventTask, std::bind(&InputSystem::MouseEventSender, m_CurrentInputSystem, m_CurrentInputSystem->m_MouseState, time)));
		m_CurrentInputSystem->m_MouseMutex.unlock();
	}
	else if (wParam == WM_LBUTTONDOWN)
	{
		m_CurrentInputSystem->m_MouseMutex.lock();
		if (m_CurrentInputSystem->m_MouseState.ButtonInfo.State[EMouseButtons::LEFT_BUTTON] == InputState::DOWN)
		{ // No change
			m_CurrentInputSystem->m_MouseMutex.unlock();
			return CallNextHookEx(nullptr, nCode, wParam, lParam);
		}
		m_CurrentInputSystem->m_MouseState.ButtonInfo.State[EMouseButtons::LEFT_BUTTON] = InputState::DOWN;
		m_CurrentInputSystem->m_MouseState.MouseEvent = EMouseEvent::BUTTON_STATE_CHANGE;
		m_CurrentInputSystem->m_MouseState.EventButton = EMouseButtons::LEFT_BUTTON;
		m_CurrentInputSystem->SendTask(CreateTask(MouseEventTask, std::bind(&InputSystem::MouseEventSender, m_CurrentInputSystem, m_CurrentInputSystem->m_MouseState, time)));
		m_CurrentInputSystem->m_MouseMutex.unlock();
	}
	else if (wParam == WM_LBUTTONUP)
	{
		m_CurrentInputSystem->m_MouseMutex.lock();
		if (m_CurrentInputSystem->m_MouseState.ButtonInfo.State[EMouseButtons::LEFT_BUTTON] == InputState::UP)
		{ // No change
			m_CurrentInputSystem->m_MouseMutex.unlock();
			return CallNextHookEx(nullptr, nCode, wParam, lParam);
		}
		m_CurrentInputSystem->m_MouseState.ButtonInfo.State[EMouseButtons::LEFT_BUTTON] = InputState::UP;
		m_CurrentInputSystem->m_MouseState.MouseEvent = EMouseEvent::BUTTON_STATE_CHANGE;
		m_CurrentInputSystem->m_MouseState.EventButton = EMouseButtons::LEFT_BUTTON;
		m_CurrentInputSystem->SendTask(CreateTask(MouseEventTask, std::bind(&InputSystem::MouseEventSender, m_CurrentInputSystem, m_CurrentInputSystem->m_MouseState, time)));
		m_CurrentInputSystem->m_MouseMutex.unlock();
	}
	else if (wParam == WM_RBUTTONDOWN)
	{
		m_CurrentInputSystem->m_MouseMutex.lock();
		if (m_CurrentInputSystem->m_MouseState.ButtonInfo.State[EMouseButtons::RIGHT_BUTTON] == InputState::DOWN)
		{ // No change
			m_CurrentInputSystem->m_MouseMutex.unlock();
			return CallNextHookEx(nullptr, nCode, wParam, lParam);
		}
		m_CurrentInputSystem->m_MouseState.ButtonInfo.State[EMouseButtons::RIGHT_BUTTON] = InputState::DOWN;
		m_CurrentInputSystem->m_MouseState.MouseEvent = EMouseEvent::BUTTON_STATE_CHANGE;
		m_CurrentInputSystem->m_MouseState.EventButton = EMouseButtons::RIGHT_BUTTON;
		m_CurrentInputSystem->SendTask(CreateTask(MouseEventTask, std::bind(&InputSystem::MouseEventSender, m_CurrentInputSystem, m_CurrentInputSystem->m_MouseState, time)));
		m_CurrentInputSystem->m_MouseMutex.unlock();
	}
	else if (wParam == WM_RBUTTONUP)
	{
		m_CurrentInputSystem->m_MouseMutex.lock();
		if (m_CurrentInputSystem->m_MouseState.ButtonInfo.State[EMouseButtons::RIGHT_BUTTON] == InputState::UP)
		{ // No change
			m_CurrentInputSystem->m_MouseMutex.unlock();
			return CallNextHookEx(nullptr, nCode, wParam, lParam);
		}
		m_CurrentInputSystem->m_MouseState.ButtonInfo.State[EMouseButtons::RIGHT_BUTTON] = InputState::UP;
		m_CurrentInputSystem->m_MouseState.MouseEvent = EMouseEvent::BUTTON_STATE_CHANGE;
		m_CurrentInputSystem->m_MouseState.EventButton = EMouseButtons::RIGHT_BUTTON;
		m_CurrentInputSystem->SendTask(CreateTask(MouseEventTask, std::bind(&InputSystem::MouseEventSender, m_CurrentInputSystem, m_CurrentInputSystem->m_MouseState, time)));
		m_CurrentInputSystem->m_MouseMutex.unlock();
	}
	else if (wParam == WM_MBUTTONDOWN)
	{
		m_CurrentInputSystem->m_MouseMutex.lock();
		if (m_CurrentInputSystem->m_MouseState.ButtonInfo.State[EMouseButtons::MIDDLE_BUTTON] == InputState::DOWN)
		{ // No change
			m_CurrentInputSystem->m_MouseMutex.unlock();
			return CallNextHookEx(nullptr, nCode, wParam, lParam);
		}
		m_CurrentInputSystem->m_MouseState.ButtonInfo.State[EMouseButtons::MIDDLE_BUTTON] = InputState::DOWN;
		m_CurrentInputSystem->m_MouseState.MouseEvent = EMouseEvent::BUTTON_STATE_CHANGE;
		m_CurrentInputSystem->m_MouseState.EventButton = EMouseButtons::MIDDLE_BUTTON;
		m_CurrentInputSystem->SendTask(CreateTask(MouseEventTask, std::bind(&InputSystem::MouseEventSender, m_CurrentInputSystem, m_CurrentInputSystem->m_MouseState, time)));
		m_CurrentInputSystem->m_MouseMutex.unlock();
	}
	else if (wParam == WM_MBUTTONUP)
	{
		m_CurrentInputSystem->m_MouseMutex.lock();
		if (m_CurrentInputSystem->m_MouseState.ButtonInfo.State[EMouseButtons::MIDDLE_BUTTON] == InputState::UP)
		{ // No change
			m_CurrentInputSystem->m_MouseMutex.unlock();
			return CallNextHookEx(nullptr, nCode, wParam, lParam);
		}
		m_CurrentInputSystem->m_MouseState.ButtonInfo.State[EMouseButtons::MIDDLE_BUTTON] = InputState::UP;
		m_CurrentInputSystem->m_MouseState.MouseEvent = EMouseEvent::BUTTON_STATE_CHANGE;
		m_CurrentInputSystem->m_MouseState.EventButton = EMouseButtons::MIDDLE_BUTTON;
		m_CurrentInputSystem->SendTask(CreateTask(MouseEventTask, std::bind(&InputSystem::MouseEventSender, m_CurrentInputSystem, m_CurrentInputSystem->m_MouseState, time)));
		m_CurrentInputSystem->m_MouseMutex.unlock();
	}

	return CallNextHookEx(nullptr, nCode, wParam, lParam);
}
#endif

static uint32 GetNextID()
{
	static std::atomic<uint32> gInputSystemID = 0;
	const auto id = gInputSystemID.load();
	++gInputSystemID;
	return id;
}

InputSystem::InputSystem()
	:TaskDispatcher("InputSystem" + std::to_string(GetNextID()), InstanceApp())
	, m_AttachedWindow(nullptr)
#if PLATFORM_WINDOWS
	, m_WindowEvents(nullptr)
	, m_MouseEvents(nullptr)
	, m_KeyboardEvents(nullptr)
#endif
{

}

InputSystem::InputSystem(const InputSystem & other)
	:TaskDispatcher(other)
	, m_MouseState(other.m_MouseState)
	, m_KeyboardState(other.m_KeyboardState)
#if PLATFORM_WINDOWS
	, m_WindowEvents(nullptr)
	, m_MouseEvents(nullptr)
	, m_KeyboardEvents(nullptr)
#endif
{
	if (other.IsAttached())
	{
		AttachWindow(other.m_AttachedWindow);
	}
}

InputSystem::InputSystem(InputSystem&& other)noexcept
	:TaskDispatcher(other)
	, m_MouseState(std::exchange(other.m_MouseState, MouseState()))
	, m_KeyboardState(std::exchange(other.m_KeyboardState, KeyboardState()))
	, m_AttachedWindow(std::exchange(other.m_AttachedWindow, nullptr))
#if PLATFORM_WINDOWS
	, m_WindowEvents(std::exchange(other.m_WindowEvents, nullptr))
	, m_MouseEvents(std::exchange(other.m_MouseEvents, nullptr))
	, m_KeyboardEvents(std::exchange(other.m_KeyboardEvents, nullptr))
#endif
{

}

InputSystem & InputSystem::operator=(const InputSystem & other)
{
	if (this != &other)
	{
		if (IsAttached())
			Detach();
		m_MouseState = other.m_MouseState;
		m_KeyboardState = other.m_KeyboardState;
		if (other.IsAttached())
			AttachWindow(other.m_AttachedWindow);
	}
	return *this;
}

InputSystem & InputSystem::operator=(InputSystem && other) noexcept
{
	if (this != &other)
	{
		if (IsAttached())
			Detach();
		m_MouseState = std::exchange(other.m_MouseState, MouseState());
		m_KeyboardState = std::exchange(other.m_KeyboardState, KeyboardState());
		m_AttachedWindow = std::exchange(other.m_AttachedWindow, nullptr);
#if PLATFORM_WINDOWS
		m_WindowEvents = std::exchange(other.m_WindowEvents, nullptr);
		m_MouseEvents = std::exchange(other.m_MouseEvents, nullptr);
		m_KeyboardEvents = std::exchange(other.m_KeyboardEvents, nullptr);
#endif
	}
	return *this;
}

InputSystem::~InputSystem()
{
	if (IsAttached())
		Detach();
}

InputState InputSystem::GetMouseButtonState(const EMouseButtons::Type button)
{
	std::shared_lock<std::shared_mutex> lock(m_MouseMutex);
	return m_MouseState.ButtonInfo.State[button];
}

MouseState::MousePositionInfo InputSystem::GetMousePosition()
{
	std::shared_lock<std::shared_mutex> lock(m_MouseMutex);
	return m_MouseState.Position;
}

MouseState::MousePositionInfo InputSystem::GetMouseDerivedPosition()
{
	std::shared_lock<std::shared_mutex> lock(m_MouseMutex);
	return m_MouseState.DerivedPosition;
}

int16 InputSystem::GetMouseWheelDelta(const EMouseWheels::Type wheel)
{
	std::shared_lock<std::shared_mutex> lock(m_MouseMutex);
	return m_MouseState.WheelInfo.Delta[wheel];
}

InputState InputSystem::GetKeyState(const EKey::Type key)
{
	std::shared_lock<std::shared_mutex> lock(m_KeyboardMutex);
	return m_KeyboardState.KeyInfo.State[key];
}

MouseState InputSystem::GetMouseState()
{
	std::shared_lock<std::shared_mutex> lock(m_MouseMutex);
	return m_MouseState;
}

KeyboardState InputSystem::GetKeyboardState()
{
	std::shared_lock<std::shared_mutex> lock(m_KeyboardMutex);
	return m_KeyboardState;
}

InputSystem::KeyboardListenerID InputSystem::RegisterKeyboardListener(const KeyboardEventListener & listener)
{
	const auto id = m_KeyboardListenerIDGen.load();
	++m_KeyboardListenerIDGen;
	m_KeyboardListenersMutex.lock();
	m_KeyboardListeners.insert_or_assign(id, listener);
	m_KeyboardListenersMutex.unlock();
	return id;
}

bool InputSystem::UnregisterKeyboardListener(const KeyboardListenerID listener)
{
	m_KeyboardListenersMutex.lock_shared();
	const auto it = m_KeyboardListeners.find(listener);
	if (it != m_KeyboardListeners.end())
	{
		m_KeyboardListenersMutex.unlock_shared();
		m_KeyboardListenersMutex.lock();
		m_KeyboardListeners.erase(it);
		m_KeyboardListenersMutex.unlock();
		return true;
	}
	else
	{
		m_KeyboardListenersMutex.unlock_shared();
		return false;
	}
}

bool InputSystem::IsKeyboardListenerRegister(const KeyboardListenerID listener)
{
	m_KeyboardListenersMutex.lock_shared();
	const auto it = m_KeyboardListeners.find(listener);
	if (it != m_KeyboardListeners.end())
	{
		m_KeyboardListenersMutex.unlock_shared();
		return true;
	}
	m_KeyboardListenersMutex.unlock_shared();
	return false;
}

InputSystem::MouseListenerID InputSystem::RegisterMouseListener(const MouseEventListener & listener)
{
	const auto id = m_MouseListenerIDGen.load();
	++m_MouseListenerIDGen;
	m_MouseListenersMutex.lock();
	m_MouseListeners.insert_or_assign(id, listener);
	m_MouseListenersMutex.unlock();
	return id;
}

bool InputSystem::UnregisterMouseListener(const MouseListenerID listener)
{
	m_MouseListenersMutex.lock_shared();
	const auto it = m_MouseListeners.find(listener);
	if (it != m_MouseListeners.end())
	{
		m_MouseListenersMutex.unlock_shared();
		m_MouseListenersMutex.lock();
		m_MouseListeners.erase(it);
		m_MouseListenersMutex.unlock();
		return true;
	}
	else
	{
		m_MouseListenersMutex.unlock_shared();
		return false;
	}
}

bool InputSystem::IsMouseListenerRegister(const MouseListenerID listener)
{
	m_MouseListenersMutex.lock_shared();
	const auto it = m_MouseListeners.find(listener);
	if (it != m_MouseListeners.end())
	{
		m_MouseListenersMutex.unlock_shared();
		return true;
	}
	m_MouseListenersMutex.unlock_shared();
	return false;
}

bool InputSystem::IsAttached() const
{
	return m_AttachedWindow
#if PLATFORM_WINDOWS
		&& (m_WindowEvents
			|| m_MouseEvents
			|| m_KeyboardEvents)
#endif
		;
}

CreateTaskName(WindowInputAttachment);

void InputSystem::AttachWindow(Window * wnd)
{
	if (!wnd)
	{
		LogManager::LogMessage(LL_ERRO, "Trying to attach a window to an InputSystem, but the window was nullptr.");
		return;
	}
	if (IsAttached())
		Detach();
	if (std::this_thread::get_id() != wnd->GetThreadID())
	{
		wnd->SendWindowTask(CreateTask(WindowInputAttachment, std::bind(&InputSystem::AttachWindow, this, wnd)));
		return;
	}

	DWORD err = 0;
	const auto id = GetCurrentThreadId();
	using namespace std::placeholders;

	InstanceApp()->RegisterTaskDispatcher(this, 1);

	m_WindowEvents = SetWindowsHookExA(WH_CALLWNDPROC, &gaf::WindowProc, nullptr, id);
	if (!m_WindowEvents)
	{
		LogManager::LogMessage(LL_ERRO, "Trying to attach an InputSystem to a Window, but something unexpected happened: error: 0x%08X.", GetLastError());
	}
	m_MouseEvents = SetWindowsHookExA(WH_MOUSE, &gaf::MouseProc, nullptr, id);
	if (!m_MouseEvents)
	{
		LogManager::LogMessage(LL_ERRO, "Trying to attach an InputSystem to a Window, but something unexpected happened: error: 0x%08X.", GetLastError());
	}
	m_KeyboardEvents = SetWindowsHookExA(WH_KEYBOARD, &gaf::KeyboardProc, nullptr, id);
	if (!m_KeyboardEvents)
	{
		LogManager::LogMessage(LL_ERRO, "Trying to attach an InputSystem to a Window, but something unexpected happened: error: 0x%08X.", GetLastError());
	}
	m_AttachedWindow = wnd;
	InstanceEvent()->DispatchEvent(EventIDOnInputSystemReady, this);
}

void InputSystem::Detach()
{
	if (!IsAttached())
	{
		LogManager::LogMessage(LL_ERRO, "Trying to detach a window from an InputSystem, but the InputSystem was not attached.");
		return;
	}

	if (m_WindowEvents)
	{
		UnhookWindowsHookEx(m_WindowEvents);
		m_WindowEvents = nullptr;
	}
	if (m_MouseEvents)
	{
		UnhookWindowsHookEx(m_MouseEvents);
		m_MouseEvents = nullptr;
	}
	if (m_KeyboardEvents)
	{
		UnhookWindowsHookEx(m_KeyboardEvents);
		m_KeyboardEvents = nullptr;
	}
	InstanceApp()->UnregisterTaskDispatcher(this);
}

Window* InputSystem::GetAttachedWindow() const
{
	return m_AttachedWindow;
}

void InputManager::SendKeyEventWindow(Window * window, const EKey::Type key, const InputState state)
{
	if (!window)
	{
		LogManager::LogMessage(LL_ERRO, "Trying to send a key event to a window, but it was a nullptr one.");
		return;
	}
	if (key == EKey::NUM_KEYS)
	{
		LogManager::LogMessage(LL_WARN, "Trying to send a key event to a window, but the given key was null.");
		return;
	}

#if PLATFORM_WINDOWS
	BYTE keycode;
	for (BYTE i = 0; i < ARRAY_SIZE(gKeyConversion); ++i)
	{
		if (gKeyConversion[i] == key)
		{
			keycode = i;
			break;
		}
	}
	if (!SetForegroundWindow(window->GetWindowHandle()))
	{
		LogManager::LogMessage(LL_ERRO, "Trying to send a key event to a window, but something unexpected happened: error: 0x%08X.", GetLastError());
	}
	keybd_event(keycode, 0, state == InputState::UP ? KEYEVENTF_KEYUP : 0, 0);
#else
	/* TODO: send key event */
#endif
}

void InputManager::SendMouseMoveEventWindow(Window * window, const int32 x, const int32 y)
{
	if (!window)
	{
		LogManager::LogMessage(LL_ERRO, "Trying to send a MouseMove event to a window, but it was a nullptr one.");
		return;
	}
	const auto wndPos = window->GetPosition();
	const auto wndRes = window->GetResolution();
	const auto X = Clamp(x, wndPos.first, wndPos.first + (int32)wndRes.first);
	const auto Y = Clamp(y, wndPos.second, wndPos.second + (int32)wndRes.second);
#if PLATFORM_WINDOWS
	if (!SetForegroundWindow(window->GetWindowHandle()))
	{
		LogManager::LogMessage(LL_ERRO, "Trying to send a MouseMove event to a window, but something unexpected happened: error: 0x%08X.", GetLastError());
	}
	mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, X, Y, 0, 0);
#else
	/* TODO mouse move event */
#endif
}

void InputManager::SendMouseButtonEventWindow(Window * window, const EMouseButtons::Type button, const InputState state)
{
	if (!window)
	{
		LogManager::LogMessage(LL_ERRO, "Trying to send a MouseButton event to a window, but it was a nullptr one.");
		return;
	}
	if (button != EMouseButtons::LEFT_BUTTON && button != EMouseButtons::RIGHT_BUTTON && button != EMouseButtons::MIDDLE_BUTTON)
	{
		LogManager::LogMessage(LL_ERRO, "Trying to send a MouseButton event to a window, but the given button, %d, was not supported.", (int32)button);
		return;
	}
#if PLATFORM_WINDOWS
	if (!SetForegroundWindow(window->GetWindowHandle()))
	{
		LogManager::LogMessage(LL_ERRO, "Trying to send a MouseButton event to a window, but something unexpected happened: error: 0x%08X.", GetLastError());
	}
	DWORD evt;
	if (button == EMouseButtons::LEFT_BUTTON)
	{
		evt = state == InputState::DOWN ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
	}
	else if(button == EMouseButtons::RIGHT_BUTTON)
	{
		evt = state == InputState::DOWN ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP;
	}
	else if (button == EMouseButtons::MIDDLE_BUTTON)
	{
		evt = state == InputState::DOWN ? MOUSEEVENTF_MIDDLEDOWN : MOUSEEVENTF_MIDDLEUP;
	}
	mouse_event(evt, 0, 0, 0, 0);
#else
	/* TODO mouse move event */
#endif
}

void InputManager::SendMouseWheelEventWindow(Window * window, const EMouseWheels::Type wheel, int16 delta)
{
	if (!window)
	{
		LogManager::LogMessage(LL_ERRO, "Trying to send a MouseWheel event to a window, but it was a nullptr one.");
		return;
	}
	if (wheel != EMouseWheels::HORIZONTAL && wheel != EMouseWheels::VERTICAL)
	{
		LogManager::LogMessage(LL_ERRO, "Trying to send a MouseWheel event to a window, but the given wheel was invalid.");
		return;
	}
#if PLATFORM_WINDOWS
	if (!SetForegroundWindow(window->GetWindowHandle()))
	{
		LogManager::LogMessage(LL_ERRO, "Trying to send a MouseWheel event to a window, but something unexpected happened: error: 0x%08X.", GetLastError());
	}
	mouse_event(wheel == EMouseWheels::HORIZONTAL ? MOUSEEVENTF_HWHEEL : MOUSEEVENTF_WHEEL, 0, 0, delta > 0 ? WHEEL_DELTA : -WHEEL_DELTA, 0);
#else
	/* TODO mouse move event */
#endif
}

void InputManager::SetMousePosition(const int32 x, const int32 y)const
{
#if PLATFORM_WINDOWS
	if (!SetCursorPos(x, y))
	{
		LogManager::LogMessage(LL_ERRO, "Trying to set the mouse position to: %d, %d, but something unexpected happened: error: 0x%08X.", x, y, GetLastError());
	}
#else
	/* TODO mouse position set */
#endif
}

MouseState::MousePositionInfo InputManager::GetMousePosition() const
{
#if PLATFORM_WINDOWS
	POINT point;
	if (!GetCursorPos(&point))
	{
		LogManager::LogMessage(LL_ERRO, "Trying to get the mouse position, but something unexpected happened: error: 0x%08X.", GetLastError());
		return { 0,0 };
	}
	return *((MouseState::MousePositionInfo*)&point);
#else 
	/* TODO mouse position get */
#endif
}

InputState InputManager::IsButtonDown(const EMouseButtons::Type button) const
{
#if PLATFORM_WINDOWS

#else
	/* TODO mouse button get */
#endif
}

InputState InputManager::IsKeyDown(const EKey::Type key) const
{
#if PLATFORM_WINDOWS

#else
	/* TODO key state get */
#endif
}
