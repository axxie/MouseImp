/**********************************************************************
"min crt" support (include source file) - based on ATL/TunyCRT(From MSDN)
**********************************************************************/

//note:
//
//include this file in project source _ONCE_ (per project) - ex in stdafx.cpp
//work only for non debug config's (_DEBUG not defined)

//note:
//
//!!!!!!!INIT/FINIT FOR STATICAL VARIABLES NOT SUPPORTED!!!!!!!
///FOR USE WITH DLL'S - DEFINE _USRDLL OR _WINDLL


//from msdn about entry point's
/*

By default, the starting address is a function name from the C run-time library. The linker selects it according to the attributes of the program, as shown in the following table.

Function name Default for 
mainCRTStartup (or wmainCRTStartup) An application using /SUBSYSTEM:CONSOLE; calls main (or wmain) 
WinMainCRTStartup (or wWinMainCRTStartup) An application using /SUBSYSTEM:WINDOWS; calls WinMain (or wWinMain), which must be defined with __stdcall 
_DllMainCRTStartup A DLL; calls DllMain, which must be defined with __stdcall, if it exists 

*/



#ifndef __SLWMINCRT_H
#define __SLWMINCRT_H

#pragma once


#include "SLWMinCrtHdr.h"



//////////////////////////////////////////////////////////////////////
//TrapRoutines

#ifdef SWL_MIN_CRT

//purecall
int __cdecl _purecall()
{
  ::DebugBreak();
  return 0;
};

extern "C" const int _fltused = 0;


//////////////////////////////////////////////////////////////////////
//HeapSupport

//heap handler
HANDLE hHeapHndl = 0;

//initialize heap
static void HeapInit()
{
  hHeapHndl = ::GetProcessHeap();
//  hHeapHndl = ::HeapCreate
//    (
//    HEAP_GENERATE_EXCEPTIONS,
//    0,
//    0
//    );
};

//finit local heap
static void HeapFinit()
{
//  ::HeapDestroy(hHeapHndl);
  hHeapHndl = 0;
};


//////////////////////////////////////////////////////////////////////
//Process/Thread init func's 

static inline bool ProcInit()
{
  bool bRes = false;

  HeapInit();
  bRes = true;

  return bRes;
};

static inline void ProcFinit()
{
  HeapFinit();
};

void* __cdecl malloc(size_t n)
{
  return ::HeapAlloc(hHeapHndl, HEAP_GENERATE_EXCEPTIONS, n);
};

void* __cdecl calloc(size_t n, size_t s)
{
  return ::HeapAlloc(hHeapHndl, HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY, n * s);
};

void* __cdecl realloc(void* p, size_t n)
{
  return (0 == p)
    ? malloc(n)
    : ::HeapReAlloc(hHeapHndl, HEAP_GENERATE_EXCEPTIONS, p, n);
};

void __cdecl free(void* p)
{
  ::HeapFree(hHeapHndl, 0, p);
};

void* __cdecl operator new(size_t n)
{
  return malloc(n);
}

void __cdecl operator delete(void* p)
{
  free(p);
}

//////////////////////////////////////////////////////////////////////
//chng entry point for module

#ifdef SWL_DLL_LINK

////Win32 dll's

extern "C" BOOL WINAPI _DllMainCRTStartup(HANDLE hDllHandle, DWORD dwReason, LPVOID lpReserved)
{
  BOOL bRes = TRUE;
  //init CRT
  switch(dwReason)
  {
  case DLL_PROCESS_ATTACH:
    bRes = false != ProcInit();
    break;

  case DLL_THREAD_ATTACH:
    break;

  default:
    break;
  };

  //user provided
  if(FALSE != bRes)
  {
    bRes = DllMain(hDllHandle, dwReason, lpReserved);
  };

  //finit CRT
  switch(dwReason)
  {
  case DLL_THREAD_DETACH:
    break;

  case DLL_PROCESS_DETACH:
    ProcFinit();
    break;

  default:
    break;
  };

  return bRes;
}

#elif defined _CONSOLE

////wine32 console

//cmd arg's helper
//dbg#ifndef __SLW_NO_COMMAND_LINE_INFO

