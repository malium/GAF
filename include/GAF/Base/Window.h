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

#ifndef GAF_WINDOW_H
#define GAF_WINDOW_H 1

namespace gaf
{
	namespace EWindowMode
	{
		enum Type
		{
			WINDOWED,
			FULLSCREEN,
			BORDERLESS
		};
	}
	const ANSICHAR* GetWindowModeStr(EWindowMode::Type mode);

	namespace EWindowPosition
	{
		enum Type
		{
			TOP_LEFT,
			TOP_CENTER,
			TOP_RIGHT,
			MIDDLE_LEFT,
			MIDDLE_CENTER,
			MIDDLE_RIGHT,
			BOTTOM_LEFT,
			BOTTOM_CENTER,
			BOTTOM_RIGHT,
		};
	}

	typedef
#if PLATFORM_WINDOWS
		HWND
#else
		void*
#endif
		WindowHandle;

	class Window
	{
		static constexpr auto ThreadSleepTimeMS = std::chrono::milliseconds(1);
		WindowHandle m_Handle;
		std::thread m_WindowThread;
		/* Thread Function */
		void Run();
		Resolution_t m_WindowResolution;
		Resolution_t m_LastWindowResolution;
		Resolution_t m_ClientResolution;
		Resolution_t m_LastClientResolution;
		Position_t m_WindowPosition;
		Position_t m_LastWindowPosition;
		EWindowMode::Type m_WindowMode;
		bool m_Active;
		bool m_Focused;
		std::string m_WindowTitle;
		AtomicFlag m_HasToClose;
		MPMCQueue<Task_t> m_CommandQueue;
		const std::string m_WindowID;

		void _CreateWindow();
		void _ChangeTitle(const std::string& title);
		void _ChangeResolution(Resolution_t res);
		void _ChangePosition(Position_t pos);
		void _Close();
		void _SetWindowMode(EWindowMode::Type mode);
		void _UpdateWindowEvents();
		void _RequestFocus();

		Window() = delete;
		Window(const std::string& windowID, const std::string& windowTitle, Resolution_t windowResolution,
			Position_t windowPosition, EWindowMode::Type windowMode);
		~Window();
#if PLATFORM_WINDOWS
		friend LRESULT WndProc(HWND, UINT, WPARAM, LPARAM);
#endif
	public:
		friend class WindowManager;

		WindowHandle GetWindowHandle()const;
		EWindowMode::Type GetWindowMode()const;
		/*
			This function sends a command to the window that changes its mode, it maybe followed by a window recreation
			so be careful with the things attached to it.
		*/
		void SetWindowMode(EWindowMode::Type mode);
		/*
			This function sends a command to the window that requests to change the resolution of it.
		*/
		void SetResolution(Resolution_t res);
		Resolution_t GetResolution()const;
		Resolution_t GetLastResolution()const;
		Resolution_t GetClientResolution()const;
		Resolution_t GetLastClientResolution()const;
		/*
			This function sends a command to the window that requests to change the position of it, some modes will ignore this.
		*/
		void SetPosition(Position_t pos);
		Position_t GetPosition()const;
		Position_t GetLastPosition()const;
		const std::string& GetTitle()const;
		/*
			Changes the title of this window
		*/
		void SetTitle(const std::string& title);
		const std::string& GetID()const;
		bool IsActive()const;
		bool IsFocused()const;
		/*
			This function will send a Closing task to the window, in order to close it and destroy everything
			attached to it, make sure that anything created by you and not handled by the window class closes
			before.
		*/
		void Close();
		void UpdateWindowEvents();
		void RequestFocus();
		/*
			This function will send a task to be done by the thread of this window, this is useful,
			for example for rendering things.
		*/
		void SendWindowTask(const Task_t& task);

		std::thread::id GetThreadID()const;
		std::thread::native_handle_type GetThreadNativeHandle();
	};
}

#endif /* GAF_WINDOW_H */