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

#include "GAF/PropertiesManager.h"

namespace gaf
{
	class Property : public IProperty
	{
	public:
		static constexpr auto NumberTrue = 1.f;
		static constexpr auto NumberFalse = 0.f;
	private:
		std::string m_Name;
		std::string m_Value;
		float m_Number;
		float m_NumberMin;
		float m_NumberMax;
		std::vector<std::string> m_ValidValues;
		const uint8 m_Attributes;
		std::shared_mutex m_Mutex;
		bool m_OnModificationEventEnabled;

		bool IsValidString(const std::string& value);
	public:
		Property(const std::string& name, uint8 attrib, bool value);
		Property(const std::string& name, uint8 attrib, float value, float min, float max);
		Property(const std::string& name, uint8 attrib, const std::string& value, const std::vector<std::string>& validValues);
		Property(const Property& other)
			:m_Name(other.m_Name)
			, m_Value(other.m_Value)
			, m_Number(other.m_Number)
			, m_NumberMin(other.m_NumberMin)
			, m_NumberMax(other.m_NumberMax)
			, m_ValidValues(other.m_ValidValues)
			, m_Attributes(other.m_Attributes)
			, m_OnModificationEventEnabled(other.m_OnModificationEventEnabled)
		{

		}
		Property& operator=(const Property& other)
		{
			if (this != &other && m_Attributes == other.m_Attributes)
			{
				m_Name = other.m_Name;
				m_Value = other.m_Value;
				m_Number = other.m_Number;
				m_NumberMin = other.m_NumberMin;
				m_NumberMax = other.m_NumberMax;
				m_ValidValues = other.m_ValidValues;
			}
			return *this;
		}
		Property(Property&&)noexcept = default;
		Property& operator=(Property&&)noexcept = default;
		~Property() = default;
		bool GetBoolValue();
		float GetNumberValue();
		const std::string& GetStringValue();
		const std::vector<std::string>& GetValidValueStrings();

		void SetBoolValue(bool value);
		void SetNumberValue(float value);
		void SetStringValue(const std::string& value);

		void SetNumberMaxValue(float value);
		void SetNumberMinValue(float value);
		float GetMaxNumberValue();
		float GetMinNumberValue();

		void SetValidStringValues(const std::vector<std::string>& validValues);
		void AddValidStringValue(const std::string& value);
		void RemoveValidStringValue(const std::string& value);
		void ClearValidStringValues();

		const std::string& GetName()const;
		bool IsConstant()const;
		bool IsMutable()const;
		bool IsStatic()const;
		bool IsString()const;
		bool IsNumber()const;
		bool IsBoolean()const;

		void EnableOnModificationEvent(bool enable);
		bool IsOnModificationEventEnabled();

		friend bool operator==(const Property& a, const Property& b)
		{
			return a.GetName() == b.GetName();
		}
		friend bool operator!=(const Property& a, const Property& b)
		{
			return !(a == b);
		}
		friend class PropertiesManager;
	};
}