#ifndef SM_CVAR_H
#define SM_CVAR_H

#ifdef _WIN32
#pragma once
#endif

bool CvarInit();
void CvarShutdown();

void CvarEnablePrint();
void CvarDisablePrint();

#endif // SM_CVAR_H