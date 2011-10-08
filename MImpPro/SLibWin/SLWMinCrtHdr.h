/**********************************************************************
"min crt" support (include source file) - based on ATL/TunyCRT(From MSDN)
**********************************************************************/

//note:
//
//include this file in project main header file - ex in stdafx.h
//work only for non debug config's (_DEBUG not defined) - look on SWL_MIN_CRT macro

//note:
//
//!!!!!!!INIT/FINIT FOR STATICAL VARIABLES NOT SUPPORTED!!!!!!!
///FOR USE WITH DLL'S - DEFINE _USRDLL OR _WINDLL (lok on SWL_DLL_LINK macro)

//size of exe files can be redused if __SLW_NO_COMMAND_LINE_INFO defined



#ifndef __SLWMINCRTHDR_H
#define __SLWMINCRTHDR_H

#pragma once


//ony for non debug build
#ifndef _DEBUG
#define SWL_MIN_CRT
#endif//_DEBUG

#ifdef SWL_MIN_CRT


//define type of linking 
#if defined(_WINDLL) || defined(_USRDLL)
#define SWL_DLL_LINK
#endif//defined(_WINDLL) || defined(_USRDLL)


//////////////////////////////////////////////////////////////////////
//user overrided entry point's

#ifdef SWL_DLL_LINK

////prototype for Win32 GUI application
extern "C" BOOL WINAPI DllMain(HANDLE hDllHandle, DWORD dwReason, LPVOID lpReserved);

#elif defined _CONSOLE

////prototype for Win32 console application
extern "C" int main(int iArgCount, char* cpArgMass[]);

#else//SWL_DLL_LINK

////Win32 DLL's

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);

#endif//SWL_DLL_LINK

#endif//SWL_MIN_CRT


#endif//__SLWMINCRTHDR_H
