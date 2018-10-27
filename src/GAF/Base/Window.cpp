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

#include "GAF/WindowManager.h"
#include "GAF/LogManager.h"

using namespace gaf;

const ANSICHAR* gaf::GetWindowModeStr(const EWindowMode::Type mode)
{
	static const ANSICHAR* ModeStr[] =
	{
		"WINDOWED",
		"FULLSCREEN",
		"BORDERLESS"
	};
	return ModeStr[static_cast<SIZET>(mode)];
}

void Window::Run()
{
	bool stop = false;
	while (!stop)
	{
		if (m_HasToClose.load())
		{
			stop = true;
		}
		else
		{
			Task_t task;
			bool hasValue = false;
			if (!m_HasToClose)
				hasValue = m_CommandQueue.PopFront(task);
			if (!hasValue)
				std::this_thread::sleep_for(ThreadSleepTimeMS);
			else
				task();
		}
	}
}

#if PLATFORM_WINDOWS
GREAPER_THLOCAL Window* m_CurrentWindow = nullptr;
namespace gaf
{
	LRESULT STDCALL WndProc(HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
	{
		const auto wndMgr = InstanceWnd();
		if (!m_CurrentWindow && wndMgr)
		{
			for (auto it = wndMgr->Begin(); it != wndMgr->End(); ++it)
			{
				if (it->second->GetWindowHandle() == hWnd)
				{
					m_CurrentWindow = it->second;
				}
			}
			if (!m_CurrentWindow)
				return DefWindowProcA(hWnd, uMsg, wParam, lParam);
		}
		const auto loWParam = LOWORD(wParam), hiWParam = HIWORD(wParam);
		const auto loLParam = LOWORD(lParam), hiLParam = HIWORD(lParam);

		const auto eventMgr = InstanceEvent();

		if (uMsg == WM_ACTIVATE)
		{
			if (loWParam == WA_ACTIVE || loWParam == WA_CLICKACTIVE)
			{
				m_CurrentWindow->m_Active = true;
				eventMgr->DispatchEvent(WindowManager::EventIDOnWindowActivation, m_CurrentWindow);
			}
			else
			{
				m_CurrentWindow->m_Active = false;
				eventMgr->DispatchEvent(WindowManager::EventIDOnWindowDeactivation, m_CurrentWindow);
			}
			return 0;
		}
		if (uMsg == WM_SYSCOMMAND)
		{
			if (wParam == SC_SCREENSAVE || wParam == SC_MONITORPOWER)
				return 0;
		}
		if (uMsg == WM_CLOSE)
		{
			eventMgr->DispatchEvent(WindowManager::EventIDOnWindowClosing, m_CurrentWindow);
			return 0;
		}
		if (uMsg == WM_DESTROY)
		{
			m_CurrentWindow->m_HasToClose = true;
			return 0;
		}
		if (uMsg == WM_KILLFOCUS)
		{
			m_CurrentWindow->m_Focused = false;
			eventMgr->DispatchEvent(WindowManager::EventIDOnWindowFocusLost, m_CurrentWindow);
			return 0;
		}
		if (uMsg == WM_SETFOCUS)
		{
			m_CurrentWindow->m_Focused = true;
			eventMgr->DispatchEvent(WindowManager::EventIDOnWindowFocusGain, m_CurrentWindow);
			return 0;
		}
		if (uMsg == WM_MOVE)
		{
			m_CurrentWindow->m_LastWindowPosition = m_CurrentWindow->m_WindowPosition;
			m_CurrentWindow->m_WindowPosition = { loLParam, hiLParam };
			eventMgr->DispatchEvent(WindowManager::EventIDOnWindowMove, m_CurrentWindow);
			return 0;
		}
		if (uMsg == WM_SIZE)
		{
			m_CurrentWindow->m_LastWindowResolution = m_CurrentWindow->m_WindowResolution;
			m_CurrentWindow->m_WindowResolution = { loLParam, hiLParam };
			eventMgr->DispatchEvent(WindowManager::EventIDOnWindowResized, m_CurrentWindow);
			if (wParam == SIZE_MAXIMIZED)
			{
				eventMgr->DispatchEvent(WindowManager::EventIDOnWindowMaximize, m_CurrentWindow);
			}
			else if (wParam == SIZE_MINIMIZED)
			{
				eventMgr->DispatchEvent(WindowManager::EventIDOnWindowMinimize, m_CurrentWindow);
			}
			else if (wParam == SIZE_RESTORED)
			{
				eventMgr->DispatchEvent(WindowManager::EventIDOnWindowRestore, m_CurrentWindow);
			}
			else
			{
				return DefWindowProcA(hWnd, uMsg, wParam, lParam);
			}
			return 0;
		}
		return DefWindowProcA(hWnd, uMsg, wParam, lParam);
	}
}

#endif

void gaf::Window::_CreateWindow()
{
	WNDCLASSEXA wc;
	ZeroMemory(&wc, sizeof(wc));
	RECT windowRect;
	DWORD dwExStyle;
	DWORD dwStyle;

	windowRect.left = 0;
	windowRect.top = 0;
	windowRect.right = m_WindowResolution.first;
	windowRect.bottom = m_WindowResolution.second;

	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = static_cast<WNDPROC>(WndProc);
	wc.hInstance = GetModuleHandleA(nullptr);
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = m_WindowID.c_str();
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	Assertion::WhenTrue(!RegisterClassExA(&wc), "Couldn't register the WindowClass, reason: 0x%08X.", GetLastError());

	if (m_WindowMode == EWindowMode::FULLSCREEN)
	{
		DEVMODEA dmScreenSettings;
		ZeroMemory(&dmScreenSettings, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = m_WindowResolution.first;
		dmScreenSettings.dmPelsHeight = m_WindowResolution.second;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		if (ChangeDisplaySettingsA(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			LogManager::LogMessage(LL_WARN, "Couldn't change to FullScreen Mode, "
				"changing to windowed, reason 0x%08X.", GetLastError());
			m_WindowMode = EWindowMode::WINDOWED;
		}
	}

	if (m_WindowMode != EWindowMode::WINDOWED)
	{
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP;
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
	}

	//const auto rt = AdjustWindowRectEx(&windowRect, dwStyle, false, dwExStyle);

	m_Handle = CreateWindowExA(dwExStyle,
		m_WindowID.c_str(), m_WindowTitle.c_str(),
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dwStyle,
		m_WindowPosition.first + windowRect.left, m_WindowPosition.second + windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr, nullptr, wc.hInstance, nullptr);
	Assertion::WhenNullptr(m_Handle, "Coudln't create a Window, reason: 0x%08X.", GetLastError());

	ShowWindow(m_Handle, SW_SHOWNORMAL);
	if (!SetForegroundWindow(m_Handle))
	{
		LogManager::LogMessage(LL_WARN, "Couldn't make the top most window, reason: 0x%08X.", GetLastError());
	}
	if (!SetFocus(m_Handle))
	{
		LogManager::LogMessage(LL_WARN, "Couldn't set the focus to the window, reason: 0x%08X.", GetLastError());
	}
	UpdateWindow(m_Handle);
	m_Active = true;
	m_Focused = true;
}

void Window::_ChangeTitle(const std::string& title)
{
#if PLATFORM_WINDOWS
	if (!SetWindowTextA(m_Handle, title.c_str()))
	{
		LogManager::LogMessage(LL_ERRO,
			"Couldn't change the window title, reason: 0x%08X.", GetLastError());
		return;
	}
#elif PLATFORM_LINUX

#else

#endif
	m_WindowTitle.assign(title);
}

void Window::_ChangeResolution(const Resolution_t res)
{
#if PLATFORM_WINDOWS
	if (!SetWindowPos(m_Handle, HWND_TOPMOST, m_WindowPosition.first, m_WindowPosition.second,
		res.first, res.second, SWP_SHOWWINDOW))
		/*if (!MoveWindow(m_Handle, m_WindowPosition.first, m_WindowPosition.second,
			res.first, res.second, true))*/
	{
		LogManager::LogMessage(LL_ERRO,
			"Couldn't change the window resolution, reason: 0x%08X.", GetLastError());
		return;
	}
#elif PLATFORM_LINUX

#else

#endif
	m_WindowResolution = res;
}

void Window::_ChangePosition(const Position_t pos)
{
#if PLATFORM_WINDOWS
	if (!MoveWindow(m_Handle, pos.first, pos.second,
		(int32)m_WindowResolution.first, (int32)m_WindowResolution.second, false))
	{
		LogManager::LogMessage(LL_ERRO,
			"Couldn't change the window position, reason: 0x%08X.", GetLastError());
		return;
	}
#elif PLATFORM_LINUX

#else

#endif
	m_WindowPosition = pos;
}

void Window::_Close()
{
#if PLATFORM_WINDOWS
	if (m_WindowMode == EWindowMode::FULLSCREEN)
	{
		ChangeDisplaySettings(nullptr, 0);
		ShowCursor(true);
	}
	if (m_Handle)
	{
		if (!DestroyWindow(m_Handle))
		{
			LogManager::LogMessage(LL_WARN, "Couldn't destroy window, reason: 0x%08X.", GetLastError());
		}
	}
	if (!UnregisterClassA(m_WindowID.c_str(), GetModuleHandle(nullptr)))
	{
		LogManager::LogMessage(LL_WARN, "Couldn't unregister the window class, reason: 0x%08X.", GetLastError());
	}
#elif PLATFORM_LINUX

#else

#endif
}

void Window::_SetWindowMode(const EWindowMode::Type mode)
{
#if PLATFORM_WINDOWS
	if (m_WindowMode == EWindowMode::FULLSCREEN)
	{
		ChangeDisplaySettings(nullptr, 0);
		ShowCursor(true);
	}
	DWORD dwExStyle, dwStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	switch (mode)
	{
	case EWindowMode::FULLSCREEN:
	{
		DEVMODEA dmScreenSettings;
		ZeroMemory(&dmScreenSettings, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = m_WindowResolution.first;
		dmScreenSettings.dmPelsHeight = m_WindowResolution.second;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		if (!ChangeDisplaySettingsA(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			LogManager::LogMessage(LL_WARN, "Couldn't change to FullScreen Mode, "
				"changing to windowed, reason: 0x%08X.", GetLastError());
			m_WindowMode = EWindowMode::WINDOWED;
		}
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle |= WS_POPUP;
	}
	break;
	case EWindowMode::BORDERLESS:
	{
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle |= WS_POPUP;
	}
	break;
	case EWindowMode::WINDOWED:
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle |= WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
	}
	break;
	default:
		break;
	}
	if (!SetWindowLongPtrA(m_Handle, GWL_STYLE, dwStyle))
	{
		LogManager::LogMessage(LL_WARN, "Couldn't change the dwStyle of the window, while changing its mode"
			" from %s to %s, reason: 0x%08X.", GetWindowModeStr(m_WindowMode), GetWindowModeStr(mode), GetLastError());
	}
	if (!SetWindowLongPtrA(m_Handle, GWL_EXSTYLE, dwExStyle))
	{
		LogManager::LogMessage(LL_WARN, "Couldn't change the dwExStyle of the window, while changing its mode"
			" from %s to %s, reason: 0x%08X.", GetWindowModeStr(m_WindowMode), GetWindowModeStr(mode), GetLastError());
	}
	ShowWindow(m_Handle, SW_SHOWNORMAL);
#elif PLATFORM_LINUX

#else

#endif
	m_WindowMode = mode;
}

void Window::_UpdateWindowEvents()
{
#if PLATFORM_WINDOWS
	MSG msg{};
	if (PeekMessageA(&msg, m_Handle, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
#elif PLATFORM_LINUX

#else

#endif
}

void Window::_RequestFocus()
{
#if PLATFORM_WINDOWS
	if (!SetFocus(m_Handle))
	{
		LogManager::LogMessage(LL_ERRO, "Couldn't request user focus for "
			"the window, reason: 0x%08X.", GetLastError());
		return;
	}
#elif PLATFORM_LINUX

#else

#endif
	m_Focused = true;
}

CreateTaskName(WindowCreationTask);

Window::Window(const std::string & windowID, const std::string & windowTitle, const Resolution_t windowResolution, const Position_t windowPosition, const EWindowMode::Type windowMode)
	:m_Handle(nullptr)
	, m_WindowResolution(windowResolution)
	, m_WindowPosition(windowPosition)
	, m_WindowMode(windowMode)
	, m_Active(false)
	, m_Focused(false)
	, m_WindowTitle(windowTitle)
	, m_HasToClose(false)
	, m_WindowID(windowID.empty() ? "GreaperWindow" + std::to_string(WindowManager::GetNextID()) : windowID)
{
	LogManager::LogMessage(LL_INFO, "Creating a Window:\n"
		"\tID: %s\n"
		"\tTitle: %s\n"
		"\tResolution: %dx%d\n"
		"\tPosition: %dx%d\n"
		"\tMode: %s", m_WindowID.c_str(), m_WindowTitle.c_str(),
		m_WindowResolution.first, m_WindowResolution.second,
		m_WindowPosition.first, m_WindowPosition.second,
		GetWindowModeStr(m_WindowMode));
	m_CommandQueue.PushBack(CreateTask(WindowCreationTask, std::bind(&Window::_CreateWindow, this)));
	m_WindowThread = std::thread(&Window::Run, this);
}

Window::~Window()
{
	Close();
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	if (m_WindowThread.joinable())
		m_WindowThread.join();
}

WindowHandle Window::GetWindowHandle() const
{
	return m_Handle;
}

EWindowMode::Type gaf::Window::GetWindowMode() const
{
	return m_WindowMode;
}
	
CreateTaskName(ChangeWindowModeTask);

void Window::SetWindowMode(const EWindowMode::Type mode)
{
	if (m_HasToClose)
		return;
	if (mode == m_WindowMode)
		return;
	LogManager::LogMessage(LL_INFO, "Changing the WindowMode, wnd: %s, from: %s to %s.", m_WindowID.c_str(),
		GetWindowModeStr(m_WindowMode), GetWindowModeStr(mode));
	m_CommandQueue.PushBack(CreateTask(ChangeWindowModeTask, std::bind(&Window::_SetWindowMode, this, mode)));
}

Resolution_t Window::GetResolution() const
{
	return m_WindowResolution;
}

Resolution_t Window::GetLastResolution() const
{
	return m_LastWindowResolution;
}

CreateTaskName(ChangeWindowResolutionTask);

void Window::SetResolution(const Resolution_t res)
{
	if (m_HasToClose)
		return;
	LogManager::LogMessage(LL_INFO, "Changing the resolution of the window: %s, from: %dx%d to %dx%d.",
		m_WindowID.c_str(), m_WindowResolution.first, m_WindowResolution.second,
		res.first, res.second);
	m_CommandQueue.PushBack(CreateTask(ChangeWindowResolutionTask, std::bind(&Window::_ChangeResolution, this, res)));
}

Position_t Window::GetPosition() const
{
	return m_WindowPosition;
}

Position_t Window::GetLastPosition() const
{
	return m_LastWindowPosition;
}
	
CreateTaskName(ChangeWindowPositionTask);

void Window::SetPosition(const Position_t pos)
{
	if (m_HasToClose)
		return;
	LogManager::LogMessage(LL_INFO, "Changing the position of the window: %s, from: %dx%d to %dx%d.",
		m_WindowID.c_str(), m_WindowPosition.first, m_WindowPosition.second,
		pos.first, pos.second);
	m_CommandQueue.PushBack(CreateTask(ChangeWindowPositionTask, std::bind(&Window::_ChangePosition, this, pos)));
}

const std::string & Window::GetTitle() const
{
	return m_WindowTitle;
}
	
CreateTaskName(ChangeWindowTitleTask);

void Window::SetTitle(const std::string & title)
{
	if (m_HasToClose)
		return;
	LogManager::LogMessage(LL_INFO, "Changing the window title, from: %s to: %s, on the window: %s.",
		m_WindowTitle.c_str(), title.c_str(), m_WindowID.c_str());
	m_CommandQueue.PushBack(CreateTask(ChangeWindowTitleTask, std::bind(&Window::_ChangeTitle, this, title)));
}

const std::string & gaf::Window::GetID() const
{
	return m_WindowID;
}

bool Window::IsActive() const
{
	return m_Active;
}

bool Window::IsFocused() const
{
	return m_Focused;
}
	
CreateTaskName(CloseWindowTask);

void Window::Close()
{
	if (m_HasToClose)
		return;
	LogManager::LogMessage(LL_INFO, "Closing the window: %s.", m_WindowID.c_str());
	m_CommandQueue.PushBack(CreateTask(CloseWindowTask, std::bind(&Window::_Close, this)));
}
	
CreateTaskName(UpdateWindowEventsTask);

void Window::UpdateWindowEvents()
{
	if (m_HasToClose)
		return;
	m_CommandQueue.PushBack(CreateTask(UpdateWindowEventsTask, std::bind(&Window::_UpdateWindowEvents, this)));
}
	
CreateTaskName(WindowRequestFocusTask);

void Window::RequestFocus()
{
	if (m_HasToClose)
		return;
	LogManager::LogMessage(LL_INFO, "Requesting user focus from the window: %s.", m_WindowID.c_str());
	m_CommandQueue.PushBack(CreateTask(WindowRequestFocusTask, std::bind(&Window::_RequestFocus, this)));
}

void Window::SendWindowTask(const Task_t & task)
{
	if (m_HasToClose)
		return;
	m_CommandQueue.PushBack(task);
}

std::thread::id Window::GetThreadID() const
{
	return m_WindowThread.get_id();
}

std::thread::native_handle_type Window::GetThreadNativeHandle()
{
	return m_WindowThread.native_handle();
}