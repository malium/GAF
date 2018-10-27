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
#include "GAF/EventManager.h"
#include "GAF/ResourceManager.h"

using namespace gaf;

ResourceType ResourceManager::InvalidResourceType = GetResourceTypeFromName("InvalidResourceType");
ResourceDataID ResourceManager::InvalidResourceDataID = GetResourceTypeFromName("InvalidResouceDataID");

EventID gaf::ResourceData::EventIDOnDataChange = EventManager::NullEventID;
EventID gaf::ResourceData::EventIDOnDataDestroying = EventManager::NullEventID;
EventID gaf::ResourceData::EventIDOnDataFinishedLoading = EventManager::NullEventID;

static std::map<ResourceType, std::string> gResourceTypeMap;
static std::shared_mutex gResourceTypeMapMutex;

ResourceType gaf::GetResourceTypeFromName(const std::string& name)
{
	const auto id = std::hash<std::string>{}(name);
	gResourceTypeMapMutex.lock_shared();
	if (gResourceTypeMap.empty())
	{
		gResourceTypeMapMutex.unlock_shared();
		return id;
	}
	const auto it = gResourceTypeMap.find(id);
	if (it == gResourceTypeMap.end())
	{
		gResourceTypeMapMutex.unlock_shared();
		gResourceTypeMapMutex.lock();
		gResourceTypeMap.insert_or_assign(id, std::move(name));
		gResourceTypeMapMutex.unlock();
	}
	else
	{
		gResourceTypeMapMutex.unlock_shared();
	}
	return id;
}

const std::string& gaf::GetResourceTypeNameFromID(const ResourceType type)
{
	gResourceTypeMapMutex.lock_shared();
	const auto it = gResourceTypeMap.find(type);
	if (it == gResourceTypeMap.end())
	{
		gResourceTypeMapMutex.unlock_shared();
		return it->second;
	}
	gResourceTypeMapMutex.unlock_shared();
	return gResourceTypeMap[ResourceManager::InvalidResourceType];
}

ResourceDataID gaf::GetResourceDataIDFromName(const std::string& name)
{
	return std::hash<std::string>{}(name);
}

/****************************************************************
*						RESOURCE MANAGER						*
****************************************************************/

ResourceManager::ResourceManager()
{
	LogManager::LogMessage(LL_INFO, "Starting ResourceManager...");
	const auto eventMgr = InstanceEvent();
	ResourceData::EventIDOnDataChange = eventMgr->RegisterEvent(ResourceData::OnDataChangeEvent);
	ResourceData::EventIDOnDataDestroying = eventMgr->RegisterEvent(ResourceData::OnDataDestroying);
	ResourceData::EventIDOnDataFinishedLoading = eventMgr->RegisterEvent(ResourceData::OnDataFinishedLoading);
}

ResourceManager::~ResourceManager()
{
	LogManager::LogMessage(LL_INFO, "Stopping ResourceManager...");
	const auto eventMgr = InstanceEvent();
	eventMgr->UnregisterEvent(ResourceData::EventIDOnDataChange);
	eventMgr->UnregisterEvent(ResourceData::EventIDOnDataDestroying);
	eventMgr->UnregisterEvent(ResourceData::EventIDOnDataFinishedLoading);

	ResourceData::EventIDOnDataChange = EventManager::NullEventID;
	ResourceData::EventIDOnDataDestroying = EventManager::NullEventID;
	ResourceData::EventIDOnDataFinishedLoading = EventManager::NullEventID;
}

