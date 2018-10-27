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

#include "GAF/LogManager.h"
#include "..\Property.h"

using namespace gaf;

Property::Property(const std::string& name, const uint8 attrib, const bool value)
	:m_Name(name)
	, m_Value(value ? "true" : "false")
	, m_Number(value ? NumberTrue : NumberFalse)
	, m_NumberMin(1.f)
	, m_NumberMax(-1.f)
	, m_Attributes(attrib)
	, m_OnModificationEventEnabled(false)
{

}

Property::Property(const std::string& name, const uint8 attrib, const float value, const float min, const float max)
	:m_Name(name)
	, m_NumberMin(min)
	, m_NumberMax(max)
	, m_Attributes(attrib)
	, m_OnModificationEventEnabled(false)
{
	m_Mutex.lock();
	if (m_NumberMax < m_NumberMin)
		m_Number = value;
	else
		m_Number = Clamp(value, m_NumberMin, m_NumberMax);
	m_Value = std::to_string(m_Number);
	m_Mutex.unlock();
}

Property::Property(const std::string& name, const uint8 attrib, const std::string& value, const std::vector<std::string>& validValues)
	:m_Name(name)
	, m_Number(0.f)
	, m_NumberMin(1.f)
	, m_NumberMax(-1.f)
	, m_ValidValues(validValues)
	, m_Attributes(attrib)
	, m_OnModificationEventEnabled(false)
{
	
	if (IsValidString(value))
	{
		m_Mutex.lock();
		m_Value = value;
		m_Mutex.unlock();
	}
	
}

bool Property::IsValidString(const std::string & value)
{
	std::shared_lock<std::shared_mutex> lock(m_Mutex);
	if (m_ValidValues.empty())
		return true;
	const auto it = std::find(m_ValidValues.begin(), m_ValidValues.end(), value);
	if (it == m_ValidValues.end())
		return false;
	return true;
}

bool Property::GetBoolValue()
{
	std::shared_lock<std::shared_mutex> lock(m_Mutex);
	return m_Number != NumberFalse;
}

float Property::GetNumberValue()
{
	std::shared_lock<std::shared_mutex> lock(m_Mutex);
	return m_Number;
}

const std::string & Property::GetStringValue()
{
	std::shared_lock<std::shared_mutex> lock(m_Mutex);
	return m_Value;
}

const std::vector<std::string>& Property::GetValidValueStrings()
{
	std::shared_lock<std::shared_mutex> lock(m_Mutex);
	return m_ValidValues;
}

void Property::SetBoolValue(const bool value)
{
	if (m_Attributes & (1 << EPropertyAttributes::CONSTANT))
	{
		LogManager::LogMessage(LL_WARN,
			"Property '%s' Trying to change a constant value.",
			m_Name.c_str());
	}
	else if (m_Attributes & (1 << EPropertyAttributes::BOOLEAN))
	{
		LogManager::LogMessage(LL_VERB, "Property '%s' "
			"changed from %s to %s", m_Name.c_str(), GetBoolValue() ? "true" : "false",
			value ? "true" : "false");
		m_Mutex.lock();
		m_Value = value ? "true" : "false";
		m_Number = value ? NumberTrue : NumberFalse;
		m_Mutex.unlock();
		if (this->IsOnModificationEventEnabled())
			InstanceEvent()->DispatchEvent(PropertiesManager::EventIDOnModification, this);
	}
	else
	{
		LogManager::LogMessage(LL_WARN,
			"Property '%s' Trying to set a boolean value which is not a boolean.", m_Name.c_str());
	}
}

void Property::SetNumberValue(const float value)
{
	if (m_Attributes & (1 << EPropertyAttributes::CONSTANT))
	{
		LogManager::LogMessage(LL_WARN,
			"Property '%s' Trying to change a constant value.",
			m_Name.c_str());
	}
	else if (m_Attributes & (1 << EPropertyAttributes::NUMBER))
	{
		m_Mutex.lock();
		const auto oldval = m_Number;
		if (m_NumberMax < m_NumberMin)
			m_Number = value;
		else
			m_Number = Clamp(value, m_NumberMin, m_NumberMax);

		m_Value = std::to_string(m_Number);
		LogManager::LogMessage(LL_VERB, "Property '%s' "
			"changed from %f to %f.", m_Name.c_str(), oldval, m_Number);
		m_Mutex.unlock();
		if (IsOnModificationEventEnabled())
			InstanceEvent()->DispatchEvent(PropertiesManager::EventIDOnModification, this);
	}
	else
	{
		LogManager::LogMessage(LL_WARN,
			"Property '%s' Trying to set a Number value which is not a Number.",
			m_Name.c_str());
	}
}

void Property::SetStringValue(const std::string & value)
{
	if (m_Attributes & (1 << EPropertyAttributes::CONSTANT))
	{
		LogManager::LogMessage(LL_WARN,
			"Property '%s' Trying to change a constant value.",
			m_Name.c_str());
	}
	else if (m_Attributes & (1 << EPropertyAttributes::STRING))
	{
		if (IsValidString(value))
		{
			m_Mutex.lock();
			LogManager::LogMessage(LL_VERB,
				"Property '%s' changed from '%s' to '%s'.",
				m_Name.c_str(), m_Value.c_str(), value.c_str());
			m_Value = value;
			m_Mutex.unlock();
			if (IsOnModificationEventEnabled())
				InstanceEvent()->DispatchEvent(PropertiesManager::EventIDOnModification, this);
		}
		else
		{
			LogManager::LogMessage(LL_INFO,
				"Property '%s' couldn't be changed because wasn't a valid string:%s.",
				m_Name.c_str(), value.c_str());
		}
	}
	else
	{
		LogManager::LogMessage(LL_WARN,
			"Property '%s' trying to set a String value which is not a String.",
			m_Name.c_str());
	}
}

