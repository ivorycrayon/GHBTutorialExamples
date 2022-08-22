#pragma once
#include <Windows.h>
#include <cstdint>

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

using namespace hazedumper;

struct Vector3
{
	float x, y, z; 

	Vector3 operator+(Vector3 d)
	{
		return { x + d.x, y + d.y, z + d.z };
	}
	Vector3 operator-(Vector3 d)
	{
		return { x - d.x, y - d.y, z - d.z };
	}
	Vector3 operator*(float d)
	{
		return { x * d, y * d, z * d };
	}

	void Normalize()
	{
		while (y < -180)
		{
			y += 360;
		}
		while (y > 180)
		{
			y -= 360;
		}
		if (x < -89)
		{
			x = -89;
		}
		if (x > 89)
		{
			x = 89;
		}
	}
};

struct ClrRender
{
	BYTE red, green, blue, alpha;
};

class entity
{
public:
	union
	{
		//              Type     Name    Offset
		DEFINE_MEMBER_N(ClrRender, m_clrRender, netvars::m_clrRender);
		DEFINE_MEMBER_N(bool, m_bDormant, signatures::m_bDormant);
		DEFINE_MEMBER_N(bool, m_iTeamNum, netvars::m_iTeamNum);
		DEFINE_MEMBER_N(int32_t, m_iHealth, netvars::m_iHealth);
		DEFINE_MEMBER_N(uint32_t, m_fFlags, netvars::m_fFlags);
		DEFINE_MEMBER_N(Vector3, m_vecVelocity, netvars::m_vecVelocity);
		DEFINE_MEMBER_N(Vector3, m_vecOrigin, netvars::m_vecOrigin);
		DEFINE_MEMBER_N(bool, m_bSpotted, netvars::m_bSpotted);
		DEFINE_MEMBER_N(int32_t, m_hActiveWeapon, netvars::m_hActiveWeapon);
		DEFINE_MEMBER_N(bool, m_bIsScoped, netvars::m_bIsScoped);
		DEFINE_MEMBER_N(bool, m_bIsDefusing, netvars::m_bIsDefusing);
		DEFINE_MEMBER_N(int32_t, m_iGlowIndex, netvars::m_iGlowIndex);
		DEFINE_MEMBER_N(int32_t, m_iCrosshairID, netvars::m_iCrosshairId);
		DEFINE_MEMBER_N(int32_t, m_fFlashDuration, netvars::m_flFlashDuration);
		DEFINE_MEMBER_N(int32_t, m_iShotsFired, netvars::m_iShotsFired);
		DEFINE_MEMBER_N(Vector3, m_aimPunchAngle, netvars::m_aimPunchAngle);

	};
};

class clientInfo
{
public:
	union
	{
		entity* entPtr; //0x0
		//              Type     Name    Offset
		DEFINE_MEMBER_N(clientInfo*, backLink, 0x8);
		DEFINE_MEMBER_N(clientInfo*, forwardLink, 0xC);
	};
};

class CBaseEntityList
{
public:
	union
	{
		//              Type     Name    Offset
		DEFINE_MEMBER_N(clientInfo, entList[64], 0x10);
	};
};

class engine
{
public:
	union
	{
		//              Type     Name    Offset
		DEFINE_MEMBER_N(Vector3, viewAngle, signatures::dwClientState_ViewAngles);
	};
};

/*class GlowEngine
{
public:
	union
	{
		//              Type     Name    Offset
		DEFINE_MEMBER_N(float, red, 0x8);
		DEFINE_MEMBER_N(float, green, 0xC);
		DEFINE_MEMBER_N(float, blue, 0x10);
		DEFINE_MEMBER_N(float, alpha, 0x14);
		DEFINE_MEMBER_N(bool, renderWhenOccluded, 0x28);
		DEFINE_MEMBER_N(bool, renderWhenUnoccluded, 0x29);
		DEFINE_MEMBER_N(bool, fullBloom, 0x30);
		DEFINE_MEMBER_N(int32_t, glowStyle, 0x36);
	};
};
*/