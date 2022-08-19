#pragma once
#include <Windows.h>
#include <vector>

class mem
{
public:
	mem();
	~mem();
	template <class val>
	val readMem(DWORD addr) //takes an address, reads and returns the value
	{
		val x;
		ReadProcessMemory(handle, (LPBYTE*)addr, &x, sizeof(x), NULL);
		return x;
	}
	template <class val>
	val writeMem(DWORD addr, val x)
	{
		WriteProcessMemory(handle, (LPBYTE*)addr, &x, sizeof(x), NULL);
		return 0;
	}
	DWORD getProcess(const wchar_t*);
	uintptr_t getModule(DWORD, const wchar_t*);
	uintptr_t getAddress(uintptr_t, std::vector<unsigned int>);
	HANDLE handle;
};