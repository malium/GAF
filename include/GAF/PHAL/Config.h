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

#ifndef GREAPER_DEBUG_ASSERTIONS
#if GREAPER_DEBUG
#define GREAPER_DEBUG_ASSERTIONS 1
#else
#define GREAPER_DEBUG_ASSERTIONS 0
#endif
#endif

#ifndef GREAPER_DEBUG_ALLOCATION
#if GREAPER_DEBUG
#define GREAPER_DEBUG_ALLOCATION 1
#else
#define GREAPER_DEBUG_ALLOCATION 0
#endif
#endif

#if !GREAPER_ALLOCATION_DEBUG
#define GREAPER_MEMORY_DEBUG_BEGIN()
#define GREAPER_MEMORY_DEBUG_END()
#endif

#ifndef GREAPER_DEBUG_COMMANDS
#if GREAPER_DEBUG
#define GREAPER_DEBUG_COMMANDS 1
#else
#define GREAPER_DEBUG_COMMANDS 0
#endif
#endif

#ifndef GREAPER_DEBUG_EVENTS
#if GREAPER_DEBUG
#define GREAPER_DEBUG_EVENTS 1
#else
#define GREAPER_DEBUG_EVENTS 0
#endif
#endif

#ifndef GREAPER_DEBUG_FILESYS
#if GREAPER_DEBUG
#define GREAPER_DEBUG_FILESYS 1
#else
#define GREAPER_DEBUG_FILESYS 0
#endif
#endif

#ifndef GREAPER_DEBUG_INPUT
#if GREAPER_DEBUG
#define GREAPER_DEBUG_INPUT 1
#else
#define GREAPER_DEBUG_INPUT 0
#endif
#endif

#ifndef GREAPER_DEBUG_PROPERTIES
#if GREAPER_DEBUG
#define GREAPER_DEBUG_PROPERTIES 1
#else
#define GREAPER_DEBUG_PROPERTIES 0
#endif
#endif

#ifndef GREAPER_DEBUG_RESOURCES
#if GREAPER_DEBUG
#define GREAPER_DEBUG_RESOURCES 1
#else
#define GREAPER_DEBUG_RESOURCES 0
#endif
#endif

#ifndef GREAPER_DEBUG_TASKMAN
#if GREAPER_DEBUG
#define GREAPER_DEBUG_TASKMAN 1
#else
#define GREAPER_DEBUG_TASKMAN 0
#endif
#endif

#ifndef GREAPER_DEBUG_WINDOW
#if GREAPER_DEBUG
#define GREAPER_DEBUG_WINDOW 1
#else
#define GREAPER_DEBUG_WINDOW 0
#endif
#endif