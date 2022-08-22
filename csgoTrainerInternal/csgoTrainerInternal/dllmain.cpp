#include "pch.h"
#include <windows.h>
#include <iostream>
#include <TlHelp32.h>
#include "mem.h"
#include "proc.h"
#include "color.hpp"
#include "csgo.hpp"
#include "hook.h"
#include "csgo_ent.h"
#include <string>

//bots//sv_cheats 1; bot_kick; bot_stop 1; mp_autoteambalance 0; mp_limitteams 10; mp_roundtime_defuse 60; mp_freezetime 0; mp_buytime 99999; ff_damage_reduction_bullets 1; mp_afterroundmoney 10000; endround; bot_add ct; bot_add t;
//bhop//sv_cheats 1;sv_enablebunnyhopping 1;sv_maxvelocity 7000;sv_staminamax 0;sv_staminalandcost 0;sv_staminajumpcost 0;sv_accelerate_use_weapon_speed 0;sv_staminarecoveryrate 0;sv_autobunnyhopping 0;sv_airaccelerate 2000;

#define FL_ONGROUND (1<<0)
#define FL_IN_JUMP (1<<8)

using namespace hazedumper;

struct gameOffsets
{
	uintptr_t dwLocalPlayer = signatures::dwLocalPlayer;
	uintptr_t dwForceJump = signatures::dwForceJump;
	uintptr_t dwForceLeft = signatures::dwForceLeft;
	uintptr_t dwForceRight = signatures::dwForceRight;
	uintptr_t dwForceAttack = signatures::dwForceAttack;
	uintptr_t dwClientState = signatures::dwClientState;
	uintptr_t dwClientState_ViewAngles = signatures::dwClientState_ViewAngles;
	uintptr_t base_entity_list = signatures::dwEntityList - 0x10;
	uintptr_t entity_list = signatures::dwEntityList;
	uintptr_t m_iItemDefinitionIndex = netvars::m_iItemDefinitionIndex;
	uintptr_t dwGlowObjectManager = signatures::dwGlowObjectManager;
	uintptr_t m_clrRender = netvars::m_clrRender;
	uintptr_t model_ambient_min = signatures::model_ambient_min;

}offsets;

struct globalVariables
{
	uintptr_t gameModule;
	uintptr_t engineModule;
	uintptr_t glowObjectAddress;
	engine* localEngine;
	entity* localPlayer;
	int32_t* forceAttack;
	int32_t* forceJump;
	int32_t* forceLeft;
	int32_t* forceRight;
	CBaseEntityList* entList;
	int16_t iWeaponID;
	float fDelay;
	bool bUpdateDisplay; 
	ClrRender clrEnemy;
	ClrRender clrTeam;
	ClrRender clrRender;
	int32_t origBrightness;
	int32_t timeSinceLastShot;
	
}global;

struct GlowStruct
{
	BYTE base[8]; // 8 byte buffer
	float red;
	float green;
	float blue;
	float alpha;
	BYTE pad0[8];
	BYTE pad1[8];
	bool renderWhenOccluded;
	bool renderWhenUnoccluded;
	bool fullBloom;
	BYTE pad2;
	BYTE pad3[4];
	int32_t glowStyle;
};

