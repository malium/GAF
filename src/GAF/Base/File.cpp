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

const std::string & gaf::GetFileErrorStr(const FileSysError_t err)
{
	static const std::string fileError[] =
	{
		"NoError",
		"InputError",
		"NotFound",
		"UnknownError"
	};
	return fileError[err];
}

const std::string & gaf::GetFilePermisionsStr(const FilePermisions_t perm)
{
	static const std::string filePerm[] =
	{
		"Closed",
		"ReadOnly",
		"ReadWrite",
		"WriteAppend"
	};
	return filePerm[perm];
}

File::~File()
{
	Open(FilePermisions_t::Closed);
}

FileSysError_t File::Open(const FilePermisions_t perm)
{
	if (m_Handle == NullFileHandle)
		m_Permisions = FilePermisions_t::Closed;
	if (m_Permisions == perm)
		return FileSysError_t::NoError;
	m_Mutex.lock();
	/* FileOpen */
	if (m_Permisions == FilePermisions_t::Closed)
	{
#if PLATFORM_WINDOWS
		m_Handle = CreateFileW(GetFullPathW().c_str(),
			perm == FilePermisions_t::ReadOnly ? FILE_GENERIC_READ :
			(FILE_GENERIC_READ | FILE_GENERIC_WRITE), 0, nullptr, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, nullptr);
		if (m_Handle == NullFileHandle)
		{
			m_Mutex.unlock();
			LogManager::LogMessage(LL_ERRO, "Trying to open a file, name: %ls, but an unexpected error happened, error: 0x%08X.", m_Name.c_str(), GetLastError());
			return FileSysError_t::UnknownError;
		}
#else
		/* TODO file open */
#endif
		m_Permisions = perm;
	}
	else
	{
		/* FileClose */
		if (perm == FilePermisions_t::Closed)
		{
			if (m_Handle == NullFileHandle)
			{
				m_Mutex.unlock();
				return FileSysError_t::NoError;
			}
#if PLATFORM_WINDOWS
			if (!CloseHandle(m_Handle))
			{
				m_Handle = NullFileHandle;
				m_Permisions = FilePermisions_t::Closed;
				m_Mutex.unlock();
				LogManager::LogMessage(LL_ERRO, "Trying to close a file, name: %ls, but something unexpected happened, error: 0x%08X.", m_Name.c_str(), GetLastError());
				return FileSysError_t::UnknownError;
			}
#else
			/* TODO close file handle */
#endif
			m_Handle = NullFileHandle;
			m_Permisions = FilePermisions_t::Closed;
		}
		else /* FileReopen */
		{
			Open(FilePermisions_t::Closed);
			Open(perm);
		}
	}
	m_Mutex.unlock();
	return FileSysError_t::NoError;
}

FileSysError_t File::ChangeName(const std::string & name)
{
	return ChangeName(StringUtils::s2ws(name));
}

FileSysError_t File::ChangeName(const std::wstring & wname)
{
	std::wstring from;
	std::wstring to;
	FilePermisions_t oldPerm = FilePermisions_t::Closed;
	FileSysError_t fsErr = FileSysError_t::NoError;
	m_Mutex.lock();
	if (m_Permisions != FilePermisions_t::Closed)
	{
		oldPerm = m_Permisions;
		fsErr = Open(FilePermisions_t::Closed);
		if (fsErr != FileSysError_t::NoError)
		{
			m_Mutex.unlock();
			return fsErr;
		}
	}
	if (m_Directory)
	{
		const auto path = GetPathW();
		from = path + m_Name;
		to = path + wname;
	}
	else
	{
		from = m_Name;
		const auto lastSlash = m_Name.find_last_of(FileSystem::PathSeparatorW);
		to = m_Name.substr(0, lastSlash) + wname;
	}

#if PLATFORM_WINDOWS
	if (!MoveFileW(from.c_str(), to.c_str()))
	{
		m_Mutex.unlock();
		LogManager::LogMessage(LL_ERRO, "Trying to change the filename of a file, from: %ls, to:%ls, but an unhandled error happened, error: 0x%08X.", from.c_str(), to.c_str(), GetLastError());
		return FileSysError_t::UnknownError;
	}
#else
	/* TODO file name change */
#endif
	m_Name = wname;
	if (oldPerm != FilePermisions_t::Closed)
	{
		fsErr = Open(oldPerm);
		if (fsErr != FileSysError_t::NoError)
		{
			m_Mutex.unlock();
			return fsErr;
		}
	}
	m_Mutex.unlock();
	return FileSysError_t::NoError;
}

