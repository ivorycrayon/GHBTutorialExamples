#include "pch.h"
#include <windows.h>
#include <TlHelp32.h>

DWORD GetProcID(const char* procName)
{
	DWORD procID = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //creates a snapshot of all process

	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(hSnap, &procEntry)) //gets first process
		{
			do
			{
				if (!_stricmp(procEntry.szExeFile, procName)) //strcomp without case sensitivity
				{
					procID = procEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnap, &procEntry)); //gets next process
		}
	}
	CloseHandle(hSnap);
	return procID;
}

int main()
{
	const char* dllPath = "C:\\Users\\Ivory\\source\\repos\\AssaultCubeTrainerInternal\\Debug\\AssaultCubeTrainerInternal.dll";
	const char* procName = "ac_client.exe";
	DWORD procID = 0;

	while (!procID)
	{
		procID = GetProcID(procName);
		Sleep(30);
	}

	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procID);

	if (hProc && hProc != INVALID_HANDLE_VALUE)
	{ //get a section of memory in the target process, allocate it
		void* loc = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); //alloc memory in an external process

		if (loc)
		{
			WriteProcessMemory(hProc, loc, dllPath, strlen(dllPath) + 1, 0); //rewrite path to dll in mem to create a thread to call the load lib
			// path has to be in the target process to work
		}
		HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, 0); //load the dll

		if (hThread)
		{
			CloseHandle(hThread);
		}

	}

	if (hProc)
	{
		CloseHandle(hProc);
	}

	return 0;
}
