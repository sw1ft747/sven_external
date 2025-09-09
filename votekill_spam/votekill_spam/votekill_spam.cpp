// C++
// Votekill Spam

#include "votekill_spam.h"

CVotekillSpam *g_VotekillSpam;

CVotekillSpam::CVotekillSpam(const struct votekill_params_s &params) :
	m_flNextVoteTime(-1.0f),
	m_flDisconnectTime(-1.0f),
	m_Activity(ACTIVITY_NONE),
	m_bConnecting(false)
{
	m_Params = params;
}

void CVotekillSpam::HandleActivityState()
{
	bool bInMenu = *reinterpret_cast<bool *>((DWORD)GetModuleHandle(L"hw.dll") + Offsets::dwInMenu);
	
	switch (m_Activity)
	{
	case ACTIVITY_NONE:
		if (bInMenu)
			ConnectToServer();
		else
			StartVote();

	case ACTIVITY_VOTING:
		if (!bInMenu)
		{
			if (m_Params.flForcedDisconnectDelay > 0.f)
			{
				if (m_flDisconnectTime == -1.0f)
				{
					m_flDisconnectTime = g_EngineFuncs->Sys_FloatTime() + m_Params.flForcedDisconnectDelay;
				}
				else if (m_flDisconnectTime <= g_EngineFuncs->Sys_FloatTime())
				{
					g_EngineFuncs->pfnClientCmd("disconnect");

					m_flNextVoteTime = g_EngineFuncs->Sys_FloatTime() + m_Params.flVoteDuration - m_Params.flConnectionDuration;
					m_Activity = ACTIVITY_WAITING;
					m_bConnecting = false;
				}
			}

			StartVote();
		}

	case ACTIVITY_WAITING:
		if (m_flNextVoteTime <= g_EngineFuncs->Sys_FloatTime())
			ConnectToServer();
	}
}

void CVotekillSpam::StartVote()
{
	g_EngineFuncs->pfnClientCmd(format_string("votekill \"%s\"", m_Params.szPlayerName));
	
	if (IsVotePoppedUp())
	{
		if (m_Params.nVoteType >= 0)
			g_EngineFuncs->pfnClientCmd(!m_Params.nVoteType ? "slot2" : "slot1");

		g_EngineFuncs->pfnClientCmd("disconnect");

		m_flNextVoteTime = g_EngineFuncs->Sys_FloatTime() + m_Params.flVoteDuration - m_Params.flConnectionDuration;
		m_Activity = ACTIVITY_WAITING;
		m_bConnecting = false;
	}
}

void CVotekillSpam::ConnectToServer()
{
	if (m_bConnecting)
		return;

	g_EngineFuncs->pfnClientCmd(format_string("connect %s", m_Params.szServerIP));
	m_flDisconnectTime = -1.0f;
	m_Activity = ACTIVITY_VOTING;
	m_bConnecting = true;
}

char *format_string(const char *szMsg, ...)
{
	static char buffer[256];
	va_list args;
	va_start(args, szMsg);
	vsnprintf(buffer, sizeof(buffer), szMsg, args);
	va_end(args);
	return buffer;
}

void ExposeSingletonObject(const struct votekill_params_s &params)
{
	if (g_VotekillSpam == nullptr)
	{
		g_VotekillSpam = new CVotekillSpam(params);
	}
}

void OnGameFrame()
{
	if (g_VotekillSpam != nullptr)
	{
		g_VotekillSpam->HandleActivityState();
	}
}

inline bool IsVotePoppedUp()
{
	if (!g_VGuiCollection)
	{
		if (g_VGuiCollection = *reinterpret_cast<PDWORD>((DWORD)GetModuleHandle(L"client.dll") + Offsets::dwVGuiCollection))
		{
			g_VotePopup = *reinterpret_cast<PDWORD>(g_VGuiCollection + Offsets::dwVotePopup);
			return *reinterpret_cast<bool *>(g_VotePopup + Offsets::dwVotePoppedUp);
		}

		return false;
	}

	return *reinterpret_cast<bool *>(g_VotePopup + Offsets::dwVotePoppedUp);
}