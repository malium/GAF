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

#include "GAF/LogManager.h"
#include "GAF/HWDetector.h"
#include <intrin.h>

using namespace gaf;

HWDetector* HWDetector::m_Instance = nullptr;

#if PLATFORM_WINDOWS
static void cpuid(int32 out[4], const int32 x)
{
	__cpuidex(out, x, 0);
}
#else
#include <cpuid.h>
void cpuid(int info[4], int InfoType) {
	__cpuid_count(InfoType, 0, info[0], info[1], info[2], info[3]);
}
#endif

static void cpuread(CPUInfo& infoStruct)
{
	int info[4];
	cpuid(info, 0);
	int nIds = info[0];

	cpuid(info, 0x80000000);
	unsigned nExIds = info[0];

	//  Detect Features
	if (nIds >= 0x00000001) {
		cpuid(info, 0x00000001);
		infoStruct.Features.htt = (info[3] & ((int)1 << 28)) != 0;
		infoStruct.Features.sse3 = (info[2] & ((int)1 << 0)) != 0;

		infoStruct.Features.ssse3 = (info[2] & ((int)1 << 9)) != 0;
		infoStruct.Features.sse4_1 = (info[2] & ((int)1 << 19)) != 0;
		infoStruct.Features.sse4_2 = (info[2] & ((int)1 << 20)) != 0;
		infoStruct.Features.AES = (info[2] & ((int)1 << 25)) != 0;

		infoStruct.Features.avx = (info[2] & ((int)1 << 28)) != 0;
		infoStruct.Features.fma3 = (info[2] & ((int)1 << 12)) != 0;

		infoStruct.Features.smx = (info[2] & ((int)1 << 6)) != 0;
		infoStruct.Features.cx16 = (info[2] & ((int)1 << 13)) != 0;
	}
	if (nIds >= 0x00000007) {
		cpuid(info, 0x00000007);
		infoStruct.Features.avx2 = (info[1] & ((int)1 << 5)) != 0;

		infoStruct.Features.sgx = (info[1] & ((int)1 << 2)) != 0;
		infoStruct.Features.erms = (info[1] & ((int)1 << 9)) != 0;

		infoStruct.Features.bmi1 = (info[1] & ((int)1 << 3)) != 0;
		infoStruct.Features.bmi2 = (info[1] & ((int)1 << 8)) != 0;
		infoStruct.Features.sha = (info[1] & ((int)1 << 29)) != 0;

		infoStruct.Features.avx512f = (info[1] & ((int)1 << 16)) != 0;
		infoStruct.Features.avx512cd = (info[1] & ((int)1 << 28)) != 0;
		infoStruct.Features.avx512pf = (info[1] & ((int)1 << 26)) != 0;
		infoStruct.Features.avx512er = (info[1] & ((int)1 << 27)) != 0;
		infoStruct.Features.avx512vl = (info[1] & ((int)1 << 31)) != 0;
		infoStruct.Features.avx512bw = (info[1] & ((int)1 << 30)) != 0;
		infoStruct.Features.avx512dq = (info[1] & ((int)1 << 17)) != 0;
		infoStruct.Features.avx512ifma = (info[1] & ((int)1 << 21)) != 0;
		infoStruct.Features.avx512vbmi = (info[2] & ((int)1 << 1)) != 0;
		infoStruct.Features.avx512vbmi2 = (info[2] & ((int)1 << 6)) != 0;
		infoStruct.Features.vaes = (info[2] & ((int)1 << 9)) != 0;
		infoStruct.Features.vpclmulqdq = (info[2] & ((int)1 << 10)) != 0;
		infoStruct.Features.avx512vnni = (info[2] & ((int)1 << 11)) != 0;
		infoStruct.Features.avx512bitalg = (info[2] & ((int)1 << 12)) != 0;
		infoStruct.Features.avx512vpopcntdq = (info[2] & ((int)1 << 14)) != 0;
	}
	if (nExIds >= 0x80000001) {
		cpuid(info, 0x80000001);
		infoStruct.Features.sse4_a = (info[2] & ((int)1 << 6)) != 0;
		infoStruct.Features.fma4 = (info[2] & ((int)1 << 16)) != 0;
	}

	cpuid(info, 1);

	bool osUsesXSAVE_XRSTORE = info[2] & (1 << 27) || false;
	

	if (osUsesXSAVE_XRSTORE && infoStruct.Features.avx)
	{
		unsigned long long xcrFeatureMask = _xgetbv(_XCR_XFEATURE_ENABLED_MASK);
		infoStruct.Features.avx = (xcrFeatureMask & 0x6) == 0x6;
	}

	/* Detect CPU string */
	ANSICHAR str[13];
	cpuid(info, 0);
	memcpy(str + 0, &info[1], 4);
	memcpy(str + 4, &info[3], 4);
	memcpy(str + 8, &info[2], 4);
	str[12] = '\0';
	if (strcmp(str, "GenuineIntel") == 0)
		infoStruct.Vendor = ECPUVendor::INTEL;
	else if (strcmp(str, "AuthenticAMD") == 0)
		infoStruct.Vendor = ECPUVendor::AMD;
	else
		infoStruct.Vendor = ECPUVendor::UNKNOWN;
}

