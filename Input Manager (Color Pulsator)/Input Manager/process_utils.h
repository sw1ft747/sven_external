// C++
// Process Utils Header

#pragma once

#include <Windows.h>
#include <TlHelp32.h>

bool GetProcessHandle(HANDLE *, DWORD);
bool GetProcessID(const wchar_t *, DWORD *);
bool GetModuleBaseAddress(const wchar_t *, DWORD *, DWORD);