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
#include "GAF/PropertiesManager.h"
#include "GAF/LogManager.h"
#include "Property.h"

using namespace gaf;

EventID PropertiesManager::EventIDOnModification = EventManager::NullEventID;

struct StaticPropElem
{
	StaticProperty* Property;
	StaticPropElem* Next;
};
static StaticPropElem* gPropHead = nullptr;
static std::map<size_t, gaf::Property> gProperties;

#if GREAPER_DEBUG_ALLOCATION
static bool gCleanAtExit = false;
#endif

void PropertiesManager::RegisterStaticProperty(const StaticProperty prop)
{
	if (!gPropHead)
	{
		gPropHead = new StaticPropElem();
		gPropHead->Next = nullptr;
		gPropHead->Property = new StaticProperty(std::move(prop));
	}
	else
	{
		auto cur = gPropHead;
		while (cur->Next)
		{
			cur = cur->Next;
		}
		cur->Next = new StaticPropElem();
		cur->Next->Next = nullptr;
		cur->Next->Property = new StaticProperty(std::move(prop));
	}
#if GREAPER_DEBUG_ALLOCATION
	if (!gCleanAtExit)
	{
		atexit([]()
		{
			auto cur = gPropHead;
			while (cur)
			{
				auto prev = cur;
				cur = cur->Next;
				SAFE_DELETE(prev->Property);
				SAFE_DELETE(prev);
			}
		});
		gCleanAtExit = true;
	}
#endif
}

static void OnPropChange(const std::function<void(IProperty*)>& fn, const std::string& propName, const EventID evt, void* params)
{
	if (evt == PropertiesManager::EventIDOnModification)
	{
		auto prop = (IProperty*)params;
		if (!prop)
			return;
		if (prop->GetName() != propName)
			return;
		fn(prop);
	}
}

PropertiesManager::PropertiesManager()
{
	LogManager::LogMessage(LL_INFO, "Starting PropertiesManager...");
	const auto eventMgr = InstanceEvent();
	EventIDOnModification = eventMgr->RegisterEvent(OnModificationEventName);
	m_Mutex.lock();
	auto cur = gPropHead;
	while (cur)
	{
		const auto hash = std::hash<std::string>{}(cur->Property->m_Name);
		if (cur->Property->m_Attributes & (1 << EPropertyAttributes::BOOLEAN))
		{
			gProperties.insert_or_assign(hash, Property(cur->Property->m_Name, cur->Property->m_Attributes, cur->Property->m_Number != Property::NumberFalse ? true : false));
		}
		else if (cur->Property->m_Attributes & (1 << EPropertyAttributes::NUMBER))
		{
			gProperties.insert_or_assign(hash, Property(cur->Property->m_Name, cur->Property->m_Attributes, cur->Property->m_Number, cur->Property->m_NumberMin, cur->Property->m_NumberMax));
		}
		else
		{
			gProperties.insert_or_assign(hash, Property(cur->Property->m_Name, cur->Property->m_Attributes, cur->Property->m_Value, cur->Property->m_ValidValues));
		}
		if (cur->Property->m_OnPropChange)
		{
			using namespace std::placeholders;
			eventMgr->RegisterEventListener(std::bind(OnPropChange, cur->Property->m_OnPropChange, cur->Property->m_Name, _1, _2), EventIDOnModification);
		}
		cur = cur->Next;
	}
	m_Mutex.unlock();
}

PropertiesManager::~PropertiesManager()
{
	LogManager::LogMessage(LL_INFO, "Stopping PropertiesManager...");
	
	InstanceEvent()->UnregisterEvent(EventIDOnModification);
	EventIDOnModification = EventManager::NullEventID;
	auto cur = gPropHead;
	while (cur)
	{
		if (!(cur->Property->m_Attributes & (1 << EPropertyAttributes::CONSTANT)))
		{
			auto prop = GetProperty(cur->Property->m_Name);
			if (prop)
			{
				cur->Property->m_Number = prop->GetNumberValue();
				cur->Property->m_NumberMax = prop->GetMaxNumberValue();
				cur->Property->m_NumberMin = prop->GetMinNumberValue();
				cur->Property->m_Value = prop->GetStringValue();
				cur->Property->m_ValidValues = prop->GetValidValueStrings();
			}
		}
		cur = cur->Next;
	}
#if GREAPER_DEBUG_ALLOCATION
	gProperties.clear();
#endif
}

