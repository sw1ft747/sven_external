// C++
// Auto Bunnyhop for Sven Co-op

#include "ini_parser.h"
#include "process_utils.h"

#define M_CONDITION if ((offset = (DWORD)strtol(vecSplit[1].c_str(), &buffer, 16)) != 0)

namespace Offsets
{
	DWORD dwOnLadder = 0x0782A490;
	DWORD dwOnGround = 0x0034A510;
	DWORD dwWaterLevel = 0x005C60C8;
	DWORD dwForceJump = 0x005FD244;
	DWORD dwPaused = 0x001DC078;
	DWORD dwChatbox = 0x067077E0;
	DWORD dwOverlay = 0x0014B6A0;
}

class ModuleOffsets
{
public:
	ModuleOffsets()
	{
		m_dwOnLadder = NULL;
		m_dwOnGround = NULL;
		m_dwWaterLevel = NULL;
		m_dwOverlay = NULL;
		m_dwPaused = NULL;
		m_dwChatbox = NULL;
		m_dwForceJump = NULL;
	}

	DWORD m_dwOnGround;
	DWORD m_dwOnLadder;
	DWORD m_dwWaterLevel;
	DWORD m_dwOverlay;
	DWORD m_dwPaused;
	DWORD m_dwChatbox;
	DWORD m_dwForceJump;
};

class Player
{
public:
	Player()
	{
		m_bOnLadder = NULL;
		m_OnGround = NULL;
		m_nWaterLevel = NULL;
		m_bOverlay = NULL;
		m_bPaused = NULL;
		m_bChatbox = NULL;
	}

	int32_t m_OnGround;
	BYTE m_bOnLadder;
	BYTE m_nWaterLevel;
	BYTE m_bOverlay;
	BYTE m_bPaused;
	BYTE m_bChatbox;
};

void AutoBunnyhop();
void StoreMemory(ModuleOffsets *, Player *, HANDLE);
bool LoadSettings(DWORD &);

bool bSound = true;
bool bIgnoreOverlay = false;

const std::string PROCESS_NAME = "svencoop.exe";
const std::string HW_DLL = "hw.dll";
const std::string CLIENT_DLL = "client.dll";
const std::string OVERLAY_DLL = "gameoverlayrenderer.dll";

HANDLE hProcess;
DWORD dwProcessID, dwHWDLLBase, dwClientDLLBase, dwOverlayDLLBase, dwToggleButton;

int main()
{
	SetConsoleTitleW(L"Sven Co-op Auto Bunnyhop");

	printf("Auto Bunnyhop for Sven Co-op\n\n");

	if (!LoadSettings(dwToggleButton))
	{
		printf("\nERROR: Couldn't parse the file\nExiting...\n");
		Sleep(5000);
		return 1;
	}

	printf("\nDetermining the game process...\n\n");

	while (true)
	{
		if (ProcessUtils::GetProcessID(PROCESS_NAME, &dwProcessID))
		{
			if (ProcessUtils::GetProcessHandle(&hProcess, dwProcessID))
			{
				if (ProcessUtils::GetModuleBaseAddress(HW_DLL, &dwHWDLLBase, dwProcessID) &&
					ProcessUtils::GetModuleBaseAddress(CLIENT_DLL, &dwClientDLLBase, dwProcessID) &&
					ProcessUtils::GetModuleBaseAddress(OVERLAY_DLL, &dwOverlayDLLBase, dwProcessID))
				{
					break;
				}
			}

			CloseHandle(hProcess);
		}

		Sleep(100);
	}

	printf("Successfully connected to Sven Co-op process\nEnjoy your game\n");

	AutoBunnyhop();

	CloseHandle(hProcess);

	Sleep(3000);

	return 0;
}

void StoreMemory(ModuleOffsets *modOffests, Player *LocalPlayer, HANDLE hCurrentProcess)
{
	ProcessUtils::ReadMemory(hCurrentProcess, modOffests->m_dwOnLadder, &LocalPlayer->m_bOnLadder, sizeof(BYTE));
	ProcessUtils::ReadMemory(hCurrentProcess, modOffests->m_dwOnGround, &LocalPlayer->m_OnGround, sizeof(DWORD));
	ProcessUtils::ReadMemory(hCurrentProcess, modOffests->m_dwWaterLevel, &LocalPlayer->m_nWaterLevel, sizeof(BYTE));
	ProcessUtils::ReadMemory(hCurrentProcess, modOffests->m_dwPaused, &LocalPlayer->m_bPaused, sizeof(BYTE));
	ProcessUtils::ReadMemory(hCurrentProcess, modOffests->m_dwChatbox, &LocalPlayer->m_bChatbox, sizeof(BYTE));

	if (!bIgnoreOverlay) ProcessUtils::ReadMemory(hCurrentProcess, modOffests->m_dwOverlay, &LocalPlayer->m_bOverlay, sizeof(BYTE));
}