void File::ClearFile()
{
	FileSysError_t fsErr = FileSysError_t::NoError;
	m_Mutex.lock();
	const auto oldPermisions = m_Permisions;
	if (m_Permisions != FilePermisions_t::Closed)
	{
		fsErr = Open(FilePermisions_t::Closed);
	}
	if (fsErr != FileSysError_t::NoError)
	{
		m_Mutex.unlock();
		return;
	}
	/* File open truncated */
#if PLATFORM_WINDOWS
	const auto path = GetFullPathW();
	m_Handle = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, TRUNCATE_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (m_Permisions != FilePermisions_t::ReadOnly)
		m_Permisions = FilePermisions_t::ReadWrite;
	if (m_Handle == NullFileHandle)
	{
		m_Mutex.unlock();
		LogManager::LogMessage(LL_ERRO, "Trying to clear a file, name: %ls, but an unhandled error happened, error: 0x%08X.", m_Name.c_str(), GetLastError());
		return;
	}
#else
	/* TODO file truncation */
#endif
	if (oldPermisions == FilePermisions_t::Closed)
	{
		Open(FilePermisions_t::Closed);
	}
	m_Mutex.unlock();
}

FileSysError_t File::GetCreationTime(DayTime& time)
{
	FileSysError_t fsErr = FileSysError_t::NoError;
	bool closeAfter = false;
	m_Mutex.lock();
	if (m_Permisions == FilePermisions_t::Closed)
	{
		closeAfter = true;
		fsErr = Open(FilePermisions_t::ReadOnly);
	}
	if (fsErr != FileSysError_t::NoError)
	{
		m_Mutex.unlock();
		return fsErr;
	}
#if PLATFORM_WINDOWS
	FILETIME ft;
	if (!GetFileTime(m_Handle, &ft, nullptr, nullptr))
	{
		m_Mutex.unlock();
		LogManager::LogMessage(LL_ERRO, "Trying to get the CreationTime of a file, name: %ls, but an unexpected error happened, error: 0x%08X.", m_Name.c_str(), GetLastError());
		return FileSysError_t::UnknownError;
	}
	SYSTEMTIME st;
	if (!FileTimeToSystemTime(&ft, &st))
	{
		m_Mutex.unlock();
		LogManager::LogMessage(LL_ERRO, "Trying to get the CreationTime of a file, name: %ls, but an unexpected error happened while converting the FILETIME to SYSTIME, error: 0x%08X.", m_Name.c_str(), GetLastError());
		return FileSysError_t::UnknownError;
	}
	time.Set(st.wMilliseconds, st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth, st.wYear);
#else
	/* TODO file get creation time */
#endif
	if (closeAfter)
	{
		fsErr = Open(FilePermisions_t::Closed);
		if (fsErr != FileSysError_t::NoError)
		{
			m_Mutex.unlock();
			return fsErr;
		}
	}
	m_Mutex.unlock();
	return FileSysError_t::NoError;
}

