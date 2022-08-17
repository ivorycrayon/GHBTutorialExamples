#pragma once
#include <Windows.h>
#include <cstdint>

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

class entity
{
public:
	union
	{
		//              Type     Name    Offset
		DEFINE_MEMBER_N(int32_t, health, 0x100);
		DEFINE_MEMBER_N(bool, bDormant, 0xED);
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