/**********************************************************************
debug macros
**********************************************************************/

#define STRICT
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#pragma hdrstop


#include "SLDbg.h"


#include <stdarg.h>


_SL_BEG


void SLDbgPrintf(LPCTSTR const cpcMask, ...)
{
  va_list marker; 
  TCHAR szBuf[256]; 
  
  va_start(marker, cpcMask); 
  wvsprintf(szBuf, cpcMask, marker); 
  va_end(marker); 
  
  OutputDebugString(szBuf); 
};


_SL_END