FileSysError_t File::GetLastAccessTime(DayTime & time)
{
	FileSysError_t fsErr = FileSysError_t::NoError;
	bool closeAfter = false;
	m_Mutex.lock();
	if (m_Permisions == FilePermisions_t::Closed)
	{
		closeAfter = true;
		fsErr = Open(FilePermisions_t::ReadOnly);
	}
	if (fsErr != FileSysError_t::NoError)
	{
		m_Mutex.unlock();
		return fsErr;
	}
#if PLATFORM_WINDOWS
	FILETIME ft;
	if (!GetFileTime(m_Handle, nullptr, &ft, nullptr))
	{
		m_Mutex.unlock();
		LogManager::LogMessage(LL_ERRO, "Trying to get the LastAccessTime of a file, name: %ls, but an unexpected error happened, error: 0x%08X.", m_Name.c_str(), GetLastError());
		return FileSysError_t::UnknownError;
	}
	SYSTEMTIME st;
	if (!FileTimeToSystemTime(&ft, &st))
	{
		m_Mutex.unlock();
		LogManager::LogMessage(LL_ERRO, "Trying to get the LastAccessTime of a file, name: %ls, but an unexpected error happened while converting the FILETIME to SYSTIME, error: 0x%08X.", m_Name.c_str(), GetLastError());
		return FileSysError_t::UnknownError;
	}
	time.Set(st.wMilliseconds, st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth, st.wYear);
#else
	/* TODO file get creation time */
#endif
	if (closeAfter)
	{
		fsErr = Open(FilePermisions_t::Closed);
		if (fsErr != FileSysError_t::NoError)
		{
			m_Mutex.unlock();
			return fsErr;
		}
	}
	m_Mutex.unlock();
	return FileSysError_t::NoError;
}

FileSysError_t File::GetLastWriteTime(DayTime & time)
{
	FileSysError_t fsErr = FileSysError_t::NoError;
	bool closeAfter = false;
	m_Mutex.lock();
	if (m_Permisions == FilePermisions_t::Closed)
	{
		closeAfter = true;
		fsErr = Open(FilePermisions_t::ReadOnly);
	}
	if (fsErr != FileSysError_t::NoError)
	{
		m_Mutex.unlock();
		return fsErr;
	}
#if PLATFORM_WINDOWS
	FILETIME ft;
	if (!GetFileTime(m_Handle, nullptr, nullptr, &ft))
	{
		m_Mutex.unlock();
		LogManager::LogMessage(LL_ERRO, "Trying to get the LastWriteTime of a file, name: %ls, but an unexpected error happened, error: 0x%08X.", m_Name.c_str(), GetLastError());
		return FileSysError_t::UnknownError;
	}
	SYSTEMTIME st;
	if (!FileTimeToSystemTime(&ft, &st))
	{
		m_Mutex.unlock();
		LogManager::LogMessage(LL_ERRO, "Trying to get the LastWriteTime of a file, name: %ls, but an unexpected error happened while converting the FILETIME to SYSTIME, error: 0x%08X.", m_Name.c_str(), GetLastError());
		return FileSysError_t::UnknownError;
	}
	time.Set(st.wMilliseconds, st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth, st.wYear);
#else
	/* TODO file get creation time */
#endif
	if (closeAfter)
	{
		fsErr = Open(FilePermisions_t::Closed);
		if (fsErr != FileSysError_t::NoError)
		{
			m_Mutex.unlock();
			return fsErr;
		}
	}
	m_Mutex.unlock();
	return FileSysError_t::NoError;
}

FileSysError_t File::GetSize(SIZET & size)
{
	FileSysError_t fsErr = FileSysError_t::NoError;
	bool closeAfter = false;
	m_Mutex.lock();
	if (m_Permisions == FilePermisions_t::Closed)
	{
		closeAfter = true;
		fsErr = Open(FilePermisions_t::ReadWrite);
	}
	if (fsErr != FileSysError_t::NoError)
	{
		m_Mutex.unlock();
		return fsErr;
	}
#if PLATFORM_WINDOWS
	LARGE_INTEGER li;
	if (!GetFileSizeEx(m_Handle, &li))
	{
		m_Mutex.unlock();
		LogManager::LogMessage(LL_ERRO, "Trying to get the file size, name: %ls, but something unexpected happened, error: 0x%08X.", m_Name.c_str(), GetLastError());
		return FileSysError_t::UnknownError;
	}
	size = (SIZET)li.QuadPart;
#else

#endif
	if (closeAfter)
	{
		fsErr = Open(FilePermisions_t::Closed);
		if (fsErr != FileSysError_t::NoError)
		{
			m_Mutex.unlock();
			return fsErr;
		}
	}
	m_Mutex.unlock();
	return FileSysError_t::NoError;
}