void AutoBunnyhop()
{
	bool bAutoBunnyhop = true;
	bool bPressingButton = false;

	Player *LocalPlayer = new Player();
	ModuleOffsets *modOffests = new ModuleOffsets();

	BYTE nJumpOverride = 4, nJump = 5;

	modOffests->m_dwOnLadder = dwHWDLLBase + Offsets::dwOnLadder;
	modOffests->m_dwOnGround = dwHWDLLBase + Offsets::dwOnGround;
	modOffests->m_dwWaterLevel = dwClientDLLBase + Offsets::dwWaterLevel;
	modOffests->m_dwPaused = dwHWDLLBase + Offsets::dwPaused;
	modOffests->m_dwChatbox = dwHWDLLBase + Offsets::dwChatbox;
	modOffests->m_dwOverlay = dwOverlayDLLBase + Offsets::dwOverlay;
	modOffests->m_dwForceJump = dwClientDLLBase + Offsets::dwForceJump;

	ProcessUtils::WriteMemory(hProcess, modOffests->m_dwForceJump, &nJumpOverride, sizeof(BYTE));

	while (true)
	{
		Sleep(1);

		DWORD dwExitCode;
		GetExitCodeProcess(hProcess, &dwExitCode);
		if (!dwExitCode)
		{
			printf("\nThe game process has been closed\nExiting...\n");
			CloseHandle(LocalPlayer);
			CloseHandle(modOffests);
			break;
		}

		DWORD dwCurrentProcess;
		HWND hWindow = GetForegroundWindow();
		GetWindowThreadProcessId(hWindow, &dwCurrentProcess);
		CloseHandle(hWindow);

		if (GetAsyncKeyState(dwToggleButton) && !bPressingButton && dwProcessID == dwCurrentProcess)
		{
			if (bAutoBunnyhop) ProcessUtils::WriteMemory(hProcess, modOffests->m_dwForceJump, &nJumpOverride, sizeof(BYTE));
			if (bSound) printf(bAutoBunnyhop ? "\a" : "\a\a");
			bAutoBunnyhop = !bAutoBunnyhop;
			bPressingButton = true;
		}
		else if (!GetAsyncKeyState(dwToggleButton))
		{
			bPressingButton = false;
		}

		if (bAutoBunnyhop && GetAsyncKeyState(VK_SPACE))
		{
			StoreMemory(modOffests, LocalPlayer, hProcess);

			if (LocalPlayer->m_bOnLadder || LocalPlayer->m_bPaused || LocalPlayer->m_bChatbox || dwProcessID != dwCurrentProcess)
				continue;

			if (!bIgnoreOverlay && LocalPlayer->m_bOverlay)
				continue;

			if (LocalPlayer->m_nWaterLevel < 2)
			{
				ProcessUtils::WriteMemory(hProcess, modOffests->m_dwForceJump, (LocalPlayer->m_OnGround != -1) ? &nJump : &nJumpOverride, sizeof(BYTE));
			}
			else if (LocalPlayer->m_OnGround == -1)
			{
				ProcessUtils::WriteMemory(hProcess, modOffests->m_dwForceJump, &nJump, sizeof(BYTE));
			}
		}
	}
}

bool LoadSettings(DWORD &button)
{
	printf("Trying to find the config file...\n");

	ini_data data;
	ini_datatype datatype;

	bool bButtonFound = false;

	if (!ini_parse_data("bhop_settings.ini", &data))
	{
		if (ini_get_last_error() == INI_MISSING_FILE)
			printf("Missing file bhop_settings.ini to parse\n");
		else
			printf("Syntax error: %s in line %d\n", ini_get_last_error_msg(), ini_get_last_line());

		printf("WARNING: Key 'INSERT' has been used as default\n");
		button = VK_INSERT;

		return false;
	}

	INI_FIELDTYPE_BOOL(datatype);

	if (ini_read_data(&data, "SETTINGS", "sound", &datatype))
	{
		if (!datatype.m_bool)
		{
			bSound = false;
			printf("SETTINGS: Sound has been disabled\n");
		}
	}

	if (ini_read_data(&data, "SETTINGS", "ignoreoverlay", &datatype))
	{
		if (datatype.m_bool)
			bIgnoreOverlay = true;
	}

	INI_FIELDTYPE_UINT32(datatype, 16);

	if (ini_read_data(&data, "CONTROLS", "button", &datatype))
	{
		bButtonFound = true;
		button = datatype.m_uint32;

		if (button != VK_INSERT)
		{
			printf("SETTINGS: Button value has been changed to 0x%X\n", button);
		}
	}

	if (ini_read_data(&data, "OFFSETS", "onground", &datatype))
		Offsets::dwOnGround = datatype.m_uint32;

	if (ini_read_data(&data, "OFFSETS", "onladder", &datatype))
		Offsets::dwOnLadder = datatype.m_uint32;

	if (ini_read_data(&data, "OFFSETS", "waterlevel", &datatype))
		Offsets::dwWaterLevel = datatype.m_uint32;

	if (ini_read_data(&data, "OFFSETS", "paused", &datatype))
		Offsets::dwPaused = datatype.m_uint32;

	if (ini_read_data(&data, "OFFSETS", "chatopened", &datatype))
		Offsets::dwChatbox = datatype.m_uint32;

	if (ini_read_data(&data, "OFFSETS", "overlayopened", &datatype))
		Offsets::dwOverlay = datatype.m_uint32;

	if (ini_read_data(&data, "OFFSETS", "forcejump", &datatype))
		Offsets::dwForceJump = datatype.m_uint32;

	if (!bButtonFound)
	{
		printf("WARNING: Couldn't find button option\n");
		printf("WARNING: Key 'INSERT' has been used as default\n");
		button = VK_INSERT;
	}

	printf("Parsed the config file\n");
	return true;
}