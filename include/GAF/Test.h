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

#ifndef GREAPER_I_TEST_H
#define GREAPER_I_TEST_H 1

#include "GAF/Test.h"

#ifndef GREAPER_TEST
#define GREAPER_TEST 0
#endif

class Test
{
public:
	static constexpr auto DiscountTime = "TEST_DISCOUNT";
	struct Result
	{
		using TimePoint = std::chrono::high_resolution_clock::time_point;
		std::string TestName;
		TimePoint BeforeTest;
		TimePoint AfterTest;
	};
	using ResultVec = std::vector<Result>;
	using TestFn = std::function<void(ResultVec& resultVec)>;
	using TestFunctionVec = std::vector<std::pair<std::string, TestFn>>;
	ResultVec TestResults;
protected:
	std::string m_TestName;
	TestFunctionVec m_Tests;
public:
	Test() = delete;
	Test(std::string testName)
		:m_TestName(std::move(testName))
	{

	}
	
	virtual ~Test() = default;
	
	void Run();
};

#ifndef PRETEST_BEGIN
#define PRETEST_BEGIN()\
Test::Result result__;\
result__.TestName = Test::DiscountTime;\
result__.BeforeTest = std::chrono::high_resolution_clock::now()
#endif

#ifndef PRETEST_END
#define PRETEST_END()\
result__.AfterTest = std::chrono::high_resolution_clock::now();\
resultVec.push_back(std::move(result__))
#endif

#ifndef DOTEST_BEGIN
#define DOTEST_BEGIN(testName)\
{\
	Test::Result _result;\
	_result.TestName = (testName);\
	_result.BeforeTest = std::chrono::high_resolution_clock::now()
#endif

#ifndef DOTEST_END
#define DOTEST_END()\
	_result.AfterTest = std::chrono::high_resolution_clock::now();\
	resultVec.push_back(std::move(_result));\
}
#endif

#endif /* GREAPER_I_TEST_H */