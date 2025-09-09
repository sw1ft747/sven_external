// C++
// Input Manager for Sven Co-op

#include "process_utils.h"
#include "string_methods.h"
#include <fstream>
#include <thread>

class CClient
{
public:
	bool m_bGamePaused;
	bool m_bChatOpened;
	bool m_bSteamOverlayOpened;

	CClient() : m_bGamePaused(0), m_bChatOpened(0), m_bSteamOverlayOpened(0) { }
};

class CModuleOffsets
{
public:
	DWORD m_dwGamePaused;
	DWORD m_dwChatOpened;
	DWORD m_dwSteamOverlayOpened;

	CModuleOffsets() : m_dwGamePaused(NULL), m_dwChatOpened(NULL), m_dwSteamOverlayOpened(NULL) { }
};

class CInput
{
private:
	BYTE *m_Keys;
	BYTE m_nOffset;
	size_t m_nLength;
	INPUT m_Input;
	
public:
	CInput(BYTE *Keys, size_t nLength) : m_nOffset(0)
	{
		m_Keys = Keys;
		m_nLength = nLength;

		m_Input.type = INPUT_KEYBOARD;
		m_Input.ki.time = 0;
		m_Input.ki.wVk = 0;
		m_Input.ki.dwExtraInfo = 0;
	}
	~CInput()
	{
		delete[] m_Keys;
	}

	void SendInput()
	{
		if (m_nOffset >= m_nLength)
		{
			m_nOffset = 0;
		}

		m_Input.ki.dwFlags = KEYEVENTF_SCANCODE;
		m_Input.ki.wScan = m_Keys[m_nOffset++];
		::SendInput(1, &m_Input, sizeof(INPUT));

		m_Input.ki.dwFlags |= KEYEVENTF_KEYUP;
		::SendInput(1, &m_Input, sizeof(INPUT));
	}
};

bool ParseFile();
void DetermineProcess();
void ToggleButtonListener();
void InputManager();

bool bEnabled = true;

const wchar_t *PROCESS_NAME = L"svencoop.exe";
const wchar_t *HW_DLL = L"hw.dll";
const wchar_t *OVERLAY_DLL = L"gameoverlayrenderer.dll";

CInput *input;
CClient *client = new CClient;
CModuleOffsets *modOffsets = new CModuleOffsets;

HANDLE hProcess;
DWORD dwDelay, dwProcessID, dwHWDLLBase, dwOverlayDLLBase;
DWORD dwToggleButton = 0x2E;

int main()
{
	SetConsoleTitleW(L"Sven Co-op Input Manager");
	printf("Sven Co-op Input Manager (c) Sw1ft\n\n");

	if (!ParseFile())
	{
		printf("\nCouldn't parse the file\nExiting...\n");
		Sleep(5000);
		return 1;
	} printf("Done.\n");

	DetermineProcess();

	printf("\nSuccessfully connected to Sven Co-op process\nEnjoy your game\n");

	std::thread Thread(ToggleButtonListener);
	Thread.detach();

	InputManager();

	CloseHandle(hProcess);

	delete input;
	delete client;
	delete modOffsets;

	Sleep(3000);

	return 0;
}

void DetermineProcess()
{
	printf("\nDetermining the game process...\n");

	while (true)
	{
		if (GetProcessID(PROCESS_NAME, &dwProcessID))
		{
			if (GetProcessHandle(&hProcess, dwProcessID))
			{
				if (GetModuleBaseAddress(HW_DLL, &dwHWDLLBase, dwProcessID) &&
					GetModuleBaseAddress(OVERLAY_DLL, &dwOverlayDLLBase, dwProcessID))
				{
					break;
				}
			}
			CloseHandle(hProcess);
		}
		Sleep(100);
	}

	printf("Done.\n");
}

void ToggleButtonListener()
{
	bool bPressingButton = false;

	while (true)
	{
		if (GetAsyncKeyState(dwToggleButton) && !bPressingButton)
		{
			bEnabled = !bEnabled;
			bPressingButton = true;
		}
		else if (!GetAsyncKeyState(dwToggleButton))
		{
			bPressingButton = false;
		}

		Sleep(1);
	}
}

