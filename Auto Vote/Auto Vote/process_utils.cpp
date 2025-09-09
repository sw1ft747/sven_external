// C++
// Process Utils

#include "process_utils.h"

bool GetProcessID(const wchar_t *wsProcessName, DWORD *dwProcessID)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	PROCESSENTRY32 modEntry;
	modEntry.dwSize = sizeof(PROCESSENTRY32);

	while (Process32Next(hSnapshot, &modEntry))
	{
		if (!_wcsicmp(modEntry.szExeFile, wsProcessName))
		{
			*dwProcessID = (DWORD)modEntry.th32ProcessID;
			CloseHandle(hSnapshot);
			return true;
		}
	}

	CloseHandle(hSnapshot);
	return false;
}

bool GetModuleBaseAddress(const wchar_t *wsModuleName, DWORD *dwModuleBase, DWORD dwProcessID)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, dwProcessID);

	MODULEENTRY32 modEntry;
	modEntry.dwSize = sizeof(MODULEENTRY32);

	while (Module32Next(hSnapshot, &modEntry))
	{
		if (!_wcsicmp(modEntry.szModule, wsModuleName))
		{
			*dwModuleBase = (DWORD)modEntry.modBaseAddr;
			CloseHandle(hSnapshot);
			return true;
		}
	}

	CloseHandle(hSnapshot);
	return false;
}

bool GetProcessHandle(HANDLE *hProcess, DWORD dwProcessID)
{
	*hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessID);
	return hProcess != NULL;
}