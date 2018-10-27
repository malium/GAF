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
#include "GAF/WindowManager.h"
#include "GAF/LogManager.h"
#include "WindowUtil.h"

using namespace gaf;

std::atomic<uint32> WindowManager::m_NextID = 0;
EventID WindowManager::EventIDOnWindowActivation = EventManager::NullEventID;
EventID WindowManager::EventIDOnWindowDeactivation = EventManager::NullEventID;
EventID WindowManager::EventIDOnWindowClosing = EventManager::NullEventID;
EventID WindowManager::EventIDOnWindowFocusLost = EventManager::NullEventID;
EventID WindowManager::EventIDOnWindowFocusGain = EventManager::NullEventID;
EventID WindowManager::EventIDOnWindowMove = EventManager::NullEventID;
EventID WindowManager::EventIDOnWindowMaximize = EventManager::NullEventID;
EventID WindowManager::EventIDOnWindowResized = EventManager::NullEventID;
EventID WindowManager::EventIDOnWindowMinimize = EventManager::NullEventID;
EventID WindowManager::EventIDOnWindowRestore = EventManager::NullEventID;
EventID WindowManager::EventIDOnWindowUnhandledEvent = EventManager::NullEventID;

static std::vector<DisplayAdapter> gDisplayAdapters;

namespace gaf
{
	static BOOL CALLBACK MonitorFn(HMONITOR hMonitor, HDC hDC, LPRECT lpRect, LPARAM lParam)
	{
		MONITORINFOEXA monInfo;
		ZeroMemory(&monInfo, sizeof(monInfo));
		monInfo.cbSize = sizeof(monInfo);
		if (!GetMonitorInfoA(hMonitor, &monInfo))
		{
			LogManager::LogMessage(LL_WARN, "Trying to retrieve the monitor info, but something went wrong, error: 0x%08X.", GetLastError());
			return true;
		}
		for (auto it = gDisplayAdapters.begin(); it != gDisplayAdapters.end(); ++it)
		{
			auto monitors = it->GetMonitors();
			for (auto itt = monitors.begin(); itt != monitors.end(); ++itt)
			{
				auto mon = ((Monitor*)(*itt));
				if (strncmp(&mon->GetName()[0], monInfo.szDevice, strlen(monInfo.szDevice)) == 0)
				{
					
					mon->SetPrimary(monInfo.dwFlags == MONITORINFOF_PRIMARY);
					const Position_t distanceTo00 = { -monInfo.rcMonitor.left, -monInfo.rcMonitor.top };

					const Resolution_t Size = {
						(uint16)((monInfo.rcMonitor.right - monInfo.rcMonitor.left) + distanceTo00.first),
						(uint16)((monInfo.rcMonitor.bottom - monInfo.rcMonitor.top) + distanceTo00.second)
					};
					const Resolution_t WorkArea = {
						(uint16)((monInfo.rcWork.right - monInfo.rcWork.left) + distanceTo00.first),
						(uint16)((monInfo.rcWork.bottom - monInfo.rcWork.top) + distanceTo00.second)
					};

					mon->SetSize(Size);
					mon->SetWorkArea(WorkArea);
					mon->SetPosition(distanceTo00);
					mon->SetHandle(hMonitor);
				}
			}
		}

		return true;
	}
	static void LogDisplayInfo()
	{
		std::string log = "---------------------------------------------------------------------\n";
		const auto numAdapters = gDisplayAdapters.size();
		log.reserve(numAdapters * 4 * 1024);
		log += "Starting DisplayAdapter information dump:\nFound " + std::to_string(numAdapters) + 
			((numAdapters == 1) ? " DisplayAdapter:" : " DisplayAdapters:");
		for (size_t i = 0; i < gDisplayAdapters.size(); ++i)
		{
			log.append("\nDisplayAdapter ").append(std::to_string(i));
			log.append(":\n\tName: ").append(gDisplayAdapters[i].GetName().c_str());
			log.append("\n\tIsDefault: ").append(gDisplayAdapters[i].IsDefault() ? "true" : "false");
			log.append("\n\tAdapterString: ").append(gDisplayAdapters[i].GetAdapterString().c_str());
			log.append("\n\tAdapterID: ").append(gDisplayAdapters[i].GetAdpaterID().c_str());
			log.append("\n\tAdapterKey: ").append(gDisplayAdapters[i].GetAdapterKey().c_str());
			const auto monitors = gDisplayAdapters[i].GetMonitors();
			const auto numMonitors = monitors.size();
			log.append("\n\tWith ").append(std::to_string(numMonitors) +
				((numMonitors == 1) ? " Monitor:" : " Monitors:"));
			for (size_t j = 0; j < numMonitors; ++j)
			{
				log.append("\n\tMonitor ").append(std::to_string(j));
				log.append(":\n\t\tName: ").append(monitors[j]->GetName().c_str());
				log.append("\n\t\tIsPrimary: ").append(monitors[j]->IsPrimary() ? "true" : "false");
				const auto deskPos = monitors[j]->GetDesktopPosition();
				log.append("\n\t\tDesktopPosition: ").append(std::to_string(deskPos.first)).append(",").append(std::to_string(deskPos.second));
				const auto monRes = monitors[j]->GetSize();
				log.append("\n\t\tCurrentResolution: ").append(std::to_string(monRes.first)).append("x").append(std::to_string(monRes.second));
				const auto monWork = monitors[j]->GetWorkArea();
				log.append("\n\t\tWorkArea: ").append(std::to_string(monWork.first)).append("x").append(std::to_string(monWork.second));
				log.append("\n\t\tMonitorString: ").append(monitors[j]->GetMonitorString().c_str());
				log.append("\n\t\tMonitorID: ").append(monitors[j]->GetMonitorID().c_str());
				log.append("\n\t\tMonitorKey: ").append(monitors[j]->GetMonitorKey().c_str());
			}
			const auto modes = gDisplayAdapters[i].GetVideoModes();
			const auto numModes = modes.size();
			log.append("\n\tWith ").append(std::to_string(numModes) +
				((numModes == 1) ? " Mode:" : " Modes:"));
			for (size_t j = 0; j < numModes; ++j)
			{
				log.append("\n\tMode ").append(std::to_string(j));
				const auto modeRes = modes[j]->GetResolution();
				log.append(":\n\t\tResolution: ").append(std::to_string(modeRes.first)).append("x").append(std::to_string(modeRes.second));
				log.append("\n\t\tFrequency: ").append(std::to_string(modes[j]->GetFrequency()));
				log.append("\n\t\tColorDepth: ").append(std::to_string(modes[j]->GetPixelDepth()));
			}
		}
		log.append("\n---------------------------------------------------------------------");
		LogManager::LogMessage(LL_INFO, log.c_str());
	}
	static void EnumDisplayAdapters()
	{
		/* Get Display Adpaters */
		DISPLAY_DEVICEA dispDev;
		for (DWORD i = 0; i < 255; ++i)
		{
			ZeroMemory(&dispDev, sizeof(dispDev));
			dispDev.cb = sizeof(dispDev);
			if (EnumDisplayDevicesA(nullptr, i, &dispDev, 0))
			{
				const bool active = dispDev.StateFlags & DISPLAY_DEVICE_ACTIVE;
				/*if (!active)
					continue;*/
				DisplayAdapter disp;
				std::string dispName, dispID, dispKey, dispStr;
				dispName.resize(33);
				dispID.resize(129);
				dispKey.resize(129);
				dispStr.resize(129);
				
				strncpy(&dispName[0], dispDev.DeviceName, 32);
				strncpy(&dispID[0], dispDev.DeviceID, 128);
				strncpy(&dispKey[0], dispDev.DeviceKey, 128);
				strncpy(&dispStr[0], dispDev.DeviceString, 128);

				const bool dfault = dispDev.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE;
				
				std::vector<Monitor> monitors;
				{
					DISPLAY_DEVICEA monDev;
					ZeroMemory(&monDev, sizeof(monDev));
					monDev.cb = sizeof(monDev);
					DWORD monitorNum = 0;
					while (EnumDisplayDevicesA(dispDev.DeviceName, monitorNum, &monDev, 0))
					{
						Monitor mon;
						std::string monName, monID, monKey, monStr;
						monName.resize(33);
						monID.resize(129);
						monKey.resize(129);
						monStr.resize(129);

						strncpy(&monName[0], monDev.DeviceName, 32);
						strncpy(&monID[0], monDev.DeviceID, 128);
						strncpy(&monKey[0], monDev.DeviceKey, 128);
						strncpy(&monStr[0], monDev.DeviceString, 128);

						mon.SetMonitorID(monID);
						mon.SetMonitorKey(monKey);
						mon.SetMonitorString(monStr);
						mon.SetName(monName);

						monitors.push_back(std::move(mon));
						ZeroMemory(&monDev, sizeof(monDev));
						monDev.cb = sizeof(monDev);
						++monitorNum;
					}
				}
				std::vector<VideoMode> modes;
				{
					DWORD modeNum = 0;
					DEVMODEA devMode;
					ZeroMemory(&devMode, sizeof(devMode));
					devMode.dmSize = sizeof(devMode);
					while (EnumDisplaySettingsA(dispDev.DeviceName, modeNum, &devMode))
					{
						VideoMode mode;
						mode.SetDepth((uint8)devMode.dmBitsPerPel);
						mode.SetResolution(Resolution_t{
							(uint16)devMode.dmPelsWidth,
							(uint16)devMode.dmPelsHeight
							});

						mode.SetFrequency((uint16)devMode.dmDisplayFrequency);
						modes.push_back(std::move(mode));
						ZeroMemory(&devMode, sizeof(devMode));
						devMode.dmSize = sizeof(devMode);
						++modeNum;
					}
				}
				{
					/* Remove redundant VideoModes */
					for (size_t j = 0; j < modes.size(); ++j)
					{
						for (size_t k = 0; k < modes.size(); ++k)
						{
							if (j == k)
								continue;
							if (modes[j].Equals((IVideoMode*)&modes[k]))
							{
								modes.erase(modes.begin() + j);
								if (modes.size() == j)
									break;
							}
						}
					}
				}

				disp.Set(dispName, dispStr, dispID, dispKey, dfault, monitors, modes);
				
				gDisplayAdapters.push_back(disp);
			}
			else
			{
				break;
			}
		}
		if (!EnumDisplayMonitors(nullptr, nullptr, (MONITORENUMPROC)&MonitorFn, 0))
		{
			LogManager::LogMessage(LL_WARN, "Trying to enumerate the MonitorInfo, but something went wrong, error: 0x%08X.", GetLastError());
		}
		LogDisplayInfo();
	}
}

