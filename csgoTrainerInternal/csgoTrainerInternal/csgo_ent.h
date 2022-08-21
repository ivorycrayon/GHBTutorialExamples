#pragma once
#include <Windows.h>
#include <cstdint>

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

struct Vector3 { float x, y, z; };

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
		DEFINE_MEMBER_N(ClrRender, m_clrRender, hazedumper::netvars::m_clrRender);
		DEFINE_MEMBER_N(bool, m_bDormant, hazedumper::signatures::m_bDormant);
		DEFINE_MEMBER_N(bool, m_iTeamNum, hazedumper::netvars::m_iTeamNum);
		DEFINE_MEMBER_N(int32_t, m_iHealth, hazedumper::netvars::m_iHealth);
		DEFINE_MEMBER_N(uint32_t, m_fFlags, hazedumper::netvars::m_fFlags);
		DEFINE_MEMBER_N(Vector3, m_vecVelocity, hazedumper::netvars::m_vecVelocity);
		DEFINE_MEMBER_N(Vector3, m_vecOrigin, hazedumper::netvars::m_vecOrigin);
		DEFINE_MEMBER_N(bool, m_bSpotted, hazedumper::netvars::m_bSpotted);
		DEFINE_MEMBER_N(int32_t, m_hActiveWeapon, hazedumper::netvars::m_hActiveWeapon);
		DEFINE_MEMBER_N(bool, m_bIsScoped, hazedumper::netvars::m_bIsScoped);
		DEFINE_MEMBER_N(bool, m_bIsDefusing, hazedumper::netvars::m_bIsDefusing);
		DEFINE_MEMBER_N(int32_t, m_iGlowIndex, hazedumper::netvars::m_iGlowIndex);
		DEFINE_MEMBER_N(int32_t, m_iCrosshairID, hazedumper::netvars::m_iCrosshairId);

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
		DEFINE_MEMBER_N(float, viewY, 0x4D90);
		DEFINE_MEMBER_N(float, viewX, 0x4D94);
	};
};

class GlowEngine
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