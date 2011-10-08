/**********************************************************************
debug macros
**********************************************************************/

#ifndef __SLDBG_H
#define __SLDBG_H
#pragma once

#include <crtdbg.h>

#include "SLUtil.h"


#ifdef _DEBUG


//send string to Debug console
#define SLTRACE(Msg) OutputDebugString(Msg)

//check expression and dump message (DBG only)
#define SLASSERT(expr, Msg) \
{\
  if (!(expr) && (1 == _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, Msg)))\
  {\
    _CrtDbgBreak();\
  };\
}

template<class Type>
inline Type __SLCondFunc(Type bcCond, const char* const cpcCond)
{
  SLASSERT(bcCond, cpcCond);
  return bcCond;
};

#define SLCOND(Expr) __SLCondFunc(Expr, #Expr)
#define SLCONDM(Expr, Msg) __SLCondFunc(Expr, Msg)

//test passed string pointer for validity (as ::IsBadStringPtr) (DBG only)
#define SLPTRSTR(Ptr) SLASSERT(false != sl::SLPtrStr(Ptr), #Ptr " - bad string")
//test passed pointer for valid read access (DBG only)
#define SLPTRRD(Ptr) SLASSERT(false != sl::SLPtrRd(Ptr), #Ptr " - can't read")
//test passed pointer for valid write access (DBG only)
#define SLPTRWR(Ptr) SLASSERT(false != sl::SLPtrWr(Ptr), #Ptr " - can't write")

//check expression and dump message (like VERIFY macro in MFC)
#define SLCHECK(expr) SLASSERT(expr, #expr " failed")

#else//_DEBUG

#define SLTRACE(Msg) ((void)0)

#define SLASSERT(expr, Msg) ((void)0)

#define SLCOND(Expr) (Expr)
#define SLCONDM(Expr, Msg) (Expr)

#define SLPTRSTR(Ptr) ((void)0)
#define SLPTRRD(Ptr) ((void)0)
#define SLPTRWR(Ptr) ((void)0) 

#define SLCHECK(expr) (expr)

#endif//_DEBUG

#endif//__SLDBG_H