WindowManager::WindowManager()
{
	LogManager::LogMessage(LL_INFO, "Starting WindowManager...");
	const auto eventMgr = InstanceEvent();
	EventIDOnWindowActivation = eventMgr->RegisterEvent(OnWindowActivationEvent);
	EventIDOnWindowDeactivation = eventMgr->RegisterEvent(OnWindowDeactivationEvent);
	EventIDOnWindowClosing = eventMgr->RegisterEvent(OnWindowClosingEvent);
	EventIDOnWindowFocusLost = eventMgr->RegisterEvent(OnWindowFocusLostEvent);
	EventIDOnWindowFocusGain = eventMgr->RegisterEvent(OnWindowFocusGainEvent);
	EventIDOnWindowMove = eventMgr->RegisterEvent(OnWindowMoveEvent);
	EventIDOnWindowMaximize = eventMgr->RegisterEvent(OnWindowMaximizeEvent);
	EventIDOnWindowResized = eventMgr->RegisterEvent(OnWindowResizedEvent);
	EventIDOnWindowMinimize = eventMgr->RegisterEvent(OnWindowMinimizeEvent);
	EventIDOnWindowRestore = eventMgr->RegisterEvent(OnWindowRestoreEvent);
	EventIDOnWindowUnhandledEvent = eventMgr->RegisterEvent(OnWindowUnhandledEventEvent);
	m_OnWindowCloseListener = eventMgr->RegisterEventListener([&](const EventID id, void* params)
	{
		auto wnd = (Window*)params;
		CloseWindow(wnd->GetID());
	}, EventIDOnWindowClosing);
	EnumDisplayAdapters();
}