enum weaponID : int16_t
{
	WEAPON_DEAGLE = 1,
	WEAPON_ELITE = 2,
	WEAPON_FIVESEVEN = 3,
	WEAPON_GLOCK = 4,
	WEAPON_AK47 = 7,
	WEAPON_AUG = 8,
	WEAPON_AWP = 9,
	WEAPON_FAMAS = 10,
	WEAPON_G3SG1 = 11,
	WEAPON_GALILAR = 13,
	WEAPON_M249 = 14,
	WEAPON_M4A1 = 16,
	WEAPON_MAC10 = 17,
	WEAPON_P90 = 19,
	WEAPON_UMP45 = 24,
	WEAPON_XM1014 = 25,
	WEAPON_BIZON = 26,
	WEAPON_MAG7 = 27,
	WEAPON_NEGEV = 28,
	WEAPON_SAWEDOFF = 29,
	WEAPON_TEC9 = 30,
	WEAPON_TASER = 31,
	WEAPON_HKP2000 = 32,
	WEAPON_MP7 = 33,
	WEAPON_MP9 = 34,
	WEAPON_NOVA = 35,
	WEAPON_P250 = 36,
	WEAPON_SCAR20 = 38,
	WEAPON_SG556 = 39,
	WEAPON_SSG08 = 40,
	WEAPON_KNIFE = 42,
	WEAPON_FLASHBANG = 43,
	WEAPON_HEGRENADE = 44,
	WEAPON_SMOKEGRENADE = 45,
	WEAPON_MOLOTOV = 46,
	WEAPON_DECOY = 47,
	WEAPON_INCGRENADE = 48,
	WEAPON_C4 = 49,
	WEAPON_KNIFE_T = 59,
	WEAPON_M4A1_SILENCER = 60,
	WEAPON_USP_SILENCER = 61,
	WEAPON_CZ75A = 63,
	WEAPON_KNIFE_BAYONET = 500,
	WEAPON_KNIFE_FLIP = 505,
	WEAPON_KNIFE_GUT = 506,
	WEAPON_KNIFE_KARAMBIT = 507,
	WEAPON_KNIFE_M9_BAYONET = 508,
	WEAPON_KNIFE_TACTICAL = 509,
	WEAPON_KNIFE_FALCHION = 512,
	WEAPON_KNIFE_BUTTERFLY = 515,
	WEAPON_KNIFE_PUSH = 516
};

void setBrightness(float brightness)
{
	float fBrightness = brightness;
	int32_t ptr = *(int32_t*)(global.engineModule + offsets.model_ambient_min);
	int32_t xorptr = *(int32_t*)&fBrightness ^ ptr;

	if (brightness == 0.0)
		*(int32_t*)(global.engineModule + offsets.model_ambient_min) = global.origBrightness;
	else
		*(int32_t*)(global.engineModule + offsets.model_ambient_min) = xorptr;
}

GlowStruct setGlowColor(GlowStruct EnemyGlow, entity* entity)
{
	bool defusing = entity->m_bIsDefusing;
	if (defusing)
	{
		EnemyGlow.red = 1.0f;
		EnemyGlow.green = 1.0f;
		EnemyGlow.blue = 1.0f;
	}
	else
	{
		EnemyGlow.red = entity->m_iHealth * -0.01 + 1;
		EnemyGlow.green = entity->m_iHealth * 0.01;
	}
	EnemyGlow.alpha = 1.0f;
	EnemyGlow.renderWhenOccluded = true;
	EnemyGlow.renderWhenUnoccluded = false;
	return EnemyGlow;
}

void setTeamGlow(entity* entity)
{
	GlowStruct TeamGlow = *(GlowStruct*)(global.glowObjectAddress + (entity->m_iGlowIndex * 0x38));
	TeamGlow.blue = 1.0f;
	TeamGlow.alpha = 1.0f;
	TeamGlow.renderWhenOccluded = true;
	TeamGlow.renderWhenUnoccluded = false;

	*(GlowStruct*)(global.glowObjectAddress + (entity->m_iGlowIndex * 0x38)) = TeamGlow;
}

void setEnemyGlow(entity* entity)
{
	GlowStruct EnemyGlow;
	EnemyGlow = *(GlowStruct*)(global.glowObjectAddress + (entity->m_iGlowIndex * 0x38));
	EnemyGlow = setGlowColor(EnemyGlow, entity);

	*(GlowStruct*)(global.glowObjectAddress + (entity->m_iGlowIndex * 0x38)) = EnemyGlow;
}

