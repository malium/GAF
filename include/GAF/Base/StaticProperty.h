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

#ifndef GAF_STATIC_PROPERTY_H
#define GAF_STATIC_PROPERTY_H 1

namespace gaf
{
	class StaticProperty
	{
		std::string m_Name;
		std::string m_Value;
		float m_Number;
		float m_NumberMin;
		float m_NumberMax;
		std::vector<std::string> m_ValidValues;
		const std::underlying_type<EPropertyAttributes::Type>::type m_Attributes;
		std::function<void(IProperty*)> m_OnPropChange;
	public:
		StaticProperty() = delete;
		~StaticProperty() = default;
		StaticProperty(const StaticProperty&) = default;
		StaticProperty(StaticProperty&&)noexcept = default;
		StaticProperty& operator=(const StaticProperty&) = default;
		StaticProperty& operator=(StaticProperty&&)noexcept = default;
		StaticProperty(const std::string& name, bool constant, bool value, const std::function<void(IProperty*)>& onChange = {});
		StaticProperty(const std::string& name, bool constant, float value, float min, float max, const std::function<void(IProperty*)>& onChange = {});
		StaticProperty(const std::string& name, bool constant, const std::string& value, const std::vector<std::string>& validValues, const std::function<void(IProperty*)>& onChange = {});
		friend class PropertiesManager;
	};
}

#endif /* GAF_STATIC_PROPERTY_H */