static const ANSICHAR* FeatureCheck(const int feature)
{
	static const auto yes = "True";
	static const auto no = "False";
	return feature == 0 ? no : yes;
}

HWDetector::HWDetector()
{
	LogManager::LogMessage(LL_INFO, "Starting System features detection...");
	cpuread(m_Info);
	const auto logical = GetNumberLogicalCores();
	const auto physical = m_Info.Features.htt != 0 ? logical / 2 : logical;
	LogManager::LogMessage(LL_INFO,
		"System information:\n"
		"\tOS: %s\n"
		"\tCPU Vendor: %s\n"
		"\tCPU Logical core count: %d\n"
		"\tCPU Physical core count: %d\n"
		"\tRAM installed: %fGB\n"
		"\tAES instruction set supported: %s\n"
		"\tEnhanced REP MOVSB/STOSB supported: %s\n"
		"\tF16C (half-precision) FP supported: %s\n"
		"\tFused multiply-add (FMA3) supported: %s\n"
		"\tFused multiply-add (FMA4) supported: %s\n"
		"\tCLMUL instruction set (VEX-256/EVEX) supported: %s\n"
		"\tBit Manipulation Instruction Set 1 supported: %s\n"
		"\tBit Manipulation Instruction Set 2 supported: %s\n"
		"\tIntel SHA extensions supported: %s\n"
		"\tVector AES instruction set (VEX-256/EVEX) supported: %s\n"
		"\tHyper-Threading supported: %s\n"
		"\tSSE3 instructions supported: %s\n"
		"\tSupplemental SSE3 instructions supported: %s\n"
		"\tSSE4.1 instructions supported: %s\n"
		"\tSSE4.2 instructions supported: %s\n"
		"\tSSE4a instructions supported: %s\n"
		"\tAdvanced Vector Extensions supported: %s\n"
		"\tAdvanced Vector Extensions 2 supported: %s\n"
		"\tAVX-512 Foundation supported: %s\n"
		"\tAVX-512 Conflict Detection Instructions supported: %s\n"
		"\tAVX-512 Exponential and Reciprocal Instructions supported: %s\n"
		"\tAVX-512 Prefetch Instructions supported: %s\n"
		"\tAVX-512 Byte and Word Instructions supported: %s\n"
		"\tAVX-512 Doubleword and Quadword Instructions supported: %s\n"
		"\tAVX-512 Vector Length Extensions supported: %s\n"
		"\tAVX-512 Integer Fused Multiply-Add Instructions supported: %s\n"
		"\tAVX-512 Vector Bit Manipulation Instructions supported: %s\n"
		"\tAVX-512 Vector Bit Manipulation Instructions 2 supported: %s\n"
		"\tAVX-512 Vector Neural Network Instructions supported: %s\n"
		"\tAVX-512 BITALG instructions supported: %s\n"
		"\tAVX-512 Vector Population Count Double and Quad-word supported: %s\n"
		"\tSafer Mode Extensions (LaGrande) supported: %s\n"
		"\tSoftware Guard Extensions supported: %s\n"
		"\tCMPXCHG16B instruction supported: %s",
		GetOSNameAndVer().c_str(),
		m_Info.Vendor == ECPUVendor::INTEL ? "INTEL" :
		m_Info.Vendor == ECPUVendor::AMD ? "AMD" : "UNKNOWN",
		logical, physical, (float)(((double)GetRAMAmount()) / (1024.0 * 1024.0)),
		FeatureCheck(m_Info.Features.AES),
		FeatureCheck(m_Info.Features.erms),
		FeatureCheck(m_Info.Features.f16c),
		FeatureCheck(m_Info.Features.fma3),
		FeatureCheck(m_Info.Features.fma4),
		FeatureCheck(m_Info.Features.vpclmulqdq),
		FeatureCheck(m_Info.Features.bmi1),
		FeatureCheck(m_Info.Features.bmi2),
		FeatureCheck(m_Info.Features.sha),
		FeatureCheck(m_Info.Features.vaes),
		FeatureCheck(m_Info.Features.htt),
		FeatureCheck(m_Info.Features.sse3),
		FeatureCheck(m_Info.Features.ssse3),
		FeatureCheck(m_Info.Features.sse4_1),
		FeatureCheck(m_Info.Features.sse4_2),
		FeatureCheck(m_Info.Features.sse4_a),
		FeatureCheck(m_Info.Features.avx),
		FeatureCheck(m_Info.Features.avx2),
		FeatureCheck(m_Info.Features.avx512f),
		FeatureCheck(m_Info.Features.avx512cd),
		FeatureCheck(m_Info.Features.avx512er),
		FeatureCheck(m_Info.Features.avx512pf),
		FeatureCheck(m_Info.Features.avx512bw),
		FeatureCheck(m_Info.Features.avx512dq),
		FeatureCheck(m_Info.Features.avx512vl),
		FeatureCheck(m_Info.Features.avx512ifma),
		FeatureCheck(m_Info.Features.avx512vbmi),
		FeatureCheck(m_Info.Features.avx512vbmi2),
		FeatureCheck(m_Info.Features.avx512vnni),
		FeatureCheck(m_Info.Features.avx512bitalg),
		FeatureCheck(m_Info.Features.avx512vpopcntdq),
		FeatureCheck(m_Info.Features.smx),
		FeatureCheck(m_Info.Features.sgx),
		FeatureCheck(m_Info.Features.cx16)
		);
}

