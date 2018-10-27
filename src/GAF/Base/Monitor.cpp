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

Monitor::Monitor()
	:m_Size{ 0,0 }
	, m_WorkArea{ 0,0 }
	, m_Handle(nullptr)
	, m_Primary(false)
	, m_DesktopPosition{ 0,0 }
	, m_Adapter(nullptr)
{

}

Monitor::Monitor(const Resolution_t size, const Resolution_t workArea, MonitorHandle handle, const bool primary,
	const Position_t desktopPosition, const std::string& name, const std::string& monStr,
	const std::string& monID, const std::string& monKey, DisplayAdapter* adapter)
	:m_Size(size)
	, m_WorkArea(workArea)
	, m_Handle(handle)
	, m_Primary(primary)
	, m_DesktopPosition(desktopPosition)
	, m_Name(name)
	, m_MonitorString(monStr)
	, m_MonitorID(monID)
	, m_MonitorKey(monKey)
	, m_Adapter(adapter)
{

}

Monitor::Monitor(const Monitor & other)
	:m_Size(other.m_Size)
	, m_WorkArea(other.m_WorkArea)
	, m_Handle(other.m_Handle)
	, m_Primary(other.m_Primary)
	, m_DesktopPosition(other.m_DesktopPosition)
	, m_Name(other.m_Name)
	, m_MonitorString(other.m_MonitorString)
	, m_MonitorID(other.m_MonitorID)
	, m_MonitorKey(other.m_MonitorKey)
	, m_Adapter(other.m_Adapter)
{

}

Monitor& Monitor::operator=(const Monitor & other)
{
	if (this != &other)
	{
		m_Size = other.m_Size;
		m_WorkArea = other.m_WorkArea;
		m_Handle = other.m_Handle;
		m_Primary = other.m_Primary;
		m_DesktopPosition = other.m_DesktopPosition;
		m_Name = other.m_Name;
		m_MonitorString = other.m_MonitorString;
		m_MonitorID = other.m_MonitorID;
		m_MonitorKey = other.m_MonitorKey;
		m_Adapter = other.m_Adapter;
	}
	return *this;
}

void Monitor::SetSize(const Resolution_t size)
{
	m_Size = size;
}

void Monitor::SetWorkArea(const Resolution_t workArea)
{
	m_WorkArea = workArea;
}

void Monitor::SetHandle(MonitorHandle handle)
{
	m_Handle = handle;
}

void Monitor::SetPrimary(const bool primary)
{
	m_Primary = primary;
}

void Monitor::SetPosition(const Position_t desktopPosition)
{
	m_DesktopPosition = desktopPosition;
}

void Monitor::SetName(const std::string& name)
{
	m_Name = name;
}

void Monitor::SetMonitorString(const std::string & str)
{
	m_MonitorString = str;
}

void Monitor::SetMonitorID(const std::string & id)
{
	m_MonitorID = id;
}

void Monitor::SetMonitorKey(const std::string & key)
{
	m_MonitorKey = key;
}

void Monitor::SetAdapter(DisplayAdapter* adapter)
{
	m_Adapter = adapter;
}

void Monitor::Set(const Resolution_t size, const Resolution_t workArea, MonitorHandle handle, const bool primary,
	const Position_t desktopPosition, const std::string& name, const std::string& monStr,
	const std::string& monID, const std::string& monKey, DisplayAdapter* adapter)
{
	m_Size = size;
	m_WorkArea = workArea;
	m_Handle = handle;
	m_Primary = primary;
	m_DesktopPosition = desktopPosition;
	m_Name = name;
	m_MonitorString = monStr;
	m_MonitorID = monID;
	m_MonitorKey = monKey;
	m_Adapter = adapter;
}

Resolution_t Monitor::GetSize()const
{
	return m_Size;
}

Resolution_t Monitor::GetWorkArea()const
{
	return m_WorkArea;
}

MonitorHandle Monitor::GetOSHandle()const
{
	return m_Handle;
}

bool Monitor::IsPrimary()const
{
	return m_Primary;
}

Position_t Monitor::GetDesktopPosition()const
{
	return m_DesktopPosition;
}

IDisplayAdapter* Monitor::GetAdapter()const
{
	return (IDisplayAdapter*)m_Adapter;
}

bool Monitor::Equals(IMonitor* other)const
{
	Assertion::WhenNullptr(other, "Trying to check if two IMonitor are equal, "
		"but the other was nullptr.");
	return m_Size == other->GetSize()
		&& m_WorkArea == other->GetWorkArea()
		&& m_Handle == other->GetOSHandle()
		&& m_DesktopPosition == other->GetDesktopPosition()
		&& m_Name == other->GetName()
		&& m_Adapter == other->GetAdapter();
}

const std::string& Monitor::GetName()const
{
	return m_Name;
}

const std::string& Monitor::GetMonitorString() const
{
	return m_MonitorString;
}

const std::string& Monitor::GetMonitorID() const
{
	return m_MonitorID;
}

const std::string& Monitor::GetMonitorKey() const
{
	return m_MonitorKey;
}