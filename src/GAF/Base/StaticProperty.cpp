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

#include "GAF/PropertiesManager.h"
#include "..\Property.h"

using namespace gaf;

StaticProperty::StaticProperty(const std::string & name, const bool constant, const bool value, const std::function<void(IProperty*)>& propChange)
	:m_Name(name)
	, m_Value(value ? "true" : "false")
	, m_Number(value ? Property::NumberTrue : Property::NumberFalse)
	, m_NumberMin(1.f)
	, m_NumberMax(-1.f)
	, m_Attributes(constant ? 0b00010011 : 0b00010010)
	, m_OnPropChange(propChange)
{
	PropertiesManager::RegisterStaticProperty(*this);
}

StaticProperty::StaticProperty(const std::string & name, const bool constant, const float value, const float min, const float max, const std::function<void(IProperty*)>& propChange)
	:m_Name(name)
	, m_NumberMin(min)
	, m_NumberMax(max)
	, m_Attributes(constant ? 0b00001011 : 0b00001010)
	, m_OnPropChange(propChange)
{
	if (m_NumberMax >= m_NumberMin)
		m_Number = Clamp(value, m_NumberMin, m_NumberMax);
	else
		m_Number = value;
	m_Value = std::to_string(m_Number);
	PropertiesManager::RegisterStaticProperty(*this);
}

StaticProperty::StaticProperty(const std::string & name, const bool constant, const std::string & value, const std::vector<std::string>& validValues, const std::function<void(IProperty*)>& propChange)
	:m_Name(name)
	, m_Value(value)
	, m_Number(0.f)
	, m_NumberMin(1.f)
	, m_NumberMax(-1.f)
	, m_ValidValues(validValues)
	, m_Attributes(constant ? 0b00000111 : 0b00000110)
	, m_OnPropChange(propChange)
{
	PropertiesManager::RegisterStaticProperty(*this);
}