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

#ifndef GAF_RESOURCEMANAGER_H
#define GAF_RESOURCEMANAGER_H 1

#include "GAF/GAFPrerequisites.h"
#include "GAF/EventManager.h"

namespace gaf
{
	typedef size_t ResourceDataID;
	typedef size_t ResourceType;

	ResourceType GetResourceTypeFromName(const std::string& name);
	const std::string& GetResourceTypeNameFromID(ResourceType type);
	ResourceDataID GetResourceDataIDFromName(const std::string& name);

	namespace EResourceDataState
	{
		enum Type
		{
			/* Some data was not set leaving the resource in a invalid state */
			INVALID,
			/* Source data is not set */
			UKNOWN_SOURCE,
			/* Anything is done to the resource data */
			NOT_LOADED,
			/* Resource data is currently being loaded */
			LOADING,
			/* Resource data is loaded and ready to be used */
			LOADED,
		};
	}
	namespace EResourceDataLifetime
	{
		enum Type
		{
			/* 
				Data won't be unloaded unless you explicitly call unload to the ResourceLocation
			*/
			STATIC,
			/*
				Data will be unloaded after every resource tied to it has called unload
			*/
			DYNAMIC,
		};
	}
	namespace EResourceDataLocation
	{
		enum Type
		{
			/* Data will be copied to the buffer inside the ResourceData struct */
			MEMORY,
			/* Data will be copied from a File in the disk, and ResourceData will hold a pointer to that file. */
			DISK
		};
	}
	namespace EResourceState
	{
		enum Type
		{
			/* Resource is not ready to be used */
			NOT_READY,
			/* Resource is not ready but is currently loading its ResourceData */
			DATA_LOADING,
			/* Resource is not ready but is currently importing its data from ResourceData */
			DATA_IMPORTING,
			/* Resource is ready to be used */
			READY
		};
	}

	class ResourceLocation
	{
		const EResourceDataLocation::Type m_Location;
		bool m_UnloadAtEnd;
	protected:
		std::shared_mutex m_LocationMutex;
		bool m_Loaded;
	public:
		ResourceLocation(EResourceDataLocation::Type location = EResourceDataLocation::MEMORY,
			bool unloadAtEnd = false);
		ResourceLocation(const ResourceLocation& other);
		ResourceLocation(ResourceLocation&&)noexcept = default;
		ResourceLocation& operator=(const ResourceLocation& other);
		ResourceLocation& operator=(ResourceLocation&&)noexcept = default;
		virtual ~ResourceLocation() = default;

		virtual void* GetData() = 0;
		virtual SIZET GetDataSize() = 0;
		virtual bool StoreData(void* data, SIZET size) = 0;
		virtual void Load() = 0;
		virtual void Unload() = 0;
		EResourceDataLocation::Type GetType()const;

		void LockRead();
		void LockWrite();
		void UnlockWrite();
		void UnlockRead();

		bool IsLoaded();
		bool IsUnloadingAtEnd();
		void SetUnloadAtEnd(bool unload);
	};

	class ResourceLocationDisk : public ResourceLocation
	{
		File* m_File;
		SIZET m_Offset;
		SIZET m_Size;
		void* m_Buffer;
	public:
		ResourceLocationDisk(File* file = nullptr,
			SIZET offset = 0, SIZET size = 0, bool closeAtEnd = true);
		~ResourceLocationDisk();
		ResourceLocationDisk(const ResourceLocationDisk& other);
		ResourceLocationDisk(ResourceLocationDisk&& other)noexcept;
		ResourceLocationDisk& operator=(const ResourceLocationDisk& other);
		ResourceLocationDisk& operator=(ResourceLocationDisk&& other)noexcept;

		void* GetData()override;
		SIZET GetDataSize()override;
		void Load()override;
		void Unload()override;
		
		bool StoreData(void* data, SIZET size);
	};

