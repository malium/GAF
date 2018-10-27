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

#ifndef GAF_FILESYSTEM_H
#define GAF_FILESYSTEM_H 1

#include "GAF/GAFPrerequisites.h"
#include "GAF/TaskManager.h"
#include "GAF/EventManager.h"
#include "GAF/Base/File.h"
#include "GAF/Base/Directory.h"

namespace gaf
{
	struct FileAsync
	{
		void* Buffer;
		SIZET BufferSize;
		File* FileToUse;
		FileAsync() = default;
		FileAsync(const FileAsync&) = default;
		FileAsync(FileAsync&&)noexcept = default;
		FileAsync& operator=(const FileAsync&) = default;
		FileAsync& operator=(FileAsync&&)noexcept = default;
		friend bool operator==(const FileAsync& a, const FileAsync& b)
		{
			if (a.Buffer != b.Buffer)
				return false;
			if (a.BufferSize != b.BufferSize)
				return false;
			if (a.FileToUse != b.FileToUse)
				return false;
			return true;
		}
		friend bool operator!=(const FileAsync& a, const FileAsync& b)
		{
			return !(a == b);
		}
	};

	class FileSystem : public TaskDispatcher
	{
		static constexpr SIZET NumberIOHandlers = 4;
		using AsyncType = bool;
		static constexpr AsyncType AsyncRead = true;
		static constexpr AsyncType AsyncWrite = false;
		Directory* m_RootDir;
		
		using AsyncVec = std::vector<std::pair<FileAsync, AsyncType>>;
		AsyncVec m_AsyncOperations;
		std::shared_mutex m_AsyncMutex;
		
		void AsyncReadFn(AsyncVec::iterator async, const std::function<void(FileAsync*)>& beginReadFunc,
			const std::function<void(FileAsync*)>& endReadFunc);
		void AsyncWriteFn(AsyncVec::iterator async, const std::function<void(FileAsync*)>& beginWriteFunc,
			const std::function<void(FileAsync*)>& endWriteFunc);

		static constexpr auto OnFileBeginReadAsync = "FileBeginReadAsync";
		static EventID EventIDOnFileBeginReadAsync;
		static constexpr auto OnFileEndReadAsync = "FileEndReadAsync";
		static EventID EventIDOnFileEndReadAsync;
		static constexpr auto OnFileBeginWriteAsync = "FileBeginWriteAsync";
		static EventID EventIDOnFileBeginWriteAsync;
		static constexpr auto OnFileEndWriteAsync = "FileEndWriteAsync";
		static EventID EventIDOnFileEndWriteAsync;
	protected:
		FileSystem();
		~FileSystem();

		friend class File;
	public:

		static constexpr auto LineTerminatorW = LINE_TERMINATOR_WIDE;
		static constexpr auto PathSeparatorW = PATH_SEPARATOR_WIDE;
		static constexpr auto LineTerminator = LINE_TERMINATOR;
		static constexpr auto PathSeparator = PATH_SEPARATOR;

		static FileSystem& Instance();
		static FileSystem* InstancePtr();
		
		/*
			Returns the application directory
		*/
		static std::string GetExeDirectory();
		static std::wstring GetExeDirectoryW();

		/*
			Starts an Async read operation, you must provide the file, the buffer (where the data will copied to), the buffer 
			size which tells how many data must be read, and the BeginRead and EndRead functions which will help you in case
			that you want to do stuff to the file or the buffer before read or after, an example is to use the beginReadFunc to
			tell the file where to start to reading, and use the endReadFunc, to use the buffer and free it if its not necessary anymore.
			Return:
				- NoError: All the input parameters were correct and the operation can start.
				- InputError: One or more input parameters where incorrect.
		*/
		FileSysError_t StartAsyncRead(File* file, void* buffer, SIZET bufferSize, const std::function<void(FileAsync*)>& beginReadFunc,
			const std::function<void(FileAsync*)>& endReadFunc);

