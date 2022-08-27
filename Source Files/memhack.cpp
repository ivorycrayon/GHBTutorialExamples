#include "memhack.h"

//internal patch
void Patch(void* dst, void* src, unsigned int size)
{
	DWORD oldProtect;
	VirtualProtect(dst, size, PAGE_READWRITE, &oldProtect);
	memcpy(dst, src, size);
	VirtualProtect(dst, size, oldProtect, &oldProtect);
}

//external
void PatchEx(HANDLE hProcess, void* dst, void* src, unsigned int size)
{
	DWORD oldProtect;
	VirtualProtectEx(hProcess, dst, size, PAGE_READWRITE, &oldProtect);
	WriteProcessMemory(hProcess, dst, src, size, NULL);
	VirtualProtectEx(hProcess, dst, size, oldProtect, &oldProtect);
}

//Internal Nop
void Nop(HANDLE hProcess, void* dst, unsigned int size)
{
	DWORD oldProtect;
	VirtualProtect(dst, size, PAGE_READWRITE, &oldProtect);
	memset(dst, 0x90, size);
	VirtualProtect(dst, size, oldProtect, &oldProtect);
}

//External Nop
void NopEx(HANDLE hProcess, void* dst, unsigned int size)
{
	BYTE* nopArray = new BYTE[size];
	memset(nopArray, 0x90, size);

	DWORD oldProtect;
	VirtualProtectEx(hProcess, dst, size, PAGE_READWRITE, &oldProtect);
	WriteProcessMemory(hProcess, dst, nopArray, size, NULL);
	VirtualProtectEx(hProcess, dst, size, oldProtect, &oldProtect);
	
	delete[] nopArray;
}