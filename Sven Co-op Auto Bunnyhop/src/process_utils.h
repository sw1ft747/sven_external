// C++
// Process Utils Header

#pragma once

#include <string>
#include <Windows.h>
#include <TlHelp32.h>

namespace ProcessUtils
{
	bool GetProcessID(const std::string, DWORD *);

	bool GetModuleBaseAddress(const std::string, DWORD *, DWORD);

	bool GetProcessHandle(HANDLE *, DWORD);

	void ReadMemory(HANDLE, DWORD, void *, SIZE_T);

	void WriteMemory(HANDLE, DWORD, void *, SIZE_T);
}