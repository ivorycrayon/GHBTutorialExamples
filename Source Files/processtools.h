#pragma once

#include <Windows.h>
#include <TlHelp32.h>

//get process id from an executable name using tlhelp32
DWORD GetProcID(const wchar_t* exeName);

//Get ModuleEntry from module name, using Tl32Spnapshot
MODULEENTRY32 GetModule(DWORD procId, const wchar_t* modName);

//Not sure how this differs from getmodule
uintptr_t GetModuleBaseAddress(DWORD procID, const wchar_t* modName);

//Resolve multi-level pointers
uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);