		/*
			Starts an Async write operation, you must provide, the file, the buffer (where the data will be copied from), the buffer
			size which tells how many data must be written, and the BeginWrite and EndWrite functions which will help you in case
			that you want to do stuff with the file or the buffer before or after, an example is to use the beginWriteFunc to
			tell the file where to start writing, and use the endWriteFunc, to tell that operation is already compleated or to use that
			file.
			Return:
				- NoError: All the input parameters were correct and the operation can start.
				- InputError: One or more input parameters where incorrect.
		*/
		FileSysError_t StartAsyncWrite(File* file, void* buffer, SIZET bufferSize, const std::function<void(FileAsync*)>& beginWriteFunc,
			const std::function<void(FileAsync*)>& endWriteFunc);

		/*
			Creates a File structure which won't be handled by the FileSystem, instead will be handled by you.
			This File structure helps with the manipulation of it, use the DeleteExternalFile function in order 
			to free the File structure and its resources, and use the EraseExternalFile in order to erase
			the physical file and the File structure.
			Return:
				- NoError: All the input parameters were correct and the operation can start.
				- NotFound: The fileName parameter were incorrect, the file cannot be found.
				- UnknownError: Maybe an error related to other system.
		*/
		static FileSysError_t GetExternalFile(const std::string& fileName, File*& file);
		static FileSysError_t GetExternalFile(const std::wstring& fileName, File*& file);

		static FileSysError_t GetExternalFile(const std::string& filePath, const std::string& fileName, File*& file);
		static FileSysError_t GetExternalFile(const std::wstring& filePath, const std::wstring& fileName, File*& file);

		/*
			Creates a file and it's File structure which won't be handled by the FileSystem, instead will be handled by you.
			This File structure helps with the manipulation of it, use the DeleteExternalFile function in order
			to free the File structure and its resources, and use the EraseExternalFile in order to erase
			the physical file and the File structure. filePathName must contain the path to the file and its name, if it only
			has the name, it will be created in the main executable path.
			Return:
				- NoError: All the input parameters were correct and the operation can start.
				- InputError: The name was already in use or there's no direct path to it, because needs directories to be created.
				- UnknownError: Maybe an error related to other system.
		*/
		static FileSysError_t CreateExternalFile(const std::string& filePathName, File*& file);
		static FileSysError_t CreateExternalFile(const std::wstring& filePathName, File*& file);

		/*
			Creates a file and it's File structure which won't be handled by the FileSystem, instead will be handled by you.
			This File structure helps with the manipulation of it, use the DeleteExternalFile function in order 
			to free the File structure and its resources, and use the EraseExternalFile in order to erase
			the physical file and the File structure. If the filePath is empty it will be replaced by the main executable path.
			Return:
				- NoError: All the input parameters were correct and the operation can start.
				- InputError: The name was already in use or there's no direct path to it, because needs directories to be created.
				- UnknownError: Maybe an error related to other system.
		*/
		static FileSysError_t CreateExternalFile(const std::string& filePath, const std::string& name, File*& file);
		static FileSysError_t CreateExternalFile(const std::wstring& filePath, const std::wstring& name, File*& file);

		/*
			Erases the physical file and the destroy the File structure and its resources
			Return:
				- NoError: The operation ended correctly.
				- UnknownError: Maybe an error related to other system.
		*/
		static FileSysError_t EraseExternalFile(File*& file);

		/*
			Destroys the File structure but it doesn't erase the physical file, letting its contents intact.
			Return:
				- NoError: The operation ended correctly.
				- UnknownError: Maybe an error related to other system.
		*/
		static FileSysError_t DeleteExternalFile(File*& file);

		/*
			Creates a Directory structure which won't be handled by the FileSystem, instead will be handled by you.
			This Directory structure helps with the manipulation of it, use the DeleteExternalDir function in order
			to free the File structure and its resources, and use the EraseExternalDir in order to erase
			the physical file and the File structure.
			Return:
				- NoError: All the input parameters were correct and the operation can start.
				- InputError: One or more input parameters where incorrect.
				- UnknownError: Maybe an error related to other system that doesn't let the operation to start.
		*/
		static FileSysError_t GetExternalDir(const std::string& dirNamePath, Directory*& dir);
		static FileSysError_t GetExternalDir(const std::wstring& dirNamePath, Directory*& dir);