IProperty* PropertiesManager::CreateProperty(const std::string & name, const bool constant, const bool value)
{
	auto hash = std::hash<std::string>{}(name);

	auto prop = GetProperty(name);
	if (prop)
	{
		prop->SetBoolValue(value);
		return prop;
	}

	m_Mutex.lock();
	gProperties.insert_or_assign(hash, Property(name, (uint8)(constant ? 0b00010001 : 0b00010000), value));
	m_Mutex.unlock();

	prop = GetProperty(name);

	LogManager::LogMessage(LL_INFO, "Boolean Property created, with properties: "
		"Name: %s, Constant: %s, Value: %s.", prop->GetName().c_str(), constant ? "true" : "false", prop->GetStringValue().c_str());
	return prop;
}

IProperty * PropertiesManager::CreateProperty(const std::string & name, const bool constant, const float value, const float min, const float max)
{
	const auto hash = std::hash<std::string>{}(name);

	auto prop = GetProperty(name);
	if (prop)
	{
		prop->SetNumberMinValue(min);
		prop->SetNumberMaxValue(max);
		prop->SetNumberValue(value);
		return prop;
	}

	m_Mutex.lock();
	gProperties.insert_or_assign(hash, Property(name, (uint8) (constant ? 0b00001001 : 0b00001000), value, min, max));
	m_Mutex.unlock();

	prop = GetProperty(name);

	LogManager::LogMessage(LL_INFO, "Number Property created, with properties: "
		"Name: %s, Constant: %s, Value: %f, MaxValue: %f, MinValue: %f.", prop->GetName().c_str(), constant ? "true" : "false", prop->GetNumberValue(),
		prop->GetMaxNumberValue(), prop->GetMinNumberValue());
	return prop;
}

IProperty * PropertiesManager::CreateProperty(const std::string & name, const bool constant, const std::string & value, const std::vector<std::string>& validValues)
{
	const auto hash = std::hash<std::string>{}(name);
	
	auto prop = GetProperty(name);
	if (prop)
	{
		prop->SetValidStringValues(validValues);
		prop->SetStringValue(value);
		return prop;
	}

	m_Mutex.lock();
	gProperties.insert_or_assign(hash, Property(name, (uint8)(constant ? 0b00000101 : 0b00000100), value, validValues));
	m_Mutex.unlock();
	
	prop = GetProperty(name);
	
	std::string tmp;
	const auto valid = prop->GetValidValueStrings();
	for (auto it = valid.begin(); it != valid.end(); ++it)
		tmp += " (" + *it + ")";

	LogManager::LogMessage(LL_INFO, "String Property created, with properties: "
		"Name: %s, Constant: %s, Value: %s, ValidStringValues: %s.", prop->GetName().c_str(), constant ? "true" : "false", prop->GetStringValue().c_str(),
		tmp.c_str());

	return prop;
}

void PropertiesManager::RemoveProperty(const std::string & name)
{
	m_Mutex.lock_shared();
	const auto it = gProperties.find(std::hash<std::string>{}(name));
	if (it != gProperties.end())
	{
		LogManager::LogMessage(LL_INFO, "Property with name: %s was deleted.", name.c_str());
		m_Mutex.unlock_shared();
		m_Mutex.lock();
		gProperties.erase(it);
		m_Mutex.unlock();
	}
	else
	{
		m_Mutex.unlock_shared();
		LogManager::LogMessage(LL_INFO, "Property with name: %s was not found to be deleted.", name.c_str());
	}
}

void PropertiesManager::RemoveProperty(IProperty * prop)
{
	Assertion::WhenNullptr(prop, "Trying to delete a property, but it was nullptr.");
	RemoveProperty(prop->GetName());
}

PropertiesManager & PropertiesManager::Instance()
{
	return *InstanceApp()->GetPropertiesManager();
}

PropertiesManager * PropertiesManager::InstancePtr()
{
	return InstanceApp()->GetPropertiesManager();
}

IProperty* PropertiesManager::GetProperty(const std::string& name)
{
	std::shared_lock<std::shared_mutex> lock(m_Mutex);
	auto it = gProperties.find(std::hash<std::string>{}(name));
	if (it != gProperties.end())
		return &it->second;
	return nullptr;
}
