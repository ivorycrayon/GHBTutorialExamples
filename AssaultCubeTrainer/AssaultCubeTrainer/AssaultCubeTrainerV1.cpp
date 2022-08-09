// alt shift ;         = all word
// // alt shift .         = all word
// alt shift arrow     = multi cursor

#include "pch.h"
#include <iostream>
#include <vector>
#include <Windows.h>
#include "proc.h"

int main()
{
    //Get ProcID of target process
    DWORD procID = GetProcID(L"ac_client.exe");

    //Get module base address
    uintptr_t moduleBase = GetModuleBaseAddress(procID, L"ac_client.exe");

    //Get Handle to Process
    HANDLE hProcess = 0;
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procID);

    //Resolve base address of the pointer chain
    uintptr_t dynamicPtrBaseAddr = moduleBase + 0x10F4F4;

    std::cout << "DynamicPtrBaseAddress =  " << "0x" << std::hex << std::uppercase << dynamicPtrBaseAddr << std::endl;

    //Resolver ammo pointer chain
    std::vector<unsigned int> ammoOffsets = { 0x374, 0x14, 0x0 };
    uintptr_t ammoAddr = FindDMAAddy(hProcess, dynamicPtrBaseAddr, ammoOffsets);

    std::cout << "Ammo Address =  " << "0x" << std::hex << ammoAddr << std::endl;

    //Read Ammo Value
    int ammoValue = 0;

    ReadProcessMemory(hProcess, (BYTE*)ammoAddr, &ammoValue, sizeof(ammoValue), nullptr);
    std::cout << "Current Ammo =  " << std::dec << ammoValue << std::endl;

    //Write Ammo
    int newAmmo = 6969;
    WriteProcessMemory(hProcess, (BYTE*)ammoAddr, &newAmmo, sizeof(newAmmo), nullptr);


    //Read Ammo Again
    ReadProcessMemory(hProcess, (BYTE*)ammoAddr, &ammoValue, sizeof(ammoValue), nullptr);
    std::cout << "New Ammo =  " << std::dec << ammoValue << std::endl;


    getchar();
    return 0;
}
