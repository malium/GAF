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
#include "GAF/FileSystem.h"
#include "GAF/LogManager.h"
#include "GAF/Util/StringUtils.h"

using namespace gaf;

std::string gaf::FileSystem::GetExeDirectory()
{
	return StringUtils::ws2s(GetExeDirectoryW());
}

std::wstring FileSystem::GetExeDirectoryW()
{
	std::wstring path;
#if PLATFORM_WINDOWS
		WIDECHAR name[2048];
		const auto retval = GetModuleFileNameW(nullptr, name, 2048);
		if (!SUCCEEDED(retval))
		{
			LogManager::LogMessage(LL_ERRO, "Couldn't retrieve the executable directory, reason: 0x%08X.", GetLastError());
			return LineTerminatorW;
		}
		path.assign(name, retval);
#else
		//TODO get exe directory
#endif
	const auto lastSlash =  path.find_last_of(PathSeparatorW);
	return path.substr(0, lastSlash);
}

void FileSystem::AsyncReadFn(AsyncVec::iterator async, const std::function<void(FileAsync*)>& beginReadFunc, const std::function<void(FileAsync*)>& endReadFunc)
{
	Assertion::WhenEqual(async->second, AsyncWrite, "Trying to call an AsyncRead with an AsyncWrite.");
	beginReadFunc(&async->first);
	SIZET readbytes;
	async->first.FileToUse->LoadContents(async->first.Buffer, async->first.BufferSize, readbytes);
	endReadFunc(&async->first);
	m_AsyncMutex.lock();
	m_AsyncOperations.erase(async);
	m_AsyncMutex.unlock();
}

void FileSystem::AsyncWriteFn(AsyncVec::iterator async, const std::function<void(FileAsync*)>& beginWriteFunc, const std::function<void(FileAsync*)>& endWriteFunc)
{
	Assertion::WhenEqual(async->second, AsyncRead, "Trying to call an AsyncWrite with an AsyncRead.");
	beginWriteFunc(&async->first);
	SIZET writtenbytes;
	async->first.FileToUse->StoreContents(async->first.Buffer, async->first.BufferSize, writtenbytes);
	endWriteFunc(&async->first);
	m_AsyncMutex.lock();
	m_AsyncOperations.erase(async);
	m_AsyncMutex.unlock();
}

FileSystem::FileSystem()
	:TaskDispatcher{"FileSystem", InstanceApp()}
{
	LogManager::LogMessage(LL_INFO, "Starting FileSystem...");
	m_RootDir = new Directory();
	m_RootDir->m_Name = GetExeDirectoryW();
	m_RootDir->m_UpperDirectory = nullptr;
	m_RootDir->Update();
	InstanceApp()->RegisterTaskDispatcher(this, NumberIOHandlers);
}

FileSystem::~FileSystem()
{
	LogManager::LogMessage(LL_INFO, "Stopping FileSystem...");
	SAFE_DELETE(m_RootDir);
}

FileSystem & FileSystem::Instance()
{
	return *InstanceApp()->GetFileSystem();
}

FileSystem* FileSystem::InstancePtr()
{
	return InstanceApp()->GetFileSystem();
}

CreateTaskName(AsyncReadTask);

FileSysError_t FileSystem::StartAsyncRead(File * file, void * buffer, const SIZET bufferSize, const std::function<void(FileAsync*)>& beginReadFunc, const std::function<void(FileAsync*)>& endReadFunc)
{
	if (!file)
	{
		LogManager::LogMessage(LL_ERRO, "Trying to start an AsyncRead with a nullptr File.");
		return FileSysError_t::InputError;
	}
	if (!buffer)
	{
		LogManager::LogMessage(LL_ERRO, "Trying to start an AsyncRead with a nullptr Buffer, on File: %ls.", file->GetNameW().c_str());
		return FileSysError_t::InputError;
	}
	if (bufferSize == 0)
	{
		LogManager::LogMessage(LL_WARN, "Trying to start an AsyncRead with an empty buffer, on File: %ls.", file->GetNameW().c_str());
		return FileSysError_t::InputError;
	}
	AsyncVec::iterator it;
	{
		const auto item = std::make_pair(FileAsync{ buffer, bufferSize, file }, AsyncRead);
		m_AsyncMutex.lock();
		m_AsyncOperations.emplace_back(item);
		m_AsyncMutex.unlock();
		m_AsyncMutex.lock_shared();
		for (it = m_AsyncOperations.begin(); it != m_AsyncOperations.end(); ++it)
		{
			if (it->first == item.first && it->second && item.second)
				break;
		}
		m_AsyncMutex.unlock_shared();
	}
	
	SendTask(CreateTask(AsyncReadTask, std::bind(&FileSystem::AsyncReadFn, this, it, beginReadFunc, endReadFunc)));
	LogManager::LogMessage(LL_INFO, "AsyncRead operation started on File: %ls.", file->GetNameW().c_str());
	return FileSysError_t::NoError;
}