HWDetector* HWDetector::Instance()
{
	if (!m_Instance)
	{
		m_Instance = new HWDetector();
	}
	return m_Instance;
}

void HWDetector::Stop()
{
	SAFE_DELETE(m_Instance);
}

uint32 HWDetector::GetNumberLogicalCores()const
{
	return Max(1U, std::thread::hardware_concurrency());
}

uint32 HWDetector::GetNumberPhysicalCores()const
{
	return m_Info.Features.htt ? GetNumberLogicalCores() / 2 : GetNumberLogicalCores();
}

CPUInfo HWDetector::GetCPUInfo()const
{
	return m_Info;
}

uint64 HWDetector::GetRAMAmount() const
{
	uint64 amount;
#if PLATFORM_WINDOWS
	if (!GetPhysicallyInstalledSystemMemory(&amount))
	{
		LogManager::LogMessage(LL_ERRO, "Trying to get the physically installed system memory, but something went wrong, error: 0x%08X.", GetLastError());
	}
#else
	/* TODO Implement in other systems */
#endif
	return amount;
}

#if PLATFORM_WINDOWS
#include <lm.h>
#pragma comment(lib, "netapi32.lib")

/* Copied from: https://stackoverflow.com/questions/2877295/get-os-in-c-win32-for-all-versions-of-win */
bool GetWinMajorMinorVersion(DWORD& major, DWORD& minor)
{
	bool bRetCode = false;
	LPBYTE pinfoRawData = 0;
	if (NERR_Success == NetWkstaGetInfo(NULL, 100, &pinfoRawData))
	{
		WKSTA_INFO_100* pworkstationInfo = (WKSTA_INFO_100*)pinfoRawData;
		major = pworkstationInfo->wki100_ver_major;
		minor = pworkstationInfo->wki100_ver_minor;
		::NetApiBufferFree(pinfoRawData);
		bRetCode = true;
	}
	return bRetCode;
}