FileSysError_t File::LoadContents(void* buffer, const SIZET bufferByteSize, SIZET& readBytes)
{
	if (bufferByteSize == 0)
	{
		readBytes = 0;
		return FileSysError_t::NoError;
	}
	if (!buffer)
	{
		LogManager::LogMessage(LL_ERRO, "Trying to load the contents of a file, name: %ls, into a buffer, but the buffer was nullptr.", m_Name.c_str());
		return FileSysError_t::InputError;
	}
	bool closeAfter = false;
	FileSysError_t fsErr = FileSysError_t::NoError;
	SIZET offset, fileSize;
	m_Mutex.lock();
	if (m_Permisions == FilePermisions_t::Closed)
	{
		closeAfter = true;
		fsErr = Open(FilePermisions_t::ReadOnly);
	}
	if (fsErr != FileSysError_t::NoError)
	{
		m_Mutex.unlock();
		return fsErr;
	}
	fsErr = GetSize(fileSize);
	if (fsErr != FileSysError_t::NoError)
	{
		m_Mutex.unlock();
		return fsErr;
	}
	fsErr = GetOffset(offset);
	if (fsErr != FileSysError_t::NoError)
	{
		m_Mutex.unlock();
		return fsErr;
	}
	const auto readSize = Min(fileSize - offset, bufferByteSize);
	if (m_Permisions == FilePermisions_t::Closed)
		fsErr = Open(FilePermisions_t::ReadOnly);
	if (fsErr != FileSysError_t::NoError)
	{
		m_Mutex.unlock();
		return fsErr;
	}
#if PLATFORM_WINDOWS
	DWORD bytesRead;
	if (!ReadFile(m_Handle, buffer, (DWORD)readSize, &bytesRead, nullptr))
	{
		m_Mutex.unlock();
		LogManager::LogMessage(LL_ERRO, "Trying to load the contents of a file, name: %ls, but an unexpected error happened, error: 0x%08X.", m_Name.c_str(), GetLastError());
		return FileSysError_t::UnknownError;
	}
	readBytes = (SIZET)bytesRead;
#else
	/* TODO file read */
#endif
	if (closeAfter)
	{
		fsErr = Open(FilePermisions_t::Closed);
		if (fsErr != FileSysError_t::NoError)
		{
			m_Mutex.unlock();
			return fsErr;
		}
	}
	m_Mutex.unlock();
	return FileSysError_t::NoError;
}

FileSysError_t File::LoadContents(void * buffer, const SIZET bufferByteSize, const SIZET fileOffset, SIZET & readBytes)
{
	m_Mutex.lock();
	auto fsErr = SetOffset(fileOffset);
	if (fsErr != EFileSysError::NoError)
	{
		m_Mutex.unlock();
		return fsErr;
	}
	fsErr = LoadContents(buffer, bufferByteSize, readBytes);
	m_Mutex.unlock();
	return fsErr;
}

