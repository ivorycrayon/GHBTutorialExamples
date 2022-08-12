#include "pch.h"
#include <windows.h>
#include <iostream>
#include <TlHelp32.h>
#include "mem.h"
#include "proc.h"
#include "color.hpp"

//uintptr_t casting is to allow compatibility with x64
//need to run debuggger -> inject -> step through
//hit end -> make changes to code -> reinject

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

struct Vector3 { float x, y, z; };

bool bUpdateDisplay;
std::string sHealth = "OFF", sAmmo = "OFF", sRecoil = "OFF", sRateOfFire = "OFF";

WORD BLACK = 30;
WORD RED = 31;
WORD GREEN = 32;

void setColor(WORD color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
	return;
}

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
		//              Type     Name    Offset
		DEFINE_MEMBER_N(Vector3, posHead, 0x4);
		DEFINE_MEMBER_N(Vector3, velocity, 0x28);
		DEFINE_MEMBER_N(Vector3, posBody, 0x34);
		DEFINE_MEMBER_N(Vector3, angle, 0x40);
		DEFINE_MEMBER_N(int32_t, health, 0xF8);
		DEFINE_MEMBER_N(weapon*, currentWeapon, 0x374);
	};

};





DWORD WINAPI HackThread(HMODULE hModule)
{
	//Create console
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);

	SetConsoleTitle(L"~Swag Central~");
	system("mode 53, 17");
	int timeSinceLastUpdate = clock();

	//std::cout << "~Swag Central Loaded~\n";

	//Get Module Base
	uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"ac_client.exe");

	//std::cout << "Module Base Address: " << std::hex << std:: uppercase << moduleBase << std::endl;

	bool bHealth = false, bAmmo = false, bRecoil = false, bRateOfFire = false;

	//Hack Loop
	while (true)
	{
		if (bUpdateDisplay || clock() - timeSinceLastUpdate > 3000) //update every 3000ms or on keypress
		{
			system("cls");
			std::cout << "----------------------------------------------------" << std::endl;
			std::cout << dye::purple("                ~SwagCentral~                       ") << std::endl;
			std::cout << "              Pasted by Ivory ;)                    " << std::endl;
			std::cout << "----------------------------------------------------" << std::endl << std::endl;
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
			std::cout << dye::red("[END] Exit") << std::endl << std::endl;
			std::cout << "---------------------------------------------------" << std::endl;

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
				sHealth = "ON ";
			}
			else
			{
				sHealth = "OFF";
			}
			
		}
		if (GetAsyncKeyState(VK_NUMPAD2) & 1)
		{
			bUpdateDisplay = true;
			bAmmo = !bAmmo;

			if (bAmmo)
			{
				sAmmo = "ON ";
			}
			else
			{
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

		//Continous write/freeze
		//PlayerEntity* localPlayer = (PlayerEntity*)*(uintptr_t*)(moduleBase + 0x10F4F4); //these lines are the same thing
		PlayerEntity* localPlayer = *(PlayerEntity**)(moduleBase + 0x10F4F4);

		if (localPlayer) //only executes when game is in session - would be nullptr in menus. will not work in all games
		{
			if (bHealth)
			{
				// *(int*)(*localPlayer + 0xF8) = 1337; // dereference player ptr, add f8, cast to int, then dereference 
				localPlayer->health = 1337;
				localPlayer->velocity = { 5.0, 5.0, 0.0 };
			}

			if (bAmmo)
			{
				//uintptr_t ammoAddr = mem::FindDMAAddy(moduleBase + 0x10F4F4, { 0x374, 0x14, 0x0 });
				//int* ammo = (int*)ammoAddr;
				//*ammo = 1337;

				//or just
				//*(int*)mem::FindDMAAddy(moduleBase + 0x10F4F4, { 0x374, 0x14, 0x0 }) = 1337;

				*localPlayer->currentWeapon->ammoPtr = 1337;
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