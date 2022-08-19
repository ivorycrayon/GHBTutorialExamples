// alt shift ;         = all word
// // alt shift .         = all word
// alt shift arrow     = multi cursor

#include "pch.h"
#include <iostream>
#include <vector>
#include <Windows.h>
#include "mem.h"

//sv_cheats 1; bot_kick; bot_stop 1; mp_autoteambalance 0; mp_limitteams 5; mp_roundtime_defuse 60; mp_freezetime 0; mp_buytime 99999; ff_damage_reduction_bullets 1; mp_afterroundmoney 16000; endround; bot_add ct; bot_add t;

mem memClass;

struct gameOffsets
{
    uintptr_t local_player = 0xdc04cc;
    uintptr_t fJump = 0x52868ec;
    uintptr_t fLeft = 0x320cde4;
    uintptr_t fRight = 0x320cdf0;
    uintptr_t m_iHealth = 0x100;
    uintptr_t flags = 0x104;
    uintptr_t client_state = 0x58cfdc;
    uintptr_t viewX = 0x4d90;
}offsets;

struct values
{
    uintptr_t localPlayer;
    uintptr_t process;
    uintptr_t baseModule;
    uintptr_t engineModule;
    uintptr_t healthAddr;
    float viewX;
    float viewXnew;
    uintptr_t viewXaddr;
    BYTE flag;
    bool bJumping;
}val;

int main()
{
    val.process = memClass.getProcess(L"csgo.exe");
    val.baseModule = memClass.getModule(val.process, L"client.dll");
    std::cout << "Module Base Address =  " << "0x" << std::hex << std::uppercase << val.baseModule << std::endl;

    val.engineModule = memClass.getModule(val.process, L"engine.dll");

    val.localPlayer = memClass.readMem<uintptr_t>(val.baseModule + offsets.local_player);
    std::cout << "Local Player Ptr Address =  " << "0x" << std::hex << std::uppercase << val.localPlayer << std::endl;

    val.healthAddr = val.localPlayer + offsets.m_iHealth;
    std::cout << "Health Base Address =  " << "0x" << std::hex << std::uppercase << val.healthAddr << std::endl;
    

    if (val.localPlayer == NULL)
        while (val.localPlayer == NULL)
            val.localPlayer = memClass.readMem<uintptr_t>(val.baseModule + offsets.local_player);

    int healthValue = 0;
    healthValue = memClass.readMem<int32_t>(val.healthAddr);
    //std::cout << "Current health = " << std::dec << healthValue << std::endl;
    
    while (true)
    {
        val.bJumping = false;
        healthValue = memClass.readMem<int32_t>(val.healthAddr);
        //std::cout << "Current health = " << std::dec << healthValue << std::endl;
        
        val.flag = memClass.readMem<BYTE>(val.localPlayer + offsets.flags);
        val.viewXaddr = memClass.getAddress(val.engineModule + offsets.client_state, {offsets.viewX});
        val.viewX = memClass.readMem<float>(val.viewXaddr);
        std::cout << "viewX = " << val.viewX << std::endl; //left goes up right goes down


        if (GetAsyncKeyState(VK_SPACE) && val.flag & (1 << 0))
        {
            memClass.writeMem<uintptr_t>(val.baseModule + offsets.fJump, 6);
            val.bJumping = true;
        }
        /*else //while in air
        {
            val.viewXaddr = memClass.getAddress(val.engineModule + offsets.client_state, { offsets.viewX });
            val.viewXnew = memClass.readMem<float>(val.viewXaddr);
            if (val.viewXnew > val.viewX && val.bJumping) // left goes up
            {
                memClass.writeMem<uintptr_t>(val.baseModule + offsets.fLeft, 6);
            }
            else if (val.viewXnew < val.viewX && val.bJumping) // right goes down
            {
                memClass.writeMem<uintptr_t>(val.baseModule + offsets.fRight, 6);
            }
        }*/

        Sleep(1);
    }

    std::cout << "Process not found, press enter to exit\n";
    getchar();
    return 0;
}
