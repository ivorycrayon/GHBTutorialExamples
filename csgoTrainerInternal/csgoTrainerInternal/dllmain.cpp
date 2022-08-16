#include "pch.h"
#include <windows.h>
#include <iostream>
#include <TlHelp32.h>
#include "mem.h"
#include "proc.h"
#include "color.hpp"
#include "hook.h"
#include <string>

//uintptr_t casting is to allow compatibility with x64
//need to run debuggger -> inject -> step through
//hit end -> make changes to code -> reinject

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

struct Vector3 { float x, y, z; };

bool bUpdateDisplay;
std::string sHealth = "OFF", sAmmo = "OFF", sRecoil = "OFF", sRateOfFire = "OFF", sIncrementAmmo = "OFF", sFlyHack = "OFF";

class weapon
{
public:
	union
	{
		//              Type     Name    Offset
		DEFINE_MEMBER_N(int32_t*, ammoPtr, 0x14);
		DEFINE_MEMBER_N(int32_t*, recoilPtr, 0x28); 
	};
};

class PlayerEntity
{
public:	
	union
	{
		DWORD vTable;
		//              Type     Name    Offset
		DEFINE_MEMBER_N(Vector3, posHead, 0x4);
		DEFINE_MEMBER_N(Vector3, velocity, 0x28);
		DEFINE_MEMBER_N(Vector3, posBody, 0x34);
		DEFINE_MEMBER_N(Vector3, angle, 0x40);
		DEFINE_MEMBER_N(int32_t, health, 0xF8);
		DEFINE_MEMBER_N(int8_t, noClipMode, 0x82);
		DEFINE_MEMBER_N(int32_t, arAmmo, 0x128);
		DEFINE_MEMBER_N(char, name[16], 0x225);
		DEFINE_MEMBER_N(int8_t, spectateMode, 0x338);
		DEFINE_MEMBER_N(weapon*, currentWeapon, 0x374);		
	};

};

struct EntList
{
	PlayerEntity* ents[31];
};

DWORD jmpBackAddy;
void __declspec(naked) ourFunct()
{
	__asm {
		inc [esi] // orig: dec [esi] FF 0E
		push edi // 57
		mov edi, [esp+0x14] // 8B 7C 24 14
		jmp [jmpBackAddy]
	}
}

bool isValidEnt(PlayerEntity* ent)
{
	if (ent)
	{
		if (ent->vTable == 0x4E4A98 || ent->vTable == 0x4E4AC0) //AC0 - BOTS vTable 
		{
			return true;
		}
	}
	return false;
}

