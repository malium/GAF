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

#include "GAF/GAFTest.h"
#include "GAF/FileSystem.h"
#include "GAF/LogManager.h"
#include "GAF/PropertiesManager.h"
#include "GAF/WindowManager.h"
#include "GAF/CryptoAPI.h"
#include "GAF/InputManager.h"

GAFTest::GAFTest()
	:Test("GAFTest")
{
	using namespace std::placeholders;
	m_Tests = { {"FileSystem Test", std::bind(&GAFTest::FileSysTest, this, _1)}
		,{ "LogManager Test", std::bind(&GAFTest::LogTest, this, _1)}
		,{ "EventManager Test", std::bind(&GAFTest::EventTest, this, _1)}
		,{ "PropertiesManager Test", std::bind(&GAFTest::PropertyTest, this, _1)}
		,{ "WindowManager Test", std::bind(&GAFTest::WindowTest, this, _1)}
		,{ "CryptoAPI Test", std::bind(&GAFTest::CryptoTest, this, _1) }
		,{ "ResourceManager Test", std::bind(&GAFTest::ResourceTest, this, _1)}
		,{ "CommandSystem Test", std::bind(&GAFTest::CommandTest, this, _1)}
		,{ "InputManager Test", std::bind(&GAFTest::InputTest, this, _1) } };
}

