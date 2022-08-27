#include "hook.h"

bool HookBasic(void* toHook, void* ourFunct, int len) // tohook- pointer to function we're hooking, ourfunct- spot we're hooking jumps to, len - # opcodes we're writing
{
	if (len < 5) //We check the length to be at least 5 bytes because this is the smallest relative jmp in x86.The instructions you will be destroying by overwriting will be at least 5 bytes.
	{
		return false;
	}

	DWORD curProtection;
	VirtualProtect(toHook, len, PAGE_EXECUTE_READWRITE, &curProtection);

	memset(toHook, 0x90, len);

	DWORD relativeAdress = ((DWORD)ourFunct - (DWORD)toHook - 5); //offset from the jump to where we need to go

	*(BYTE*)toHook = 0xE9;
	*(DWORD*)((DWORD)toHook + 1) = relativeAdress;

	DWORD temp;
	VirtualProtect(toHook, len, curProtection, &temp);

	return true;
}

bool Detour32(BYTE* src, BYTE* dst, const uintptr_t len) //src- where we want to write our jump. //dst - what we want to jump to. //len - the number of bytes we're overwriting, always at least 5
{
	if (len < 5) //We check the length to be at least 5 bytes because this is the smallest relative jmp in x86.The instructions you will be destroying by overwriting will be at least 5 bytes. jmp + 4byte address = 5
	{
		return false;
	}

	DWORD curProtection;
	VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &curProtection); //the pages we are overwriting do not have write permissions because they're an execution section

	uintptr_t relativeAdress = dst - src - 5; //offset from the jump to where we need to go

	*src = 0xE9; // jmp
	*(uintptr_t*)(src + 1) = relativeAdress;

	VirtualProtect(src, len, curProtection, &curProtection);

	return true;
}
BYTE* TrampHook32(BYTE* src, BYTE* dst, const uintptr_t len) //Wrapper for the detour function
{
	if (len < 5) //We check the length to be at least 5 bytes because this is the smallest relative jmp in x86.The instructions you will be destroying by overwriting will be at least 5 bytes. jmp + 4byte address = 5
	{
		return 0;
	}

	//Create Gateway
	BYTE* gateway = (BYTE*)VirtualAlloc(0, len, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE); //standard way of creating codepages where you're going to be executing
	//returns the new address of memory we've allocated for our gateway

	//Write the stolen bytes to the gateway
	memcpy_s(gateway, len, src, len);
	//adds the original stolen instructions first

	//Get the gateway to destination address - relative address from the gateway back to the function/ jump back address
	uintptr_t gatewayRelativeAddress = src - gateway - 5;

	//add jmp opcode to the end of the gateway
	*(gateway + len) = 0xE9;

	//Write the address of the gateway to the jmp
	*(uintptr_t*)((uintptr_t)gateway + len + 1) = gatewayRelativeAddress;

	//Perform the detour
	Detour32(src, dst, len);

	return gateway;
}

Hook::Hook(BYTE* src, BYTE* dst, BYTE* ptrToGatewayFnPtr, uintptr_t len)
{
	this->src = src;
	this->dst = dst;
	this->len = len;
	this->ptrToGatewayFnPtr = ptrToGatewayFnPtr;
}
Hook::Hook(const char* exportName, const char* modName, BYTE* dst, BYTE* ptrToGatewayFnPtr, uintptr_t len)
{
	HMODULE hMod = GetModuleHandleA(modName);
	this->src = (BYTE*)GetProcAddress(hMod, exportName);
	this->dst = dst;
	this->len = len;
	this->ptrToGatewayFnPtr = ptrToGatewayFnPtr;
}

void Hook::Enable()
{
	memcpy(originalBytes, src, len);
	*(uintptr_t*)ptrToGatewayFnPtr = (uintptr_t)TrampHook32(src, dst, len);
	bStatus = true;
}
void Hook::Disable()
{
	mem::Patch(src, originalBytes, len);
	bStatus = false;
}
void Hook::Toggle()
{
	if (bStatus) Disable();
	else Enable();
}