WindowManager::~WindowManager()
{
	LogManager::LogMessage(LL_INFO, "Stopping WindowManager...");
	m_Mutex.lock_shared();
	if (!m_WindowMap.empty())
	{
		for (auto it = m_WindowMap.begin(); it != m_WindowMap.end(); ++it)
			it->second->Close();
	}
	m_Mutex.unlock_shared();
	const auto eventMgr = InstanceEvent();
	eventMgr->UnregisterEvent(EventIDOnWindowActivation);
	eventMgr->UnregisterEvent(EventIDOnWindowDeactivation);
	eventMgr->UnregisterEvent(EventIDOnWindowClosing);
	eventMgr->UnregisterEvent(EventIDOnWindowFocusLost);
	eventMgr->UnregisterEvent(EventIDOnWindowFocusGain);
	eventMgr->UnregisterEvent(EventIDOnWindowMove);
	eventMgr->UnregisterEvent(EventIDOnWindowMaximize);
	eventMgr->UnregisterEvent(EventIDOnWindowResized);
	eventMgr->UnregisterEvent(EventIDOnWindowMinimize);
	eventMgr->UnregisterEvent(EventIDOnWindowRestore);
	eventMgr->UnregisterEvent(EventIDOnWindowUnhandledEvent);
	eventMgr->UnregisterEventListener(m_OnWindowCloseListener);

	EventIDOnWindowActivation = EventManager::NullEventID;
	EventIDOnWindowDeactivation = EventManager::NullEventID;
	EventIDOnWindowClosing = EventManager::NullEventID;
	EventIDOnWindowFocusLost = EventManager::NullEventID;
	EventIDOnWindowFocusGain = EventManager::NullEventID;
	EventIDOnWindowMove = EventManager::NullEventID;
	EventIDOnWindowMaximize = EventManager::NullEventID;
	EventIDOnWindowResized = EventManager::NullEventID;
	EventIDOnWindowMinimize = EventManager::NullEventID;
	EventIDOnWindowRestore = EventManager::NullEventID;
	EventIDOnWindowUnhandledEvent = EventManager::NullEventID;
#if GREAPER_DEBUG_ALLOCATION
	gDisplayAdapters.clear();
#endif
}