void GAFTest::FileSysTest(ResultVec& resultVec)
{
	PRETEST_BEGIN();
	gaf::File* testFile = nullptr;
	gaf::Directory* testDir = nullptr;
	gaf::FileSysError_t fsErr = gaf::EFileSysError::NoError;
	std::string outBuffer = "This is a test.";
	std::string inBuffer;
	gaf::DayTime dayTime;
	inBuffer.resize(sizeof("This is a test."));
	const auto fSys = gaf::InstanceFS();
	PRETEST_END();

	/* File test */

	DOTEST_BEGIN("FileCreation");
	fsErr = fSys->GetRootDirectory()->AddFile(L"TestFile.txt", testFile);
	gaf::Assertion::WhenInequal(fsErr, gaf::EFileSysError::NoError, "Error creating a file in the main directory while performing a test.");
	DOTEST_END();

	DOTEST_BEGIN("FileOpen");
	testFile->Open();
	DOTEST_END();

	DOTEST_BEGIN("FileNameChange");
	fsErr = testFile->ChangeName(L"TestFile2.txt");
	gaf::Assertion::WhenInequal(fsErr, gaf::EFileSysError::NoError, "Error changing the name of a file while performing a test.");
	DOTEST_END();

	DOTEST_BEGIN("FileCreationTime");
	fsErr = testFile->GetCreationTime(dayTime);
	gaf::Assertion::WhenInequal(fsErr, gaf::EFileSysError::NoError, "Error getting the creation time of a file while performing a test.");
	DOTEST_END();

	DOTEST_BEGIN("FileLastAccessTime");
	fsErr = testFile->GetLastAccessTime(dayTime);
	gaf::Assertion::WhenInequal(fsErr, gaf::EFileSysError::NoError, "Error getting the last access time of a file while performing a test.");
	DOTEST_END();

	DOTEST_BEGIN("FileLastWriteTime");
	fsErr = testFile->GetLastWriteTime(dayTime);
	gaf::Assertion::WhenInequal(fsErr, gaf::EFileSysError::NoError, "Error getting the last write time of a file while performing a test.");
	DOTEST_END();

	DOTEST_BEGIN("FileExtension");
	const auto ext = testFile->GetExtensionW();
	gaf::Assertion::WhenInequal(ext, L".txt", "Error getting the file extension while performing a test.");
	DOTEST_END();

	DOTEST_BEGIN("FileWithoutExtension");
	const auto ext = testFile->GetNameWithoutExtensionW();
	gaf::Assertion::WhenInequal(ext, L"TestFile2", "Error getting the file extension while performing a test.");
	DOTEST_END();

	DOTEST_BEGIN("FileName");
	const auto ext = testFile->GetNameWithoutExtensionW();
	gaf::Assertion::WhenInequal(ext, L"TestFile2", "Error getting the file without extension while performing a test.");
	DOTEST_END();

	DOTEST_BEGIN("FileFullPath");
	const auto ext = testFile->GetFullPathW();
	gaf::Assertion::WhenTrue(ext.empty(), "Error getting the file full path while performing a test.");
	DOTEST_END();

	DOTEST_BEGIN("FilePath");
	const auto ext = testFile->GetPathW();
	gaf::Assertion::WhenTrue(ext.empty(), "Error getting the file path while performing a test.");
	DOTEST_END();

	DOTEST_BEGIN("FileWrite");
	SIZET writtenBytes;
	fsErr = testFile->StoreContents(&outBuffer[0], outBuffer.size(), writtenBytes);
	gaf::Assertion::WhenInequal(fsErr, gaf::EFileSysError::NoError, "Error writting into a file while performing a test.");
	gaf::Assertion::WhenInequal(writtenBytes, outBuffer.size(), "Error writting into a file while performing a test, written bytes mismatch.");
	DOTEST_END();

	DOTEST_BEGIN("FileRead");
	SIZET readBytes;
	fsErr = testFile->LoadContents(&inBuffer[0], outBuffer.size(), gaf::File::OffsetBegin, readBytes);
	gaf::Assertion::WhenInequal(fsErr, gaf::EFileSysError::NoError, "Error reading from a file while performing a test.");
	gaf::Assertion::WhenInequal(strcmp(&inBuffer[0], &outBuffer[0]), 0, "Error reading from a file while performing a test.");
	gaf::Assertion::WhenInequal(readBytes, outBuffer.size(), "Error reading from a file while performing a test, read bytes mismatch.");
	DOTEST_END();

	DOTEST_BEGIN("FileGetOffset");
	SIZET offset;
	fsErr = testFile->GetOffset(offset);
	gaf::Assertion::WhenInequal(fsErr, gaf::EFileSysError::NoError, "Error getting the offset of a file while performing a test.");
	DOTEST_END();

	DOTEST_BEGIN("FileSetOffset");
	fsErr = testFile->SetOffset(gaf::File::OffsetEnd);
	gaf::Assertion::WhenInequal(fsErr, gaf::EFileSysError::NoError, "Error setting the offset of a file while performing a test.");
	DOTEST_END();

	DOTEST_BEGIN("FileGetSize");
	SIZET size;
	fsErr = testFile->GetSize(size);
	gaf::Assertion::WhenInequal(fsErr, gaf::EFileSysError::NoError, "Error getting the size of a file while performing a test.");
	DOTEST_END();

	DOTEST_BEGIN("FileClear");
	testFile->ClearFile();
	DOTEST_END();

	DOTEST_BEGIN("FileClose");
	testFile->Close();
	DOTEST_END();

	DOTEST_BEGIN("FileFind");
	auto file = fSys->GetRootDirectory()->ContainsFile("TestFile2.txt", true);
	gaf::Assertion::WhenNullptr(file, "Error finding a file while performing a test");
	DOTEST_END();

	DOTEST_BEGIN("FileErase");
	testFile->Erase();
	DOTEST_END();

	/* Directory Test */

	DOTEST_BEGIN("DirectoryCreation");
	fsErr = fSys->GetRootDirectory()->AddDir(L"TestDir", testDir);
	gaf::Assertion::WhenInequal(fsErr, gaf::EFileSysError::NoError, "Error creating a directory in the main directory while performing a test.");
	DOTEST_END();

	DOTEST_BEGIN("DirectoryNameChange");
	fsErr = testDir->ChangeName(L"TestDir2");
	gaf::Assertion::WhenInequal(fsErr, gaf::EFileSysError::NoError, "Error changing the name of a directory while performing a test.");
	DOTEST_END();

	DOTEST_BEGIN("DirectoryFind");
	auto dir = fSys->GetRootDirectory()->ContainsDir(L"TestDir2", true);
	gaf::Assertion::WhenNullptr(dir, "Error finding a directory while performing a test");
	DOTEST_END();

	DOTEST_BEGIN("DirectoryCreationTime");
	fsErr = fSys->GetRootDirectory()->GetCreationTime(dayTime);
	gaf::Assertion::WhenInequal(fsErr, gaf::EFileSysError::NoError, "Error getting the creation time of a directory while performing a test.");
	DOTEST_END();

	DOTEST_BEGIN("DirName");
	const auto ext = testDir->GetNameW();
	gaf::Assertion::WhenInequal(ext, L"TestDir2", "Error getting the directory name while performing a test.");
	DOTEST_END();

	DOTEST_BEGIN("DirFullPath");
	const auto ext = testDir->GetFullPathW();
	gaf::Assertion::WhenTrue(ext.empty(), "Error getting the directory full path while performing a test.");
	DOTEST_END();

	DOTEST_BEGIN("DirPath");
	const auto ext = testDir->GetPathW();
	gaf::Assertion::WhenTrue(ext.empty(), "Error getting the directory path while performing a test.");
	DOTEST_END();

	DOTEST_BEGIN("DirGetNumFiles");
	testDir->GetNumFiles();
	DOTEST_END();

	DOTEST_BEGIN("DirGetNumDirs");
	testDir->GetNumDirs();
	DOTEST_END();

	DOTEST_BEGIN("DirErase");
	testDir->Erase();
	DOTEST_END();
}

