#include <Windows.h>
#include <iostream>
#include "memhack.h"
#include "patternscan.h"
#include "processtools.h"

int main()
{
	//Get Process ID by enumerating the processes using TlHelp32Snapshot
	DWORD processID = GetProcID(L"ac_client.exe");

	//Get handle by using OpenProcess
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, processID);

	//get module
	MODULEENTRY32 module = GetModule(processID, L"ac_client.exe");

	//Pattern Scan
	char* pattern = const_cast<char*>("\x29\x7b\x00\x8b\xc7");
	char* mask = const_cast<char*>("xx?xx");

	char* healthDecAddress = ScanModEx(pattern, mask, module, hProcess);
	std::cout << std::hex << healthDecAddress;

	//Nop the instructions
	NopEx(hProcess, healthDecAddress, 5);

	return 0;
}