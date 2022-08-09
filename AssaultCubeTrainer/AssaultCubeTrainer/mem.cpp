#include "pch.h"
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