void GAFTest::LogTest(ResultVec& resultVec)
{
	PRETEST_BEGIN();
	volatile bool done = false;
	std::string message = "Test message";
	gaf::LogManager::LogHandlerID hndID = gaf::LogManager::NullLogHandlerID;
	Test::Result logTimeTest;
	logTimeTest.TestName = "LogTime";
	const auto logMgr = gaf::InstanceLog();
	PRETEST_END();
	DOTEST_BEGIN("AddLogHandler");
	hndID = logMgr->AddLogHandler([&](const std::string& msg, const gaf::DayTime& time, gaf::LogLevel level)
	{
		if (msg == message)
		{
			logTimeTest.AfterTest = std::chrono::high_resolution_clock::now();
			done = true;
		}
	});
	DOTEST_END();

	{
		Test::Result test;
		test.TestName = "LogSendTime";
		test.BeforeTest = std::chrono::high_resolution_clock::now();
		logTimeTest.BeforeTest = test.BeforeTest;
		gaf::LogManager::LogMessage(gaf::LL_INFO, message.c_str());
		test.AfterTest = std::chrono::high_resolution_clock::now();
		resultVec.push_back(std::move(test));
	}
	while (!done)
		std::this_thread::sleep_for(std::chrono::microseconds(100));
	resultVec.push_back(std::move(logTimeTest));

	DOTEST_BEGIN("RemoveLogHandler");
	logMgr->RemoveLogHandler(hndID);
	DOTEST_END();
}

void GAFTest::EventTest(ResultVec & resultVec)
{
	PRETEST_BEGIN();
	gaf::EventListenerID listenerID = gaf::EventManager::NullEventListenerID;
	gaf::EventID eventID = gaf::EventManager::NullEventID;
	volatile bool done = false;
	Test::Result test__;
	test__.TestName = "EventTime";
	const auto eventMgr = gaf::InstanceEvent();
	const auto testEvent2 = eventMgr->RegisterEvent("TestEvent2"),
		testEvent3 = eventMgr->RegisterEvent("TestEvent3"),
		testEvent4 = eventMgr->RegisterEvent("TestEvent4"),
		testEvent5 = eventMgr->RegisterEvent("TestEvent5");
	PRETEST_END();
	DOTEST_BEGIN("AddEvent");
	eventID = eventMgr->RegisterEvent("TestEvent");
	gaf::Assertion::WhenEqual(eventID, gaf::EventManager::NullEventID, "Trying to register an Event but something went wrong, while performing a test.");
	DOTEST_END();
	DOTEST_BEGIN("AddEventListener");
	listenerID = eventMgr->RegisterEventListener([&](const gaf::EventID evt, void* params)
	{
		if (evt == eventID)
		{
			test__.AfterTest = std::chrono::high_resolution_clock::now();
			done = true;
		}
	}, eventID);
	gaf::Assertion::WhenEqual(listenerID, gaf::EventManager::NullEventListenerID, "Trying to register an EventListener but something went wrong, while performing a test.");
	DOTEST_END();

	{
		Test::Result test;
		test.TestName = "EventDispatchTime";
		test.BeforeTest = std::chrono::high_resolution_clock::now();
		test__.BeforeTest = test.BeforeTest;
		eventMgr->DispatchEvent(eventID, nullptr);
		test.AfterTest = std::chrono::high_resolution_clock::now();
		resultVec.push_back(std::move(test));
	}

	while (!done)
		std::this_thread::sleep_for(std::chrono::microseconds(100));
	resultVec.push_back(std::move(test__));

	const auto testID = eventMgr->RegisterEvent("TestEvent2");
	gaf::Assertion::WhenEqual(testID, gaf::EventManager::NullEventID, "Trying to register an Event but something went wrong, while performing a test.");
	DOTEST_BEGIN("AddEventToListener");
	eventMgr->AddEventToListener(listenerID, testID);
	DOTEST_END();
	DOTEST_BEGIN("AddEventToListenerList");
	eventMgr->AddEventToListener(listenerID, { testEvent2, testEvent3, testEvent4, testEvent5 }	);
	DOTEST_END();
	DOTEST_BEGIN("GetEventIDFromName");
	const auto id = eventMgr->GetEventIDFromName("TestEvent");
	gaf::Assertion::WhenEqual(id, gaf::EventManager::NullEventID, "Trying to get the EventID by its name but something went wrong, while performing a test.");
	DOTEST_END();
	DOTEST_BEGIN("GetEventName");
	const auto name = eventMgr->GetEventName(testID);
	gaf::Assertion::WhenInequal(name, "TestEvent2", "Trying to get the event Name but something went wrong, while performing a test.");
	DOTEST_END();
	DOTEST_BEGIN("RemoveEventFromListener");
	eventMgr->RemoveEventFromListener(listenerID, testID);
	DOTEST_END();
	DOTEST_BEGIN("RemoveEventFromListenerList");
	eventMgr->RemoveEventFromListener(listenerID, { testEvent2, testEvent3, testEvent4, testEvent5 });
	DOTEST_END();
	DOTEST_BEGIN("EventListenerUnregister");
	eventMgr->UnregisterEventListener(listenerID);
	DOTEST_END();
	DOTEST_BEGIN("EventUnregister");
	eventMgr->UnregisterEvent(eventID);
	DOTEST_END();
}

