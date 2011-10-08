/**********************************************************************
PSApi func types
**********************************************************************/

#ifndef __PSAPIADD_H
#define __PSAPIADD_H

#pragma once

typedef BOOL (WINAPI *EnumProcessesType)(DWORD * lpidProcess, DWORD cb, DWORD* cbNeeded);
typedef BOOL (WINAPI *EnumProcessModulesType)(HANDLE hProcess, HMODULE* lphModule, DWORD cb, LPDWORD lpcbNeeded);
typedef DWORD (WINAPI *GetModuleBaseNameType)(HANDLE hProcess, HMODULE hModule, LPSTR lpBaseName, DWORD nSize);

extern LPCTSTR const cpcEnumProcessesName;
extern LPCTSTR const cpcEnumProcessModulesName;
extern LPCTSTR const cpcPSApiModuleName;
extern LPCTSTR const cpcGetModuleBaseNameName;

#endif//__PSAPIADD_H