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

#include "PHAL/Platform.h"

/* Forward declarations */
namespace gaf
{
	class Application;
	typedef std::atomic_char32_t Atomic;
	typedef std::atomic_bool AtomicFlag;
	class BinarySerializer;
	class Command;
	class CommandSystem;
	class Console;
	class Crypto;
	class DayTime;
	class Directory;
	class IDisplayAdapter;
	class EventManager;
	class File;
	class FileSystem;
	class HWDetector;
	class HIDDevice;
	class InputManager;
	class InputSystem;
	class LogManager;
	class IMonitor;
	typedef std::pair<int32, int32> Position_t;
	class IProperty;
	class PropertyExporter;
	class PropertyImporter;
	class PropertiesManager;
	typedef std::pair<uint16, uint16> Resolution_t;
	class ResourceExporter;
	class ResourceData;
	class ResourceImporter;
	class ResourceLocation;
	class ResourceManager;
	class TaskDispatcher;
	class TaskHandler;
	class TaskManager;
	class Version;
	class IVideoMode;
	class Window;
	class WindowManager;
}

#include "GAF/Assertion.h"
#include "GAF/Test.h"
#include "GAF/MainHelper.h"