void GAFTest::PropertyTest(ResultVec & resultVec)
{
	PRETEST_BEGIN();
	const auto propMgr = gaf::InstanceProp();
	const auto eventMgr = gaf::InstanceEvent();
	gaf::IProperty* testPropBool = nullptr,
		*testPropNum = nullptr,
		*testPropStr = nullptr,
		*testPropConst = propMgr->CreateProperty("TestPropertyConst", true, true),
		*testRem1 = propMgr->CreateProperty("TestRemoval1", true, true),
		*testRem2 = propMgr->CreateProperty("TestRemoval2", true, true);
	volatile bool done = false;
	Test::Result test__;
	test__.TestName = "PropertyTime";
	eventMgr->RegisterEventListener([&](const gaf::EventID evt, void* params)
	{
		if (evt == gaf::PropertiesManager::EventIDOnModification && params == testPropNum)
		{
			done = true;
			test__.AfterTest = std::chrono::high_resolution_clock::now();
		}
	});
	PRETEST_END();

	DOTEST_BEGIN("PropertyCreationBool");
	testPropBool = propMgr->CreateProperty("TestPropertyBool", false, true);
	DOTEST_END();
	DOTEST_BEGIN("PropertyCreationNum");
	testPropNum = propMgr->CreateProperty("TestPropertyNum", false, 1.f, 0.f, 3.f);
	DOTEST_END();
	DOTEST_BEGIN("PropertyCreationStr");
	testPropStr = propMgr->CreateProperty("TestPropertyStr", false, "TestValue", { "TestValue", "TestValue1", "TestValue2" });
	DOTEST_END();
	DOTEST_BEGIN("PropertyGetter");
	const auto tmp = propMgr->GetProperty("TestPropertyBool");
	gaf::Assertion::WhenInequal(testPropBool, tmp, "Trying to retrieve a Property the the pointer received is not the same, while performing a test.");
	DOTEST_END();
	DOTEST_BEGIN("PropertyRemovalPtr");
	propMgr->RemoveProperty(testRem1);
	DOTEST_END();
	DOTEST_BEGIN("PropertyRemovalStr");
	propMgr->RemoveProperty("TestRemoval2");
	DOTEST_END();
	DOTEST_BEGIN("PropertyChangeConstant");
	testPropConst->SetBoolValue(false);
	DOTEST_END();
	DOTEST_BEGIN("PropertyChangeBool");
	testPropBool->SetBoolValue(false);
	DOTEST_END();

	DOTEST_BEGIN("PropertyEnableEvent");
	testPropNum->EnableOnModificationEvent(true);
	DOTEST_END();
	{
		Test::Result test;
		test.TestName = "PropertyChangeNum";
		test.BeforeTest = std::chrono::high_resolution_clock::now();
		test__.BeforeTest = test.BeforeTest;
		testPropNum->SetNumberValue(2.f);
		test.AfterTest = std::chrono::high_resolution_clock::now();
		resultVec.push_back(std::move(test));
	}

	while (!done)
		std::this_thread::sleep_for(std::chrono::microseconds(100));
	resultVec.push_back(std::move(test__));

	DOTEST_BEGIN("PropertyDisableEvent");
	testPropNum->EnableOnModificationEvent(false);
	DOTEST_END();


	DOTEST_BEGIN("PropertyChangeMaxNum");
	testPropNum->SetNumberMaxValue(4.f);
	DOTEST_END();
	DOTEST_BEGIN("PropertyChangeMinNum");
	testPropNum->SetNumberMinValue(-1.f);
	DOTEST_END();
	DOTEST_BEGIN("PropertyChangeNumOutBounds");
	testPropNum->SetNumberValue(5.f);
	DOTEST_END();
	DOTEST_BEGIN("PropertyChangeStr");
	testPropStr->SetStringValue("TestValue1");
	DOTEST_END();
	DOTEST_BEGIN("PropertyChangeStrInvalid");
	testPropStr->SetStringValue("TestFValue");
	DOTEST_END();
	DOTEST_BEGIN("AddValidValueString");
	testPropStr->AddValidStringValue("TestPropG");
	DOTEST_END();
	DOTEST_BEGIN("RemoveValidValueString");
	testPropStr->RemoveValidStringValue("TestPropG");
	DOTEST_END();
	DOTEST_BEGIN("ClearValidValueStrings");
	testPropStr->ClearValidStringValues();
	DOTEST_END();
}

