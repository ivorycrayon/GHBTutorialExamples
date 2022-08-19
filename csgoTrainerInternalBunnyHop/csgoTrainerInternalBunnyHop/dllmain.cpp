#include "pch.h"
#include <windows.h>
#include <iostream>
#include <TlHelp32.h>
#include "mem.h"
#include "proc.h"
#include "color.hpp"
#include "hook.h"
#include "csgo_ent.h"
#include <string>

//sv_cheats 1; sv_lan 1; bot_kick; bot_stop 1; mp_autoteambalance 0; mp_limitteams 5; mp_roundtime_defuse 60; mp_freezetime 0; mp_buytime 99999; ff_damage_reduction_bullets 1; mp_afterroundmoney 16000; endround; bot_add ct; bot_add t; bot_add ct; bot_add t;

//uintptr_t casting is to allow compatibility with x64
//need to run debuggger -> inject -> step through
//hit end -> make changes to code -> reinject

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
	uintptr_t entity_list = 0x4ddc90c;
}offsets;

bool isPlayerMoving(entity* localPlayer)
{
	Vector3 playerVel = localPlayer->velocity;
	int vel = playerVel.x + playerVel.y + playerVel.z;

	if (vel != 0)
		return true;
	else
		return false;
}

DWORD WINAPI HackThread(HMODULE hModule)
{
	//Create console
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	
	//Setup Console
	SetConsoleTitle(L"~Swag Central~");
	system("mode 53, 20");
	int timeSinceLastUpdate = clock();

	//Get addresses
	uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"client.dll");
	uintptr_t engineBase = (uintptr_t)GetModuleHandle(L"engine.dll");
	CBaseEntityList* entList = (CBaseEntityList*)(moduleBase + offsets.entity_list - 0x10);

	entity* localPlayer = *(entity**)(moduleBase + offsets.local_player);
	engine* localEngine = *(engine**)(engineBase + offsets.client_state);
	uintptr_t fJump = *(uintptr_t*)(moduleBase + offsets.fJump);

	if(localPlayer == NULL)
		while (localPlayer == NULL)
			localPlayer = *(entity**)(moduleBase + offsets.local_player);

	//Hack Loop
	while (true)
	{
		localPlayer = *(entity**)(moduleBase + offsets.local_player);
		localEngine = *(engine**)(engineBase + offsets.client_state);
		float oldViewX = localEngine->viewX;

		std::cout << "view x: " << localEngine->viewX << std::endl;
		// left is positive right is negative
		// at 180 it switches to -180


		//Get key input

		if (GetAsyncKeyState(VK_END) & 1)
		{
			break;
		} 
		if (isPlayerMoving(localPlayer))
		{
			if (GetAsyncKeyState(VK_SPACE) && localPlayer->flags & (1 << 0))
			{
				*(uintptr_t*)(moduleBase + offsets.fJump) = 6;
			}
			//check if in air
		}

		
		/*
		int i = 0;
		for (auto e : entList->entList)
		{
			if (i == 0 && e.entPtr)
			{
				std::cout << "Your health is " << e.entPtr->health << std::endl;
			}
			else if (e.entPtr)
			{
				if (e.entPtr->bDormant == false)
				{
					std::cout << "Entity #" << i << " health is " << e.entPtr->health << std::endl;
				}
				else
				{
					std::cout << "Entity #" << i << " is dormant." << std::endl;
				}
			}
			i++;
		} 
		*/

		//std::cout << "=======================" << std::endl;
		 
		Sleep(20);

	}

	//Cleanup&eject
	fclose(f);
	FreeConsole();
	FreeLibraryAndExitThread(hModule, 0);
	return 0;	
}

BOOL APIENTRY DllMain(HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH://just create thread and let it return. do not add any code here
	{
		HANDLE hThread = nullptr;
		hThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr);
		if (hThread)
		{
			CloseHandle(hThread);
		}
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}