CreateTaskName(AsyncWriteTask);

FileSysError_t FileSystem::StartAsyncWrite(File * file, void * buffer, const SIZET bufferSize, const std::function<void(FileAsync*)>& beginWriteFunc, const std::function<void(FileAsync*)>& endWriteFunc)
{
	if (!file)
	{
		LogManager::LogMessage(LL_ERRO, "Trying to start an AsyncWrite with a nullptr File.");
		return FileSysError_t::InputError;
	}
	if (!buffer)
	{
		LogManager::LogMessage(LL_ERRO, "Trying to start an AsyncWrite with a nullptr Buffer, on File: %ls.", file->GetNameW().c_str());
		return FileSysError_t::InputError;
	}
	if (bufferSize == 0)
	{
		LogManager::LogMessage(LL_WARN, "Trying to start an AsyncWrite with an empty buffer, on File: %ls.", file->GetNameW().c_str());
		return FileSysError_t::InputError;
	}
	AsyncVec::iterator it;
	{
		const auto item = std::make_pair(FileAsync{ buffer, bufferSize, file }, AsyncWrite);
		m_AsyncMutex.lock();
		m_AsyncOperations.emplace_back(item);
		m_AsyncMutex.unlock();
		m_AsyncMutex.lock_shared();
		for (it = m_AsyncOperations.begin(); it != m_AsyncOperations.end(); ++it)
			if (*it == item)
				break;
		m_AsyncMutex.unlock_shared();
	}

	SendTask(CreateTask(AsyncWriteTask, std::bind(&FileSystem::AsyncWriteFn, this, it, beginWriteFunc, endWriteFunc)));
	LogManager::LogMessage(LL_INFO, "AsyncWrite operation started on File: %ls.", file->GetNameW().c_str());
	return FileSysError_t::NoError;
}

FileSysError_t FileSystem::GetExternalFile(const std::string & filePathName, File *& file)
{
	return GetExternalFile(StringUtils::s2ws(filePathName), file);
}

FileSysError_t FileSystem::GetExternalFile(const std::wstring & filePathName, File *& file)
{
	if (filePathName.empty())
	{
		LogManager::LogMessage(LL_ERRO, "Trying to create a file structure from an existant file but the filePathName is empty.");
		return FileSysError_t::InputError;
	}
	if (!FileSystem::IsFile(filePathName))
	{
		LogManager::LogMessage(LL_ERRO, "Trying to create a file structure from an existant file but filePathName is not a File: %ls.", filePathName.c_str());
		return FileSysError_t::NotFound;
	}
	file = new File();
	file->m_Directory = nullptr;
	file->m_Handle = NullFileHandle;
	file->m_Name = std::move(filePathName);
	file->m_Permisions = FilePermisions_t::Closed;
	return FileSysError_t::NoError;
}

FileSysError_t FileSystem::GetExternalFile(const std::string & filePath, const std::string & fileName, File *& file)
{
	return GetExternalFile(StringUtils::s2ws(filePath), StringUtils::s2ws(fileName), file);
}

FileSysError_t FileSystem::GetExternalFile(const std::wstring & filePath, const std::wstring & fileName, File *& file)
{
	std::wstring output;
	if (filePath.empty() || !IsDirectory(filePath))
		output = GetExeDirectoryW();
	else
		output = filePath;

	if (fileName.empty())
	{
		LogManager::LogMessage(LL_ERRO, "Trying to create a File structure from an external file, but the file name was empty.");
		return FileSysError_t::InputError;
	}
	static const auto sepSize = wcslen(PathSeparatorW);
	if (output.compare(output.size() - sepSize, sepSize, PathSeparatorW, sepSize) != 0)
	{
		output.append(PathSeparatorW);
	}

	output += fileName;

	return GetExternalFile(output, file);
}

FileSysError_t FileSystem::CreateExternalFile(const std::string & filePath, const std::string & name, File *& file)
{
	return CreateExternalFile(StringUtils::s2ws(filePath), StringUtils::s2ws(name), file);
}

FileSysError_t FileSystem::CreateExternalFile(const std::wstring & filePath, const std::wstring & name, File *& file)
{
	std::wstring output;
	if (filePath.empty() || !IsDirectory(filePath))
		output = GetExeDirectoryW();
	else
		output = filePath;

	if (name.empty())
	{
		LogManager::LogMessage(LL_ERRO, "Trying to create a file and its File structure, but the file name was empty.");
		return FileSysError_t::InputError;
	}
	static const auto sepSize = wcslen(PathSeparatorW);
	if (output.compare(output.size() - sepSize, sepSize, PathSeparatorW, sepSize) != 0)
	{
		output.append(PathSeparatorW);
	}

	output += name;

	return CreateExternalFile(output, file);
}

