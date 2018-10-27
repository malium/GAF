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
#include "GAF/PropertiesManager.h"

using namespace gaf;

#if PLATFORM_WINDOWS
union
{
	float fVal;
	int32 iVal;
}FIConv;

void Impl::GreaperInitWin(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	StaticProperty instanceProp{ gaf::InstancePropName, true, std::to_string((PTRUINT)(void*)hInstance), {}, {} };
	FIConv.iVal = nShowCmd;
	StaticProperty cmdShowProp{ gaf::CmdShowPropName, true, FIConv.fVal,1.f, -1.f, {} };
	std::vector<std::string> args;
	int32 nArgs;
	auto rawArgs = CommandLineToArgvA(lpCmdLine, &nArgs);
	gaf::Assertion::WhenNullptr(rawArgs, "Couldn't retrieve the commandline arguments.");
	args.reserve(nArgs);
	args.emplace_back("ARGS");
	for (int32 i = 1; i < nArgs; ++i)
		args.emplace_back(rawArgs[i]);
	LocalFree(rawArgs);
	StaticProperty gArgs("ARGS", true, "ARGS", args, {});
	StaticProperty gIsConsoleEnabled("OS_CONSOLE_ENABLED", true, false, {});
}
#endif

void Impl::GreaperInit(int argc, char* argv[])
{
	std::vector<std::string> args;
	args.reserve(argc);
	args.emplace_back("ARGS");
	for (int32 i = 1; i < argc; ++i)
		args.emplace_back(argv[i]);
	StaticProperty gArgs("ARGS", true, "ARGS", args, {});
	StaticProperty gIsConsoleEnabled("OS_CONSOLE_ENABLED", true, true, {});
}

uint32 gaf::GetNumberCommandLineArgs()
{
	const auto prop = InstanceProp()->GetProperty(CommandLineArgNumPropName);
	return (uint32)(prop->GetValidValueStrings().size() - 1);
}

std::string gaf::GetCommandLineArg(const uint32 num)
{
	const auto prop = InstanceProp()->GetProperty(CommandLineArgNumPropName);
	const auto& vvs = prop->GetValidValueStrings();
	if (vvs.size() < num + 1)
		return "NULL_COMMAND_LINE";
	else
		return vvs[num + 1];
}

#if PLATFORM_WINDOWS

HINSTANCE gaf::GetINSTANCE()
{
	const auto prop = InstanceProp()->GetProperty(InstancePropName);
	return (HINSTANCE)std::strtoull(prop->GetStringValue().c_str(), nullptr, 10);
}

int32 gaf::GetShowCommand()
{
	FIConv.fVal = InstanceProp()->GetProperty(CmdShowPropName)->GetNumberValue();
	return FIConv.iVal;
}

LPSTR * CommandLineToArgvA(LPSTR lpCmdLine, INT * pNumArgs)
{
	int32 retval;
	retval = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, lpCmdLine, -1, nullptr, 0);
	if (!SUCCEEDED(retval))
		return nullptr;
	LPWSTR lpWideCharStr = (LPWSTR)malloc(retval * sizeof(WCHAR));
	if (!lpWideCharStr)
		return nullptr;
	retval = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, lpCmdLine, -1, lpWideCharStr, retval);
	if (!SUCCEEDED(retval))
	{
		free(lpWideCharStr);
		return nullptr;
	}

	int32 numArgs;
	LPWSTR* args = CommandLineToArgvW(lpWideCharStr, &numArgs);
	free(lpWideCharStr);
	if (!args)
		return nullptr;

	int32 storage = numArgs * sizeof(LPSTR);
	for (int32 i = 0; i < numArgs; ++i)
	{
		BOOL lpUsedDefaultChar = FALSE;
		retval = WideCharToMultiByte(CP_ACP, 0, args[i], -1, nullptr, 0, nullptr, &lpUsedDefaultChar);
		if (!SUCCEEDED(retval))
		{
			LocalFree(args);
			return nullptr;
		}
		storage += retval;
	}
	LPSTR* result = (LPSTR*)LocalAlloc(LMEM_FIXED, storage);
	if (!result)
	{
		LocalFree(args);
		return nullptr;
	}

	int32 bufLen = storage - numArgs * sizeof(LPSTR);
	LPSTR buffer = ((LPSTR)result) + numArgs * sizeof(LPSTR);
	for (int32 i = 0; i < numArgs; ++i)
	{
		gaf::Assertion::WhenLessEqual(bufLen, 0, "");
		BOOL lpUsedDefaultChar = FALSE;
		retval = WideCharToMultiByte(CP_ACP, 0, args[i], -1, buffer, bufLen, nullptr, &lpUsedDefaultChar);
		if (!SUCCEEDED(retval))
		{
			LocalFree(result);
			LocalFree(args);
			return nullptr;
		}

		result[i] = buffer;
		buffer += retval;
		bufLen -= retval;
	}

	LocalFree(args);
	*pNumArgs = numArgs;
	return result;
}

#endif