//hardly based on tinycrt article from MSDN
static inline int __ConvertCommandLineToArgcArgv(char* cpArgMass[], const int icArgMassSize, char** const cppDynMem)
{
  int cbCmdLine;
  int argc;
  LPSTR pszSysCmdLine, pszCmdLine;
  
  // Set to no argv elements, in case we have to bail out
  cpArgMass[0] = 0;
  
  // First get a pointer to the system's version of the command line, and
  // figure out how long it is.
  pszSysCmdLine = GetCommandLine();
  cbCmdLine = lstrlen( pszSysCmdLine );
  
  // Allocate memory to store a copy of the command line.  We'll modify
  // this copy, rather than the original command line.  Yes, this memory
  // currently doesn't explicitly get freed, but it goes away when the
  // process terminates.
  *cppDynMem = reinterpret_cast<char*>(HeapAlloc( GetProcessHeap(), 0, cbCmdLine+1 ));
  pszCmdLine = *cppDynMem;
  if ( !pszCmdLine )
    return 0;
  
  // Copy the system version of the command line into our copy
  lstrcpy( pszCmdLine, pszSysCmdLine );
  
  if ( '"' == *pszCmdLine )   // If command line starts with a quote ("),
  {                           // it's a quoted filename.  Skip to next quote.
    pszCmdLine++;
    
    cpArgMass[0] = pszCmdLine;  // argv[0] == executable name
    
    while ( *pszCmdLine && (*pszCmdLine != '"') )
      pszCmdLine++;
    
    if ( *pszCmdLine )      // Did we see a non-NULL ending?
      *pszCmdLine++ = 0;  // Null terminate and advance to next char
    else
      return 0;           // Oops!  We didn't see the end quote
  }
  else    // A regular (non-quoted) filename
  {
    cpArgMass[0] = pszCmdLine;  // argv[0] == executable name
    
    while ( *pszCmdLine && (' ' != *pszCmdLine) && ('\t' != *pszCmdLine) )
      pszCmdLine++;
    
    if ( *pszCmdLine )
      *pszCmdLine++ = 0;  // Null terminate and advance to next char
  }
  
  // Done processing argv[0] (i.e., the executable name).  Now do th
  // actual arguments
  
  argc = 1;
  
  while ( 1 )
  {
    // Skip over any whitespace
    while ( *pszCmdLine && (' ' == *pszCmdLine) || ('\t' == *pszCmdLine) )
      pszCmdLine++;
    
    if ( 0 == *pszCmdLine ) // End of command line???
      return argc;
    
    if ( '"' == *pszCmdLine )   // Argument starting with a quote???
    {
      pszCmdLine++;   // Advance past quote character
      
      cpArgMass[ argc++ ] = pszCmdLine;
      cpArgMass[ argc ] = 0;
      
      // Scan to end quote, or NULL terminator
      while ( *pszCmdLine && (*pszCmdLine != '"') )
        pszCmdLine++;
      
      if ( 0 == *pszCmdLine )
        return argc;
      
      if ( *pszCmdLine )
        *pszCmdLine++ = 0;  // Null terminate and advance to next char
    }
    else                        // Non-quoted argument
    {
      cpArgMass[ argc++ ] = pszCmdLine;
      cpArgMass[ argc ] = 0;
      
      // Skip till whitespace or NULL terminator
      while ( *pszCmdLine && (' '!=*pszCmdLine) && ('\t'!=*pszCmdLine) )
        pszCmdLine++;
      
      if ( 0 == *pszCmdLine )
        return argc;
      
      if ( *pszCmdLine )
        *pszCmdLine++ = 0;  // Null terminate and advance to next char
    }
    
    if ( argc >= (icArgMassSize) )
      return argc;
  }
};
//dbg#endif//__SLW_NO_COMMAND_LINE_INFO


//main entry
extern "C" void mainCRTStartup()
{
  //init CR stub
  ProcInit();

  int iRes = 0;
#ifndef __SLW_NO_COMMAND_LINE_INFO

  //build cmd line
  char* cpCmdLine[128];
  char* pDynMem = 0;
  const int icArgCount = __ConvertCommandLineToArgcArgv(cpCmdLine, COUNTOF(cpCmdLine), &pDynMem);
  //call module
  iRes = main(icArgCount, cpCmdLine);
  //free mem for cmd line
  ::HeapFree(::GetProcessHeap(), 0, pDynMem);

#else//__SLW_NO_COMMAND_LINE_INFO

  //no cmd line
  iRes = main(0, 0);

#endif//__SLW_NO_COMMAND_LINE_INFO

  //finit CR stub
  ProcFinit();

  ::ExitProcess(iRes);
};

#else//SWL_DLL_LINK


////win32 GUI application's

#define SPACECHAR   ' '
#define DQUOTECHAR  '\"'


#ifdef _UNICODE
extern "C" void wWinMainCRTStartup()
#else // _UNICODE
extern "C" void WinMainCRTStartup()
#endif // _UNICODE
{
  LPTSTR lpszCommandLine = 0;
#ifndef __SLW_NO_COMMAND_LINE_INFO

  lpszCommandLine = ::GetCommandLine();
  if(lpszCommandLine == NULL)
    ::ExitProcess((UINT)-1);

  // Skip past program name (first token in command line).
  // Check for and handle quoted program name.
  if(*lpszCommandLine == DQUOTECHAR)
  {
    // Scan, and skip over, subsequent characters until
    // another double-quote or a null is encountered.
    do
    {
      lpszCommandLine = ::CharNext(lpszCommandLine);
    }
    while((*lpszCommandLine != DQUOTECHAR) && (*lpszCommandLine != '\0'));

    // If we stopped on a double-quote (usual case), skip over it.
    if(*lpszCommandLine == DQUOTECHAR)
      lpszCommandLine = ::CharNext(lpszCommandLine);
  }
  else
  {
    while(*lpszCommandLine > SPACECHAR)
      lpszCommandLine = ::CharNext(lpszCommandLine);
  }

  // Skip past any white space preceeding the second token.
  while(*lpszCommandLine && (*lpszCommandLine <= SPACECHAR))
    lpszCommandLine = ::CharNext(lpszCommandLine);

#endif//__SLW_NO_COMMAND_LINE_INFO

  STARTUPINFO StartupInfo;
  StartupInfo.dwFlags = 0;
  ::GetStartupInfo(&StartupInfo);

  //init crt stub
  ProcInit();

  int nRet = WinMain(::GetModuleHandle(NULL), NULL, lpszCommandLine,
    (StartupInfo.dwFlags & STARTF_USESHOWWINDOW) ?
    StartupInfo.wShowWindow : SW_SHOWDEFAULT);

  //finit CR stub
  ProcFinit();

  ::ExitProcess((UINT)nRet);
}


#endif//SWL_DLL_LINK


#endif//SWL_MIN_CRT


#endif//__SLWMINCRT_H