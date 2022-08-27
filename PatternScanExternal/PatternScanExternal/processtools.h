#pragma once

#include <Windows.h>
#include <TlHelp32.h>

//get process id from an executable name using tlhelp32
DWORD GetProcID(const wchar_t* exeName);

//Get ModuleEntry from module name, using Tl32Spnapshot
MODULEENTRY32 GetModule(DWORD procId, const wchar_t* modName);