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

#ifndef GAF_DIRECTORY_H
#define GAF_DIRECTORY_H 1

namespace gaf
{
	typedef std::vector<class Directory*> DirList;
	class Directory
	{
		std::shared_mutex m_FileMutex;
		FileList m_NestedFiles;
		std::shared_mutex m_DirMutex;
		DirList m_NestedDirs;
		Directory* m_UpperDirectory;
		std::wstring m_Name;

		void Update();
		Directory() = default;
		~Directory();
	public:
		
		/*
			Adds a directory inside this one with the given dirName.
			Returns:
				- NoError: The directory was successfully created.
				- InputError: There was an existent directory with the same name.
				- UnknownError: Something unexpected happened.
		*/
		FileSysError_t AddDir(const std::string& dirName, Directory*& dir);
		FileSysError_t AddDir(const std::wstring& dirName, Directory*& dir);
		/*
			Adds a file inside this directory with the given fileName.
			Return:
				- NoError: The file was successfully created.
				- InputError: There was an existent file with the same name.
				- UnknownError: Something unexpected happened.
		*/
		FileSysError_t AddFile(const std::string& fileName, File*& file);
		FileSysError_t AddFile(const std::wstring& fileName, File*& file);
		/*
			Changes the name of this directory.
			Returns:
				- NoError: The directory was successfully renamed.
				- InputError: There was an existent directory with the same name.
				- UnknownError: Something unexpected happened.
		*/
		FileSysError_t ChangeName(const std::string& name);
		FileSysError_t ChangeName(const std::wstring& name);
		/*
			Copies a this directory to another path.
		*/
		FileSysError_t CopyDirTo(const std::string& path, bool replace = false);
		FileSysError_t CopyDirTo(const std::wstring& path, bool replace = false);
		/*
			Copies a this directory to another path.
		*/
		FileSysError_t CopyDirTo(Directory* dir, bool replace = false);
		/*
			Move this directory to another directory, if the directory already exists in that directory
			this Directory structure will be invalid, so after the move go to that directory and look
			for this directory.
		*/
		FileSysError_t MoveDirTo(Directory* dir);
		/*
			Move this directory to another directory, if the directory already exists in that directory
			this Directory structure will be invalid, so after the move go to that directory and look
			for this directory.
		*/
		FileSysError_t MoveDirTo(const std::string& path);
		FileSysError_t MoveDirTo(const std::wstring& path);
		/*
			Erases the physical directory and deletes the Directory structure and its resources,
			so you must not use this after calling this function.
		*/
		void Erase();
		/*
			Returns the number of files that this directory has.
		*/
		SIZET GetNumFiles();
		/*
			Returns the number of directories that this directory has.
		*/
		SIZET GetNumDirs();
		/*
			Returns the creation time of this directory.
		*/
		FileSysError_t GetCreationTime(DayTime& time)const;
		/*
			Returns the directory with the given name which is nested inside this one.
			If not exists returns nullptr; If there's more than one directory with the 
			same name, but in diferent directories will return the first one to find
			if you set recursive to true.
		*/
		Directory* ContainsDir(const std::string& dirName, bool recursive = true);
		Directory* ContainsDir(const std::wstring& dirName, bool recursive = true);
		/*
			Returns the file with the given name which is nested inside this one.
			If not exists returns nullptr; If there's more than one file with the 
			same name, but in diferent directories will return the first one to find
			if you set recursive to true.
		*/
		File* ContainsFile(const std::string& fileName, bool recursive = true);
		File* ContainsFile(const std::wstring& fileName, bool recursive = true);
		
		/*
			Returns a pointer to the fileList begin()
		*/
		FileList::iterator GetFileListBegin();
		/*
			Returns a pointer to the fileList end()
		*/
		FileList::iterator GetFileListEnd();
		/*
			FileList synchronization methods
		*/
		void LockFileListRead();
		void UnlockFileListRead();
		/*
			Returns a pointer to the dirList begin()
		*/
		DirList::iterator GetDirectoryListBegin();
		/*
			Returns a pointer to the dirList end()
		*/
		DirList::iterator GetDirectoryListEnd();
		/*
			DirList synchronization methods
		*/
		void LockDirListRead();
		void UnlockDirListRead();
		/*
			Returns a pointer to the parent directory which may be nullptr
			if this directory is not handled or this is the top directory
			in the directory hierachy
		*/
		Directory* GetParentDirectory()const;
		/*
			Returns the full path of the file, ex: C\GAF\Files
		*/
		std::string GetFullPath()const;
		std::wstring GetFullPathW()const;

		/*
			Returns the full path but without the file name
		*/
		std::string GetPath()const;
		std::wstring GetPathW()const;
		/*
			Returns the name of the directory.
		*/
		std::string GetName()const;
		std::wstring GetNameW()const;
		friend class File;
		friend class FileSystem;
	};
}

#endif /* GAF_DIRECTORY_H */