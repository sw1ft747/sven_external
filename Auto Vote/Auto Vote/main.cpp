// C++
// Auto Vote for Sven Co-op

#include "process_utils.h"
#include <string>
#include <fstream>

void DetermineProcess();
void AutoVote(bool);

const wchar_t *PROCESS_NAME = L"svencoop.exe";
const wchar_t *HW_DLL = L"hw.dll";
const wchar_t *CLIENT_DLL = L"client.dll";
const wchar_t *OVERLAY_DLL = L"gameoverlayrenderer.dll";

const DWORD VOTE_POPUP_PANEL_SHOWED_OFFSET = 0x00000070;
const DWORD VOTE_POPUP_PANEL_OFFSET = 0x000011DC;
const DWORD GVIEWPORT_OFFSET = 0x001BB884;
const DWORD GAME_PAUSED_OFFSET = 0x001DC078;
const DWORD CHAT_OPENED_OFFSET = 0x067077E0;
const DWORD STEAMOVERLAY_OPENED_OFFSET = 0x0014B6A0;

HANDLE hProcess;
DWORD dwProcessID, dwHWDLLBase, dwClientDLLBase, dwOverlayDLLBase;

int main()
{
	SetConsoleTitleW(L"Sven Co-op Auto Vote");
	printf("Sven Co-op Auto Vote (c) Sw1ft\n\n");

	DetermineProcess();

	bool bVoteNo = true;

	std::string sLine;
	std::ifstream settings("auto_vote.cfg");

	if (settings.is_open())
	{
		getline(settings, sLine);
		if (sLine == "false" || sLine == "0")
			bVoteNo = false;
	}
	else
	{
		printf("\nWARNING: Couldn't find a file with settings");
		printf("\nWARNING: Vote 'No' used as default\n");
	}

	settings.close();

	printf("\nSuccessfully connected to Sven Co-op process\nEnjoy your game\n");

	AutoVote(bVoteNo);

	CloseHandle(hProcess);

	Sleep(3000);

	return 0;
}

void DetermineProcess()
{
	printf("Determining the game process...\n");
	while (true)
	{
		if (GetProcessID(PROCESS_NAME, &dwProcessID))
		{
			if (GetProcessHandle(&hProcess, dwProcessID))
			{
				if (GetModuleBaseAddress(HW_DLL, &dwHWDLLBase, dwProcessID) &&
					GetModuleBaseAddress(CLIENT_DLL, &dwClientDLLBase, dwProcessID) &&
					GetModuleBaseAddress(OVERLAY_DLL, &dwOverlayDLLBase, dwProcessID))
				{
					break;
				}
			}
			CloseHandle(hProcess);
			Sleep(100);
		}
	}
}

void AutoVote(bool bVoteNo)
{
	bool bPressed = false;

	INPUT input;
	WORD nCycle = 5000;
	DWORD dwVGUICollection, dwVoteStarted, dwPaused, dwChatbox, dwOverlay;

	input.type = INPUT_KEYBOARD;
	input.ki.time = 0;
	input.ki.wVk = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.wScan = bVoteNo ? 0x03 : 0x02; // '2' or '1'

	dwPaused = dwHWDLLBase + GAME_PAUSED_OFFSET;
	dwChatbox = dwHWDLLBase + CHAT_OPENED_OFFSET;
	dwOverlay = dwOverlayDLLBase + STEAMOVERLAY_OPENED_OFFSET;
	dwVGUICollection = dwClientDLLBase + GVIEWPORT_OFFSET;

	while (true)
	{
		Sleep(1);
		nCycle++;

		DWORD dwExitCode, dwCurrentProcess, bPaused, bChatbox, bOverlay, bVoteStarted;
		GetExitCodeProcess(hProcess, &dwExitCode);
		if (!dwExitCode)
		{
			printf("\nThe game process has been closed\nExiting...\n");
			break;
		}

		if (nCycle >= 5000)
		{
			ReadProcessMemory(hProcess, (LPCVOID)dwVGUICollection, &dwVoteStarted, sizeof(DWORD), NULL);
			ReadProcessMemory(hProcess, (LPCVOID)(dwVoteStarted + VOTE_POPUP_PANEL_OFFSET), &dwVoteStarted, sizeof(DWORD), NULL);
			dwVoteStarted += VOTE_POPUP_PANEL_SHOWED_OFFSET;
			nCycle = 0;
		}

		HWND hWindow = GetForegroundWindow();
		GetWindowThreadProcessId(hWindow, &dwCurrentProcess);
		CloseHandle(hWindow);

		ReadProcessMemory(hProcess, (LPCVOID)dwPaused, &bPaused, sizeof(DWORD), NULL);
		ReadProcessMemory(hProcess, (LPCVOID)dwChatbox, &bChatbox, sizeof(DWORD), NULL);
		ReadProcessMemory(hProcess, (LPCVOID)dwOverlay, &bOverlay, sizeof(DWORD), NULL);
		ReadProcessMemory(hProcess, (LPCVOID)dwVoteStarted, &bVoteStarted, sizeof(DWORD), NULL);

		if (bVoteStarted)
		{
			if (!bPressed)
			{
				if (bPaused || bChatbox || bOverlay || dwProcessID != dwCurrentProcess)
					continue;

				bPressed = true;

				input.ki.dwFlags = KEYEVENTF_SCANCODE;
				SendInput(1, &input, sizeof(INPUT));

				input.ki.dwFlags |= KEYEVENTF_KEYUP;
				SendInput(1, &input, sizeof(INPUT));
			}
		}
		else
		{
			bPressed = false;
		}
	}
}