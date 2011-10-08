/**********************************************************************
tool help additional
**********************************************************************/

#define STRICT
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#pragma hdrstop


#include "ToolHelpAdd.h"


#include <tchar.h>


LPCTSTR const cpcCreateToolhelp32SnapshotName = _T("CreateToolhelp32Snapshot");
LPCTSTR const cpcToolHelpModuleName = _T("kernel32.dll");
LPCTSTR const cpcProcess32FirstName = _T("Process32First");
LPCTSTR const cpcProcess32NextName = _T("Process32Next");
LPCTSTR const cpcModule32FirstName = _T("Module32First");
LPCTSTR const cpcModule32NextName = _T("Module32Next");
