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

#ifndef GAF_APPLICATION_H
#define GAF_APPLICATION_H 1

#include "GAF\GAFPrerequisites.h"
#include "GAF/TaskManager.h"
#include "GAF/Base/IModule.h"

namespace gaf
{
	class Application : public TaskManager
	{
		typedef
#if PLATFORM_WINDOWS
			HMODULE
#else
			void*
#endif
		ModuleHandle;
		const std::string m_Name;
		std::map<IModule*, ModuleHandle> m_RegisteredModules;
		using ModuleIterator = std::map<IModule*, ModuleHandle>::iterator;
		FileSystem* m_FileSystem;
		CommandSystem* m_CommandSystem;
		InputManager* m_InputManager;
		LogManager* m_LogManager;
		EventManager* m_EventManager;
		PropertiesManager* m_PropertiesManager;
		ResourceManager* m_ResourceManager;
		WindowManager* m_WindowManager;
		
		ModuleIterator FindModule(const std::string& moduleName);
		ModuleHandle FindModuleInDisk(const std::string& moduleName);
		bool UnloadModule(ModuleHandle handle);

		void StopModules();
		void StartSystems();
		void StopSystems();
	protected:
		static Application* m_Instance;
		static bool m_Started;
		Application() = default;
		Application(std::string appName);
		~Application();
		Application(Application&&)noexcept = default;
		Application& operator=(Application&&)noexcept = default;
	public:

		/* 
			Initialize function must send a valid string, which must
			not be empty, if it is will be replaced by the default
			name.
		*/
		static void Start(std::string appName = "DefaultApplication");
		/*
			Stops everything and releases all the resources in order to 
			use a module or a System you must start again the Application
		*/
		void Stop();
		
		/* Instance getter */
		static Application& Instance();
		static Application*const InstancePtr();

		/* 
			Loads a dynamic link library that implements the 
			Module class, which will initialize everything which that
			module needs and deinitialize when Unregistering.
				@module - Module object to register.
				@return - True if registering and initializing the module
				was successful otherwise false.
		*/
		bool RegisterModule(IModule* module);
		
		/*
			If the module was found, unloads the dinamic link library and
			releases the module.
				@moduleName - Name of the dynamic link library without the
				extension.
				@return - The module object in order to free it by yourself,
				if its nullptr, there was an error while trying to unload it.
		*/
		IModule* UnregisterModule(const std::string& moduleName);
		/*
			If the module was found, unloads the dinamic link library and
			releases the module.
				@module - Module object to unregister.
				@return - True if unregistering and deinitializing the module
				was successful otherwise false.
		*/
		bool UnregisterModule(IModule* module);

		bool IsModuleRegistered(IModule* module);
		bool IsModuleRegistered(const std::string& moduleName);

		/* Getters */
		const std::map<IModule*, ModuleHandle>& GetModules()const;
		const std::string& GetName()const;
		CommandSystem* GetCommandSystem()const;
		FileSystem* GetFileSystem()const;
		InputManager* GetInputManager()const;
		LogManager* GetLogManager()const;
		EventManager* GetEventManager()const;
		PropertiesManager* GetPropertiesManager()const;
		ResourceManager* GetResourceManager()const;
		WindowManager* GetWindowManager()const;
	};
	static Application* InstanceApp() { return Application::InstancePtr(); }
}

#endif /* GAF_APPLICATION_H */