FileSysError_t File::StoreContents(void * const buffer, const SIZET bufferByteSize, SIZET & writtenBytes)
{
	if (bufferByteSize == 0)
	{
		writtenBytes = 0;
		return FileSysError_t::NoError;
	}
	if (!buffer)
	{
		LogManager::LogMessage(LL_ERRO, "Trying to write the contents of a buffer into a file, name: %ls, but the buffer was nullptr.", m_Name.c_str());
		return FileSysError_t::InputError;
	}
	bool closeAfter = false;
	FileSysError_t fsErr = FileSysError_t::NoError;
	m_Mutex.lock();
	if (m_Permisions == FilePermisions_t::Closed)
	{
		closeAfter = true;
		fsErr = Open(FilePermisions_t::ReadWrite);
	}
	if (fsErr != FileSysError_t::NoError)
	{
		m_Mutex.unlock();
		return fsErr;
	}

	if (m_Permisions != FilePermisions_t::ReadWrite)
		fsErr = Open(FilePermisions_t::ReadWrite);
	if (fsErr != FileSysError_t::NoError)
	{
		m_Mutex.unlock();
		return fsErr;
	}
#if PLATFORM_WINDOWS
	DWORD written;
	if (!WriteFile(m_Handle, buffer, (DWORD)bufferByteSize, &written, nullptr))
	{
		m_Mutex.unlock();
		LogManager::LogMessage(LL_ERRO, "Trying to store the contents of a buffer into a file, name: %ls, but an unexpected error happened, error: 0x%08X.", m_Name.c_str(), GetLastError());
		return FileSysError_t::UnknownError;
	}
	writtenBytes = (SIZET)written;
#else
	/* TODO file write */
#endif
	if (closeAfter)
	{
		fsErr = Open(FilePermisions_t::Closed);
		if (fsErr != FileSysError_t::NoError)
		{
			m_Mutex.unlock();
			return fsErr;
		}
	}
	m_Mutex.unlock();
	return FileSysError_t::NoError;
}

FileSysError_t File::StoreContents(void * const buffer, SIZET bufferByteSize, SIZET fileOffset, SIZET & writtenBytes)
{
	m_Mutex.lock();
	auto fsErr = SetOffset(fileOffset);
	if (fsErr != EFileSysError::NoError)
	{
		m_Mutex.unlock();
		return fsErr;
	}
	fsErr = StoreContents(buffer, bufferByteSize, writtenBytes);
	m_Mutex.unlock();
	return fsErr;
}

FilePermisions_t File::GetPermisions()const
{
	return m_Permisions;
}

FileHandle File::GetOSHandle()const
{
	return m_Handle;
}

FileSysError_t File::GetOffset(SIZET & offset)
{
	FileSysError_t fsErr = FileSysError_t::NoError;
	bool closeAfter = false;
	m_Mutex.lock();
	if (m_Permisions == FilePermisions_t::Closed)
	{
		closeAfter = true;
		fsErr = Open(FilePermisions_t::ReadOnly);
	}
	if (fsErr != FileSysError_t::NoError)
	{
		m_Mutex.unlock();
		return fsErr;
	}
#if PLATFORM_WINDOWS
	LARGE_INTEGER li;
	if (!SetFilePointerEx(m_Handle, LARGE_INTEGER{ 0 }, &li, FILE_CURRENT))
	{
		m_Mutex.unlock();
		LogManager::LogMessage(LL_ERRO, "Trying to get the offset of a file, name: %ls, but an unexpected error happened, error: 0x%08X.", m_Name.c_str(), GetLastError());
		return FileSysError_t::UnknownError;
	}
	offset = (SIZET)li.QuadPart;
#else
	/* TODO offset get */
#endif
	if (closeAfter)
	{
		fsErr = Open(FilePermisions_t::Closed);
		if (fsErr != FileSysError_t::NoError)
		{
			m_Mutex.unlock();
			return fsErr;
		}
	}
	m_Mutex.unlock();
	return FileSysError_t::NoError;
}

