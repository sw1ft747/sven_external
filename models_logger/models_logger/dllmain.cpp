// C++
// Models Parser for Sven Co-op

#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <stdio.h>

#include "signature_scanner.h"
#include "signatures.h"
#include "sdk.h"

cl_enginefunc_t *g_EngineFuncs = nullptr;

bool GetGameFuncs()
{
	void *pEngineFuncs = FindPattern(L"hw.dll", Signatures::EngineFuncs, Signatures::Masks::EngineFuncs);

	if (pEngineFuncs == nullptr)
		return false;

	pEngineFuncs = reinterpret_cast<void *>(*(PDWORD)((DWORD)pEngineFuncs + 0x03));
	g_EngineFuncs = reinterpret_cast<cl_enginefunc_t *>(pEngineFuncs);

	return true;
}

DWORD WINAPI MainThread(HMODULE hModule)
{
	FILE *file;
	AllocConsole();
	freopen_s(&file, "CONOUT$", "w", stdout);

	SetConsoleTitle(L"Sven Co-op Models Logger");

	DWORD dwHWDLL;
	if (dwHWDLL = (DWORD)GetModuleHandle(L"hw.dll"))
	{
		if (!GetGameFuncs())
			goto __ERROR;

		FILE *output = fopen("models.txt", "w");

		hud_player_info_t playerInfo;
		for (int i = 1; i <= g_EngineFuncs->GetMaxClients(); i++)
		{
			g_EngineFuncs->pfnGetPlayerInfo(i, &playerInfo);

			if (playerInfo.name == nullptr)
				continue;

			fprintf(output, "player = %s\n", playerInfo.name);
			fprintf(output, "model = %s\n", playerInfo.model);
			fprintf(output, "topcolor = %hu\n", playerInfo.topcolor);
			fprintf(output, "bottomcolor = %hu\n\n", playerInfo.bottomcolor);
		}

		fclose(output);
	}
	else
	{
	__ERROR:
		printf("[ERROR] Failed to get 'hw.dll' module or find 'cl_enginefunc_s' struct.\n");
		printf("[ERROR] Exiting...\n");
		goto __EXIT;
	}

	printf("[Models Logger] Done.\n");
	printf("[Models Logger] Check 'models.txt' file in the game directory.\n");

__EXIT:
	Sleep(3000);

	if (file) fclose(file);
	FreeConsole();
	FreeLibraryAndExitThread(hModule, 0);

	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		HANDLE hThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, nullptr);
		if (hThread != nullptr) CloseHandle(hThread);
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}