ResourceDataID ResourceManager::CreateResourceData(const std::string & dataName, const ResourceType type, ResourceLocation * location,
	const EResourceDataLifetime::Type lifetime)
{
	if (dataName.empty())
	{
		LogManager::LogMessage(LL_ERRO, "Trying to create a ResourceData, but its unique name was invalid, name: %s.", dataName.c_str());
		return ResourceManager::InvalidResourceDataID;
	}
	if (type == ResourceManager::InvalidResourceType)
	{
		LogManager::LogMessage(LL_ERRO, "Trying to create a ResourceData, but the given ResourceType was Invalid, type: %lld.", type);
		return ResourceManager::InvalidResourceDataID;
	}
	const auto id = GetResourceDataIDFromName(dataName);
	if (id == ResourceManager::InvalidResourceDataID)
	{
		LogManager::LogMessage(LL_ERRO, "Trying to create a ResourceData, but its unique name was invalid, name: %s.", dataName.c_str());
		return ResourceManager::InvalidResourceDataID;
	}
	m_ResourceDataMutex.lock_shared();
	const auto it = m_ResourceData.find(id);
	if (it != m_ResourceData.end())
	{
		m_ResourceDataMutex.unlock_shared();
		return it->second.GetID();
	}
	m_ResourceDataMutex.unlock_shared();
	
	m_ResourceDataMutex.lock();
	m_ResourceData.insert_or_assign(id, ResourceData{ dataName, lifetime, type });
	m_ResourceDataMutex.unlock();

	LogManager::LogMessage(LL_INFO, "Added ResourceData with name: %s, id: %lld, type: %s.", dataName.c_str(), id, GetResourceTypeNameFromID(type).c_str());
	return id;
}

ResourceDataID ResourceManager::CreateResourceData(const std::string & dataName, const std::string & resourceType, ResourceLocation * location, 
	const EResourceDataLifetime::Type lifetime)
{
	return CreateResourceData(dataName, GetResourceTypeFromName(resourceType), location, lifetime);
}

ResourceData * ResourceManager::GetResourceData(const ResourceDataID id)
{
	m_ResourceDataMutex.lock_shared();
	const auto it = m_ResourceData.find(id);
	if (it != m_ResourceData.end())
	{
		m_ResourceDataMutex.unlock_shared();
		return &it->second;
	}
	m_ResourceDataMutex.unlock();
	return nullptr;
}

ResourceData * ResourceManager::GetResourceData(const std::string & dataName)
{
	return GetResourceData(GetResourceDataIDFromName(dataName));
}

bool ResourceManager::DestroyResourceData(const ResourceDataID id)
{
	if (id == ResourceManager::InvalidResourceDataID)
	{
		LogManager::LogMessage(LL_ERRO, "Trying to destroy a ResourceData, but the given ResourceDataID was Invalid.");
		return false;
	}
	m_ResourceDataMutex.lock_shared();
	const auto it = m_ResourceData.find(id);
	if (it == m_ResourceData.end())
	{
		m_ResourceDataMutex.unlock_shared();
		LogManager::LogMessage(LL_ERRO, "Trying to destroy a ResourceData, but the given ResourceDataID was not found.");
		return false;
	}
	m_ResourceDataMutex.unlock_shared();
	m_ResourceDataMutex.lock();
	m_ResourceData.erase(it);
	m_ResourceDataMutex.unlock();
	return true;
}

bool ResourceManager::DestroyResourceData(const std::string & dataName)
{
	if (dataName.empty())
	{
		LogManager::LogMessage(LL_ERRO, "Trying to destroy a ResourceData, but the given ResourceDataName was Invalid.");
		return false;
	}
	return DestroyResourceData(GetResourceDataIDFromName(dataName));
}

ResourceManager & ResourceManager::Instance()
{
	return *InstanceApp()->GetResourceManager();
}

ResourceManager * ResourceManager::InstancePtr()
{
	return InstanceApp()->GetResourceManager();
}

/****************************************************************
*						RESOURCE LOCATION						*
****************************************************************/

ResourceLocation::ResourceLocation(const EResourceDataLocation::Type location, const bool unload)
	:m_Location(location)
	,m_UnloadAtEnd(unload)
	,m_Loaded(false)
{

}

ResourceLocation::ResourceLocation(const ResourceLocation & other)
	:m_Location(other.m_Location)
{
	m_UnloadAtEnd = other.m_UnloadAtEnd;
	m_Loaded = other.m_Loaded;
}

