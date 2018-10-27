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

#ifndef GAF_MAINHELPER_H
#define GAF_MAINHELPER_H 1

namespace gaf
{
	constexpr auto CommandLineArgNumPropName = "CMDLINE";
	uint32 GetNumberCommandLineArgs();
	std::string GetCommandLineArg(uint32 num);
}
namespace Impl
{
#if PLATFORM_WINDOWS
	void GreaperInitWin(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd);
	void GreaperInit(int argc, char* argv[]);
#else
	void GreaperInit(int argc, char* argv[]);
#endif
}

#ifndef MAIN_CREATE_BEGIN
#if PLATFORM_WINDOWS && !defined(_CONSOLE)
namespace gaf
{
	constexpr auto InstancePropName = "HINSTANCE";
	constexpr auto CmdShowPropName = "CMDSHOW";
	HINSTANCE GetINSTANCE();
	int32 GetShowCommand();
}

LPSTR* CommandLineToArgvA(LPSTR lpCmdLine, INT* pNumArgs);

#define MAIN_CREATE_BEGIN()\
INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)\
{\
	Impl::GreaperInitWin(hInstance, hPrevInstance, lpCmdLine, nShowCmd); \
	GREAPER_MEMORY_DEBUG_BEGIN(); \
	INT rtn = EXIT_SUCCESS; \
	try	{

#define MAIN_CREATE_END()\
	}catch(std::exception& e){ gaf::Assertion::Always("Exception has risen, reason: %s, check error.log.", e.what()); };\
	GREAPER_MEMORY_DEBUG_END();\
	return rtn;\

#else /* ^^^ WIN32 !CONSOLE ^^^ */
#define MAIN_CREATE_BEGIN()\
int main(int argc, char* argv[])\
{\
	Impl::GreaperInit(argc, argv);\
	GREAPER_MEMORY_DEBUG_BEGIN(); \
	int rtn = EXIT_SUCCESS;\
	try{


#define MAIN_CREATE_END()\
	}catch(std::exception& e){ gaf::Assertion::Always("Exception has risen, reason: %s, check error.log.", e.what()); }\
	catch(...){ gaf::Assertion::Always("Unhandled exception risen!, check error.log"); };\
	GREAPER_MEMORY_DEBUG_END();\
	return rtn;\
}
#endif
#endif

#endif /* GAF_MAINHELPER_H */