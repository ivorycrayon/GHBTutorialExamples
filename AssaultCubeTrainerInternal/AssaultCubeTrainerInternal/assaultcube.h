#pragma once
#include "geom.h"
#include <Windows.h>

class weapon;

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

//struct Vec3 { float x, y, z; };

class PlayerEntity
{
public:
	union
	{
		DWORD vTable; //0x0
		//              Type     Name    Offset
		DEFINE_MEMBER_N(Vec3, posHead, 0x4);
		DEFINE_MEMBER_N(Vec3, velocity, 0x28);
		DEFINE_MEMBER_N(Vec3, posBody, 0x34);
		DEFINE_MEMBER_N(Vec3, angle, 0x40);
		DEFINE_MEMBER_N(bool, bScoping, 0x71);
		DEFINE_MEMBER_N(int32_t, health, 0xF8);
		DEFINE_MEMBER_N(int32_t, armor, 0xFC);
		DEFINE_MEMBER_N(int8_t, noClipMode, 0x82);
		DEFINE_MEMBER_N(int32_t, arAmmo, 0x128);
		DEFINE_MEMBER_N(bool, bAttacking, 0x224);
		DEFINE_MEMBER_N(char, name[16], 0x225);
		DEFINE_MEMBER_N(BYTE, team, 0x32C);
		DEFINE_MEMBER_N(int8_t, spectateMode, 0x338);
		DEFINE_MEMBER_N(weapon*, currentWeapon, 0x374);
	};

};

class weapon
{
public:
	union
	{
		//              Type     Name    Offset
		DEFINE_MEMBER_N(int32_t*, weaponID, 0x04);
		DEFINE_MEMBER_N(PlayerEntity*, owner, 0x08);
		DEFINE_MEMBER_N(uintptr_t*, gunInfo, 0x0C);
		DEFINE_MEMBER_N(int32_t*, ammoPtr2, 0x10);
		DEFINE_MEMBER_N(int32_t*, ammoPtr, 0x14);
		DEFINE_MEMBER_N(int32_t*, gunWait, 0x18);
		DEFINE_MEMBER_N(int32_t, shotsFired, 0x1C);
		DEFINE_MEMBER_N(int32_t, bReload, 0x20);
		DEFINE_MEMBER_N(int32_t*, recoilPtr, 0x28);
	};
};

struct EntList
{
	PlayerEntity* ents[31];
};