ResourceLocation & ResourceLocation::operator=(const ResourceLocation & other)
{
	if (this != &other && m_Location == other.m_Location)
	{
		m_UnloadAtEnd = other.m_UnloadAtEnd;
		m_Loaded = other.m_Loaded;
	}
	return *this;
}

EResourceDataLocation::Type ResourceLocation::GetType()const
{
	return m_Location;
}

void ResourceLocation::LockRead()
{
	m_LocationMutex.lock_shared();
}

void ResourceLocation::LockWrite()
{
	m_LocationMutex.lock();
}

void ResourceLocation::UnlockWrite()
{
	m_LocationMutex.unlock();
}

void ResourceLocation::UnlockRead()
{
	m_LocationMutex.unlock_shared();
}

bool ResourceLocation::IsLoaded()
{
	std::shared_lock<std::shared_mutex> lock(m_LocationMutex);
	return m_Loaded;
}

bool ResourceLocation::IsUnloadingAtEnd()
{
	std::shared_lock<std::shared_mutex> lock(m_LocationMutex);
	return m_UnloadAtEnd;
}

void ResourceLocation::SetUnloadAtEnd(const bool unload)
{
	m_UnloadAtEnd = unload;
}

/****************************************************************
*					RESOURCE LOCATION DISK						*
****************************************************************/

ResourceLocationDisk::ResourceLocationDisk(File* file, const SIZET offset, const SIZET size, const bool closeAtEnd)
	:ResourceLocation(EResourceDataLocation::DISK, closeAtEnd)
	,m_File(file)
	,m_Offset(offset)
	,m_Size(size)
	,m_Buffer(nullptr)
{

}

ResourceLocationDisk::~ResourceLocationDisk()
{
	if (IsUnloadingAtEnd())
	{
		if (m_File)
			m_File->Close();
		Unload();
	}
}

ResourceLocationDisk::ResourceLocationDisk(const ResourceLocationDisk & other)
	:ResourceLocation(other)
	,m_File(other.m_File)
	,m_Offset(other.m_Offset)
	,m_Size(other.m_Size)
	,m_Buffer(nullptr)
{
	if (other.m_Buffer && m_Size != 0)
	{
		m_LocationMutex.lock();
		m_Buffer = malloc(m_Size);
		m_Buffer = memcpy(m_Buffer, other.m_Buffer, m_Size);
		m_LocationMutex.unlock();
	}
}

ResourceLocationDisk::ResourceLocationDisk(ResourceLocationDisk && other) noexcept
	:ResourceLocation(other)
	, m_File(other.m_File)
	, m_Offset(other.m_Offset)
	, m_Size(other.m_Size)
	, m_Buffer(other.m_Buffer)
{
	other.m_File = nullptr;
	other.m_Offset = 0;
	other.m_Size = 0;
	other.m_Buffer = nullptr;
}

ResourceLocationDisk& ResourceLocationDisk::operator=(const ResourceLocationDisk & other)
{
	if (this != &other)
	{
		m_LocationMutex.lock();
		if (m_File && IsUnloadingAtEnd())
		{
			m_File->Close();
		}
		Unload();
		m_File = other.m_File;
		m_Offset = other.m_Offset;
		m_Size = other.m_Size;
		if (other.m_Buffer && m_Size != 0)
		{
			m_Buffer = malloc(m_Size);
			m_Buffer = memcpy(m_Buffer, other.m_Buffer, m_Size);
		}
		m_LocationMutex.unlock();
	}
	return *this;
}

ResourceLocationDisk & ResourceLocationDisk::operator=(ResourceLocationDisk && other) noexcept
{
	if (this != &other)
	{
		if (m_File && IsUnloadingAtEnd())
			m_File->Close();
		Unload();
		m_LocationMutex.lock();
		m_File = std::exchange(other.m_File, nullptr);
		m_Offset = std::exchange(other.m_Offset, 0);
		m_Size = std::exchange(other.m_Size, 0);
		m_Buffer = std::exchange(other.m_Buffer, nullptr);
		m_LocationMutex.unlock();
	}
	return *this;
}