CreateTaskName(WindowCreationTestTask);
CreateTaskName(WindowModeChangeFullScreenTestTask);
CreateTaskName(WindowModeChangeBorderlessTestTask);
CreateTaskName(WindowModeChangeWindowedTestTask);
CreateTaskName(WindowResolutionChangeTestTask);
CreateTaskName(WindowPositionChangeTestTask);
CreateTaskName(WindowTitleChangeTestTask);
CreateTaskName(UpdateWindowEventsTestTask);
CreateTaskName(RequestFocusTestTask);

void GAFTest::WindowTest(ResultVec & resultVec)
{
	PRETEST_BEGIN();
	gaf::Window* window = nullptr;
	gaf::IMonitor* monitor = nullptr;
	std::string idnName = "TestWindow";
	std::string testName = "NameChangeTest";
	const auto wndMgr = gaf::InstanceWnd();
	gaf::Resolution_t res1 = { 1280,720 },
		res2 = { 800,600 };
	gaf::Position_t pos1, pos2;
	volatile bool done = false;
	auto IsDone = [&](Test::Result* testRes) {
		done = true;
		testRes->AfterTest = std::chrono::high_resolution_clock::now();
	};
	PRETEST_END();

	DOTEST_BEGIN("GetDefaultMonitor");
	monitor = wndMgr->GetPrimaryMonitor();
	DOTEST_END();

	DOTEST_BEGIN("EnumerateDisplayAdapters");
	const auto vec = wndMgr->EnumerateDisplayAdapters();
	DOTEST_END();

	{
		Test::Result _result;
		_result.TestName = "WindowCreation";
		_result.BeforeTest = std::chrono::high_resolution_clock::now();
		window = wndMgr->CreateAWindow(idnName, idnName, res1,
			pos1, gaf::EWindowMode::WINDOWED);
		window->SendWindowTask(CreateTask(WindowCreationTestTask, std::bind(IsDone, &_result)));
		while (!done)
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		done = false;
		resultVec.push_back(std::move(_result));
	}
	
	{
		Test::Result _result;
		_result.TestName = "WindowModeChangeFullScreen";
		_result.BeforeTest = std::chrono::high_resolution_clock::now();
		window->SetWindowMode(gaf::EWindowMode::FULLSCREEN);
		window->SendWindowTask(CreateTask(WindowModeChangeFullScreenTestTask, std::bind(IsDone, &_result)));
		while (!done)
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		done = false;
		resultVec.push_back(std::move(_result));
	}

	{
		Test::Result _result;
		_result.TestName = "WindowModeChangeBorderless";
		_result.BeforeTest = std::chrono::high_resolution_clock::now();
		window->SetWindowMode(gaf::EWindowMode::BORDERLESS);
		window->SendWindowTask(CreateTask(WindowModeChangeBorderlessTestTask, std::bind(IsDone, &_result)));
		while (!done)
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		done = false;
		resultVec.push_back(std::move(_result));
	}

	{
		Test::Result _result;
		_result.TestName = "WindowModeChangeWindowed";
		_result.BeforeTest = std::chrono::high_resolution_clock::now();
		window->SetWindowMode(gaf::EWindowMode::WINDOWED);
		window->SendWindowTask(CreateTask(WindowModeChangeWindowedTestTask, std::bind(IsDone, &_result)));
		while (!done)
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		done = false;
		resultVec.push_back(std::move(_result));
	}

	{
		Test::Result _result;
		_result.TestName = "WindowResolutionChange";
		_result.BeforeTest = std::chrono::high_resolution_clock::now();
		window->SetResolution(res2);
		window->SendWindowTask(CreateTask(WindowResolutionChangeTestTask, std::bind(IsDone, &_result)));
		while (!done)
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		done = false;
		resultVec.push_back(std::move(_result));
	}

	{
		Test::Result _result;
		_result.TestName = "WindowPositionChange";
		_result.BeforeTest = std::chrono::high_resolution_clock::now();
		window->SetPosition(pos2);
		window->SendWindowTask(CreateTask(WindowPositionChangeTestTask, std::bind(IsDone, &_result)));
		while (!done)
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		done = false;
		resultVec.push_back(std::move(_result));
	}

	{
		Test::Result _result;
		_result.TestName = "WindowTitleChange";
		_result.BeforeTest = std::chrono::high_resolution_clock::now();
		window->SetTitle(testName);
		window->SendWindowTask(CreateTask(WindowTitleChangeTestTask, std::bind(IsDone, &_result)));
		while (!done)
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		done = false;
		resultVec.push_back(std::move(_result));
	}

	{
		Test::Result _result;
		_result.TestName = "UpdateWindowEvents";
		_result.BeforeTest = std::chrono::high_resolution_clock::now();
		window->UpdateWindowEvents();
		window->SendWindowTask(CreateTask(UpdateWindowEventsTestTask, std::bind(IsDone, &_result)));
		while (!done)
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		done = false;
		resultVec.push_back(std::move(_result));
	}

	{
		Test::Result _result;
		_result.TestName = "RequestFocus";
		_result.BeforeTest = std::chrono::high_resolution_clock::now();
		window->RequestFocus();
		window->SendWindowTask(CreateTask(RequestFocusTestTask, std::bind(IsDone, &_result)));
		while (!done)
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		done = false;
		resultVec.push_back(std::move(_result));
	}

	DOTEST_BEGIN("GetAWindow");
	auto wnd = wndMgr->GetWindow(window->GetID());
	gaf::Assertion::WhenInequal(window, wnd, "Trying to get a Window by its id, but the returned pointer is not the same, while performing a test.");
	DOTEST_END();

	DOTEST_BEGIN("CloseWindow");
	window->Close();
	DOTEST_END();
}

