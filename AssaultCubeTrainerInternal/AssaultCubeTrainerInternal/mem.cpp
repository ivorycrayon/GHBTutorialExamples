#include "mem.h"

void mem::PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess) 
//src - new byte instructions to patch, dst - destination address to patch over
//size is the # of instruction BYTE*s we're using
{
	DWORD oldprotect; //old permissions we will rewrite later
	VirtualProtectEx(hProcess, dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	WriteProcessMemory(hProcess, dst, src, size, nullptr);
	VirtualProtectEx(hProcess, dst, size, oldprotect, &oldprotect);

}
void mem::NopEx(BYTE* dst, unsigned int size, HANDLE hProcess)
{
	BYTE* nopArray = new BYTE[size];
	memset(nopArray, 0x90, size); //0x90 is NOP instruction

	PatchEx(dst, nopArray, size, hProcess);
	delete[] nopArray;
}

uintptr_t mem::FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets)
{
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), NULL);
		addr += offsets[i];
	}
	return addr;
}

/*
Internal Versions Do not use Handles
Internal Does not use RPM or WPM - uses memory directly
*/

void mem::Patch(BYTE* dst, BYTE* src, unsigned int size)
//src - new byte instructions to patch, dst - destination address to patch over
//size is the # of instruction BYTE*s we're using
{
	DWORD oldprotect; //old permissions we will rewrite later
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);

	memcpy(dst, src, size); //memcpy is used in place of WPM for internal
	VirtualProtect(dst, size, oldprotect, &oldprotect);

}
void mem::Nop(BYTE* dst, unsigned int size)
{
	DWORD oldprotect; //old permissions we will rewrite later
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);

	memset(dst, 0x90, size); //0x90 - nop fucntion
	VirtualProtect(dst, size, oldprotect, &oldprotect);
}

uintptr_t mem::FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets)
{
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		addr = *(uintptr_t*)addr;
		addr += offsets[i];
	}
	return addr;
}