void InputManager()
{
	modOffsets->m_dwGamePaused += dwHWDLLBase;
	modOffsets->m_dwChatOpened += dwHWDLLBase;
	modOffsets->m_dwSteamOverlayOpened += dwOverlayDLLBase;

	while (true)
	{
		Sleep(dwDelay);

		DWORD dwExitCode = 0, dwCurrentProcess = 0;
		GetExitCodeProcess(hProcess, &dwExitCode);
		
		if (!dwExitCode)
		{
			printf("\nThe game process has been closed\nExiting...\n");
			break;
		}

		if (bEnabled)
		{
			HWND hWindow = GetForegroundWindow();
			GetWindowThreadProcessId(hWindow, &dwCurrentProcess);
			CloseHandle(hWindow);

			ReadProcessMemory(hProcess, (LPCVOID)modOffsets->m_dwGamePaused, &client->m_bGamePaused, sizeof(BYTE), NULL);
			ReadProcessMemory(hProcess, (LPCVOID)modOffsets->m_dwChatOpened, &client->m_bChatOpened, sizeof(BYTE), NULL);
			ReadProcessMemory(hProcess, (LPCVOID)modOffsets->m_dwSteamOverlayOpened, &client->m_bSteamOverlayOpened, sizeof(BYTE), NULL);

			if (client->m_bGamePaused || client->m_bChatOpened || client->m_bSteamOverlayOpened || dwProcessID != dwCurrentProcess)
				continue;

			input->SendInput();
		}
	}
}

bool ParseFile()
{
	printf("Parsing the config file...\n");

	std::string sLine;
	std::ifstream Settings("input_settings.cfg");

	if (Settings.is_open())
	{
		char *buffer;

		const std::string sComment = "//";
		const std::string sSeparator = "=";

		while (getline(Settings, sLine))
		{
			if (StringBeginsWithPrefix(sLine, sComment))
				continue;

			std::vector<std::string> vecSplit;
			SplitString(vecSplit, sLine, sSeparator);

			if (vecSplit.size() == 2)
			{
				Strip(vecSplit[0]);
				Strip(vecSplit[1]);

				std::string sKey = vecSplit[0];

				if (sKey == "delay")
				{
					dwDelay = (DWORD)strtol(vecSplit[1].c_str(), &buffer, 10);
				}
				else if (sKey == "toggle_key")
				{
					dwToggleButton = (DWORD)strtol(vecSplit[1].c_str(), &buffer, 16);
				}
				else if (sKey == "pause")
				{
					modOffsets->m_dwGamePaused = (DWORD)strtol(vecSplit[1].c_str(), &buffer, 16);
				}
				else if (sKey == "chat")
				{
					modOffsets->m_dwChatOpened = (DWORD)strtol(vecSplit[1].c_str(), &buffer, 16);
				}
				else if (sKey == "steamoverlay")
				{
					modOffsets->m_dwSteamOverlayOpened = (DWORD)strtol(vecSplit[1].c_str(), &buffer, 16);
				}
				else if (sKey == "keys")
				{
					std::vector<std::string> vecKeys;
					SplitString(vecKeys, vecSplit[1], (std::string)",");

					size_t size = vecKeys.size();
					if (size == 0)
					{
						printf("ERROR: Specify at least one key with comma as separator\n");
						Settings.close();
						return false;
					}

					BYTE *keys = new BYTE[size];
					for (size_t i = 0; i < size; i++)
					{
						Strip(vecKeys[i]);
						keys[i] = (BYTE)strtol(vecKeys[i].c_str(), &buffer, 16);
					}

					input = new CInput(keys, size);
					keys = nullptr;
				}
			}
		}
	}
	else
	{
		printf("ERROR: Couldn't find 'input_settings.cfg' file\n");
		return false;
	}

	Settings.close();
	return true;
}