FileSysError_t File::SetOffset(const SIZET offset)
{
	FileSysError_t fsErr = FileSysError_t::NoError;
	bool closeAfter = false;
	m_Mutex.lock();
	if (m_Permisions == FilePermisions_t::Closed)
	{
		closeAfter = true;
		fsErr = Open(FilePermisions_t::ReadOnly);
	}
	if (fsErr != FileSysError_t::NoError)
	{
		m_Mutex.unlock();
		return fsErr;
	}
	SIZET current;
	fsErr = GetOffset(current);
	if (fsErr != FileSysError_t::NoError)
	{
		m_Mutex.unlock();
		return fsErr;
	}
#if PLATFORM_WINDOWS
	LARGE_INTEGER li;
	LARGE_INTEGER lo;
	
	lo.QuadPart = (LONGLONG)(offset == OffsetBegin ? 0 : offset == OffsetEnd ? 0 : (offset - current));
	if (!SetFilePointerEx(m_Handle, lo, &li,
		offset == OffsetBegin ? FILE_BEGIN :
		offset == OffsetEnd ? FILE_END :
		FILE_CURRENT))
	{
		m_Mutex.unlock();
		LogManager::LogMessage(LL_ERRO, "Trying to get the offset of a file, name: %ls, but an unexpected error happened, error: 0x%08X.", m_Name.c_str(), GetLastError());
		return FileSysError_t::UnknownError;
	}
#else
	/* TODO offset get */
#endif
	if (closeAfter)
	{
		fsErr = Open(FilePermisions_t::Closed);
		if (fsErr != FileSysError_t::NoError)
		{
			m_Mutex.unlock();
			return fsErr;
		}
	}
	m_Mutex.unlock();
	return FileSysError_t::NoError;
}

void File::Erase()
{
#if PLATFORM_WINDOWS
	m_Mutex.lock();
	if (!DeleteFileW(GetFullPathW().c_str()))
	{
		LogManager::LogMessage(LL_ERRO, "Trying to erase a physical file, name: %ls, but something unexpected happened, error: 0x%08X.", m_Name.c_str(), GetLastError());
	}
	m_Handle = NullFileHandle;
#else
	/* TODO file physical erase */
#endif
	if (m_Directory)
	{
		bool found = false;
		m_Directory->m_FileMutex.lock_shared();
		for (auto it = m_Directory->m_NestedFiles.begin();
			it != m_Directory->m_NestedFiles.end(); ++it)
		{
			if ((*it) == this)
			{
				m_Directory->m_FileMutex.unlock_shared();
				m_Directory->m_FileMutex.lock();
				m_Directory->m_NestedFiles.erase(it);
				m_Directory->m_FileMutex.unlock();
				found = true;
				break;
			}
		}
		if (!found)
			m_Directory->m_FileMutex.unlock_shared();
	}
	m_Mutex.unlock();
	delete this;
}

FileSysError_t File::Resize(const SIZET sz)
{
	bool closeAfter = false;
	FileSysError_t fsErr = FileSysError_t::NoError;
	m_Mutex.lock();
	if (m_Permisions != FilePermisions_t::ReadWrite)
	{
		closeAfter = m_Permisions == FilePermisions_t::Closed;
		fsErr = Open(FilePermisions_t::ReadWrite);
	}
	if (fsErr != FileSysError_t::NoError)
	{
		m_Mutex.unlock();
		return fsErr;
	}

	SIZET size;
	fsErr = GetSize(size);
	if (fsErr != FileSysError_t::NoError)
	{
		m_Mutex.unlock();
		return fsErr;
	}

	if (sz > size)
		fsErr = Reserve(sz + 1);
	if (fsErr != FileSysError_t::NoError)
	{
		m_Mutex.unlock();
		return fsErr;
	}

#if PLATFORM_WINDOWS
	if (!SetFileValidData(m_Handle, (LONGLONG)sz))
	{
		m_Mutex.unlock();
		LogManager::LogMessage(LL_ERRO, "Trying to resize a file, name: %ls, but something unexpected happened, error: 0x%08X.", m_Name.c_str(), GetLastError());
		return FileSysError_t::UnknownError;
	}
#else
	/* File resize */
#endif
	if (closeAfter)
	{
		fsErr = Open(FilePermisions_t::Closed);
		if (fsErr != FileSysError_t::NoError)
		{
			m_Mutex.unlock();
			return fsErr;
		}
	}
	m_Mutex.unlock();
	return FileSysError_t::NoError;
}

