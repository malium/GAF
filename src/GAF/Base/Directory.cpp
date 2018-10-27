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

#include "GAF/FileSystem.h"
#include "GAF/LogManager.h"
#include "GAF/Util/StringUtils.h"

using namespace gaf;

void Directory::Update()
{
	m_DirMutex.lock();
	m_FileMutex.lock();
#if PLATFORM_WINDOWS
	WIN32_FIND_DATAW fData;
	const auto pattern = GetFullPathW() + L"\\*.*";
	const auto hFile = FindFirstFileW(pattern.c_str(), &fData);
	std::wstring name;
	name.reserve(ARRAY_SIZE(fData.cFileName) + 1);
	if (hFile != NullFileHandle)
	{
		do
		{
			name.assign(fData.cFileName, wcsnlen(fData.cFileName, ARRAY_SIZE(fData.cFileName)));
			if (name == L"." || name == L"..")
				continue;
			if (fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				auto tmpDir = new Directory();
				tmpDir->m_Name = name;
				tmpDir->m_UpperDirectory = this;
				m_NestedDirs.emplace_back(tmpDir);
			}
			else
			{
				auto tmpFile = new File();
				tmpFile->m_Directory = this;
				tmpFile->m_Handle = NullFileHandle;
				tmpFile->m_Name = name;
				tmpFile->m_Permisions = FilePermisions_t::Closed;
				m_NestedFiles.emplace_back(tmpFile);
			}
		} while (FindNextFileW(hFile, &fData) != 0);

		FindClose(hFile);

		const auto err = GetLastError();
		if (err != ERROR_NO_MORE_FILES)
		{
			LogManager::LogMessage(LL_WARN, "Trying to create the FileSystem hierachy, from the dir: %ls, but something unexpected went wrong at end, error: 0x%08X.", m_Name.c_str(), err);
		}
	}
#else
	/* TODO fileSystem hierachy */
#endif
	m_DirMutex.unlock();
	m_FileMutex.unlock();
	m_DirMutex.lock_shared();
	for (auto it = m_NestedDirs.begin(); it != m_NestedDirs.end(); ++it)
		(*it)->Update();
	m_DirMutex.unlock_shared();
}

Directory::~Directory()
{
	if (!m_NestedFiles.empty())
	{
		for (auto it = m_NestedFiles.begin(); it != m_NestedFiles.end(); ++it)
			delete *it;
	}
	if (!m_NestedDirs.empty())
	{
		for (auto it = m_NestedDirs.begin(); it != m_NestedDirs.end(); ++it)
			delete *it;
	}
}

FileSysError_t Directory::AddDir(const std::string& dirName, Directory*& dir)
{
	return AddDir(StringUtils::s2ws(dirName), dir);
}

FileSysError_t Directory::AddDir(const std::wstring & dirName, Directory*& dir)
{
	dir = ContainsDir(dirName, false);
	if (!dir)
	{
#if PLATFORM_WINDOWS
		if (!CreateDirectoryW((GetFullPathW() + FileSystem::PathSeparatorW + dirName).c_str(), nullptr))
		{
			const auto err = GetLastError();
			if ((err != ERROR_ALREADY_EXISTS && err != ERROR_FILE_EXISTS))
			{
				LogManager::LogMessage(LL_ERRO, "Trying to add a directory inside another one, parentDir: %ls, newDir: %ls, but something went wrong, error: 0x%08X.", m_Name.c_str(), dirName.c_str(), err);
				return FileSysError_t::UnknownError;
			}
		}
#else
		/* TODO directory creation */
#endif
		dir = new Directory();
		dir->m_Name = dirName;
		dir->m_UpperDirectory = this;
		m_DirMutex.lock();
		m_NestedDirs.emplace_back(dir);
		m_DirMutex.unlock();
		return FileSysError_t::NoError;
	}
	LogManager::LogMessage(LL_WARN, "Trying to create an already existant directory, parentDir: %ls, newDir: %ls, returning that directory.", m_Name.c_str(), dirName.c_str());
	return FileSysError_t::NoError;
}

FileSysError_t Directory::AddFile(const std::string & fileName, File*& file)
{
	return AddFile(StringUtils::s2ws(fileName), file);
}

