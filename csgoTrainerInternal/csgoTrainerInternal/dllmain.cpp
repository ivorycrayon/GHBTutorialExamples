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

//sv_cheats 1; bot_kick; bot_stop 1; mp_autoteambalance 0; mp_limitteams 5; mp_roundtime_defuse 60; mp_freezetime 0; mp_buytime 99999; ff_damage_reduction_bullets 1; endround; bot_add ct; bot_add t;

//uintptr_t casting is to allow compatibility with x64
//need to run debuggger -> inject -> step through
//hit end -> make changes to code -> reinject

struct Vector3 { float x, y, z; };

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
	CBaseEntityList* entList = (CBaseEntityList*)(moduleBase + 0x4DDC8FC);

	//Hack Loop
	while (true)
	{

		//Get key input
		if (GetAsyncKeyState(VK_END) & 1)
		{
			break;
		}

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

		std::cout << "=======================" << std::endl;
		 
		Sleep(5);

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