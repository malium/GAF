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

#ifndef GAF_WINDOW_MANAGER_H
#define GAF_WINDOW_MANAGER_H 1

#include "GAF/GAFPrerequisites.h"
#include "GAF/TaskManager.h"
#include "GAF/EventManager.h"
#include "GAF/Base/IVideoMode.h"
#include "GAF/Base/IMonitor.h"
#include "GAF/Base/IDisplayAdapter.h"
#include "GAF/Base/Window.h"

namespace gaf
{
	class WindowManager
	{
		using WindowMap = std::map<std::hash<std::string>::result_type, Window*>;
		WindowMap m_WindowMap;
		std::shared_mutex m_Mutex;
		static std::atomic<uint32> m_NextID;
		EventListenerID m_OnWindowCloseListener;
		WindowManager();
		~WindowManager();
	public:
		static constexpr auto OnWindowActivationEvent = "OnWindowActivation";
		static EventID EventIDOnWindowActivation;
		static constexpr auto OnWindowDeactivationEvent = "OnWindowDeactivation";
		static EventID EventIDOnWindowDeactivation;
		static constexpr auto OnWindowClosingEvent = "OnWindowClosing";
		static EventID EventIDOnWindowClosing;
		static constexpr auto OnWindowFocusLostEvent = "OnWindowFocusLost";
		static EventID EventIDOnWindowFocusLost;
		static constexpr auto OnWindowFocusGainEvent = "OnWindowFocusGain";
		static EventID EventIDOnWindowFocusGain;
		static constexpr auto OnWindowMoveEvent = "OnWindowMove";
		static EventID EventIDOnWindowMove;
		static constexpr auto OnWindowMaximizeEvent = "OnWindowMaximize";
		static EventID EventIDOnWindowMaximize;
		static constexpr auto OnWindowResizedEvent = "OnWindowResized";
		static EventID EventIDOnWindowResized;
		static constexpr auto OnWindowMinimizeEvent = "OnWindowMinimize";
		static EventID EventIDOnWindowMinimize;
		static constexpr auto OnWindowRestoreEvent = "OnWindowRestore";
		static EventID EventIDOnWindowRestore;
		static constexpr auto OnWindowUnhandledEventEvent = "OnWindowUnhandledEvent";
		static EventID EventIDOnWindowUnhandledEvent;

		static WindowManager& Instance();
		static WindowManager* InstancePtr();

		Window* CreateAWindow(const std::string& windowID, const std::string& windowTitle, Resolution_t windowResolution,
			Position_t windowPosition, EWindowMode::Type windowMode);

		Window* CreateAWindow(const std::string& windowID, const std::string& windowTitle, Resolution_t windowResolution,
			IMonitor* monitor, EWindowPosition::Type windowPosition, EWindowMode::Type windowMode);

		void CloseWindow(const std::string& windowID);

		Window* GetWindow(const std::string& windowID);

		static uint32 GetNextID();

		DisplayAdapterVec EnumerateDisplayAdapters()const;

		void LockWindowMapRead();
		WindowMap::iterator Begin();
		WindowMap::iterator End();
		void UnlockWindowMapRead();

		IMonitor* GetPrimaryMonitor()const;	

		friend class Application;
	};

	static WindowManager* InstanceWnd() { return WindowManager::InstancePtr(); }
}

#endif /* GAF_WINDOW_MANAGER_H */