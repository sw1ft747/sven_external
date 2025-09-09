// C++
// Process Utils

#include "process_utils.h"

bool ProcessUtils::GetProcessID(const std::string sProcessName, DWORD *dwProcessID)
{
	std::wstring widestr = std::wstring(sProcessName.begin(), sProcessName.end());
	const wchar_t *wsProcessName = widestr.c_str();

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

bool ProcessUtils::GetModuleBaseAddress(const std::string sModuleName, DWORD *dwModuleBase, DWORD dwProcessID)
{
	std::wstring widestr = std::wstring(sModuleName.begin(), sModuleName.end());
	const wchar_t *wsModuleName = widestr.c_str();

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

bool ProcessUtils::GetProcessHandle(HANDLE *hProcess, DWORD dwProcessID)
{
	*hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, dwProcessID);
	return hProcess != NULL;
}

void ProcessUtils::ReadMemory(HANDLE hProcess, DWORD dwAddress, void *buffer, SIZE_T size)
{
	ReadProcessMemory(hProcess, (LPCVOID)dwAddress, buffer, size, NULL);
}

void ProcessUtils::WriteMemory(HANDLE hProcess, DWORD dwAddress, void *buffer, SIZE_T size)
{
	WriteProcessMemory(hProcess, (LPVOID)dwAddress, buffer, size, NULL);
}