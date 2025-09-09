// C++
// Hooks Header

#ifndef HOOKS_H
#define HOOKS_H

#include <Windows.h>
#include "sdk.h"

extern cl_clientfunc_t *g_ClientFuncs;
extern void OnGameFrame();

void (*HUD_Frame_Original) (double time);

void HUD_Frame_Hook(double time)
{
	HUD_Frame_Original(time);
	OnGameFrame();
}

void HookFunctions()
{
	HUD_Frame_Original = g_ClientFuncs->HUD_Frame;
	g_ClientFuncs->HUD_Frame = HUD_Frame_Hook;
}

void UnhookFunctions()
{
	g_ClientFuncs->HUD_Frame = HUD_Frame_Original;
}

#endif