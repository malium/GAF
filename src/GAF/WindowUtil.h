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

namespace gaf
{
	class Monitor;
	class DisplayAdapter;

	class VideoMode final : public IVideoMode
	{
		Resolution_t m_Resolution;
		uint8 m_PixelDepth;
		uint16 m_Frequency;
		DisplayAdapter* m_Adapter;

	public:
		VideoMode();
		VideoMode(Resolution_t res, uint8 depth, uint16 freq, DisplayAdapter* adapter);
		VideoMode(const VideoMode&) = default;
		VideoMode(VideoMode&&)noexcept = default;
		VideoMode& operator=(const VideoMode&) = default;
		VideoMode& operator=(VideoMode&&)noexcept = default;
		~VideoMode() = default;

		void SetResolution(Resolution_t res);
		void SetDepth(uint8 depth);
		void SetFrequency(uint16 freq);
		void SetAdapter(DisplayAdapter* mon);
		void Set(Resolution_t res, uint8 depth, uint16 freq, DisplayAdapter* adapter);

		Resolution_t GetResolution()const override;
		uint8 GetPixelDepth()const override;
		IDisplayAdapter* GetAdapter()const override;
		uint16 GetFrequency()const override;
		bool Equals(IVideoMode* other)const override;
		friend void EnumDisplayAdapters();
	};

	class Monitor final : public IMonitor
	{
		Resolution_t m_Size;
		Resolution_t m_WorkArea;
		MonitorHandle m_Handle;
		Position_t m_DesktopPosition;
		std::string m_Name;
		std::string m_MonitorString;
		std::string m_MonitorID;
		std::string m_MonitorKey;
		bool m_Primary;
		DisplayAdapter* m_Adapter;
	public:
		Monitor();
		Monitor(Resolution_t size, Resolution_t workArea, MonitorHandle handle, bool primary,
			Position_t desktopPosition, const std::string& name, const std::string& monStr,
			const std::string& monID, const std::string& monKey, DisplayAdapter* adapter);
		Monitor(const Monitor& other);
		Monitor(Monitor&&)noexcept = default;
		Monitor& operator=(const Monitor& other);
		Monitor& operator=(Monitor&&)noexcept = default;
		~Monitor() = default;

		void SetSize(Resolution_t size);
		void SetWorkArea(Resolution_t area);
		void SetHandle(MonitorHandle handle);
		void SetPrimary(bool primary);
		void SetPosition(Position_t pos);
		void SetName(const std::string& name);
		void SetMonitorString(const std::string& str);
		void SetMonitorID(const std::string& id);
		void SetMonitorKey(const std::string& key);
		void SetAdapter(DisplayAdapter* adapter);
		void Set(Resolution_t size, Resolution_t workArea, MonitorHandle handle, bool primary,
			Position_t desktopPosition, const std::string& name, const std::string& monStr,
			const std::string& monID, const std::string& monKey, DisplayAdapter* adapter);

		Resolution_t GetSize()const override;
		Resolution_t GetWorkArea()const override;
		MonitorHandle GetOSHandle()const override;
		bool IsPrimary()const override;
		Position_t GetDesktopPosition()const override;
		IDisplayAdapter* GetAdapter()const override;
		bool Equals(IMonitor* other)const override;
		const std::string& GetName()const override;
		const std::string& GetMonitorString()const override;
		const std::string& GetMonitorID()const override;
		const std::string& GetMonitorKey()const override;
		friend void EnumDisplayAdapters();
	};

	class DisplayAdapter final : public IDisplayAdapter
	{
		std::string m_Name;
		std::string m_AdapterString;
		std::string m_AdapterID;
		std::string m_AdapterKey;
		bool m_Default;
		std::vector<Monitor> m_Monitors;
		std::vector<VideoMode> m_VideoModes;
	public:
		DisplayAdapter();
		DisplayAdapter(const std::string& name, const std::string& adapterStr,
			const std::string& adapterID, const std::string& adapterKey, bool dfault,
			const std::vector<Monitor>& monitors, const std::vector<VideoMode>& modes);
		DisplayAdapter(const DisplayAdapter& other);
		DisplayAdapter(DisplayAdapter&&)noexcept = default;
		DisplayAdapter& operator=(const DisplayAdapter& other);
		DisplayAdapter& operator=(DisplayAdapter&&)noexcept = default;
		~DisplayAdapter() = default;

		void SetName(const std::string& name);
		void SetAdapterString(const std::string& adapterStr);
		void SetAdapterID(const std::string& adapterID);
		void SetAdapterKey(const std::string& adapaterKey);
		void SetDefault(bool dfault);
		void SetMonitors(const std::vector<Monitor>& monitors);
		void SetVideoModes(const std::vector<VideoMode>& modes);
		void Set(const std::string& name, const std::string& adapterStr,
			const std::string& adapterID, const std::string& adapterKey, bool dfault,
			const std::vector<Monitor>& monitors, const std::vector<VideoMode>& modes);

		MonitorVec GetMonitors()const override;
		VideoModeVec GetVideoModes()const override;
		IMonitor* GetMonitorWithSize(Resolution_t size)const override;
		const std::string& GetName()const override;
		const std::string& GetAdapterString()const override;
		const std::string& GetAdpaterID()const override;
		const std::string& GetAdapterKey()const override;
		bool Equals(IDisplayAdapter* other)const override;
		bool HasAnyMonitorConnected()const override;
		bool IsDefault()const override;
		friend void EnumDisplayAdapters();
	};
}