void handleGlow()
{
	CBaseEntityList* entList = (CBaseEntityList*)(global.gameModule + offsets.base_entity_list);
	int i = 0;

	for (auto e : entList->entList)
	{
		if (e.entPtr && i != 0)
		{
			int32_t enemyTeam = 0;
			int32_t COUNTER_TERRORIST = 3;
			int32_t TERRORIST = 2;

			if (global.localPlayer->m_iTeamNum == COUNTER_TERRORIST)
				enemyTeam = TERRORIST;
			else if (global.localPlayer->m_iTeamNum == TERRORIST)
				enemyTeam = COUNTER_TERRORIST;

			if (e.entPtr->m_iTeamNum == global.localPlayer->m_iTeamNum)
			{
				e.entPtr->m_clrRender = global.clrTeam;
				setTeamGlow(e.entPtr);
			}
			else if (e.entPtr->m_iTeamNum == enemyTeam)
			{
				e.entPtr->m_clrRender = global.clrEnemy;
				setEnemyGlow(e.entPtr);
			}
		}
		i++;
	}
}

void handleUnglow()
{
	CBaseEntityList* entList = (CBaseEntityList*)(global.gameModule + offsets.base_entity_list);
	int i = 0;

	for (auto e : entList->entList)
	{
		if (e.entPtr && i != 0)
		{
			e.entPtr->m_clrRender = global.clrRender;
		}
		i++;
	}
	setBrightness(0.0f);
}

void setTrigBotDelay(float distance)
{
	float delay;
	switch (global.iWeaponID)
	{
	case WEAPON_M4A1_SILENCER: delay = 8; break;
	case WEAPON_M4A1: delay = 10; break;
	case WEAPON_USP_SILENCER: delay = 15; break;
	case WEAPON_GLOCK: delay = 6.5; break;
	case WEAPON_DEAGLE: delay = 33; break;
	case WEAPON_AK47: delay = 15; break;
	case WEAPON_AUG: delay = 2.0; break;
	case WEAPON_AWP: delay = .3; break;
	case WEAPON_SSG08: delay = .3; break;
	case WEAPON_UMP45: delay = 3.3; break;
	case WEAPON_P90: delay = 3.3; break;

	default: delay = 0;
	}
	global.fDelay = distance * delay;
	//std::cout << "weapon delay is " << global.fDelay << " ms" << std::endl;
	
}

void getWeaponID()
{
	uint32_t currentWeapon = global.localPlayer->m_hActiveWeapon;
	uint32_t currentWeaponEntIndex = (currentWeapon & 0xFFF) - 1;
	uintptr_t currentWeaponEntity = *(uintptr_t*)(global.gameModule + offsets.entity_list + currentWeaponEntIndex * 0x10);

	if (currentWeaponEntity != NULL)
	{
		global.iWeaponID = *(int16_t*)(currentWeaponEntity + offsets.m_iItemDefinitionIndex);
		//std::cout << global.iWeaponID << std::endl;
	}
}

bool isPlayerMoving()
{
	Vector3 playerVel = global.localPlayer->m_vecVelocity;
	int vel = playerVel.x + playerVel.y + playerVel.z;

	if (vel != 0)
		return true;
	else
		return false;
}

float getDistance(entity* target)
{
	float distance = sqrt(
		pow(global.localPlayer->m_vecOrigin.x - target->m_vecOrigin.x, 2) +
		pow(global.localPlayer->m_vecOrigin.y - target->m_vecOrigin.y, 2) +
		pow(global.localPlayer->m_vecOrigin.z - target->m_vecOrigin.z, 2)) * 0.0254; //convert to meters 
	//std::cout << distance << std::endl;

	return distance;
	
}

void shoot()
{
	if (clock() - global.timeSinceLastShot > global.fDelay)
	{
		*global.forceAttack = 5;
		Sleep(5);
		*global.forceAttack = 4;
		global.timeSinceLastShot = clock();
	}
}//could add random delay