void* ResourceLocationDisk::GetData()
{
	std::shared_lock<std::shared_mutex> lock(m_LocationMutex);
	return m_Buffer;
}

SIZET ResourceLocationDisk::GetDataSize()
{
	std::shared_lock<std::shared_mutex> lock(m_LocationMutex);
	return m_Size;
}

void ResourceLocationDisk::Load()
{
	if (!m_File)
	{
		LogManager::LogMessage(LL_ERRO, "Trying to load data from disk to a ResourceLocation, but the File was nullptr.");
		return;
	}
	std::lock_guard<std::shared_mutex> lock(m_LocationMutex);
	Unload();
	m_File->Open();
	if (m_Size == 0)
	{
		if (m_File->GetSize(m_Size) != FileSysError_t::NoError)
		{
			LogManager::LogMessage(LL_ERRO, "Trying to get the file size from a ResourceLocation, but something went wrong.");
			m_Loaded = false;
			return;
		}
	}
	m_Buffer = malloc(m_Size);
	if (m_File->LoadContents(m_Buffer, m_Size, m_Offset, m_Size) != FileSysError_t::NoError)
	{
		LogManager::LogMessage(LL_ERRO, "Trying to get the file contents from a ResourceLocation, but something went wrong.");
		m_Loaded = false;
		return;
	}
	m_Loaded = true;
}

void ResourceLocationDisk::Unload()
{
	std::lock_guard<std::shared_mutex> lock(m_LocationMutex);
	SAFE_FREE(m_Buffer);
	m_Loaded = false;
}

bool ResourceLocationDisk::StoreData(void * data, const SIZET size)
{
	if (size == 0)
	{
		Unload();
		return true;
	}
	if (!data)
	{
		LogManager::LogMessage(LL_ERRO, "Trying to store new data to a ResourceLocationDisk, but the input data was nullptr.");
		return false;
	}

	std::lock_guard<std::shared_mutex> lock(m_LocationMutex);

	if (!m_Buffer)
		m_Buffer = malloc(size);
	else if (m_Size != size)
		m_Buffer = realloc(m_Buffer, size);
	
	m_Size = size;
	m_Buffer = memcpy(m_Buffer, data, size);
	m_Loaded = true;
	SIZET written;
	if (m_File->StoreContents(m_Buffer, m_Size, m_Offset, written) != FileSysError_t::NoError)
	{
		LogManager::LogMessage(LL_ERRO, "Trying to store new data to a ResourceLocationDisk, but something went wrong.");
		return false;
	}
	if (written != m_Size)
	{
		LogManager::LogMessage(LL_ERRO, "Trying to store new data to a ResourceLocationDisk, but not all data were copied.");
	}
	return true;
}

/****************************************************************
*					RESOURCE LOCATION MEMORY					*
****************************************************************/

ResourceLocationMemory::ResourceLocationMemory(void* buffer, const SIZET size, const bool destroyAtEnd)
	:ResourceLocation(EResourceDataLocation::MEMORY, destroyAtEnd)
	,m_Buffer(buffer)
	,m_Size(size)
{

}

ResourceLocationMemory::ResourceLocationMemory(const ResourceLocationMemory & other)
	:ResourceLocation(other)
	, m_Buffer(nullptr)
	,m_Size(other.m_Size)
{
	if (other.m_Buffer && m_Size != 0)
	{
		m_LocationMutex.lock();
		m_Buffer = malloc(m_Size);
		m_Buffer = memcpy(m_Buffer, other.m_Buffer, m_Size);
		m_LocationMutex.unlock();
	}
}

ResourceLocationMemory::ResourceLocationMemory(ResourceLocationMemory && other) noexcept
	:ResourceLocation(other)
	,m_Buffer(std::exchange(other.m_Buffer, nullptr))
	,m_Size(std::exchange(other.m_Size, 0))
{

}

