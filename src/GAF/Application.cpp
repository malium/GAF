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
#include "GAF/LogManager.h"
#include "GAF/CommandSystem.h"
#include "GAF/FileSystem.h"
#include "GAF/InputManager.h"
#include "GAF/PropertiesManager.h"
#include "GAF/EventManager.h"
#include "GAF/ResourceManager.h"
#include "GAF/WindowManager.h"
#include "GAF/HWDetector.h"

using namespace gaf;

Application* Application::m_Instance = nullptr;
bool Application::m_Started = false;

Application::Application(std::string appName)
	:m_Name(std::move(appName))
	,m_FileSystem(nullptr)
	,m_CommandSystem(nullptr)
	,m_InputManager(nullptr)
	,m_LogManager(nullptr)
	,m_PropertiesManager(nullptr)
	,m_ResourceManager(nullptr)
	,m_WindowManager(nullptr)
{

}

Application::~Application()
{

}

void Application::Start(std::string appName)
{
	if (m_Started)
		return;
	if (appName.empty())
		appName.assign("DefaultApplication");
	m_Instance = new Application(appName);
	LogManager::LogMessage(LL_INFO, "Starting %s...", appName.c_str());
	m_Instance->StartSystems();
	m_Instance->m_PropertiesManager->CreateProperty("APPLICATION_NAME", true, appName, {});
	m_Started = true;
}

void Application::Stop()
{
	if (!m_Started)
		return;
	LogManager::LogMessage(LL_INFO, "Stopping Greaper Application Framework...");
	m_Instance->m_PropertiesManager->RemoveProperty("APPLICATION_NAME");
	m_Instance->StopSystems();
	m_Instance->StopModules();
	if (LogManager::m_DefaultLogFile)
	{
		LogManager::m_DefaultLogFile->Close();
		FileSystem::DeleteExternalFile(LogManager::m_DefaultLogFile);
	}
#if GREAPER_DEBUG_ALLOCATION
	InstanceLog()->m_Messages.clear();
	InstanceHW()->Stop();
#endif
	SAFE_DELETE(m_Instance);
	m_Instance = nullptr;
	m_Started = false;
}

Application::ModuleIterator Application::FindModule(const std::string& moduleName)
{
	for (auto it = m_RegisteredModules.begin(); it != m_RegisteredModules.end(); ++it)
		if (it->first->m_ModuleName == moduleName)
			return it;
	return m_RegisteredModules.end();
}

Application::ModuleHandle gaf::Application::FindModuleInDisk(const std::string & moduleName)
{
#if PLATFORM_WINDOWS
	return LoadLibraryA((moduleName + ".dll").c_str());	
#else
	return dlopen((moduleName + ".so").c_str(), RTLD_LAZY | RTLD_LOCAL);
#endif
}

bool Application::UnloadModule(ModuleHandle handle)
{
#if PLATFORM_WINDOWS
	return FreeLibrary(handle);
#else
	return dlclose(handle) == 0;
#endif
}

void gaf::Application::StopModules()
{
	for (auto it = m_RegisteredModules.begin(); it != m_RegisteredModules.end(); ++it)
		UnloadModule(it->second);
	m_RegisteredModules.clear();
}

void Application::StartSystems()
{
	LogManager::LogMessage(LL_INFO, "Starting Greaper Application Framework...");
	m_FileSystem = new FileSystem();
	m_LogManager = new LogManager();
	m_EventManager = new EventManager();
	m_PropertiesManager = new PropertiesManager();
	m_ResourceManager = new ResourceManager();
	m_CommandSystem = new CommandSystem();
	m_WindowManager = new WindowManager();
	m_InputManager = new InputManager();
}

void Application::StopSystems()
{
	SAFE_DELETE(m_InputManager);
	SAFE_DELETE(m_WindowManager);
	SAFE_DELETE(m_CommandSystem);
	SAFE_DELETE(m_ResourceManager);
	SAFE_DELETE(m_PropertiesManager);
	SAFE_DELETE(m_EventManager);
	SAFE_DELETE(m_LogManager);
	SAFE_DELETE(m_FileSystem);
}

