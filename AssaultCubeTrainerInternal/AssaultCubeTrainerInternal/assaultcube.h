#pragma once

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

struct Vector3 { float x, y, z; };

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
		DWORD vTable; //0x0
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