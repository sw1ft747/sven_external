#include "injector.h"
#include "ini-parser/ini_parser.h"

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

struct InjectSettings
{
	const char *pszProcessName;
	const char *pszLaunchParameters;

	const wchar_t *pwcProcessName;
	const wchar_t *pwcDllPath;

	DWORD dwInjectDelay;
	BOOL bAutoRun;
	BOOL bUseLaunchParameters;

} g_InjectSettings;

bool IsCorrectTargetArchitecture(HANDLE hProc) {
	BOOL bTarget = FALSE;
	if (!IsWow64Process(hProc, &bTarget)) {
		printf("Can't confirm target process architecture: 0x%X\n", GetLastError());
		return false;
	}

	BOOL bHost = FALSE;
	IsWow64Process(GetCurrentProcess(), &bHost);

	return (bTarget == bHost);
}

DWORD GetProcessIdByName(const wchar_t* name) {
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry) == TRUE) {
		while (Process32Next(snapshot, &entry) == TRUE) {
			if (_wcsicmp(entry.szExeFile, name) == 0) {
				return entry.th32ProcessID;
				CloseHandle(snapshot);
			}
		}
	}

	CloseHandle(snapshot);
	return 0;
}

const wchar_t *CStringToWideCString(const char *pszString)
{
	const size_t length = strlen(pszString) + 1;
	wchar_t *wcString = new wchar_t[length];

	mbstowcs(wcString, pszString, length);

	return wcString;
}

bool ParseFile()
{
	printf("Trying to find the config file...\n");

	ini_data data;
	ini_datatype datatype;

	if (!ini_parse_data("mm_injector.ini", &data))
	{
		if (ini_get_last_error() == INI_MISSING_FILE)
			printf("Missing file mm_injector.ini to parse\n");
		else
			printf("Syntax error: %s in line %d\n", ini_get_last_error_msg(), ini_get_last_line());

		return false;
	}

	printf("Found the config file\nReading data..\n");

	INI_FIELDTYPE_CSTRING(datatype);

	if (ini_read_data(&data, "SETTINGS", "ProcessName", &datatype))
	{
		g_InjectSettings.pwcProcessName = CStringToWideCString(datatype.m_pszString);
		g_InjectSettings.pszProcessName = datatype.m_pszString;
	}
	else
	{
		printf("Missing parameter ProcessName in section SETTINGS");
		return false;
	}
	
	if (ini_read_data(&data, "SETTINGS", "DllPath", &datatype))
	{
		g_InjectSettings.pwcDllPath = CStringToWideCString(datatype.m_pszString);
		free((void *)datatype.m_pszString);
	}
	else
	{
		printf("Missing parameter DllPath in section SETTINGS");
		return false;
	}
	
	INI_FIELDTYPE_UINT32(datatype, 10);

	if (ini_read_data(&data, "SETTINGS", "InjectDelay", &datatype))
	{
		g_InjectSettings.dwInjectDelay = datatype.m_uint32;
	}
	else
	{
		printf("Missing parameter InjectDelay in section SETTINGS");
		return false;
	}

	INI_FIELDTYPE_BOOL(datatype);

	if (ini_read_data(&data, "AUTORUN", "AutoRun", &datatype))
	{
		g_InjectSettings.bAutoRun = datatype.m_bool;
	}
	else
	{
		printf("Missing parameter AutoRun in section AUTORUN");
		return false;
	}
	
	INI_FIELDTYPE_BOOL(datatype);

	if (ini_read_data(&data, "AUTORUN", "UseLaunchParameters", &datatype))
	{
		g_InjectSettings.bUseLaunchParameters = datatype.m_bool;
	}
	else
	{
		printf("Missing parameter UseLaunchParameters in section AUTORUN");
		return false;
	}

	if (g_InjectSettings.bUseLaunchParameters)
	{
		INI_FIELDTYPE_CSTRING(datatype);

		if (ini_read_data(&data, "AUTORUN", "LaunchParameters", &datatype))
		{
			g_InjectSettings.pszLaunchParameters = datatype.m_pszString;
		}
		else
		{
			printf("Missing parameter LaunchParameters in section AUTORUN");
			return false;
		}
	}

	printf("Done.\n");

	ini_free_data(&data, 0);

	return true;
}

int main(void)
{
	DWORD PID = 0;

	if (!ParseFile())
	{
		system("pause");
		return -5;
	}

	if (g_InjectSettings.bAutoRun)
	{
		string autoRunTarget = "start ";
		autoRunTarget += g_InjectSettings.pszProcessName;

		if (g_InjectSettings.bUseLaunchParameters)
		{
			autoRunTarget += " ";
			autoRunTarget += g_InjectSettings.pszLaunchParameters;
		}

		system(autoRunTarget.c_str());
	}

	if (g_InjectSettings.dwInjectDelay)
		Sleep(g_InjectSettings.dwInjectDelay);

	PID = GetProcessIdByName(g_InjectSettings.pwcProcessName);

	if (PID == 0) {
		printf("Process not found\n");
		system("pause");
		return -1;
	}

	printf("Process pid: %d\n", PID);
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	if (!hProc) {
		DWORD Err = GetLastError();
		printf("OpenProcess failed: 0x%X\n", Err);
		system("PAUSE");
		return -2;
	}

	if (!IsCorrectTargetArchitecture(hProc)) {
		printf("Invalid Process Architecture.\n");
		CloseHandle(hProc);
		system("PAUSE");
		return -3;
	}

	printf("Mapping...\n");
	if (!ManualMap(hProc, g_InjectSettings.pwcDllPath))
	{
		CloseHandle(hProc);
		printf("Error while mapping.\n");
		system("PAUSE");
		return -4;
	}

	CloseHandle(hProc);
	printf("OK\n");
	return 0;
}
