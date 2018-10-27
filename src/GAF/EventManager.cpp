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
#include "GAF/EventManager.h"
#include "GAF/LogManager.h"

using namespace gaf;

EventManager::EventManager()
	:m_NextEventID(0)
	,m_NextListenerID(0)
{
	LogManager::LogMessage(LL_INFO, "Starting EventManager...");
}

EventManager::~EventManager()
{
	LogManager::LogMessage(LL_INFO, "Stopping EventManager...");
}

void EventManager::EventTask(const EventID event, void * params)
{
	if (event == EventManager::NullEventID)
		return;
	m_ListenersLock.lock_shared();
	for (auto it = m_RegisteredListeners.begin(); it != m_RegisteredListeners.end(); ++it)
	{
		it->second.Mutex.lock_shared();
		if (it->second.ListeneningEvents.empty())
		{
			it->second.Mutex.unlock_shared();
			continue;
		}
		if (it->second.ListeneningEvents[0] == AllEventsID)
		{
			it->second.ListeningFunction(event, params);
		}
		else
		{
			for (auto itt = it->second.ListeneningEvents.begin();
				itt != it->second.ListeneningEvents.end(); ++itt)
			{
				if ((*itt) == event)
				{
					it->second.ListeningFunction(event, params);
				}
			}
		}
		it->second.Mutex.unlock_shared();
	}
	m_ListenersLock.unlock_shared();
}

EventID EventManager::RegisterEvent(const std::string & eventName)
{
	auto id = GetEventIDFromName(eventName);
	if (id != NullEventID)
	{
		LogManager::LogMessage(LL_WARN, "Trying to register an already registerd"
			" event:%s.", eventName.c_str());
		return id;
	}
	id = m_NextEventID.load(std::memory_order_acquire);
	m_EventsLock.lock();
	m_RegisteredEvents.emplace(id, eventName);
	m_EventsLock.unlock();
	++m_NextEventID;
	return id;
}

void EventManager::UnregisterEvent(const EventID event)
{
	m_EventsLock.lock_shared();
	const auto it = m_RegisteredEvents.find(event);
	m_EventsLock.unlock_shared();
	if (it == m_RegisteredEvents.end())
	{
		LogManager::LogMessage(LL_WARN, "Trying to unregister a non-existant"
			" event:%d.", event);
	}
	else
	{
		m_EventsLock.lock();
		m_RegisteredEvents.erase(it);
		m_EventsLock.unlock();
	}
}

void EventManager::UnregisterEvent(const std::string & eventName)
{
	UnregisterEvent(GetEventIDFromName(eventName));
}

std::string EventManager::GetEventName(const EventID evt)
{
	m_EventsLock.lock_shared();
	const auto it = m_RegisteredEvents.find(evt);
	if (it == m_RegisteredEvents.end())
	{
		m_EventsLock.unlock_shared();
		return NullEventIDStr;
	}
	const auto ret = it->second;
	m_EventsLock.unlock_shared();
	return ret;
}

EventID EventManager::GetEventIDFromName(const std::string & eventName)
{
	if (eventName.empty())
		return NullEventID;
	m_EventsLock.lock_shared();
	for (auto it = m_RegisteredEvents.begin(); it != m_RegisteredEvents.end(); ++it)
	{
		if (it->second == eventName)
		{
			const auto ret = (EventID)it->first;
			m_EventsLock.unlock_shared();
			return ret;
		}
	}
	m_EventsLock.unlock_shared();
	return NullEventID;
}

void EventManager::AddEventToListener(const EventListenerID listener, const EventID event)
{
	m_ListenersLock.lock_shared();
	const auto it = m_RegisteredListeners.find(listener);
	
	if (it == m_RegisteredListeners.end())
	{
		m_ListenersLock.unlock_shared();
		LogManager::LogMessage(LL_WARN, "Trying to add an event:%d to an non-registered"
			" EventListener:%d.", event, listener);
		return;
	}
	else
	{
		it->second.Mutex.lock();
		it->second.ListeneningEvents.emplace_back(event);
		it->second.Mutex.unlock();
	}
	m_ListenersLock.unlock_shared();
}

void EventManager::AddEventToListener(const EventListenerID listener, const std::vector<EventID>& events)
{
	m_ListenersLock.lock_shared();
	const auto it = m_RegisteredListeners.find(listener);
	

	if (it == m_RegisteredListeners.end())
	{
		m_ListenersLock.unlock_shared();
		LogManager::LogMessage(LL_WARN, "Trying to add a series of events to an non-registered"
			" EventListener:%d.", listener);
		return;
	}
	else
	{
		it->second.Mutex.lock();
		for (auto itt = events.begin(); itt != events.end(); ++itt)
			it->second.ListeneningEvents.emplace_back(*itt);
		it->second.Mutex.unlock();
	}
	m_ListenersLock.unlock_shared();
}