std::string GetWindowsVersionString()
{
	std::string     winver;
	OSVERSIONINFOEX osver;
	SYSTEM_INFO     sysInfo;
	typedef void(__stdcall *GETSYSTEMINFO) (LPSYSTEM_INFO);

	__pragma(warning(push))
		__pragma(warning(disable:4996))
		memset(&osver, 0, sizeof(osver));
	osver.dwOSVersionInfoSize = sizeof(osver);
	GetVersionEx((LPOSVERSIONINFO)&osver);
	__pragma(warning(pop))
		DWORD major = 0;
	DWORD minor = 0;
	if (GetWinMajorMinorVersion(major, minor))
	{
		osver.dwMajorVersion = major;
		osver.dwMinorVersion = minor;
	}
	else if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 2)
	{
		OSVERSIONINFOEXW osvi;
		ULONGLONG cm = 0;
		cm = VerSetConditionMask(cm, VER_MINORVERSION, VER_EQUAL);
		ZeroMemory(&osvi, sizeof(osvi));
		osvi.dwOSVersionInfoSize = sizeof(osvi);
		osvi.dwMinorVersion = 3;
		if (VerifyVersionInfoW(&osvi, VER_MINORVERSION, cm))
		{
			osver.dwMinorVersion = 3;
		}
	}

	GETSYSTEMINFO getSysInfo = (GETSYSTEMINFO)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "GetNativeSystemInfo");
	if (getSysInfo == NULL)  getSysInfo = ::GetSystemInfo;
	getSysInfo(&sysInfo);

	if (osver.dwMajorVersion == 10 && osver.dwMinorVersion >= 0 && osver.wProductType != VER_NT_WORKSTATION)  winver = "Windows 10 Server";
	if (osver.dwMajorVersion == 10 && osver.dwMinorVersion >= 0 && osver.wProductType == VER_NT_WORKSTATION)  winver = "Windows 10";
	if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 3 && osver.wProductType != VER_NT_WORKSTATION)  winver = "Windows Server 2012 R2";
	if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 3 && osver.wProductType == VER_NT_WORKSTATION)  winver = "Windows 8.1";
	if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 2 && osver.wProductType != VER_NT_WORKSTATION)  winver = "Windows Server 2012";
	if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 2 && osver.wProductType == VER_NT_WORKSTATION)  winver = "Windows 8";
	if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 1 && osver.wProductType != VER_NT_WORKSTATION)  winver = "Windows Server 2008 R2";
	if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 1 && osver.wProductType == VER_NT_WORKSTATION)  winver = "Windows 7";
	if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 0 && osver.wProductType != VER_NT_WORKSTATION)  winver = "Windows Server 2008";
	if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 0 && osver.wProductType == VER_NT_WORKSTATION)  winver = "Windows Vista";
	if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 2 && osver.wProductType == VER_NT_WORKSTATION
		&& sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)  winver = "Windows XP x64";
	if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 2)   winver = "Windows Server 2003";
	if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 1)   winver = "Windows XP";
	if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 0)   winver = "Windows 2000";
	if (osver.dwMajorVersion < 5)   winver = "unknown";

	if (osver.wServicePackMajor != 0)
	{
		std::string sp;
		char buf[128] = { 0 };
		sp = " Service Pack ";
		sprintf_s(buf, sizeof(buf), "%hd", osver.wServicePackMajor);
		sp.append(buf);
		winver += sp;
	}

	return winver;
}

#endif

std::string HWDetector::GetOSNameAndVer()
{
#if PLATFORM_WINDOWS
	static const auto rtn = GetWindowsVersionString();
	return rtn;
#elif PLATFORM_MAC
	return "MACOSX";
#else
	return "LINUX SYSTEM";
#endif
}