FileSysError_t File::Reserve(const SIZET sz)
{
	bool closeAfter = false;
	FileSysError_t fsErr = FileSysError_t::NoError;
	m_Mutex.lock();
	if (m_Permisions != FilePermisions_t::ReadWrite)
	{
		closeAfter = m_Permisions == FilePermisions_t::Closed;
		fsErr = Open(FilePermisions_t::ReadWrite);
	}
	if (fsErr != FileSysError_t::NoError)
	{
		m_Mutex.unlock();
		return fsErr;
	}

	SIZET size;
	fsErr = GetSize(size);
	if (fsErr != FileSysError_t::NoError)
	{
		m_Mutex.unlock();
		return fsErr;
	}

	SIZET offset;
	fsErr = GetOffset(offset);
	if (fsErr != FileSysError_t::NoError)
	{
		m_Mutex.unlock();
		return fsErr;
	}

#if PLATFORM_WINDOWS
	LARGE_INTEGER lin;
	lin.QuadPart = (LONGLONG)(sz - size);
	if (!SetFilePointerEx(m_Handle, lin, nullptr, FILE_CURRENT))
	{
		m_Mutex.unlock();
		LogManager::LogMessage(LL_ERRO, "Trying to change the capacity of a file, name: %ls, but something unexpecte happened, error: 0x%08X.", m_Name.c_str(), GetLastError());
		return FileSysError_t::UnknownError;
	}
	if (!SetEndOfFile(m_Handle))
	{
		m_Mutex.unlock();
		LogManager::LogMessage(LL_ERRO, "Trying to change the capacity of a file but, name: %ls, something unexpecte happened, error: 0x%08X.", m_Name.c_str(), GetLastError());
		return FileSysError_t::UnknownError;
	}
#else
	/* TODO file reserve */
#endif
	fsErr = SetOffset(offset);
	if (fsErr != FileSysError_t::NoError)
	{
		m_Mutex.unlock();
		return fsErr;
	}
	if (closeAfter)
	{
		fsErr = Open(FilePermisions_t::Closed);
		if (fsErr != FileSysError_t::NoError)
		{
			m_Mutex.unlock();
			return fsErr;
		}
	}
	m_Mutex.unlock();
	return FileSysError_t::NoError;
}

FileSysError_t File::LockRegion(const SIZET offset, const SIZET bytesToLock)
{
	bool closeAfter = false;
	FileSysError_t fsErr = FileSysError_t::NoError;
	m_Mutex.lock();
	if (m_Permisions == FilePermisions_t::Closed)
	{
		closeAfter = true;
		fsErr = Open(FilePermisions_t::ReadOnly);
	}
	if (fsErr != FileSysError_t::NoError)
	{
		m_Mutex.unlock();
		return fsErr;
	}
#if PLATFORM_WINDOWS
	LARGE_INTEGER la, lb;
	la.QuadPart = (LONGLONG)offset;
	lb.QuadPart = (LONGLONG)bytesToLock;
	if (!LockFile(m_Handle, la.LowPart, la.HighPart, lb.LowPart, lb.HighPart))
	{
		m_Mutex.unlock();
		LogManager::LogMessage(LL_ERRO, "Trying to lock a file region, name: %ls, but something unexpected happened, error: 0x%08X.", m_Name.c_str(), GetLastError());
		return FileSysError_t::UnknownError;
	}
#else
	/* TODO file locking */
#endif
	if (closeAfter)
	{
		fsErr = Open(FilePermisions_t::Closed);
		if (fsErr != FileSysError_t::NoError)
		{
			m_Mutex.unlock();
			return fsErr;
		}
	}
	m_Mutex.unlock();
	return FileSysError_t::NoError;
}

