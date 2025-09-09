// C++
// Votekill Spam Header

#ifndef VOTEKILL_SPAM_H
#define VOTEKILL_SPAM_H

#include <Windows.h>
#include "sdk.h"

namespace Offsets
{
	extern DWORD dwVGuiCollection;
	extern DWORD dwVotePopup;
	extern DWORD dwVotePoppedUp;
	extern DWORD dwInMenu;
}

extern cl_enginefunc_t *g_EngineFuncs;
extern DWORD g_VGuiCollection;
extern DWORD g_VotePopup;

typedef enum
{
	ACTIVITY_NONE = 0,
	ACTIVITY_WAITING,
	ACTIVITY_VOTING
} votekill_activity_t;

struct votekill_params_s
{
	const char *szServerIP;
	const char *szPlayerName;
	float flVoteDuration;
	float flConnectionDuration;
	float flForcedDisconnectDelay;
	BYTE nVoteType;
};

class CVotekillSpam
{
public:
	CVotekillSpam(const struct votekill_params_s &params);

	void HandleActivityState();

private:
	void StartVote();

	void ConnectToServer();

	float m_flNextVoteTime;
	float m_flDisconnectTime;
	bool m_bConnecting;
	votekill_activity_t m_Activity;
	struct votekill_params_s m_Params;
};

char *format_string(const char *szMsg, ...);
void ExposeSingletonObject(const struct votekill_params_s &);
void OnGameFrame();

inline bool IsVotePoppedUp();

#endif