void EventManager::RemoveEventFromListener(const EventListenerID listener, const EventID event)
{
	if (listener == NullEventListenerID)
	{
		LogManager::LogMessage(LL_WARN, "Trying to remove an Event from a Listener but the ListenerID was null.");
		return;
	}
	const auto hash = listener;
	m_ListenersLock.lock_shared();
	const auto it = m_RegisteredListeners.find(hash);
	if (it == m_RegisteredListeners.end())
	{
		m_ListenersLock.unlock_shared();
		LogManager::LogMessage(LL_WARN, "Trying to remove an Event from a non-registered EventListener, eventID: %d, listenerID: %d.",
			event, listener);
		return;
	}
	m_ListenersLock.unlock_shared();
	if (event == AllEventsID)
	{
		it->second.Mutex.lock();
		it->second.ListeneningEvents.clear();
		it->second.Mutex.unlock();
		return;
	}
	it->second.Mutex.lock_shared();
	if (it->second.ListeneningEvents.empty())
	{
		it->second.Mutex.unlock_shared();
		LogManager::LogMessage(LL_WARN, "Trying to remove an Event: %d, from a EventListener: %d, but that EventListener doesn't contains that event.",
			event, listener);
		return;
	}
	if (it->second.ListeneningEvents[0] == AllEventsID)
	{
		it->second.Mutex.unlock_shared();
		LogManager::LogMessage(LL_WARN, "Trying to remove an Event: %d, from a EventListener: %d, but that EventListener allows any event to be received, and"
			" removing just one event is not currently supported, erase the EventListener and add another one without that event.",
			event, listener);
		return;
	}
	const auto evtIt = std::find(it->second.ListeneningEvents.begin(), it->second.ListeneningEvents.end(), event);
	if (evtIt == it->second.ListeneningEvents.end())
	{
		it->second.Mutex.unlock_shared();
		LogManager::LogMessage(LL_WARN, "Trying to remove an Event: %d, from a EventListener: %d, but that EventListener doesn't contains that event.",
			event, listener);
	}
	else
	{
		it->second.Mutex.unlock_shared();
		it->second.Mutex.lock();
		it->second.ListeneningEvents.erase(evtIt);
		it->second.Mutex.unlock();
	}
}

void EventManager::RemoveEventFromListener(const EventListenerID listener, const std::vector<EventID>& events)
{
	if (events.empty())
	{
		LogManager::LogMessage(LL_WARN, "Trying to remove a series of events from an EventListener, but that series was empty.");
		return;
	}
	if (listener == NullEventListenerID)
	{
		LogManager::LogMessage(LL_WARN, "Trying to remove a series of events from an EventListener, but the listener was null.");
		return;
	}
	m_ListenersLock.lock_shared();
	if (m_RegisteredListeners.find(listener) == m_RegisteredListeners.end())
	{
		m_ListenersLock.unlock_shared();
		LogManager::LogMessage(LL_WARN, "Trying to remove a series of events from an EventListener, but the listener was not registered.");
		return;
	}
	m_ListenersLock.unlock_shared();
	for (auto it = events.begin(); it != events.end(); ++it)
		RemoveEventFromListener(listener, *it);
}

EventListenerID EventManager::RegisterEventListener(const std::function<void(EventID, void*)>& evtHandling, const EventID event)
{
	EventListener listener;
	listener.ListeningFunction = evtHandling;
	listener.ListeneningEvents.emplace_back(event);
	const auto id = m_NextListenerID.load(std::memory_order_acquire);
	++m_NextListenerID;

	m_ListenersLock.lock();
	m_RegisteredListeners.insert_or_assign(id, listener);
	m_ListenersLock.unlock();

	return id;
}

EventListenerID EventManager::RegisterEventListener(const std::function<void(EventID, void*)>& evtHandling, const std::vector<EventID>& events)
{
	const auto id = m_NextListenerID.load(std::memory_order_acquire);
	++m_NextListenerID;

	m_ListenersLock.lock();
	m_RegisteredListeners.insert_or_assign(id, EventListener(evtHandling, events));
	m_ListenersLock.unlock();

	return id;
}

EventListenerID EventManager::RegisterEventListener(const std::function<void(EventID, void*)>& evtHandling)
{
	return RegisterEventListener(evtHandling, AllEventsID);
}

void EventManager::UnregisterEventListener(const EventListenerID listener)
{
	m_ListenersLock.lock_shared();
	const auto it = m_RegisteredListeners.find(listener);
	
	if (it == m_RegisteredListeners.end())
	{
		m_ListenersLock.unlock_shared();
		LogManager::LogMessage(LL_WARN, "Trying to unregister a non-registered listener:%d.", listener);
	}
	else
	{
		m_ListenersLock.unlock_shared();
		m_ListenersLock.lock();
		m_RegisteredListeners.erase(it);
		m_ListenersLock.unlock();
	}
}

CreateTaskName(EventDispatchTask);

void EventManager::DispatchEvent(const EventID event, void * params)
{
	InstanceApp()->SendTask(CreateTask(EventDispatchTask, std::bind(&EventManager::EventTask, this, event, params)));
}

EventManager & EventManager::Instance()
{
	return *InstanceApp()->GetEventManager();
}

EventManager* EventManager::InstancePtr()
{
	return InstanceApp()->GetEventManager();
}