FileSysError_t FileSystem::CreateExternalFile(const std::string & filePathName, File *& file)
{
	return CreateExternalFile(StringUtils::s2ws(filePathName), file);
}

FileSysError_t FileSystem::CreateExternalFile(const std::wstring & filePathName, File *& file)
{
	if (filePathName.empty())
	{
		LogManager::LogMessage(LL_ERRO, "Trying to create an external file and its File structure, but the filePathName is empty.");
		return FileSysError_t::InputError;
	}

	FileHandle handle = NullFileHandle;

	if (!FileSystem::IsFile(filePathName))
	{
#if PLATFORM_WINDOWS
		handle = CreateFileW(filePathName.c_str(), (FILE_GENERIC_READ | FILE_GENERIC_WRITE),
			0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (handle == NullFileHandle)
		{
			const auto err = GetLastError();
			if (err != ERROR_ALREADY_EXISTS && err != ERROR_FILE_EXISTS)
			{
				LogManager::LogMessage(LL_ERRO, "Trying to create an external file and its File structure, path: %ls, but unhandled error happened, error: 0x%08X.", filePathName.c_str(), err);
				return FileSysError_t::UnknownError;
			}
		}
#else
		/* TODO file creation */
#endif

	}
	file = new File();
	file->m_Directory = nullptr;
	file->m_Handle = handle;
	file->m_Name = std::move(filePathName);
	file->m_Permisions = handle == NullFileHandle ? FilePermisions_t::Closed : FilePermisions_t::ReadWrite;
	return FileSysError_t::NoError;
}

FileSysError_t FileSystem::EraseExternalFile(File*& file)
{
	if (!file)
	{
		LogManager::LogMessage(LL_ERRO, "Trying to erase an external file, but the File structure is nullptr.");
		return FileSysError_t::InputError;
	}
	file->Erase();
	file = nullptr;
	return FileSysError_t::NoError;
}

FileSysError_t FileSystem::DeleteExternalFile(File*& file)
{
	if (!file)
	{
		LogManager::LogMessage(LL_ERRO, "Trying to delete an external File, but the File structure is nullptr.");
		return FileSysError_t::InputError;
	}
	SAFE_DELETE(file);
	return FileSysError_t::NoError;
}

FileSysError_t gaf::FileSystem::GetExternalDir(const std::string & dirPath, const std::string & dirName, Directory *& dir)
{
	return GetExternalDir(StringUtils::s2ws(dirPath), StringUtils::s2ws(dirName), dir);
}

FileSysError_t gaf::FileSystem::GetExternalDir(const std::wstring & dirPath, const std::wstring & dirName, Directory *& dir)
{
	std::wstring output;
	if (dirPath.empty() || !IsDirectory(dirPath))
		output = GetExeDirectoryW();
	else
		output = dirPath;

	if (dirName.empty())
	{
		LogManager::LogMessage(LL_ERRO, "Trying to create a Directory structure from an external directory, but the directory name was empty.");
		return FileSysError_t::InputError;
	}
	static const auto sepSize = wcslen(PathSeparatorW);
	if (output.compare(output.size() - sepSize, sepSize, PathSeparatorW, sepSize) != 0)
	{
		output.append(PathSeparatorW);
	}
	
	output += dirName;

	return GetExternalDir(output, dir);
}

FileSysError_t FileSystem::CreateExternalDir(const std::string & dirNamePath, Directory *& dir)
{
	return CreateExternalDir(StringUtils::s2ws(dirNamePath), dir);
}

FileSysError_t FileSystem::CreateExternalDir(const std::wstring & dirNamePath, Directory *& dir)
{
	if (dirNamePath.empty())
	{
		LogManager::LogMessage(LL_ERRO, "Trying to create a directory and its Directory structure from an external directory, but the name and its path are empty.");
		return FileSysError_t::InputError;
	}
	if (!IsDirectory(dirNamePath))
	{
#if PLATFORM_WINDOWS
		if (!CreateDirectoryW(dirNamePath.c_str(), nullptr))
		{
			const auto err = GetLastError();
			if (err != ERROR_ALREADY_EXISTS)
			{
				LogManager::LogMessage(LL_ERRO, "Trying to create an external directory and its Directory structure, path: %ls, but something unexpected happened, error: 0x%08X.", dirNamePath.c_str(), err);
				return FileSysError_t::UnknownError;
			}
		}
#else
		/* TODO directory creation */
#endif
	}
	dir = new Directory();
	dir->m_Name = std::move(dirNamePath);
	dir->m_UpperDirectory = nullptr;
	return FileSysError_t::NoError;
}

FileSysError_t FileSystem::GetExternalDir(const std::string & dirNamePath, Directory *& dir)
{
	return GetExternalDir(StringUtils::s2ws(dirNamePath), dir);
}

FileSysError_t FileSystem::GetExternalDir(const std::wstring & dirNamePath, Directory *& dir)
{
	if (dirNamePath.empty())
	{
		LogManager::LogMessage(LL_ERRO, "Trying to create a Directory structure from an external directory, but the name is empty.");
		return FileSysError_t::InputError;
	}
	if (!FileSystem::IsDirectory(dirNamePath))
	{
		LogManager::LogMessage(LL_ERRO, "Trying to create a Directory structure from an external directory, path: %ls, but the dirNamePath is not a directory.", dirNamePath.c_str());
		return FileSysError_t::InputError;
	}
	dir = new Directory();
	dir->m_Name = std::move(dirNamePath);
	dir->m_UpperDirectory = nullptr;
	return FileSysError_t::NoError;
}

FileSysError_t FileSystem::CreateExternalDir(const std::string & dirPath, const std::string & name, Directory *& dir)
{
	return CreateExternalDir(StringUtils::s2ws(dirPath), StringUtils::s2ws(name), dir);
}

FileSysError_t FileSystem::CreateExternalDir(const std::wstring & dirPath, const std::wstring & name, Directory *& dir)
{
	std::wstring output;
	if (dirPath.empty() || !IsDirectory(dirPath))
		output = GetExeDirectoryW();
	else
		output = dirPath;

	if (name.empty())
	{
		LogManager::LogMessage(LL_ERRO, "Trying to create a directory and its Directory structure, but the directory name was empty.");
		return FileSysError_t::InputError;
	}
	static const auto sepSize = wcslen(PathSeparatorW);
	if (output.compare(output.size() - sepSize, sepSize, PathSeparatorW, sepSize) != 0)
	{
		output.append(PathSeparatorW);
	}

	output += name;

	return CreateExternalDir(output, dir);
}

FileSysError_t FileSystem::EraseExternalDir(Directory*& dir)
{
	if (!dir)
	{
		LogManager::LogMessage(LL_ERRO, "Trying to erase an external directory, but the Directory structure is nullptr");
		return FileSysError_t::InputError;
	}
	dir->Erase();
	dir = nullptr;
	return FileSysError_t::NoError;
}

FileSysError_t FileSystem::DeleteExternalDir(Directory*& dir)
{
	if (!dir)
	{
		LogManager::LogMessage(LL_ERRO, "Trying to erase a Directory structure, but it's nullptr");
		return FileSysError_t::InputError;
	}
	SAFE_DELETE(dir);
	return FileSysError_t::NoError;
}

bool FileSystem::IsDirectory(const std::string & path)
{
#if PLATFORM_WINDOWS
	const auto ftyp = GetFileAttributesW(StringUtils::s2ws(path).c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;
	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;
	return false;
#else
	/* TODO directory check */
#endif
}

bool FileSystem::IsDirectory(const std::wstring & path)
{
#if PLATFORM_WINDOWS
	const auto ftyp = GetFileAttributesW(path.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;
	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;
	return false;
#else
	/* TODO directory check */
#endif
}

bool FileSystem::IsFile(const std::string & path)
{
#if PLATFORM_WINDOWS
	const auto ftyp = GetFileAttributesW(StringUtils::s2ws(path).c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;
	if (ftyp & FILE_ATTRIBUTE_NORMAL)
		return true;
	return false;
#else
	/* TODO file check */
#endif
}

bool FileSystem::IsFile(const std::wstring & path)
{
#if PLATFORM_WINDOWS
	const auto ftyp = GetFileAttributesW(path.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;
	if (ftyp & FILE_ATTRIBUTE_NORMAL)
		return true;
	return false;
#else
	/* TODO file check */
#endif
}

Directory * FileSystem::GetRootDirectory() const
{
	return m_RootDir;
}

FileSysError_t FileSystem::GetDirectory(const std::string & name, Directory *& dir, const bool recursive)
{
	return GetDirectory(StringUtils::s2ws(name), dir, recursive);
}

FileSysError_t FileSystem::GetDirectory(const std::wstring & name, Directory *& dir, const bool recursive)
{
	dir = m_RootDir->ContainsDir(name, recursive);
	if (!dir)
		return FileSysError_t::NotFound;
	return FileSysError_t::NoError;
}

FileSysError_t FileSystem::GetFile(const std::string & name, File *& file, bool recursive)
{
	return GetFile(StringUtils::s2ws(name), file, recursive);
}

FileSysError_t FileSystem::GetFile(const std::wstring & name, File *& file, bool recursive)
{
	file = m_RootDir->ContainsFile(name, recursive);
	if (!file)
		return FileSysError_t::NotFound;
	return FileSysError_t::NoError;
}