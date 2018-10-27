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

#ifndef GAF_IMAGE_MANAGER_H
#define GAF_IMAGE_MANAGER_H 1

#include "GAF/GAFPrerequisites.h"
#include "GAF/Base/Image.h"

namespace gaf
{
	class ImageManager
	{
	public:
		using ImageID = size_t;
		static constexpr ImageID NullImageID = static_cast<ImageID>(-1);
	private:
		static ImageManager* m_Instance;
		std::map<ImageID, Image*> m_Images;
		std::shared_mutex m_ImageMutex;

		ImageManager();
		~ImageManager();
	public:
		static ImageManager& Instance();
		static ImageManager* InstancePtr();

		ImageID LoadFromFile(File* file);
		ImageID LoadFromMemory(void* data, SIZET byteSize);

		Image* GetImage(ImageID id);

		bool DestroyImage(ImageID id);
		bool DestroyImage(Image* image);

#if GREAPER_DEBUG_ALLOCATION
		friend class Application;
#endif
	};

	static ImageManager* InstanceImg() { return ImageManager::InstancePtr(); }
}

#endif /* GAF_IMAGE_MANAGER_H */