ResourceLocationMemory & ResourceLocationMemory::operator=(const ResourceLocationMemory & other)
{
	if (this != &other)
	{
		Unload();
		if (other.m_Buffer && other.m_Size != 0)
		{
			m_LocationMutex.lock();
			m_Size = other.m_Size;
			m_Buffer = malloc(m_Size);
			m_Buffer = memcpy(m_Buffer, other.m_Buffer, m_Size);
			m_LocationMutex.unlock();
		}
	}
	return *this;
}

ResourceLocationMemory & ResourceLocationMemory::operator=(ResourceLocationMemory && other) noexcept
{
	if (this != &other)
	{
		Unload();
		m_LocationMutex.lock();
		m_Buffer = std::exchange(other.m_Buffer, nullptr);
		m_Size = std::exchange(other.m_Size, 0);
		m_LocationMutex.unlock();
	}
	return *this;
}

ResourceLocationMemory::~ResourceLocationMemory()
{
	if (IsUnloadingAtEnd())
	{
		Unload();
	}
}

void * ResourceLocationMemory::GetData()
{
	std::shared_lock<std::shared_mutex> lock(m_LocationMutex);
	return m_Buffer;
}

SIZET ResourceLocationMemory::GetDataSize()
{
	std::shared_lock<std::shared_mutex> lock(m_LocationMutex);
	return m_Size;
}

bool ResourceLocationMemory::StoreData(void * data, const SIZET size)
{
	if (size == 0)
	{
		return true;
	}
	if (!data)
	{
		LogManager::LogMessage(LL_ERRO, "Trying to store data from memory to a ResourceLocation, but that memory was nullptr.");
		return false;
	}
	std::lock_guard<std::shared_mutex> lock(m_LocationMutex);
	if (!m_Buffer)
	{
		m_Buffer = malloc(size);
	}
	else if (m_Size != size)
	{
		m_Buffer = realloc(m_Buffer, size);
	}
	m_Size = size;
	m_Buffer = memcpy(m_Buffer, data, m_Size);
	m_Loaded = true;
	return true;
}

void ResourceLocationMemory::Load()
{
	if (m_Buffer)
	{
		m_LocationMutex.lock();
		m_Loaded = true;
		m_LocationMutex.unlock();
	}
}

void ResourceLocationMemory::Unload()
{
	std::lock_guard<std::shared_mutex> lock(m_LocationMutex);
	SAFE_FREE(m_Buffer);
	m_Size = 0;
	m_Loaded = false;
}

/****************************************************************
*						RESOURCE DATA							*
****************************************************************/

void ResourceData::UpdateState()
{
	if (!m_SourceData)
	{
		m_StateMutex.lock();
		m_State = EResourceDataState::UKNOWN_SOURCE;
		m_StateMutex.unlock();
		return;
	}
	const auto loaded = m_SourceData->IsLoaded();
	if (!loaded)
	{
		m_StateMutex.lock();
		m_State = EResourceDataState::NOT_LOADED;
		m_StateMutex.unlock();
	}
	else
	{
		m_StateMutex.lock();
		m_State = EResourceDataState::LOADED;
		m_StateMutex.unlock();
	}
}

ResourceData::ResourceData()
	:m_State(EResourceDataState::INVALID)
	,m_Lifetime(EResourceDataLifetime::STATIC)
	,m_ResourceDataType(ResourceManager::InvalidResourceType)
	,m_Name("InvalidResouceDataID")
	,m_SourceData(nullptr)
	,m_OldSourceData(nullptr)
	,m_TiedResources(0)
{

}

ResourceData::ResourceData(std::string name, const EResourceDataLifetime::Type lifetime,
	const ResourceType resourceType)
	:m_State(EResourceDataState::UKNOWN_SOURCE)
	, m_Lifetime(lifetime)
	, m_ResourceDataType(resourceType)
	, m_Name(std::move(name))
	,m_SourceData(nullptr)
	,m_OldSourceData(nullptr)
	,m_TiedResources(0)
{

}

ResourceData::ResourceData(const ResourceData & other)
	:m_State(other.m_State)
	,m_Lifetime(other.m_Lifetime)
	,m_ResourceDataType(other.m_ResourceDataType)
	,m_Name(other.m_Name)
	,m_SourceData(other.m_SourceData)
	,m_OldSourceData(other.m_OldSourceData)
	,m_TiedResources(other.m_TiedResources.load())
{

}