void Property::SetNumberMaxValue(const float value)
{
	if (IsConstant())
	{
		LogManager::LogMessage(LL_WARN,
			"Property '%s' trying to change the MaxNumberValue of a constant property",
			m_Name.c_str());
		return;
	}

	m_Mutex.lock();
	LogManager::LogMessage(LL_VERB,
		"Property '%s' changing NumberMaxValue from %f to %f.",
		m_Name.c_str(), m_NumberMax, value);

	m_NumberMax = value;
	if (IsNumber())
	{
		if (m_NumberMax >= m_NumberMin)
		{
			m_Number = Clamp(m_Number, m_NumberMin, m_NumberMax);
			m_Value = std::to_string(m_Number);
		}
		m_Mutex.unlock();
		if (IsOnModificationEventEnabled())
			InstanceEvent()->DispatchEvent(PropertiesManager::EventIDOnModification, this);
	}
	else
		m_Mutex.unlock();
}

void Property::SetNumberMinValue(const float value)
{
	if (IsConstant())
	{
		LogManager::LogMessage(LL_WARN,
			"Property '%s' trying to change the MaxNumberValue of a constant property",
			m_Name.c_str());
		return;
	}

	m_Mutex.lock();
	LogManager::LogMessage(LL_VERB,
		"Property '%s' changing NumberMinValue from %f to %f.",
		m_Name.c_str(), m_NumberMin, value);

	m_NumberMin = value;
	if (IsNumber())
	{
		if (m_NumberMax >= m_NumberMin)
		{
			m_Number = Clamp(m_Number, m_NumberMin, m_NumberMax);
			m_Value = std::to_string(m_Number);
		}
		m_Mutex.unlock();
		if (IsOnModificationEventEnabled())
			InstanceEvent()->DispatchEvent(PropertiesManager::EventIDOnModification, this);
	}
	else
		m_Mutex.unlock();
}

float Property::GetMaxNumberValue()
{
	std::shared_lock<std::shared_mutex> lock(m_Mutex);
	return m_NumberMax;
}

float Property::GetMinNumberValue()
{
	std::shared_lock<std::shared_mutex> lock(m_Mutex);
	return m_NumberMin;
}

void Property::SetValidStringValues(const std::vector<std::string>& validValues)
{
	m_Mutex.lock();
	m_ValidValues = validValues;
	m_Mutex.unlock();
	SetStringValue(m_Value);
}

void Property::AddValidStringValue(const std::string & value)
{
	if (value.empty())
	{
		LogManager::LogMessage(LL_WARN,
			"Property '%s' trying to set a non-valid ValidStringValue.",
			m_Name.c_str());
		return;
	}
	m_Mutex.lock();
	m_ValidValues.emplace_back(value);
	m_Mutex.unlock();
}

void Property::RemoveValidStringValue(const std::string & value)
{
	if (value.empty())
	{
		LogManager::LogMessage(LL_WARN, "Property '%s' trying to remove a ValidValueString, but was empty.", m_Name.c_str());
		return;
	}
	m_Mutex.lock_shared();
	if (m_ValidValues.empty())
	{
		m_Mutex.unlock_shared();
		LogManager::LogMessage(LL_WARN, "Property '%s' trying to remove a ValidValueString, but this property doesn't use the ValidValueStrings.", m_Name.c_str());
		return;
	}
	const auto it = std::find(m_ValidValues.begin(), m_ValidValues.end(), value);
	if (it == m_ValidValues.end())
	{
		m_Mutex.unlock_shared();

		LogManager::LogMessage(LL_WARN, "Property '%s' trying to remove a ValidValueString, str: '%s', but was not found.",
			m_Name.c_str(), value.c_str());
		return;
	}
	m_Mutex.unlock_shared();
	m_Mutex.lock();
	m_ValidValues.erase(it);
	m_Mutex.unlock();
	m_Mutex.lock_shared();
	if (!m_ValidValues.empty())
	{
		m_Mutex.unlock_shared();
		const auto needsChange = IsValidString(m_Value);
		if (needsChange)
		{
			m_Mutex.lock();
			m_Value = m_ValidValues[0];
			m_Mutex.unlock();
		}
	}
	else
	{
		m_Mutex.unlock_shared();
	}
}

void Property::ClearValidStringValues()
{
	m_Mutex.lock();
	m_ValidValues.clear();
	m_Mutex.unlock();
	LogManager::LogMessage(LL_VERB,
		"Property '%s' has cleared its ValidValueStrings.", m_Name.c_str());
}

const std::string & Property::GetName() const
{
	return m_Name;
}

bool Property::IsConstant() const
{
	return m_Attributes & (1 << EPropertyAttributes::CONSTANT);
}

bool Property::IsMutable() const
{
	return !IsConstant();
}

bool Property::IsStatic() const
{
	return m_Attributes & (1 << EPropertyAttributes::STATIC);
}

bool Property::IsString() const
{
	return m_Attributes & (1 << EPropertyAttributes::STRING);
}

bool Property::IsNumber() const
{
	return m_Attributes & (1 << EPropertyAttributes::NUMBER);
}

bool Property::IsBoolean() const
{
	return m_Attributes & (1 << EPropertyAttributes::BOOLEAN);
}

void Property::EnableOnModificationEvent(const bool enable)
{
	m_Mutex.lock();
	m_OnModificationEventEnabled = enable;
	m_Mutex.unlock();
}

bool Property::IsOnModificationEventEnabled()
{
	std::shared_lock<std::shared_mutex> lock(m_Mutex);
	return m_OnModificationEventEnabled;
}