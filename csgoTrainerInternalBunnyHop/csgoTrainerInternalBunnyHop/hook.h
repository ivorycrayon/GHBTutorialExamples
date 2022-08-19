#pragma once
#include "pch.h"
#include <windows.h>

bool Hook(void* toHook, void* ourFunct, int len); // tohook- pointer to function we're hooking, ourfunct- spot we're hooking jumps to, len - # opcodes we're writing