FileSysError_t File::UnlockRegion(const SIZET offset, const SIZET bytesToUnlock)
{
	bool closeAfter = false;
	FileSysError_t fsErr = FileSysError_t::NoError;
	m_Mutex.lock();
	if (m_Permisions == FilePermisions_t::Closed)
	{
		closeAfter = true;
		fsErr = Open(FilePermisions_t::ReadOnly);
	}
	if (fsErr != FileSysError_t::NoError)
	{
		m_Mutex.unlock();
		return fsErr;
	}
#if PLATFORM_WINDOWS
	LARGE_INTEGER la, lb;
	la.QuadPart = (LONGLONG)offset;
	lb.QuadPart = (LONGLONG)bytesToUnlock;
	if (!UnlockFile(m_Handle, la.LowPart, la.HighPart, lb.LowPart, lb.HighPart))
	{
		m_Mutex.unlock();
		LogManager::LogMessage(LL_ERRO, "Trying to unlock a file region, name: %ls, but something unexpected happened, error: 0x%08X.", m_Name.c_str(), GetLastError());
		return FileSysError_t::UnknownError;
	}
#else
	/* TODO file unlocking */
#endif
	if (closeAfter)
	{
		fsErr = Open(FilePermisions_t::Closed);
		if (fsErr != FileSysError_t::NoError)
		{
			m_Mutex.unlock();
			return fsErr;
		}
	}
	m_Mutex.unlock();
	return FileSysError_t::NoError;
}

FileSysError_t File::Lock()
{
	SIZET size;
	m_Mutex.lock();
	auto fsErr = GetSize(size);
	if (fsErr != FileSysError_t::NoError)
	{
		m_Mutex.unlock();
		return fsErr;
	}
	fsErr = LockRegion(0, size);
	m_Mutex.unlock();
	return fsErr;
}

FileSysError_t File::Unlock()
{
	SIZET size;
	m_Mutex.lock();
	auto fsErr = GetSize(size);
	if (fsErr != FileSysError_t::NoError)
	{
		m_Mutex.unlock();
		return fsErr;
	}
	fsErr = UnlockRegion(0, size);
	m_Mutex.unlock();
	return fsErr;
}

std::string File::GetFullPath()const
{
	return StringUtils::ws2s(GetFullPathW());
}

std::wstring File::GetFullPathW()const
{
	if (m_Directory)
		return GetPathW() + m_Name;
	return m_Name;
}

std::string File::GetExtension()const
{
	return StringUtils::ws2s(GetExtensionW());
}

std::wstring File::GetExtensionW()const
{
	const auto name = GetNameW();
	const auto lastDot = name.find_last_of(L'.');
	if (lastDot == std::string::npos)
		return {};
	return name.substr(lastDot);
}

std::string File::GetNameWithoutExtension() const
{
	return StringUtils::ws2s(GetNameWithoutExtensionW());
}

std::wstring File::GetNameWithoutExtensionW() const
{
	const auto name = GetNameW();
	const auto lastDot = name.find_last_of(L'.');
	if (lastDot == std::string::npos)
		return m_Name;
	return name.substr(0, lastDot);
}

std::string File::GetPath() const
{
	return StringUtils::ws2s(GetPathW());
}

std::wstring File::GetPathW() const
{
	if (m_Directory)
	{
		std::wstring path;
		Directory* upperDir = m_Directory;
		while (upperDir)
		{
			path = upperDir->GetNameW() + FileSystem::PathSeparatorW + path;
			if (!upperDir->GetParentDirectory())
				path = upperDir->GetPathW() + path;
			upperDir = upperDir->GetParentDirectory();
		}
		return path;
	}
	const auto lastSlash = m_Name.find_last_of(FileSystem::PathSeparatorW);
	return m_Name.substr(0, lastSlash);
}

Directory* File::GetDirectory()const
{
	return m_Directory;
}

void File::Close()
{
	m_Mutex.lock();
	Open(FilePermisions_t::Closed);
	m_Mutex.unlock();
}

void File::Open()
{
	m_Mutex.lock();
	Open(FilePermisions_t::ReadWrite);
	m_Mutex.unlock();
}

std::string File::GetName() const
{
	return StringUtils::ws2s(GetNameW());
}

std::wstring File::GetNameW() const
{
	if (m_Directory)
		return m_Name;
	const auto lastSlash = m_Name.find_last_of(FileSystem::PathSeparatorW);
	return m_Name.substr(lastSlash);
}