ResourceData & ResourceData::operator=(const ResourceData & other)
{
	if (this != &other)
	{
		m_StateMutex.lock();
		m_State = other.m_State;
		m_StateMutex.unlock();

		m_Lifetime = other.m_Lifetime;
		m_ResourceDataType = other.m_ResourceDataType;
		m_Name = other.m_Name;

		m_SourceDataMutex.lock();
		m_SourceData = other.m_SourceData;
		m_OldSourceData = other.m_OldSourceData;
		m_SourceDataMutex.unlock();

		m_TiedResources = other.m_TiedResources.load();
	}
	return *this;
}

ResourceData::~ResourceData()
{
	InstanceEvent()->DispatchEvent(EventIDOnDataDestroying, (void*)(PTRUINT)GetID());
}

void ResourceData::ChangeSourceData(ResourceLocation * location)
{
	m_SourceDataMutex.lock();
	m_OldSourceData = m_SourceData;
	m_SourceData = location;
	m_SourceDataMutex.unlock();

	UpdateState();
	InstanceEvent()->DispatchEvent(EventIDOnDataChange, this);
}

void ResourceData::Load()
{
	if (GetState() == EResourceDataState::UKNOWN_SOURCE)
	{
		LogManager::LogMessage(LL_WARN, "Trying to load an uknown ResourceLocation.");
		return;
	}
	m_StateMutex.lock();
	m_State = EResourceDataState::LOADING;
	m_StateMutex.unlock();
	
	m_SourceData->Load();

	m_StateMutex.lock();
	if (m_SourceData->IsLoaded())
	{
		m_State = EResourceDataState::LOADED;
	}
	else
	{
		m_State = EResourceDataState::NOT_LOADED;
	}
	m_StateMutex.unlock();
	++m_TiedResources;
}

CreateTaskName(ResourceDataLoadAsyncTask);

void ResourceData::LoadAsync()
{
	InstanceApp()->SendTask(CreateTask(ResourceDataLoadAsyncTask, [this]()
	{
		this->Load();
		InstanceEvent()->DispatchEvent(EventIDOnDataFinishedLoading, this);
	}));
}

void ResourceData::Unload()
{
	if (m_Lifetime == EResourceDataLifetime::STATIC)
	{
		--m_TiedResources;
		return;
	}
	/* DATALIFE DYNAMIC */
	if (m_TiedResources > 1)
	{
		--m_TiedResources;
		return;
	}
	const auto state = GetState();
	if (state == EResourceDataState::LOADING)
	{
		m_SourceData->LockWrite(); // Wait until is loaded
		m_SourceData->UnlockWrite();
		m_SourceData->Unload();
	}
	else if (state == EResourceDataState::LOADED)
	{
		m_SourceData->Unload();
	}
	--m_TiedResources;
	UpdateState();
}

EResourceDataLifetime::Type ResourceData::GetLifetime() const
{
	return m_Lifetime;
}

ResourceType ResourceData::GetType() const
{
	return m_ResourceDataType;
}

EResourceDataState::Type gaf::ResourceData::GetState()
{
	std::shared_lock<std::shared_mutex> lock(m_StateMutex);
	return m_State;
}

ResourceLocation * ResourceData::GetOldDataLocation()
{
	std::shared_lock<std::shared_mutex> lock(m_SourceDataMutex);
	return m_OldSourceData;
}

ResourceLocation * ResourceData::GetDataLocation()
{
	std::shared_lock<std::shared_mutex> lock(m_SourceDataMutex);
	return m_SourceData;
}

const std::string & gaf::ResourceData::GetName() const
{
	return m_Name;
}

ResourceDataID gaf::ResourceData::GetID() const
{
	return GetResourceDataIDFromName(m_Name);
}

uint32 ResourceData::GetNumTiedResources() const
{
	return m_TiedResources.load();
}
