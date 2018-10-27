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

#ifndef GAF_I_PROPERTY_H
#define GAF_I_PROPERTY_H 1

namespace gaf
{
	namespace EPropertyAttributes
	{
		enum Type : uint8
		{
			CONSTANT,
			STATIC,
			STRING,
			NUMBER,
			BOOLEAN
		};
	};

	class IProperty
	{
	public:
		virtual ~IProperty() = default;
		virtual bool GetBoolValue() = 0;
		virtual float GetNumberValue() = 0;
		virtual const std::string& GetStringValue() = 0;
		virtual const std::vector<std::string>& GetValidValueStrings() = 0;

		virtual void SetBoolValue(bool value) = 0;
		virtual void SetNumberValue(float value) = 0;
		virtual void SetStringValue(const std::string& value) = 0;

		virtual void SetNumberMaxValue(float value) = 0;
		virtual void SetNumberMinValue(float value) = 0;
		virtual float GetMaxNumberValue() = 0;
		virtual float GetMinNumberValue() = 0;

		virtual void SetValidStringValues(const std::vector<std::string>& validValues) = 0;
		virtual void AddValidStringValue(const std::string& value) = 0;
		virtual void RemoveValidStringValue(const std::string& value) = 0;
		virtual void ClearValidStringValues() = 0;

		virtual const std::string& GetName()const = 0;
		virtual bool IsConstant()const = 0;
		virtual bool IsMutable()const = 0;
		virtual bool IsStatic()const = 0;
		virtual bool IsString()const = 0;
		virtual bool IsNumber()const = 0;
		virtual bool IsBoolean()const = 0;

		virtual void EnableOnModificationEvent(bool enable) = 0;
		virtual bool IsOnModificationEventEnabled() = 0;
	};
}

#endif /* GAF_I_PROPERTY_H */
