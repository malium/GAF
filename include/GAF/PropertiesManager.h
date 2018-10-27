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

#ifndef GAF_PROPERTIES_MANAGER_H
#define GAF_PROPERTIES_MANAGER_H 1

#include "GAF/GAFPrerequisites.h"
#include "GAF/EventManager.h"
#include "GAF/Base/IProperty.h"
#include "GAF/Base/StaticProperty.h"

namespace gaf
{
	class PropertiesManager
	{
		static void RegisterStaticProperty(StaticProperty prop);
		std::shared_mutex m_Mutex;
		PropertiesManager();
		~PropertiesManager();
	public:
		
		static constexpr auto OnModificationEventName = "OnPropertyModification";
		static EventID EventIDOnModification;

		IProperty* CreateProperty(const std::string& name, bool constant, bool value);
		IProperty* CreateProperty(const std::string& name, bool constant, float value, float min, float max);
		IProperty* CreateProperty(const std::string& name, bool constant, const std::string& value, const std::vector<std::string>& validValues);

		void RemoveProperty(const std::string& name);
		void RemoveProperty(IProperty* prop);

		static PropertiesManager& Instance();
		static PropertiesManager* InstancePtr();
		
		IProperty* GetProperty(const std::string& name);

		friend class Application;
		friend class StaticProperty;
	};

	static PropertiesManager* InstanceProp() { return PropertiesManager::InstancePtr(); }
}

#endif /* GAF_PROPERTIES_MANAGER_H */