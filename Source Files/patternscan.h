#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <String>
#include "processtools.h"
#include "ntdll.h"

//Basis of all scanning
char* ScanBasic(char* pattern, char* mask, char* begin, intptr_t size);

//Internal Pattern Scan Wrapper
char* ScanInternal(char* pattern, char* mask, char* begin, intptr_t size);

//Internal Module Scan Wrapper
char* TO_CHAR(wchar_t* string);
PEB* GetPEB();
LDR_DATA_TABLE_ENTRY* GetLDREntry(std::string name);
char* ScanModIn(char* pattern, char* mask, std::string modName);

//External Wrapper
char* ScanEx(char* pattern, char* mask, char* begin, intptr_t size, HANDLE hProc);

//Module wrapper for external pattern scan - only scan in certain modules
char* ScanModEx(char* pattern, char* mask, MODULEENTRY32& modEntry, HANDLE hProc);