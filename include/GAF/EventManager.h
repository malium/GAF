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

#ifndef GAF_EVENTMANAGER_H
#define GAF_EVENTMANAGER_H 1

#include "GAF/GAFPrerequisites.h"

namespace gaf
{
	typedef uint32 EventID;
	typedef uint32 EventListenerID;

	class EventManager
	{
		static constexpr EventID AllEventsID = static_cast<EventID>(-2);
		EventManager();
		~EventManager();
		std::thread m_Thread;
		void EventTask(EventID event, void* params);

		struct EventListener
		{
			std::function<void(EventID, void*)> ListeningFunction;
			std::vector<EventID> ListeneningEvents; /* If its subscribed to all events there will an EventID which will be AllEventsID */
			std::shared_mutex Mutex;
			EventListener(const EventListener& other)
				:ListeningFunction(other.ListeningFunction)
				,ListeneningEvents(other.ListeneningEvents)
			{

			}
			EventListener& operator=(const EventListener& other)
			{
				if (this != &other)
				{
					ListeningFunction = other.ListeningFunction;
					ListeneningEvents = other.ListeneningEvents;
				}
				return *this;
			}
			EventListener()
			{

			}
			EventListener(const std::function<void(EventID, void*)>& fn, const std::vector<EventID>& eventVec)
				:ListeningFunction(fn)
				, ListeneningEvents(eventVec)
			{

			}
		};
		
		std::atomic<uint32> m_NextEventID;
		std::atomic<uint32> m_NextListenerID;
		
		std::map<size_t, EventListener> m_RegisteredListeners;
		std::shared_mutex m_ListenersLock;

		std::map<size_t, std::string> m_RegisteredEvents;
		std::shared_mutex m_EventsLock;
	public:
		static constexpr auto NullEventIDStr = "NullEvent";
		static constexpr EventID NullEventID = static_cast<EventID>(-1);
		static constexpr EventListenerID NullEventListenerID = static_cast<EventListenerID>(-1);

		EventID RegisterEvent(const std::string& eventName);
		void UnregisterEvent(EventID event);
		void UnregisterEvent(const std::string& eventName);
		std::string GetEventName(EventID evt);
		EventID GetEventIDFromName(const std::string& eventName);

		void AddEventToListener(EventListenerID listener, EventID event);
		void AddEventToListener(EventListenerID listener, const std::vector<EventID>& events);
		void RemoveEventFromListener(EventListenerID listener, EventID event);
		void RemoveEventFromListener(EventListenerID listener, const std::vector<EventID>& events);

		EventListenerID RegisterEventListener(const std::function<void(EventID, void*)>& evtHandling, EventID event);
		EventListenerID RegisterEventListener(const std::function<void(EventID, void*)>& evtHandling, const std::vector<EventID>& events);
		EventListenerID RegisterEventListener(const std::function<void(EventID, void*)>& evtHandling);
		void UnregisterEventListener(EventListenerID listener);
		
		void DispatchEvent(EventID event, void* params);

		static EventManager& Instance();
		static EventManager* InstancePtr();

		friend class Application;
	};

	static EventManager* InstanceEvent() { return EventManager::InstancePtr(); }
}

#endif /* GAF_EVENTMANAGER_H */