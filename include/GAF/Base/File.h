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

#ifndef GAF_FILE_H
#define GAF_FILE_H 1

#include "GAF/Util/DayTime.h"

namespace gaf
{
	namespace EFileSysError
	{
		enum Type
		{
			NoError,
			InputError,
			NotFound,
			UnknownError
		};
	}
	typedef EFileSysError::Type FileSysError_t;
	const std::string& GetFileErrorStr(FileSysError_t err);
	namespace EFilePermisions
	{
		enum Type
		{
			Closed,
			ReadOnly,
			ReadWrite
		};
	}
	typedef EFilePermisions::Type FilePermisions_t;
	const std::string& GetFilePermisionsStr(FilePermisions_t perm);

	typedef
#if PLATFORM_WINDOWS
		HANDLE
#else
		int
#endif
		FileHandle;

	const FileHandle NullFileHandle =
#if PLATFORM_WINDOWS
		INVALID_HANDLE_VALUE;
#else
		-1;
#endif

	class Directory;
	class File
	{
		/* The Directory which is in this File */
		Directory* m_Directory;
		/* The name of the File */
		std::wstring m_Name;

		/* The permisions that this File has */
		EFilePermisions::Type m_Permisions;
		FileHandle m_Handle;
		File() = default;
		~File();

		std::recursive_mutex m_Mutex;
		FileSysError_t Open(FilePermisions_t perm);

	public:
		static constexpr SIZET OffsetBegin = 0;
		static constexpr SIZET OffsetEnd = static_cast<SIZET>(-1);

		/*
			Changes the name of this File and the name of the physical file.
			Return:
				- NoError: The physical file and the File name where changed successfully.
				- InputError: The name was already used in that directory of it cannot be changed.
				- UnknownError: Maybe an error related to other system.
		*/
		FileSysError_t ChangeName(const std::string& name);
		FileSysError_t ChangeName(const std::wstring& name);
		
		/*
			Erases the contents of the physical file but doesn't change it's file size.
		*/
		void ClearFile();
		
		/*
			Returns the Creation time of this physical file.
		*/
		FileSysError_t GetCreationTime(DayTime& time);

		/*
			Returns the last time that this physical file was accessed.
		*/
		FileSysError_t GetLastAccessTime(DayTime& time);

		/*
			Returns the last time that something was written into this physical file.
		*/
		FileSysError_t GetLastWriteTime(DayTime& time);

		/*
			Returns the physical size of this file.
		*/
		FileSysError_t GetSize(SIZET& size);

		/*
			Loads the contents of the file into the given buffer. The buffer must require the given size,
			because will be filled, by the contents of the file and if the bufferSize is bigger than 
			the file size minus the offset, the buffer will be filled by zeros.
			Return:
				- NoError: The contents of the file are correctly loaded.
				- InputError: The buffer was nullptr.
				- UnknownError: Something unexpected went wrong.
		*/
		FileSysError_t LoadContents(void* buffer, SIZET bufferByteSize, SIZET& readBytes);
		FileSysError_t LoadContents(void* buffer, SIZET bufferByteSize, SIZET fileOffset, SIZET& readBytes);

		/*
			Writes the contents of the buffer into the file, if needed the file will be resized.
			Return:
				- NoError: The contents of the buffer are correctly stored into the buffer.
				- InputError: The buffer was nullptr;
				- UnknownError: Something unexpected went wrong.
		*/
		FileSysError_t StoreContents(void*const buffer, SIZET bufferByteSize, SIZET& writtenBytes);
		FileSysError_t StoreContents(void*const buffer, SIZET bufferByteSize, SIZET fileOffset, SIZET& writtenBytes);

		/*
			Returns the current permisions that the file has.
		*/
		FilePermisions_t GetPermisions()const;

		/*
			Returns the raw File handle that the OS use, try not to close or change the 
			permisions of the file.
		*/
		FileHandle GetOSHandle()const;

		/*
			Returns the currently in use file offset.
		*/
		FileSysError_t GetOffset(SIZET& offset);
		
		/*
			Changes the offset of the file (where the read/write occurs).
		*/
		FileSysError_t SetOffset(SIZET offset);

		/*
			Erases the physical file and deletes the File structure and its resources,
			so you must not use this after calling this function.
		*/
		void Erase();

		FileSysError_t Resize(SIZET sz);

		FileSysError_t Reserve(SIZET sz);

		/*
			Locks the modification of a region of the File in order to be not 
			modified by other processes.
		*/
		FileSysError_t LockRegion(SIZET offset, SIZET bytesToLock);
		/*
			Unlocks a region of a File in order to be used by other processes.
		*/
		FileSysError_t UnlockRegion(SIZET offset, SIZET bytesToUnlock);

		/*
			Locks the entire File to prevent the modification of it by other
			processes.
		*/
		FileSysError_t Lock();
		/*
			Unlocks the entire File to allow the modification of it by other
			processes.
		*/
		FileSysError_t Unlock();

		/*
			Returns the full path of the file, ex: C\GAF\Files\file.f
		*/
		std::string GetFullPath()const;
		std::wstring GetFullPathW()const;
		/*
			Returns the file extension. Returns empty string if there's no dot in 
			the fileName.
		*/
		std::string GetExtension()const;
		std::wstring GetExtensionW()const;
		/*
			Returns the whole file name without the extension, returns the file name 
			entirely if there's no dot in the name.
		*/
		std::string GetNameWithoutExtension()const;
		std::wstring GetNameWithoutExtensionW()const;
		/*
			Returns the full path but without the file name
		*/
		std::string GetPath()const;
		std::wstring GetPathW()const;

		/*
			Copies this file to another directory.
		*/
		FileSysError_t CopyFileTo(Directory* dir, bool replace = false);
		/*
			Copies this file to another directory.
		*/
		FileSysError_t CopyFileTo(const std::string& path, bool replace = false);
		FileSysError_t CopyFileTo(const std::wstring& path, bool replace = false);
		/*
			Move this file to another directory, if the file already exists in that directory
			this File structure will be invalid, so after the move go to that directory and look
			for this file.
		*/
		FileSysError_t MoveFileTo(Directory* dir);
		/*
			Move this file to another directory, if the file already exists in that directory
			this File structure will be invalid, so after the move go to that directory and look
			for this file.
		*/
		FileSysError_t MoveFileTo(const std::string& path);
		FileSysError_t MoveFileTo(const std::wstring& path);

		/*
			Returns the directory where the file it's in it, if the file its unhandled may return
			nullptr or an unhandled Directory.
		*/
		Directory* GetDirectory()const;
		/*
			Closes the file and releases the OS handle.
		*/
		void Close();
		/*
			Opens the file and makes it opened until you call close, 
			because the other methods opens it temporarily.
		*/
		void Open();
		/*
			Returns the file name without any path
		*/
		std::string GetName()const;
		std::wstring GetNameW()const;
		friend class FileSystem;
		friend class Directory;
	};
	typedef std::vector<File*> FileList;
}

#endif /* GAF_FILE_H */