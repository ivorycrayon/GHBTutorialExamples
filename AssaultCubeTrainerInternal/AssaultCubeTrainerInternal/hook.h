#pragma once
#include <windows.h>
#include "mem.h"

bool HookBasic(void* toHook, void* ourFunct, int len); // tohook- pointer to function we're hooking, ourfunct- spot we're hooking jumps to, len - # opcodes we're writing

bool Detour32(BYTE* src, BYTE* dst, const uintptr_t len);
BYTE* TrampHook32(BYTE* src, BYTE* dst, const uintptr_t len);

struct Hook
{
	bool bStatus = false;

	BYTE* src{ nullptr };
	BYTE* dst{ nullptr };
	BYTE* ptrToGatewayFnPtr{ nullptr };
	int len{ 0 };

	BYTE originalBytes[10]{ 0 };

	Hook(BYTE* src, BYTE* dst, BYTE* ptrToGatewayFnPtr, uintptr_t len);
	Hook(const char* exportName, const char* modName, BYTE* dst, BYTE* ptrToGatewayFnPtr, uintptr_t len);

	void Enable();
	void Disable();
	void Toggle();
};