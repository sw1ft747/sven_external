// C++
// Votekill Spam - Main

#include <Windows.h>

#include "config_parser.h"
#include "votekill_spam.h"
#include "signature_scanner.h"
#include "signatures.h"
#include "hooks.h"
#include "sdk.h"

namespace Offsets
{
	DWORD dwVGuiCollection = 0x001BB884;
	DWORD dwVotePopup = 0x11DC;
	DWORD dwVotePoppedUp = 0x0070;
	DWORD dwInMenu = 0x001DC078;
}

struct votekill_params_s g_Params = { nullptr, nullptr, 16.0f, 1.0f, 0.0f, 1 };
extern void ExposeSingletonObject(const struct votekill_params_s &);

cl_enginefunc_t *g_EngineFuncs = nullptr;
cl_clientfunc_t *g_ClientFuncs = nullptr;

DWORD g_VGuiCollection = 0;
DWORD g_VotePopup = 0;

bool GetGameFuncs();

DWORD WINAPI MainThread(HMODULE hModule)
{
	FILE *file;
	AllocConsole();
	freopen_s(&file, "CONOUT$", "w", stdout);

	DWORD dwHWDLL;
	if (dwHWDLL = (DWORD)GetModuleHandle(L"hw.dll"))
	{
		if (!GetGameFuncs())
			goto EXIT;

		DWORD dwClientDLL;
		if (dwClientDLL = (DWORD)GetModuleHandle(L"client.dll"))
		{
			if (g_VGuiCollection = *reinterpret_cast<PDWORD>(dwClientDLL + Offsets::dwVGuiCollection))
				g_VotePopup = *reinterpret_cast<PDWORD>(g_VGuiCollection + Offsets::dwVotePopup);
		}
		else
		{
			goto EXIT;
		}

		if (!ParseConfig(&g_Params))
			goto EXIT;

		printf("[Votekill Spam] Successfully parsed the config file\n");

		ExposeSingletonObject(g_Params);
		HookFunctions();

		g_EngineFuncs->Con_Printf("[Votekill Spam] Successfully injected\n");
		printf("[Votekill Spam] Press 'END' key to exit\n");

		while (true)
		{
			if (GetAsyncKeyState(VK_END))
			{
				printf("\a");
				break;
			}

			Sleep(250);
		}

		UnhookFunctions();
	}
	else
	{
	EXIT:
		printf("[ERROR] Exiting...\n");
		Sleep(3000);
	}

	if (file) fclose(file);
	FreeConsole();
	FreeLibraryAndExitThread(hModule, 0);

	return 0;
}

bool GetGameFuncs()
{
	void *pEngineFuncs = FindPattern(L"hw.dll", Signatures::EngineFuncs, Signatures::Masks::EngineFuncs);
	void *pClientFuncs = FindPattern(L"hw.dll", Signatures::ClientFuncs, Signatures::Masks::ClientFuncs);

	if (pEngineFuncs == nullptr || pClientFuncs == nullptr)
		return false;

	pEngineFuncs = reinterpret_cast<void *>(*(PDWORD)((DWORD)pEngineFuncs + 0x03));
	pClientFuncs = reinterpret_cast<void *>(*(PDWORD)((DWORD)pClientFuncs + 0x16));

	g_EngineFuncs = reinterpret_cast<cl_enginefunc_t *>(pEngineFuncs);
	g_ClientFuncs = reinterpret_cast<cl_clientfunc_t *>(pClientFuncs);

	return true;
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