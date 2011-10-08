/**********************************************************************
PSApi func types
**********************************************************************/

#define STRICT
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#pragma hdrstop


#include "PSApiAdd.h"


#include <tchar.h>


LPCTSTR const cpcEnumProcessesName = _T("EnumProcesses");
LPCTSTR const cpcEnumProcessModulesName = _T("EnumProcessModules");
LPCTSTR const cpcPSApiModuleName = _T("psapi.dll");
LPCTSTR const cpcGetModuleBaseNameName = _T("GetModuleBaseNameA");