void GAFTest::CryptoTest(ResultVec & resultVec)
{
	PRETEST_BEGIN();
	std::string input = "Crypto test.";
	const auto size = input.size();
	std::string output;
	gaf::Crypto* crypto = nullptr;
	SIZET seed = 0;
	PRETEST_END();

	DOTEST_BEGIN("CryptoCreation");
	crypto = gaf::CreateDefaultCrypto();
	gaf::Assertion::WhenNullptr(crypto, "Trying to create a DefaultCryptoAPI, but something went wrong, while performing a test.");
	DOTEST_END();
	DOTEST_BEGIN("GenerateSeed");
	seed = gaf::GenerateSeed();
	DOTEST_END();
	DOTEST_BEGIN("Encrypt");
	crypto->Crypt(&input[0], size);
	DOTEST_END();
	DOTEST_BEGIN("Decrypt");
	crypto->Decrypt(&input[0], size);
	gaf::Assertion::WhenInequal(strcmp(&input[0], "Crypto test."), 0, "Trying to decrypt a message, but the return value wasn't the expected one.");
	DOTEST_END();
	DOTEST_BEGIN("CryptoDestruction");
	gaf::ReleaseDefaultCrypto(crypto);
	DOTEST_END();
}

void GAFTest::ResourceTest(ResultVec & resultVec)
{
	PRETEST_BEGIN();

	PRETEST_END();
}

