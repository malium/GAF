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
#include "..\WindowUtil.h"

using namespace gaf;

DisplayAdapter::DisplayAdapter()
	:m_Default(false)
{

}

DisplayAdapter::DisplayAdapter(const std::string& name, const std::string& adapterStr,
	const std::string& adapterID, const std::string& adapterKey, const bool dfault,
	const std::vector<Monitor>& monitors, const std::vector<VideoMode>& modes)
	: m_Name(name)
	, m_AdapterString(adapterStr)
	, m_AdapterID(adapterID)
	, m_AdapterKey(adapterKey)
	, m_Default(dfault)
	, m_Monitors(monitors)
	, m_VideoModes(modes)
{
	if (!m_Monitors.empty())
	{
		for (auto it = m_Monitors.begin(); it != m_Monitors.end(); ++it)
			it->SetAdapter(this);
	}
	if (!m_VideoModes.empty())
	{
		for (auto it = m_VideoModes.begin(); it != m_VideoModes.end(); ++it)
			it->SetAdapter(this);
	}
}

DisplayAdapter::DisplayAdapter(const DisplayAdapter & other)
	:m_Name(other.m_Name)
	, m_AdapterString(other.m_AdapterString)
	, m_AdapterID(other.m_AdapterID)
	, m_AdapterKey(other.m_AdapterKey)
	, m_Default(other.m_Default)
	, m_Monitors(other.m_Monitors)
	, m_VideoModes(other.m_VideoModes)
{
	if (!m_Monitors.empty())
	{
		for (auto it = m_Monitors.begin(); it != m_Monitors.end(); ++it)
			it->SetAdapter(this);
	}
	if (!m_VideoModes.empty())
	{
		for (auto it = m_VideoModes.begin(); it != m_VideoModes.end(); ++it)
			it->SetAdapter(this);
	}
}

DisplayAdapter & DisplayAdapter::operator=(const DisplayAdapter & other)
{
	if (this != &other)
	{
		m_Name = other.m_Name;
		m_AdapterString = other.m_AdapterString;
		m_AdapterID = other.m_AdapterID;
		m_AdapterKey = other.m_AdapterKey;
		m_Default = other.m_Default;
		m_Monitors = other.m_Monitors;
		m_VideoModes = other.m_VideoModes;

		if (!m_Monitors.empty())
		{
			for (auto it = m_Monitors.begin(); it != m_Monitors.end(); ++it)
				it->SetAdapter(this);
		}
		if (!m_VideoModes.empty())
		{
			for (auto it = m_VideoModes.begin(); it != m_VideoModes.end(); ++it)
				it->SetAdapter(this);
		}
	}
	return *this;
}

void DisplayAdapter::SetName(const std::string& name)
{
	m_Name = name;
}

void DisplayAdapter::SetAdapterString(const std::string& adapterStr)
{
	m_AdapterString = adapterStr;
}

void DisplayAdapter::SetAdapterID(const std::string& adapterID)
{
	m_AdapterID = adapterID;
}

void DisplayAdapter::SetAdapterKey(const std::string& adapterKey)
{
	m_AdapterKey = adapterKey;
}

void DisplayAdapter::SetDefault(const bool dfault)
{
	m_Default = dfault;
}

void DisplayAdapter::SetMonitors(const std::vector<Monitor>& monitors)
{
	m_Monitors = monitors;
	if (!m_Monitors.empty())
	{
		for (auto it = m_Monitors.begin(); it != m_Monitors.end(); ++it)
			it->SetAdapter(this);
	}
}

void DisplayAdapter::SetVideoModes(const std::vector<VideoMode>& modes)
{
	m_VideoModes = modes;
	if (!m_VideoModes.empty())
	{
		for (auto it = m_VideoModes.begin(); it != m_VideoModes.end(); ++it)
			it->SetAdapter(this);
	}
}

void DisplayAdapter::Set(const std::string& name, const std::string& adapterStr,
	const std::string& adapterID, const std::string& adapterKey, const bool dfault,
	const std::vector<Monitor>& monitors, const std::vector<VideoMode>& modes)
{
	m_Name = name;
	m_AdapterString = adapterStr;
	m_AdapterID = adapterID;
	m_AdapterKey = adapterKey;
	m_Default = dfault;
	m_Monitors = monitors;
	m_VideoModes = modes;

	if (!m_Monitors.empty())
	{
		for (auto it = m_Monitors.begin(); it != m_Monitors.end(); ++it)
			it->SetAdapter(this);
	}
	if (!m_VideoModes.empty())
	{
		for (auto it = m_VideoModes.begin(); it != m_VideoModes.end(); ++it)
			it->SetAdapter(this);
	}
}

MonitorVec DisplayAdapter::GetMonitors()const
{
	if (m_Monitors.empty())
		return {};
	MonitorVec rtn;
	for (auto it = m_Monitors.begin(); it != m_Monitors.end(); ++it)
		rtn.push_back((IMonitor*)&(*it));
	return rtn;
}

VideoModeVec gaf::DisplayAdapter::GetVideoModes() const
{
	if (m_VideoModes.empty())
		return {};
	VideoModeVec rtn;
	for (auto it = m_VideoModes.begin(); it != m_VideoModes.end(); ++it)
		rtn.push_back((IVideoMode*)&(*it));
	return rtn;
}

IMonitor * DisplayAdapter::GetMonitorWithSize(const Resolution_t size) const
{
	if (m_Monitors.empty())
		return nullptr;
	for (auto it = m_Monitors.begin(); it != m_Monitors.end(); ++it)
	{
		if (it->GetSize() == size)
			return (IMonitor*)&(*it);
	}
	return nullptr;
}

const std::string& DisplayAdapter::GetName()const
{
	return m_Name;
}

const std::string& DisplayAdapter::GetAdapterString()const
{
	return m_AdapterString;
}

const std::string& DisplayAdapter::GetAdpaterID()const
{
	return m_AdapterID;
}

const std::string& DisplayAdapter::GetAdapterKey()const
{
	return m_AdapterKey;
}

bool DisplayAdapter::IsDefault()const
{
	return m_Default;
}

bool DisplayAdapter::Equals(IDisplayAdapter* other)const
{
	Assertion::WhenNullptr(other, "Trying to check if two IDisplayAdapter are equal, "
		"but the other was nullptr.");
	return m_Name == other->GetName()
		&& m_AdapterString == other->GetAdapterString()
		&& m_AdapterID == other->GetAdpaterID()
		&& m_AdapterKey == other->GetAdapterKey();
}

bool DisplayAdapter::HasAnyMonitorConnected() const
{
	return !m_Monitors.empty();
}