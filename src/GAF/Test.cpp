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

#include "GAF/GAFPrerequisites.h"
#include "GAF/Application.h"
#include "GAF/FileSystem.h"
#include "GAF/Util/StringUtils.h"

void Test::Run()
{
	if (!m_Tests.empty())
	{
		for (auto it = m_Tests.begin(); it != m_Tests.end(); ++it)
		{
			it->second(TestResults);
		}
	}
	if (!TestResults.empty())
	{
		std::stringstream ss;
		uint32 test = 0;
		ss << m_TestName;
		std::chrono::nanoseconds wholeTestTime = std::chrono::nanoseconds(0);
		std::chrono::nanoseconds insideTestTime = std::chrono::nanoseconds(0);
		for (auto it = TestResults.begin(); it != TestResults.end(); ++it)
		{
			const auto duration = it->AfterTest - it->BeforeTest;
			ss << LINE_TERMINATOR;
			if (it->TestName != Test::DiscountTime)
			{
				ss << "\t" << it->TestName;
				insideTestTime += duration;
			}
			else
			{
				if (test != 0)
				{
					ss << m_Tests[test - 1].first << " took: " << insideTestTime.count() / (1000.0 * 1000.0) << "ms." << LINE_TERMINATOR;
					wholeTestTime += insideTestTime;
					insideTestTime = std::chrono::nanoseconds(0);
				}
				ss << m_Tests[test].first << " config";
				++test;
			}
			ss << " took: " << duration.count() / (1000.0 * 1000.0) << "ms.";
		}
		wholeTestTime += insideTestTime;
		ss << LINE_TERMINATOR << m_Tests[test - 1].first << " took: " << insideTestTime.count() / (1000.0 * 1000.0) << "ms."
			<< LINE_TERMINATOR << "WholeTest took: " << wholeTestTime.count() / (1000.0 * 1000.0) << "ms.";
		gaf::File* file = nullptr;
		const auto fSys = gaf::InstanceFS();
		auto err = fSys->CreateExternalFile(L"", gaf::StringUtils::s2ws(m_TestName) +  L"_TestResult.log", file);
		const auto str = ss.str();
		if (err != gaf::FileSysError_t::NoError || !file)
		{
			std::ofstream ofs(m_TestName + "_Result.log");
			ofs.write(str.c_str(), str.size());
		}
		else
		{
			SIZET written;
			file->ClearFile();
			err = file->StoreContents((void*)str.data(), str.size(), gaf::File::OffsetBegin, written);
			if (err != gaf::FileSysError_t::NoError || written != str.size())
			{
				std::ofstream ofs(m_TestName + "_TestResult.log");
				ofs.write(str.c_str(), str.size());
			}
			else
			{
				fSys->DeleteExternalFile(file);
			}
		}
	}
}
