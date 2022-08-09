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
    HANDLE hProcess = 0;

    uintptr_t moduleBase = 0, localPlayerPtr = 0, healthAddr = 0;
    bool bHealth = false, bAmmo = false, bRecoil = false;

    const int newValue = 1337;

    DWORD procID = GetProcID(L"ac_client.exe");

    if (procID)
    {
        hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procID);

        moduleBase = GetModuleBaseAddress(procID, L"ac_client.exe");
        std::cout << "Module Base Address =  " << "0x" << std::hex << std::uppercase << moduleBase << std::endl;

        localPlayerPtr = moduleBase + 0x10F4F4;
        std::cout << "Local Player Ptr Address =  " << "0x" << std::hex << std::uppercase << localPlayerPtr << std::endl;

        healthAddr = FindDMAAddy(hProcess, localPlayerPtr, { 0xF8 });
        std::cout << "Health Base Address =  " << "0x" << std::hex << std::uppercase << healthAddr << std::endl;



    }
    else
    {
        std::cout << "Process not found, press enter to exit\n";
        getchar();
        return 0;
    }

    DWORD dwExit = 0;

    while (GetExitCodeProcess(hProcess, &dwExit) && dwExit == STILL_ACTIVE)
    {
        if (GetAsyncKeyState(VK_NUMPAD1) & 1)
        {
            bHealth = !bHealth;
        }

        if (GetAsyncKeyState(VK_NUMPAD2) & 1)
        {
            bAmmo = !bAmmo;

            if (bAmmo)
            {
                //ff 06 = inc [esi];
                mem::PatchEx((BYTE*)(moduleBase + 0X637E9), (BYTE*)"\xFF\x06", 2, hProcess); //CE shows in dissasembler the address of instructions ex: "ac_client + 637E9:
            }
            else
            {
                //ff 0E = dec [esi]; original bytes will re-patch whe toggled off
                mem::PatchEx((BYTE*)(moduleBase + 0x637E9), (BYTE*)"\xFF\x0E", 2, hProcess);
            }
        }

        if (GetAsyncKeyState(VK_NUMPAD3) & 1) 
        {
            bRecoil = !bRecoil;

            if (bRecoil)
            {
                // 0x63786 is the offset from the base 0x400000 for the instructions on recoil
                mem::NopEx((BYTE*)(moduleBase + 0x63786), 10, hProcess);
            }
            else
            {
                //"\x50\x8D\x4C\x24\x1C\x51\x8B\xCE\xFF\xD2" is the recoil instructions from dissasembler 
                mem::PatchEx((BYTE*)(moduleBase + 0x63786), (BYTE*)"\x50\x8D\x4C\x24\x1C\x51\x8B\xCE\xFF\xD2", 10, hProcess); 
                // "\x" tells the compliler to use the literal bytes we're defining in the char array
            }
        }

        if (GetAsyncKeyState(VK_INSERT) & 1) //exits the hack
        {
            return 0;
        }

        //continous write / freeze
        if (bHealth)
        {
            mem::PatchEx((BYTE*)healthAddr, (BYTE*)&newValue, sizeof(newValue), hProcess);
        }

        Sleep(10);

    }

    std::cout << "Process not found, press enter to exit\n";
    getchar();
    return 0;
}
