#include "pch.h"
#include <windows.h>
#include <iostream>
#include <TlHelp32.h>
#include "mem.h"
#include "proc.h"

//uintptr_t casting is to allow compatibility with x64
//need to run debuggger -> inject -> step through
//hit end -> make changes to code -> reinject

DWORD WINAPI HackThread(HMODULE hModule)
{
	//Create console
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);


	std::cout << "~Swag Central Loaded~\n";


	//Get Module Base
	uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"ac_client.exe");

	bool bHealth = false, bAmmo = false, bRecoil = false;

	//Hack Loop
	while (true)
	{

		//Get key input
		if (GetAsyncKeyState(VK_END) & 1)
		{
			break;
		}
		if (GetAsyncKeyState(VK_NUMPAD1) & 1)
		{
			bHealth = !bHealth;
		}
		if (GetAsyncKeyState(VK_NUMPAD2) & 1)
		{
			bAmmo = !bAmmo;
		}
		if (GetAsyncKeyState(VK_NUMPAD3) & 1)
		{
			bRecoil = !bRecoil;

			if (bRecoil)
			{
				mem::Nop((BYTE*)(moduleBase + 0x63786), 10);
			}
			else
			{
				//write original instructions again
				mem::Patch((BYTE*)(moduleBase + 0x63786), (BYTE*)"\x50\x8D\x4C\x24\x1C\x51\x8B\xCE\xFF\xD2", 10);
			}
		}


		//Continous write/freeze
		uintptr_t* localPlayerPtr = (uintptr_t*)(moduleBase + 0x10F4F4);

		if (localPlayerPtr) //only executes when game is in session - would be nullptr in menus. will not work in all games
		{
			if (bHealth)
			{
				*(int*)(*localPlayerPtr + 0xF8) = 1337; // dereference player ptr, add f8, cast to int, then dereference 
			}

			if (bAmmo)
			{
				uintptr_t ammoAddr = mem::FindDMAAddy(moduleBase + 0x10F4F4, { 0x374, 0x14, 0x0 });
				int* ammo = (int*)ammoAddr;
				*ammo = 1337;

				//or just
				*(int*)mem::FindDMAAddy(moduleBase + 0x10F4F4, { 0x374, 0x14, 0x0 }) = 1337;
			}
		}

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
		CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr));
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}