WindowManager& WindowManager::Instance()
{
	return *Application::InstancePtr()->GetWindowManager();
}

WindowManager * WindowManager::InstancePtr()
{
	return Application::InstancePtr()->GetWindowManager();
}

Window* WindowManager::CreateAWindow(const std::string & windowID, const std::string & windowTitle, const Resolution_t windowResolution, const Position_t windowPosition, const EWindowMode::Type windowMode)
{
	if (!windowID.empty()) /* If empty windowID will be the window threadID converted to string */
	{
		const auto it = m_WindowMap.find(std::hash<std::string>()(windowID));
		if (it != m_WindowMap.end())
		{
			InstanceApp()->GetLogManager()->LogMessage(LL_WARN, "Trying to create a Window with an id that already exists, ID:'%s'.", windowID.c_str());
			return it->second;
		}
	}
	auto wnd = new Window(windowID,
		windowTitle.empty() ? "Greaper Window" : windowTitle,
		windowResolution,
		windowPosition,
		windowMode);
	m_Mutex.lock();
	m_WindowMap.emplace(std::make_pair(std::hash<std::string>{}(wnd->m_WindowID), wnd));
	m_Mutex.unlock();
	return wnd;
}

Window * gaf::WindowManager::CreateAWindow(const std::string & windowID, const std::string & windowTitle, 
	const Resolution_t windowResolution, IMonitor* monitor, const EWindowPosition::Type windowPosition, const EWindowMode::Type windowMode)
{
	if (!monitor)
		monitor = GetPrimaryMonitor();

	Assertion::WhenNullptr(monitor, "Trying to create a window but something unexpected happend, there were no monitors.");

	Position_t windowPos;
	
	if (windowMode != EWindowMode::FULLSCREEN)
	{
		const auto desktopOrigin = monitor->GetDesktopPosition();
		const auto desktopRes = monitor->GetWorkArea();


		switch (windowPosition)
		{
		case EWindowPosition::TOP_LEFT:
		{
			windowPos = desktopOrigin;
		}
		break;
		case EWindowPosition::TOP_CENTER:
		{
			windowPos = {
				((int32)(desktopRes.first / 2 - windowResolution.first / 2)) + desktopOrigin.first,
				desktopOrigin.second
			};
		}
		break;
		case EWindowPosition::TOP_RIGHT:
		{
			windowPos = {
				((int32)desktopRes.first - windowResolution.first) + desktopOrigin.first,
				desktopOrigin.second
			};
		}
		break;
		case EWindowPosition::MIDDLE_LEFT:
		{
			windowPos = {
				desktopOrigin.first,
				((int32)desktopRes.second / 2 - windowResolution.second / 2) + desktopOrigin.second
			};
		}
		break;
		case EWindowPosition::MIDDLE_RIGHT:
		{
			windowPos = {
				((int32)desktopRes.first - windowResolution.first) + desktopOrigin.first,
				((int32)desktopRes.second / 2 - windowResolution.second / 2) + desktopOrigin.second
			};
		}
		break;
		case EWindowPosition::BOTTOM_LEFT:
		{
			windowPos = {
				desktopOrigin.first,
				((int32)desktopRes.second - windowResolution.second) + desktopOrigin.second
			};
		}
		break;
		case EWindowPosition::BOTTOM_CENTER:
		{
			windowPos = {
				((int32)(desktopRes.first / 2 - windowResolution.first / 2)) + desktopOrigin.first,
				((int32)desktopRes.second - windowResolution.second) + desktopOrigin.second
			};
		}
		break;
		case EWindowPosition::BOTTOM_RIGHT:
		{
			windowPos = {
				((int32)desktopRes.first - windowResolution.first) + desktopOrigin.first,
				((int32)desktopRes.second - windowResolution.second) + desktopOrigin.second
			};
		}
		break;
		default: /* The default is centered */
		{
			windowPos = {
				((int32)(desktopRes.first / 2 - windowResolution.first / 2)) + desktopOrigin.first,
				((int32)desktopRes.second / 2 - windowResolution.second / 2) + desktopOrigin.second
			};
		}
		break;
		}
	}
	else
	{
		windowPos = { 0,0 };
	}
	return CreateAWindow(windowID, windowTitle, windowResolution, windowPos, windowMode);
}

