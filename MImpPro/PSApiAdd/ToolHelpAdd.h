/**********************************************************************
tool help additional
**********************************************************************/

#ifndef __TOOLHELPADD_H
#define __TOOLHELPADD_H

#pragma once

#include <TLHELP32.H>

typedef HANDLE (WINAPI* CreateToolhelp32SnapshotType)(DWORD dwFlags, DWORD th32ProcessID);
typedef BOOL (WINAPI* Process32FirstType)(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
typedef BOOL (WINAPI* Process32NextType)(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
typedef BOOL (WINAPI* Module32FirstType)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);
typedef BOOL (WINAPI* Module32NextType)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);

extern LPCTSTR const cpcCreateToolhelp32SnapshotName;
extern LPCTSTR const cpcProcess32FirstName;
extern LPCTSTR const cpcProcess32NextName;
extern LPCTSTR const cpcToolHelpModuleName;
extern LPCTSTR const cpcModule32FirstName;
extern LPCTSTR const cpcModule32NextName;

#endif//__TOOLHELPADD_H