Application& Application::Instance()
{
	return *m_Instance;
}

Application*const Application::InstancePtr()
{
	return m_Instance;
}

bool Application::RegisterModule(IModule* module)
{
	if (!module)
	{
		m_LogManager->LogMessage(LL_WARN, "Trying to register a nullptr module.");
		return false;
	}
	const auto& name = module->m_ModuleName;
	const auto handle = FindModuleInDisk(name);
	if (!handle)
	{
		m_LogManager->LogMessage(LL_ERRO, "Couldn't register module, unable to "
			"find it on the disk or load from it, moduleName: '%s'.", name.c_str());
		return false;
	}
#if GREAPER_DEBUG // Debug checks
	if (FindModule(name) != m_RegisteredModules.end())
	{
		m_LogManager->LogMessage(LL_WARN, "Trying to register an already registered "
			"module, moduleName: '%s'.", name.c_str());
		return false;
	}
#endif
	m_RegisteredModules.emplace(module, handle);
	module->Start();
	m_LogManager->LogMessage(LL_INFO, "Module successfully registered, moduleName: '%s'.",
		name.c_str());
	return true;
}

IModule* Application::UnregisterModule(const std::string& moduleName)
{
	if (moduleName.empty())
	{
		m_LogManager->LogMessage(LL_WARN, "Trying to unregister an unnamed module.");
		return nullptr;
	}
	const auto it = FindModule(moduleName);
	if (it == m_RegisteredModules.end())
	{
		m_LogManager->LogMessage(LL_ERRO, "Couldn't find the module in the registered "
			"ones, moduleName: '%s'.", moduleName.c_str());
		return nullptr;
	}
	it->first->Stop();
	if (!UnloadModule(it->second))
	{
		m_LogManager->LogMessage(LL_ERRO, "Couldn't unload the module, moduleName: '%s'.",
			moduleName.c_str());
	}
	IModule* rtn = it->first;
	m_RegisteredModules.erase(it);
	return rtn;
}

bool Application::UnregisterModule(IModule* module)
{
	if (!module)
	{
		m_LogManager->LogMessage(LL_WARN, "Trying to unregister a nullptr module.");
		return false;
	}
	const auto it = FindModule(module->GetName());
	if (it == m_RegisteredModules.end())
	{
		m_LogManager->LogMessage(LL_ERRO,  "Couldn't find the module in the registered "
			"ones, moduleName: '%s'.", module->m_ModuleName.c_str());
		return false;
	}
	it->first->Stop();
	if (!UnloadModule(it->second))
	{
		m_LogManager->LogMessage(LL_ERRO, "Couldn't unload the module, moduleName: '%s'.",
			module->m_ModuleName.c_str());
	}
	m_RegisteredModules.erase(it);
	return true;
}

bool Application::IsModuleRegistered(IModule * module)
{
	return IsModuleRegistered(module->GetName());
}

bool Application::IsModuleRegistered(const std::string & moduleName)
{
	const auto it = FindModule(moduleName);
	if (it == m_RegisteredModules.end())
		return false;
	return true;
}

const std::map<IModule*, Application::ModuleHandle>& Application::GetModules() const
{
	return m_RegisteredModules;
}

const std::string& Application::GetName()const
{
	return m_Name;
}

CommandSystem* Application::GetCommandSystem()const
{
	return m_CommandSystem;
}

FileSystem* Application::GetFileSystem()const
{
	return m_FileSystem;
}

InputManager* Application::GetInputManager()const
{
	return m_InputManager;
}

LogManager* Application::GetLogManager()const
{
	return m_LogManager;
}

EventManager* Application::GetEventManager()const
{
	return m_EventManager;
}

PropertiesManager* Application::GetPropertiesManager()const
{
	return m_PropertiesManager;
}

ResourceManager * Application::GetResourceManager() const
{
	return m_ResourceManager;
}

WindowManager* Application::GetWindowManager()const
{
	return m_WindowManager;
}