void WindowManager::CloseWindow(const std::string & windowID)
{
	if (!windowID.empty()) /* If empty windowID will be the window threadID converted to string */
	{
		m_Mutex.lock_shared();
		const auto it = m_WindowMap.find(std::hash<std::string>()(windowID));
		if (it != m_WindowMap.end())
		{
			m_Mutex.unlock_shared();
			m_Mutex.lock();
			delete it->second;
			m_WindowMap.erase(it);
			m_Mutex.unlock();
		}
		else
		{
			m_Mutex.unlock_shared();
			LogManager::LogMessage(LL_ERRO, "Couldn't find the window with id:'%s' in order to be closed.", windowID.c_str());
		}
	}
	else
	{
		LogManager::LogMessage(LL_ERRO, "Trying to close a window without sending its id.");
	}
}

Window * WindowManager::GetWindow(const std::string & windowID)
{
	if (!windowID.empty()) /* If empty windowID will be the window threadID converted to string */
	{
		m_Mutex.lock_shared();
		const auto it = m_WindowMap.find(std::hash<std::string>()(windowID));
		if (it != m_WindowMap.end())
		{
			m_Mutex.unlock_shared();
			return it->second;
		}
		m_Mutex.unlock_shared();
	}
	return nullptr;
}

uint32 gaf::WindowManager::GetNextID()
{
	const auto id = m_NextID.load();
	++m_NextID;
	return id;
}

DisplayAdapterVec WindowManager::EnumerateDisplayAdapters() const
{
	DisplayAdapterVec dispVec;
	for (auto it = gDisplayAdapters.begin(); it != gDisplayAdapters.end(); ++it)
		dispVec.push_back((IDisplayAdapter*)&(*it));
	return dispVec;
}

WindowManager::WindowMap::iterator WindowManager::Begin()
{
	return m_WindowMap.begin();
}

WindowManager::WindowMap::iterator WindowManager::End()
{
	return m_WindowMap.end();
}

void WindowManager::LockWindowMapRead()
{
	m_Mutex.lock_shared();
}

void WindowManager::UnlockWindowMapRead()
{
	m_Mutex.unlock_shared();
}

IMonitor* WindowManager::GetPrimaryMonitor() const
{
	for (auto it = gDisplayAdapters.begin(); it != gDisplayAdapters.end(); ++it)
	{
		const auto monitors = it->GetMonitors();
		for (auto itt = monitors.begin(); itt != monitors.end(); ++itt)
			if ((*itt)->IsPrimary())
				return *itt;
	}
	return nullptr;
}