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

#ifndef GAF_I_MODULE_H
#define GAF_I_MODULE_H 1

namespace gaf
{
	class IModule
	{
	protected:
		const std::string m_ModuleName;
		/*
			Allocs and initiates the module, this must be called
			before any interaction with the module.
		*/
		virtual void Start() = 0;
		/*
			Deinitializes the module and its data after this call
			you must not call any part of the module
		*/
		virtual void Stop() = 0;
	public:
		virtual ~IModule() = default;
		
		const std::string& GetName()const { return m_ModuleName; }
		friend class Application;
	};
}

#endif /* GAF_I_MODULE_H */