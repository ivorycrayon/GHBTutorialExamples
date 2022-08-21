#include "pch.h"
#include <windows.h>

bool Hook(void* toHook, void* ourFunct, int len) // tohook- pointer to function we're hooking, ourfunct- spot we're hooking jumps to, len - # opcodes we're writing
{
	if (len < 5)
	{
		return false;
	}

	DWORD curProtection;
	VirtualProtect(toHook, len, PAGE_EXECUTE_READWRITE, &curProtection);

	memset(toHook, 0x90, len);

	DWORD relativeAdress = ((DWORD)ourFunct - (DWORD)toHook - 5); //offset from the jump to where we need to go

	*(BYTE*)toHook = 0xE9;
	*(DWORD*)((DWORD)toHook + 1) = relativeAdress;

	DWORD temp;
	VirtualProtect(toHook, len, curProtection, &temp);

	return true;
}