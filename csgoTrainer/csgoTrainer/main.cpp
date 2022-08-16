// alt shift ;         = all word
// // alt shift .         = all word
// alt shift arrow     = multi cursor

#include "pch.h"
#include <iostream>
#include <vector>
#include <Windows.h>
#include "proc.h"
#include "mem.h"

int main()
{
    uintptr_t moduleBase = 0, localPlayerPtr = 0, healthAddr = 0;
    HANDLE hProcess = 0;
    DWORD procID = GetProcID(L"csgo.exe");

    if (procID)
    {
        hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procID);

        moduleBase = GetModuleBaseAddress(procID, L"client.dll");
        std::cout << "Module Base Address =  " << "0x" << std::hex << std::uppercase << moduleBase << std::endl;

        localPlayerPtr = moduleBase + 0x0DBF4BC;
        std::cout << "Local Player Ptr Address =  " << "0x" << std::hex << std::uppercase << localPlayerPtr << std::endl;

        healthAddr = FindDMAAddy(hProcess, localPlayerPtr, { 0x100 });
        std::cout << "Health Base Address =  " << "0x" << std::hex << std::uppercase << healthAddr << std::endl;
    }
    else
    {
        std::cout << "Process not found, press enter to exit\n";
        getchar();
        return 0;
    }

    int healthValue = 0;
    ReadProcessMemory(hProcess, (BYTE*)healthAddr, &healthValue, sizeof(healthValue), nullptr);
    std::cout << "Current health = " << std::dec << healthValue << std::endl;

    DWORD dwExit = 0;
    
    while (GetExitCodeProcess(hProcess, &dwExit) && dwExit == STILL_ACTIVE)
    {
        ReadProcessMemory(hProcess, (BYTE*)healthAddr, &healthValue, sizeof(healthValue), nullptr);
        std::cout << "Current health = " << std::dec << healthValue << std::endl;
        getchar();
        Sleep(10);
    }

    std::cout << "Process not found, press enter to exit\n";
    getchar();
    return 0;
}
