/**********************************************************************
exception info hanlder
**********************************************************************/
#ifndef __SLEXCHANDLER_H
#define __SLEXCHANDLER_H

#pragma once

#include "SLutil.h"

////heavy based on "Under the Hood" Matt Pietrek's article in MSDN
//for use:
//  1. define "line numbers only", "c7.0 compatible option" or "programm database"
//  2. define "debugtype" linker option to "COFF Format" or to "Both Formats"

_SL_BEG


//install exception filer module
void SLExcInstall(LPCTSTR const cpcRepFileName = 0, const bool bcInstallGlobal = false);
//uninstall exception filter module
void SLExcUnInstall();
//exception filter for use in __try/__except block's as sample
//  //gen exception
//  __try
//  {
//    BYTE* const cpMem = 0;
//    *cpMem = 0;
//  }
//  //call excetion filter for generate exception report and execute handler
//  __except(SLExceptionFilter(GetExceptionInformation()), EXCEPTION_CONTINUE_SEARCH)
//  {
//  };
LONG WINAPI SLExceptionFilter(_EXCEPTION_POINTERS *pExceptionInfo);

_SL_END

#endif//__SLEXCHANDLER_H