		/*
			Creates a Directory structure which won't be handled by the FileSystem, instead will be handled by you.
			This Directory structure helps with the manipulation of it, use the DeleteExternalDir function in order
			to free the File structure and its resources, and use the EraseExternalDir in order to erase
			the physical file and the File structure.
			Return:
				- NoError: All the input parameters were correct and the operation can start.
				- InputError: One or more input parameters where incorrect.
				- UnknownError: Maybe an error related to other system that doesn't let the operation to start.
		*/
		static FileSysError_t GetExternalDir(const std::string& dirPath, const std::string& dirName, Directory*& dir);
		static FileSysError_t GetExternalDir(const std::wstring& dirPath, const std::wstring& dirName, Directory*& dir);

		/*
			Creates a directory and it's Directory structure which won't be handled by the FileSystem, instead will be handled by you.
			This Directory structure helps with the manipulation of it, use the DeleteExternalDir function in order
			to free the File structure and its resources, and use the EraseExternalDir in order to erase
			the physical file and the File structure. If dirPath is empty it will be replaced by the main executable path.
			Return:
				- NoError: All the input parameters were correct and the operation can start.
				- InputError: One or more input parameters where incorrect.
				- UnknownError: Maybe an error related to other system that doesn't let the operation to start.
		*/
		static FileSysError_t CreateExternalDir(const std::string& dirNamePath, Directory*& dir);
		static FileSysError_t CreateExternalDir(const std::wstring& dirNamePath, Directory*& dir);

		/*
			Creates a directory and it's Directory structure which won't be handled by the FileSystem, instead will be handled by you.
			This Directory structure helps with the manipulation of it, use the DeleteExternalDir function in order
			to free the File structure and its resources, and use the EraseExternalDir in order to erase
			the physical file and the File structure. drirathName must contain the path to the directory and its name, if it only 
			has the name, it will be created in the main executable path.
			Return:
				- NoError: All the input parameters were correct and the operation can start.
				- InputError: One or more input parameters where incorrect.
				- UnknownError: Maybe an error related to other system that doesn't let the operation to start.
		*/
		static FileSysError_t CreateExternalDir(const std::string& dirPath, const std::string& name, Directory*& dir);
		static FileSysError_t CreateExternalDir(const std::wstring& dirPath, const std::wstring& name, Directory*& dir);

		/*
			Erases the physical directory and the destroy the Directory structure and its resources
			Return:
				- NoError: The operation ended correctly.
				- UnknownError: Maybe an error related to other system.
		*/
		static FileSysError_t EraseExternalDir(Directory*& dir);
		/*
			Destroys the Directory structure but it doesn't erase the physical directory, letting its contents intact.
			Return:
				- NoError: The operation ended correctly.
				- UnknownError: Maybe an error related to other system.
		*/
		static FileSysError_t DeleteExternalDir(Directory*& dir);

		static bool IsDirectory(const std::string& path);
		static bool IsDirectory(const std::wstring& path);

		static bool IsFile(const std::string& path);
		static bool IsFile(const std::wstring& path);

		Directory* GetRootDirectory()const;

		FileSysError_t GetDirectory(const std::string& name, Directory*& dir, bool recursive = true);
		FileSysError_t GetDirectory(const std::wstring& name, Directory*& dir, bool recursive = true);

		FileSysError_t GetFile(const std::string& name, File*& file, bool recursive = true);
		FileSysError_t GetFile(const std::wstring& name, File*& file, bool recursive = true);

		friend class Application;
	};

	static FileSystem* InstanceFS() { return FileSystem::InstancePtr(); }
}

#endif /* GAF_FILESYSTEM_H */