void GAFTest::CommandTest(ResultVec & resultVec)
{
	PRETEST_BEGIN();

	PRETEST_END();
}

CreateTaskName(WindowReadyCheck);

void GAFTest::InputTest(ResultVec & resultVec)
{
	PRETEST_BEGIN();
	const auto inputMgr = gaf::InstanceInput();
	const auto wndMgr = gaf::InstanceWnd();
	const auto eventMgr = gaf::InstanceEvent();
	const auto wnd = wndMgr->CreateAWindow("InputTestWnd", "InputTestWnd", { 800, 600 }, { 0, 0 }, gaf::EWindowMode::WINDOWED);
	volatile bool wndCreationDone = false;
	wnd->SendWindowTask(CreateTask(WindowReadyCheck, [&]() {
		wndCreationDone = true;
	}));
	while (!wndCreationDone)
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	const auto wndPos = wnd->GetPosition();
	const auto wndRes = wnd->GetResolution();
	gaf::InputManager::InputSystemID inputSysID = gaf::InputManager::NullInputSystemID;
	gaf::InputSystem* inputSys = nullptr;
	volatile bool done = false;
	
	gaf::InputSystem::MouseListenerID mouseListener;
	gaf::InputSystem::KeyboardListenerID keyboardListener;

	auto mouseListeningFn = [&](const gaf::MouseState& state, const std::chrono::high_resolution_clock::time_point& time)
	{

	};
	auto keyboardListeningFn = [&](const gaf::KeyboardState& state, const std::chrono::high_resolution_clock::time_point& time)
	{

	};

	PRETEST_END();

	DOTEST_BEGIN("InputSystemCreationWithoutWindow");
	inputSysID = inputMgr->CreateSystem(nullptr);
	gaf::Assertion::WhenEqual(inputSysID, gaf::InputManager::NullInputSystemID, "Trying to create an InputSystem, but a null id was given, while performing a test.");
	DOTEST_END();

	DOTEST_BEGIN("GetInputSystem");
	inputSys = inputMgr->GetInputSystem(inputSysID);
	gaf::Assertion::WhenNullptr(inputSys, "Trying to get an InputSystem, but a nullptr one was returned, while performing a test.");
	DOTEST_END();

	{
		Test::Result _result;
		_result.TestName = "InputSystemWindowAttachment";
		const auto listener = eventMgr->RegisterEventListener([&](const gaf::EventID id, void* params)
		{
			if (id == gaf::InputSystem::EventIDOnInputSystemReady)
			{
				_result.AfterTest = std::chrono::high_resolution_clock::now();
				done = true;
			}
		}, gaf::InputSystem::EventIDOnInputSystemReady);
		_result.BeforeTest = std::chrono::high_resolution_clock::now();
		
		inputSys->AttachWindow(wnd);

		while (!done)
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		done = false;
		resultVec.push_back(std::move(_result));
		eventMgr->UnregisterEvent(listener);
	}

	DOTEST_BEGIN("MouseSetPosition");
	inputMgr->SetMousePosition(wndPos.first + 100, wndPos.second + 100);
	DOTEST_END();

	DOTEST_BEGIN("InputSystemMouseEventRegister");
	mouseListener = inputSys->RegisterMouseListener(mouseListeningFn);
	DOTEST_END();

	DOTEST_BEGIN("InputSystemKeyboardEventRegister");
	keyboardListener = inputSys->RegisterKeyboardListener(keyboardListeningFn);
	DOTEST_END();

	DOTEST_BEGIN("InputSystemMouseEventUnregister");
	const auto rtn = inputSys->UnregisterMouseListener(mouseListener);
	gaf::Assertion::WhenEqual(rtn, false, "Trying to unregister a mouse listener, but something went wrong, while performing a test.");
	DOTEST_END();

	DOTEST_BEGIN("InputSystemKeyboardEventUnregister");
	const auto rtn = inputSys->UnregisterKeyboardListener(keyboardListener);
	gaf::Assertion::WhenEqual(rtn, false, "Trying to unregister a keyboard listener, but something went wrong, while performing a test.");
	DOTEST_END();

	DOTEST_BEGIN("InputSystemDestroy");
	inputMgr->DestroySystem(inputSysID);
	DOTEST_END();

	wnd->Close();
}