bool checkTrigBot()
{
	int entityID = global.localPlayer->m_iCrosshairID;
	if (entityID != 0 && entityID < 64)
	{
		entity* target = *(entity**)(global.gameModule + offsets.entity_list + (entityID - 1) * 0x10);
		if (target->m_iTeamNum != global.localPlayer->m_iTeamNum && target->m_iHealth > 0)
		{
			float distance = getDistance(target);
			getWeaponID();
			setTrigBotDelay(distance);

			if (global.iWeaponID == WEAPON_AWP | global.iWeaponID == WEAPON_SSG08)
			{
				if (!global.localPlayer->m_bIsScoped)
					return false;
			}
			return true;
		}			
		else
			return false;
	}
	else
		return false;
}

void handleTrigBot()
{
	if (checkTrigBot())
	{
		shoot();
	}
}

DWORD WINAPI HackThread(HMODULE hModule)
{
	//Create console
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	
	//Setup Console
	SetConsoleTitle(L"~Swag Central~");
	system("mode 53, 26");
	int32_t timeSinceLastUpdate = clock();

	//Get addresses
	global.gameModule = (uintptr_t)GetModuleHandle(L"client.dll");
	global.engineModule = (uintptr_t)GetModuleHandle(L"engine.dll");
	global.glowObjectAddress = *(uintptr_t*)(global.gameModule + offsets.dwGlowObjectManager);
	
	//set objects
	global.localPlayer = *(entity**)(global.gameModule + offsets.dwLocalPlayer);
	global.localEngine = *(engine**)(global.engineModule + offsets.dwClientState);
	//Vector3* viewAngles = (Vector3*)(*(uintptr_t*)(global.engineModule + offsets.dwClientState) + offsets.dwClientState_ViewAngles);

	//set pointers
	global.forceJump = (int32_t*)(global.gameModule + offsets.dwForceJump);
	global.forceAttack = (int32_t*)(global.gameModule + offsets.dwForceAttack);
	global.forceLeft = (int32_t*)(global.gameModule + offsets.dwForceLeft);
	global.forceRight = (int32_t*)(global.gameModule + offsets.dwForceRight);
	global.entList = (CBaseEntityList*)(global.gameModule + offsets.base_entity_list);

	//toggle bools
	bool bTrigBot = false;
	bool bBHop = false;
	bool bRadar = false;
	bool bGlow = false;
	bool bFlash = false;
	bool bRecoil = false;

	//console strings
	std::string sTrigBot = "OFF", sBHop = "OFF", sRadar = "OFF", sGlow = "OFF", sFlash = "OFF", sRecoil = "OFF";

	//check local player
	if(global.localPlayer == NULL)
		while(global.localPlayer == NULL)
			global.localPlayer = *(entity**)(global.gameModule + offsets.dwLocalPlayer);

	//odds and ends
	Vector3 oldView = global.localEngine->viewAngle;
	oldView.Normalize();
	float oldViewX = oldView.y;

	global.clrEnemy.red = 255;
	global.clrEnemy.green = 0;
	global.clrEnemy.blue = 0;

	global.clrTeam.red = 0;
	global.clrTeam.green = 0;
	global.clrTeam.blue = 255;

	global.clrRender.red = -1;
	global.clrRender.green = -1;
	global.clrRender.blue = -1;

	global.origBrightness = *(int32_t*)(global.engineModule + offsets.model_ambient_min);

	Vector3 oldPunch = { 0,0,0 };

	//Hack Loop
	while (true)  
	{
		//setup globals
		global.localPlayer = *(entity**)(global.gameModule + offsets.dwLocalPlayer);
		if (global.localPlayer == NULL)
		{
			system("cls");
			std::cout << "Waiting to load local player into match" << std::endl;
			while (global.localPlayer == NULL)
				global.localPlayer = *(entity**)(global.gameModule + offsets.dwLocalPlayer);
		}
		global.localEngine = *(engine**)(global.engineModule + offsets.dwClientState);
		global.glowObjectAddress = *(uintptr_t*)(global.gameModule + offsets.dwGlowObjectManager);
		

		//Console
		if (global.bUpdateDisplay || clock() - timeSinceLastUpdate > 1000) //update every 3000ms or on keypress
		{
			system("cls");
			std::cout << "----------------------------------------------------" << std::endl;
			std::cout << dye::purple("                  ~SwagCentral~                     ") << std::endl;
			std::cout << dye::aqua("                Pasted by Ivory ;)                  ") << std::endl;
			std::cout << "----------------------------------------------------" << std::endl << std::endl;
			std::cout << "Your health: " << global.localPlayer->m_iHealth << "                       " << std::endl << std::endl;
			if(global.localPlayer->m_iTeamNum == 3)
				std::cout << "Your team: " << dye::blue("Counter - Terrorist         ") << std::endl << std::endl;
			else if(global.localPlayer->m_iTeamNum == 2)
				std::cout << "Your team: " << dye::red("Terrorist                ") << std::endl << std::endl;
			else
				std::cout << "Your team: NULL                     " << std::endl << std::endl;
			std::cout << "Your weapon: " << global.iWeaponID << "                       " << std::endl << std::endl;
			std::cout << "[NUMPAD1] Trigger Bot                   -> ";
			if (bTrigBot) { std::cout << dye::green(sTrigBot); }
			else { std::cout << dye::red(sTrigBot); }
			std::cout << " <-" << std::endl << std::endl;
			std::cout << "[NUMPAD2] Bunny Hop                     -> ";
			if (bBHop) { std::cout << dye::green(sBHop); }
			else { std::cout << dye::red(sBHop); }
			std::cout << " <-" << std::endl << std::endl;
			std::cout << "[NUMPAD3] Radar                         -> ";
			if (bRadar) { std::cout << dye::green(sRadar); }
			else { std::cout << dye::red(sRadar); }
			std::cout << " <-" << std::endl << std::endl;
			std::cout << "[NUMPAD4] Glow                          -> ";
			if (bGlow) { std::cout << dye::green(sGlow); }
			else { std::cout << dye::red(sGlow); }
			std::cout << " <-" << std::endl << std::endl;
			std::cout << "[NUMPAD5] Anti-Flash                    -> ";
			if (bFlash) { std::cout << dye::green(sFlash); }
			else { std::cout << dye::red(sFlash); }
			std::cout << " <-" << std::endl << std::endl;
			std::cout << "[NUMPAD6] Recoil Control                -> ";
			if (bRecoil) { std::cout << dye::green(sRecoil); }
			else { std::cout << dye::red(sRecoil); }
			std::cout << " <-" << std::endl << std::endl;
			std::cout << dye::red("[END] Exit") << std::endl << std::endl;
			std::cout << "---------------------------------------------------";

			global.bUpdateDisplay = false;
			timeSinceLastUpdate = clock();
		}
		
		getWeaponID();

		//Get key input
		if (GetAsyncKeyState(VK_END) & 1)
		{
			handleUnglow();
			break;
		}
		if (GetAsyncKeyState(VK_NUMPAD1) & 1)
		{
			global.bUpdateDisplay = true;
			bTrigBot = !bTrigBot;
			
			if (bTrigBot)
				sTrigBot = "ON ";
			else
				sTrigBot = "OFF";
		} //toggles trig bot
		if (GetAsyncKeyState(VK_XBUTTON1) & 0x8000) // 0x8000 indicates the button is currently down, only triggers while held
		{
			handleTrigBot();
		}
		if (GetAsyncKeyState(VK_NUMPAD2) & 1)
		{
			global.bUpdateDisplay = true;
			bBHop = !bBHop;

			if (bBHop)
				sBHop = "ON ";
			else
				sBHop = "OFF";
		}
		if (GetAsyncKeyState(VK_NUMPAD3) & 1)
		{
			global.bUpdateDisplay = true;
			bRadar = !bRadar;

			if (bRadar)
				sRadar = "ON ";
			else
			{
				CBaseEntityList* entList = (CBaseEntityList*)(global.gameModule + offsets.entity_list);
				for (auto e : entList->entList)
				{
					if (e.entPtr)
					{
						if (e.entPtr->m_bSpotted == true)
							e.entPtr->m_bSpotted = false;
					}
				}
				sRadar = "OFF";
			}
		}
		if (GetAsyncKeyState(VK_NUMPAD4) & 1)
		{
			global.bUpdateDisplay = true;
			bGlow = !bGlow;

			if (bGlow)
			{
				setBrightness(4.0f);
				sGlow = "ON ";
			}
			else
			{
				handleUnglow();
				sGlow = "OFF";
			}
				
		}
		if (GetAsyncKeyState(VK_NUMPAD5) & 1)
		{
			global.bUpdateDisplay = true;
			bFlash = !bFlash;

			if (bFlash)
				sFlash = "ON ";
			else
				sFlash = "OFF";
		}
		if (GetAsyncKeyState(VK_NUMPAD6) & 1)
		{
			global.bUpdateDisplay = true;
			bRecoil = !bRecoil;

			if (bRecoil)
				sRecoil = "ON ";
			else
				sRecoil = "OFF";
		}

		//Hack Logic
		if (bTrigBot)
		{
			handleTrigBot();
		}
		if (bBHop) // needs a little work - still better than manual
		{
			if (isPlayerMoving())
			{
				if (GetAsyncKeyState(VK_SPACE) && global.localPlayer->m_fFlags & FL_ONGROUND)
				{
					*global.forceJump = 6;
				}

				if (!(global.localPlayer->m_fFlags & FL_ONGROUND))
				{
					std::cout << "in air \n";
					if (global.localEngine->viewAngle.y > oldViewX)
					{
						oldView = global.localEngine->viewAngle;
						oldView.Normalize();
						oldViewX = oldView.y;

						if (*global.forceRight == 1)
							*global.forceRight = 0;

						*global.forceLeft = 1;
						Sleep(5);
						*global.forceLeft = 0;
						std::cout << "moving left \n";
					}
					else if (global.localEngine->viewAngle.y < oldViewX)
					{
						oldView = global.localEngine->viewAngle;
						oldView.Normalize();
						oldViewX = oldView.y;

						if(*global.forceLeft == 1)
							*global.forceLeft = 0;

						*global.forceRight = 1;
						Sleep(5);
						*global.forceRight = 0;						
						std::cout << "moving right \n";
					}
				}				
			}
		}
		if (bRadar)
		{
			CBaseEntityList* entList = (CBaseEntityList*)(global.gameModule + offsets.base_entity_list);
			int i = 0;
			for (auto e : entList->entList)
			{
				if (e.entPtr && i != 0)
				{
					e.entPtr->m_bSpotted = true;
					if (e.entPtr->m_bDormant == false)
					{
						//std::cout << "Entity health is " << e.entPtr->m_iHealth << std::endl;
						if (e.entPtr->m_bSpotted == true)
						{
							//std::cout << "Entity # is spotted." << std::endl;
						}
					}
					else
					{
						//std::cout << "Entity #" << i << " is dormant." << std::endl;
					}
				}
				i++;
			}
		}
		if (bGlow)
		{
			handleGlow();
		}
		if (bFlash)
		{
			if (global.localPlayer->m_fFlashDuration > 0)
				global.localPlayer->m_fFlashDuration = 0;
		}
		if (bRecoil)
		{
			Vector3 punchAngle = global.localPlayer->m_aimPunchAngle * 2;
			if (global.localPlayer->m_iShotsFired > 1)
			{
				//calc rcs
				Vector3 newAngle = global.localEngine->viewAngle + oldPunch - punchAngle;
				//normalize
				newAngle.Normalize();
				//set
				global.localEngine->viewAngle = newAngle;
			}
			oldPunch = punchAngle;
		}

		Sleep(5);

	}

	//Cleanup&eject
	setBrightness(0.0f);
	fclose(f);
	FreeConsole();
	FreeLibraryAndExitThread(hModule, 0);
	CloseHandle(hModule);
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