DWORD WINAPI HackThread(HMODULE hModule)
{
	//Create console
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);

	SetConsoleTitle(L"~Swag Central~");
	system("mode 53, 20");
	int timeSinceLastUpdate = clock();

	//Get Module Base
	uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"ac_client.exe");
	//std::cout << "Module Base Address: " << std::hex << std:: uppercase << moduleBase << std::endl;

	bool bHealth = false, bAmmo = false, bRecoil = false, bRateOfFire = false, bIncrementAmmo = false, bFlyHack = false;

	//Hack Loop
	while (true)
	{
		//PlayerEntity* localPlayer = (PlayerEntity*)*(uintptr_t*)(moduleBase + 0x10F4F4); //these lines are the same thing
		PlayerEntity* localPlayer = *(PlayerEntity**)(moduleBase + 0x10F4F4); //these lines are the same thing

		int* numOfPlayers = (int*)(moduleBase + 0x10F500);
		EntList* entList = *(EntList**)(moduleBase + 0x10F4F8);
		char newName[16] = "LOL";

		for (int i = 0; i < *numOfPlayers; i++)
		{
			if (entList && isValidEnt(entList->ents[i]))
			{
				entList->ents[i]->health = 1;
				for (int j = 0; j < sizeof(newName); j++)
				{
					entList->ents[i]->name[j] = newName[j];
				}
			}
		}

		if (bUpdateDisplay || clock() - timeSinceLastUpdate > 3000) //update every 3000ms or on keypress
		{
			system("cls");
			std::cout << "----------------------------------------------------" << std::endl;
			std::cout << dye::purple("                  ~SwagCentral~                     ") << std::endl;
			std::cout << dye::aqua("                Pasted by Ivory ;)                  ") << std::endl;
			std::cout << "----------------------------------------------------" << std::endl << std::endl;
			std::cout << "Your name: " << localPlayer->name << "                         " << std::endl << std::endl;
			if (*numOfPlayers > 1) {
				std::cout << "# Players: " << *numOfPlayers << "                         " << std::endl << std::endl;
			}
			std::cout << "[NUMPAD1] Health Lock                   -> ";
			if (bHealth) { std::cout << dye::green(sHealth); } else { std::cout << dye::red(sHealth); } 
			std::cout << " <-" << std::endl << std::endl;
			std::cout << "[NUMPAD2] Ammo Lock                     -> ";
			if (bAmmo) { std::cout << dye::green(sAmmo); } else { std::cout << dye::red(sAmmo); } 
			std::cout << " <-" << std::endl << std::endl;
			std::cout << "[NUMPAD3] No Reocil                     -> ";
			if (bRecoil) { std::cout << dye::green(sRecoil); } else { std::cout << dye::red(sRecoil); } 
			std::cout << " <-" << std::endl << std::endl;
			std::cout << "[NUMPAD4] Max Rate Of Fire              -> ";
			if (bRateOfFire) { std::cout << dye::green(sRateOfFire); } else { std::cout << dye::red(sRateOfFire); }
			std::cout << " <-" << std::endl << std::endl;
			std::cout << "[NUMPAD5] Increment Ammo                -> ";
			if (bIncrementAmmo) { std::cout << dye::green(sIncrementAmmo); }
			else { std::cout << dye::red(sIncrementAmmo); }
			std::cout << " <-" << std::endl << std::endl;
			std::cout << "[NUMPAD6] Fly Hack (+Invis)             -> ";
			if (bFlyHack) { std::cout << dye::green(sFlyHack); }
			else { std::cout << dye::red(sFlyHack); }
			std::cout << " <-" << std::endl << std::endl;
			std::cout << dye::red("[END] Exit") << std::endl << std::endl;
			std::cout << "---------------------------------------------------";

			bUpdateDisplay = false;
			timeSinceLastUpdate = clock();
		}

		//Get key input
		if (GetAsyncKeyState(VK_END) & 1)
		{
			break;
		}
		if (GetAsyncKeyState(VK_NUMPAD1) & 1)
		{
			bUpdateDisplay = true;
			bHealth = !bHealth;

			if (bHealth)
			{
				//mem::Nop((BYTE*)(moduleBase + 0x29D1F), 3); //hp code
				sHealth = "ON ";
			}
			else
			{
				//mem::Patch((BYTE*)(moduleBase + 0x29D1F), (BYTE*)"\x29\x7B\x04", 3); // sub [ebx+04], edi
				sHealth = "OFF";
			}
			
		}
		if (GetAsyncKeyState(VK_NUMPAD2) & 1)
		{
			bUpdateDisplay = true;
			bAmmo = !bAmmo;

			if (bAmmo)
			{
				if (bIncrementAmmo)
				{
					mem::Patch((BYTE*)(moduleBase + 0x637E9), (BYTE*)"\xFF\x0E", 2); //dec [esi] - ammo dec
					mem::Patch((BYTE*)(moduleBase + 0x637EB), (BYTE*)"\x57", 1); //dec [esi] - ammo dec
					mem::Patch((BYTE*)(moduleBase + 0x637EC), (BYTE*)"\x8B\x7C\x24\x14", 4); //dec [esi] - ammo dec
					sIncrementAmmo = "OFF";
					bIncrementAmmo = false;
				}

				mem::Nop((BYTE*)(moduleBase + 0x637E9), 2); //dec [esi] - ammo dec
				sAmmo = "ON ";
			}
			else
			{
				mem::Patch((BYTE*)(moduleBase + 0x637E9), (BYTE*)"\xFF\x0E", 2); //dec [esi] - ammo dec
				sAmmo = "OFF";
			}
		}
		if (GetAsyncKeyState(VK_NUMPAD3) & 1)
		{
			bUpdateDisplay = true;
			bRecoil = !bRecoil;
			if (bRecoil)
			{
				mem::Nop((BYTE*)(moduleBase + 0x63786), 10);
				sRecoil = "ON ";
			}
			else
			{
				//write original instructions again
				mem::Patch((BYTE*)(moduleBase + 0x63786), (BYTE*)"\x50\x8D\x4C\x24\x1C\x51\x8B\xCE\xFF\xD2", 10);
				sRecoil = "OFF";
			}
		}
		if (GetAsyncKeyState(VK_NUMPAD4) & 1)
		{
			bUpdateDisplay = true;
			bRateOfFire = !bRateOfFire;

			if (bRateOfFire)
			{
				mem::Nop((BYTE*)(moduleBase + 0x637E4), 2); //gun ROF
				//mem::Nop((BYTE*)(moduleBase + 0x642F4), 2); //nade ROF
				mem::Nop((BYTE*)(moduleBase + 0x64514), 2); //knife ROF
				sRateOfFire = "ON ";
			}
			else
			{
				//write original instructions again
				mem::Patch((BYTE*)(moduleBase + 0x637E4), (BYTE*)"\x89\x0A", 2); //gun ROF
				//mem::Patch((BYTE*)(moduleBase + 0x642F4), (BYTE*)"\x8B\32", 2); //nade ROF
				mem::Patch((BYTE*)(moduleBase + 0x64514), (BYTE*)"\x89\x0A", 2); //knife ROF
				sRateOfFire = "OFF";
			}
		}
		if (GetAsyncKeyState(VK_NUMPAD5) & 1)
		{
			bUpdateDisplay = true;
			bIncrementAmmo = !bIncrementAmmo;

			if (bIncrementAmmo)
			{
				if (bAmmo)
				{
					mem::Patch((BYTE*)(moduleBase + 0x637E9), (BYTE*)"\xFF\x0E", 2); //dec [esi] - ammo dec
					bAmmo = false;
					sAmmo = "OFF";
				}

				int hookLength = 7;
				DWORD hookAddress = moduleBase + 0x637E9;
				jmpBackAddy = hookAddress + hookLength;
				Hook((void*)hookAddress, ourFunct, hookLength);
				sIncrementAmmo = "ON ";
			}
			else
			{
				mem::Patch((BYTE*)(moduleBase + 0x637E9), (BYTE*)"\xFF\x0E", 2); //dec [esi] - ammo dec
				mem::Patch((BYTE*)(moduleBase + 0x637EB), (BYTE*)"\x57", 1); //dec [esi] - ammo dec
				mem::Patch((BYTE*)(moduleBase + 0x637EC), (BYTE*)"\x8B\x7C\x24\x14", 4); //dec [esi] - ammo dec
				sIncrementAmmo = "OFF";
			}
		}
		if (GetAsyncKeyState(VK_NUMPAD6) & 1)
		{
			bUpdateDisplay = true;
			bFlyHack = !bFlyHack;



			if (bFlyHack)
			{
				localPlayer->noClipMode = 11;
				localPlayer->spectateMode = 5;
				sFlyHack = "ON ";
			}
			else
			{
				localPlayer->noClipMode = 0;
				localPlayer->spectateMode = 0;
				sFlyHack = "OFF";
			}
		}

		//Continous write/freeze

		if (localPlayer) //only executes when game is in session - would be nullptr in menus. will not work in all games
		{
			if (bHealth)
			{
				// *(int*)(*localPlayer + 0xF8) = 1337; // dereference player ptr, add f8, cast to int, then dereference 
				localPlayer->health = 1337;
			}

			if (bAmmo)
			{
				//uintptr_t ammoAddr = mem::FindDMAAddy(moduleBase + 0x10F4F4, { 0x374, 0x14, 0x0 });
				//int* ammo = (int*)ammoAddr;
				//*ammo = 1337;

				//or just
				//*(int*)mem::FindDMAAddy(moduleBase + 0x10F4F4, { 0x374, 0x14, 0x0 }) = 1337;

				*localPlayer->currentWeapon->ammoPtr = 69; 
				localPlayer->arAmmo = 420;
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