FileSysError_t Directory::AddFile(const std::wstring & fileName, File*& file)
{
	file = ContainsFile(fileName, false);
	if (!file)
	{

#if PLATFORM_WINDOWS
		const auto path = GetFullPathW() + FileSystem::PathSeparatorW + fileName;
		auto handle = CreateFileW(path.c_str(), (FILE_GENERIC_READ | FILE_GENERIC_WRITE),
			0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
		const auto err = GetLastError();
		if (handle == NullFileHandle && (err != ERROR_FILE_EXISTS && err != ERROR_ALREADY_EXISTS))
		{
			LogManager::LogMessage(LL_ERRO, "Trying to create a file inside a directory, parentDir: %ls, fileName: %ls, but an unhandled error happened, error: 0x%08X.", m_Name.c_str(), fileName.c_str(), err);
			return FileSysError_t::UnknownError;
		}

		file = new File();
		file->m_Handle = handle;
#else
		/* TODO file creation */
#endif
		file->m_Name = fileName;
		file->m_Directory = this;
		file->m_Permisions = file->m_Handle != NullFileHandle ? FilePermisions_t::ReadWrite : FilePermisions_t::Closed;
		m_FileMutex.lock();
		m_NestedFiles.emplace_back(file);
		m_FileMutex.unlock();
		return FileSysError_t::NoError;
	}
	LogManager::LogMessage(LL_WARN, "Trying to create an already existant file, parentDir: %ls, fileName: %ls, returning that file.", m_Name.c_str(), fileName.c_str());
	return FileSysError_t::NoError;
}

FileSysError_t Directory::ChangeName(const std::string & name)
{
	return ChangeName(StringUtils::s2ws(name));
}

FileSysError_t Directory::ChangeName(const std::wstring & name)
{
	if (name.empty())
	{
		LogManager::LogMessage(LL_ERRO, "Trying to change the directory name, oldName: %ls, but the new name is empty.", m_Name.c_str());
		return FileSysError_t::InputError;
	}
#if PLATFORM_WINDOWS
	const auto path = GetPathW();
	if (!MoveFileW((path + GetNameW()).c_str(), (path + name).c_str()))
	{
		LogManager::LogMessage(LL_ERRO, "Trying to change the directory name, oldName: %ls, but something went wrong, error: 0x%08X.", m_Name.c_str(), GetLastError());
		return FileSysError_t::UnknownError;
	}
#else
	/* TODO directory renaming */
#endif
	m_Name = name;
	return FileSysError_t::NoError;
}

void Directory::Erase()
{
	m_FileMutex.lock();
	if (!m_NestedFiles.empty())
	{
		for (auto it = m_NestedFiles.begin(); it != m_NestedFiles.end(); ++it)
			(*it)->Erase();

		m_NestedFiles.clear();
	}
	m_FileMutex.unlock();
	m_DirMutex.lock();
	if (!m_NestedDirs.empty())
	{
		for (auto it = m_NestedDirs.begin(); it != m_NestedDirs.end(); ++it)
			(*it)->Erase();
	}
	m_DirMutex.unlock();
	if (m_UpperDirectory)
	{
		bool found = false;
		m_UpperDirectory->m_DirMutex.lock_shared();
		for (auto it = m_UpperDirectory->GetDirectoryListBegin();
			it != m_UpperDirectory->GetDirectoryListEnd(); ++it)
		{
			if ((*it) == this)
			{
				m_UpperDirectory->m_DirMutex.unlock_shared();
				m_UpperDirectory->m_DirMutex.lock();
				m_UpperDirectory->m_NestedDirs.erase(it);
				m_UpperDirectory->m_DirMutex.unlock();
				found = true;
				break;
			}
		}
		if (!found)
		{
			m_UpperDirectory->m_DirMutex.unlock_shared();
			LogManager::LogMessage(LL_WARN, "Trying to erase a directory from its upper one, dirName: %ls, parentDir: %ls, but it was not found there.", m_Name.c_str(), m_UpperDirectory->m_Name.c_str());
		}
	}

#if PLATFORM_WINDOWS
	if (!RemoveDirectoryW(GetFullPathW().c_str()))
	{
		LogManager::LogMessage(LL_ERRO, "Trying to delete a directory, dirName: %ls, but something unhandled happened, error: 0x%08X.", m_Name.c_str(), GetLastError());
		delete this;
		return;
	}
#else
	/* TODO erase physical directory */
#endif
	delete this;
}

SIZET Directory::GetNumFiles()
{
	m_FileMutex.lock_shared();
	const auto num = m_NestedFiles.size();
	m_FileMutex.unlock_shared();
	return num;
}

SIZET Directory::GetNumDirs()
{
	m_DirMutex.lock_shared();
	const auto num = m_NestedDirs.size();
	m_DirMutex.unlock_shared();
	return num;
}

FileSysError_t Directory::GetCreationTime(DayTime & time) const
{
#if PLATFORM_WINDOWS
	const auto path = GetFullPathW();
	WIN32_FILE_ATTRIBUTE_DATA fad;
	if (!GetFileAttributesExW(path.c_str(), GET_FILEEX_INFO_LEVELS::GetFileExInfoStandard, &fad))
	{
		LogManager::LogMessage(LL_ERRO, "Trying to get the CreationTime of directory, name: %ls, but an unhandled error happened, error: 0x%08X.", m_Name.c_str(), GetLastError());
		return FileSysError_t::UnknownError;
	}
	SYSTEMTIME st;
	if (!FileTimeToSystemTime(&fad.ftCreationTime, &st))
	{
		LogManager::LogMessage(LL_ERRO, "Trying to get the CreationTime of directory, name: %ls, but an unhandled error happened, error: 0x%08X.", m_Name.c_str(), GetLastError());
		return FileSysError_t::UnknownError;
	}
	time.Set(st.wMilliseconds, st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth, st.wYear);
#else

#endif
	return FileSysError_t::NoError;
}

Directory* Directory::ContainsDir(const std::string& dirName, bool recursive)
{
	return ContainsDir(StringUtils::s2ws(dirName), recursive);
}

Directory* Directory::ContainsDir(const std::wstring& dirName, bool recursive)
{
	Directory* rtn = nullptr;
	m_DirMutex.lock_shared();
	for (auto it = m_NestedDirs.begin(); it != m_NestedDirs.end(); ++it)
	{
		if ((*it)->GetNameW() == dirName)
		{
			rtn = *it;
			break;
		}
		else if (recursive)
		{
			rtn = (*it)->ContainsDir(dirName, recursive);
			if (rtn)
				break;
		}
	}
	m_DirMutex.unlock_shared();
	return rtn;
}

File* Directory::ContainsFile(const std::string& fileName, bool recursive)
{
	return ContainsFile(StringUtils::s2ws(fileName), recursive);
}

File* Directory::ContainsFile(const std::wstring& fileName, bool recursive)
{
	File* rtn = nullptr;
	m_FileMutex.lock_shared();
	for (auto it = m_NestedFiles.begin(); it != m_NestedFiles.end(); ++it)
	{
		if ((*it)->GetNameW() == fileName)
		{
			rtn = *it;
			break;
		}
	}
	m_FileMutex.unlock_shared();

	if (!rtn && recursive)
	{
		m_DirMutex.lock_shared();
		for (auto it = m_NestedDirs.begin(); it != m_NestedDirs.end(); ++it)
		{
			rtn = (*it)->ContainsFile(fileName, recursive);
			if (rtn)
				break;
		}
		m_DirMutex.unlock_shared();
	}

	return rtn;
}

FileList::iterator Directory::GetFileListBegin()
{
	return m_NestedFiles.begin();
}

FileList::iterator Directory::GetFileListEnd()
{
	return m_NestedFiles.end();
}

void Directory::LockFileListRead()
{
	m_FileMutex.lock_shared();
}

void Directory::UnlockFileListRead()
{
	m_FileMutex.unlock_shared();
}

DirList::iterator Directory::GetDirectoryListBegin()
{
	return m_NestedDirs.begin();
}

DirList::iterator Directory::GetDirectoryListEnd()
{
	return m_NestedDirs.end();
}

void Directory::LockDirListRead()
{
	m_DirMutex.lock_shared();
}

void Directory::UnlockDirListRead()
{
	m_DirMutex.unlock_shared();
}

Directory * Directory::GetParentDirectory() const
{
	return m_UpperDirectory;
}

std::string Directory::GetFullPath() const
{
	return StringUtils::ws2s(GetFullPathW());
}

std::wstring Directory::GetFullPathW() const
{
	if (m_UpperDirectory)
	{
		std::wstring path(m_Name);
		Directory* current = m_UpperDirectory;
		while (current)
		{
			path = current->GetNameW() + FileSystem::PathSeparatorW + path;
			if (!current->GetParentDirectory())
			{
				path = current->GetPathW() + path;
			}
			current = current->GetParentDirectory();
		}
		return path;
	}
	return m_Name;
}

std::string Directory::GetPath() const
{
	return StringUtils::ws2s(GetPathW());
}

std::wstring Directory::GetPathW() const
{
	if (m_UpperDirectory)
	{
		std::wstring path(m_UpperDirectory->GetNameW() + FileSystem::PathSeparatorW);
		Directory* current = m_UpperDirectory->GetParentDirectory();
		if (!current)
		{
			return m_UpperDirectory->GetPathW() + path;
		}
		while (current)
		{
			path = current->GetNameW() + FileSystem::PathSeparatorW + path;
			current = current->GetParentDirectory();
		}
		return path;
	}
	const auto lastSlash = m_Name.find_last_of(FileSystem::PathSeparatorW);
	return m_Name.substr(0, lastSlash);
}

std::string Directory::GetName() const
{
	return StringUtils::ws2s(GetNameW());
}

std::wstring Directory::GetNameW() const
{
	if (m_UpperDirectory)
		return m_Name;
	const auto lastSlash = m_Name.find_last_of(FileSystem::PathSeparatorW);
	return m_Name.substr(lastSlash);
}