	class ResourceLocationMemory : public ResourceLocation
	{
		void* m_Buffer;
		SIZET m_Size;
	public:
		ResourceLocationMemory(void* buffer = nullptr, SIZET size = 0, bool destroyAtEnd = false);
		ResourceLocationMemory(const ResourceLocationMemory& other);
		ResourceLocationMemory(ResourceLocationMemory&& other)noexcept;
		ResourceLocationMemory& operator=(const ResourceLocationMemory& other);
		ResourceLocationMemory& operator=(ResourceLocationMemory&& other)noexcept;
		~ResourceLocationMemory();

		void* GetData()override;
		SIZET GetDataSize()override;
		bool StoreData(void* data, SIZET size)override;
		void Load()override;
		void Unload()override;
	};

	class ResourceData
	{
	public:
		static constexpr auto OnDataChangeEvent = "OnResourceDataChange";
		static EventID EventIDOnDataChange;
		static constexpr auto OnDataFinishedLoading = "OnResourceDataFinishedLoading";
		static EventID EventIDOnDataFinishedLoading;
		static constexpr auto OnDataDestroying = "OnResourceDataDestroying";
		static EventID EventIDOnDataDestroying;
	private:
		
		EResourceDataState::Type m_State;
		std::shared_mutex m_StateMutex;

		EResourceDataLifetime::Type m_Lifetime;
		ResourceType m_ResourceDataType;
		std::string m_Name;

		ResourceLocation* m_SourceData;
		ResourceLocation* m_OldSourceData;
		std::shared_mutex m_SourceDataMutex;

		std::atomic<uint32> m_TiedResources;

		void UpdateState();
	public:
		ResourceData();
		ResourceData(std::string name, 
			EResourceDataLifetime::Type lifetime = EResourceDataLifetime::STATIC,
			ResourceType resourceType = 0);

		ResourceData(const ResourceData& other);
		ResourceData(ResourceData&& other)noexcept = default;
		ResourceData& operator=(const ResourceData& other);
		ResourceData& operator=(ResourceData&& other)noexcept = default;

		~ResourceData();

		void ChangeSourceData(ResourceLocation* location);
		
		void Load();
		void LoadAsync();

		void Unload();

		EResourceDataLifetime::Type GetLifetime()const;
		ResourceType GetType()const;
		const std::string& GetName()const;
		ResourceDataID GetID()const;
		uint32 GetNumTiedResources()const;

		EResourceDataState::Type GetState();

		ResourceLocation* GetOldDataLocation();
		ResourceLocation* GetDataLocation();
	};

	class SerializableResource
	{
	public:
		virtual ~SerializableResource() = default;
		virtual bool Import() = 0;
		virtual bool Export() = 0;
		virtual bool ImportFrom(ResourceLocation* data) = 0;
		virtual bool ExportTo(ResourceLocation* data) = 0;
	};

	class ResourceManager
	{
		std::map<ResourceDataID, ResourceData> m_ResourceData;
		std::shared_mutex m_ResourceDataMutex;
				
		ResourceManager();
		~ResourceManager();
	public:
		static ResourceType InvalidResourceType;
		static ResourceDataID InvalidResourceDataID;

		ResourceDataID CreateResourceData(const std::string& dataName, ResourceType type, ResourceLocation* location = nullptr,
			EResourceDataLifetime::Type lifetime = EResourceDataLifetime::STATIC);
		ResourceDataID CreateResourceData(const std::string& dataName, const std::string& resourceType, ResourceLocation* location = nullptr,
			EResourceDataLifetime::Type lifetime = EResourceDataLifetime::STATIC);

		ResourceData* GetResourceData(ResourceDataID id);
		ResourceData* GetResourceData(const std::string& dataName);
		bool DestroyResourceData(ResourceDataID id);
		bool DestroyResourceData(const std::string& dataName);

		static ResourceManager& Instance();
		static ResourceManager* InstancePtr();

		friend class Application;
	};

	static ResourceManager* InstanceRes() { return ResourceManager::InstancePtr(); }
}
#endif /* GAF_RESOURCEMANAGER_H */