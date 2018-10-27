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

#ifndef GREAPER_GAF_TEST_H
#define GREAPER_GAF_TEST_H 1

#include "GAF/GAFPrerequisites.h"

class GAFTest : public Test
{
	void FileSysTest(ResultVec& resultVec);
	void LogTest(ResultVec& resultVec);
	void EventTest(ResultVec& resultVec);
	void PropertyTest(ResultVec& resultVec);
	void WindowTest(ResultVec& resultVec);
	void CryptoTest(ResultVec& resultVec);
	void ResourceTest(ResultVec& resultVec);
	void CommandTest(ResultVec& resultVec);
	void InputTest(ResultVec& resultVec);
public:

	GAFTest();
	GAFTest(const GAFTest&) = default;
	GAFTest(GAFTest&&)noexcept = default;
	GAFTest& operator=(const GAFTest&) = default;
	GAFTest& operator=(GAFTest&&)noexcept = default;
	~GAFTest() = default;
};

#endif
