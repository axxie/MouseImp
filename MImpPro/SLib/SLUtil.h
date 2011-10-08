/**********************************************************************
utility classes
**********************************************************************/

#ifndef __SLUTIL_H
#define __SLUTIL_H
#pragma once


#include <limits.h>


//////////////////////////////////////////////////////////////////////
//NameSpace's

#define _SL_BEG namespace sl{
#define _SL_END };
#define _SL_USE using namespace sl;

_SL_BEG

//tchar.h functionaly
#ifndef _T
# ifndef _UNICODE
#   define _T(x) x
# else//_UNICODE
#   define _T(x) L##x
# endif//_UNICODE
#endif//_T

//////////////////////////////////////////////////////////////////////
//mem check func's

////swap
template<class Type>
inline void SLSwap(Type& rFrom, Type& rTo)
{
  Type Mem = rFrom;
  rFrom = rTo;
  rTo = Mem;
};

////align helper's
inline ULONG SLAlignUp(const ULONG ulcAddr, const ULONG ulcAlignVal = sizeof(void*))
{
  return (ulcAddr + ulcAlignVal - 1) & (~(ulcAlignVal - 1));
};
inline ULONG SLAlignDown(const ULONG ulcAddr, const ULONG ulcAlignVal = sizeof(void*))
{
  return (ulcAddr) & (~(ulcAlignVal - 1));
};

//test for read memory passed type
template<class Type>
inline bool SLPtrRd(const Type* const cpcPtr)
{
  return FALSE == ::IsBadReadPtr(cpcPtr, sizeof(*cpcPtr));
};

//test for write memory passed type
template<class Type>
inline bool SLPtrWr(Type* const cpPtr)
{
  return FALSE == ::IsBadWritePtr(cpPtr, sizeof(*cpPtr));
};

//test for string validity
inline bool SLPtrStr(LPCTSTR const cpcPtr)
{
  return FALSE == ::IsBadStringPtr(cpcPtr, static_cast<DWORD>(-1));
};


//////////////////////////////////////////////////////////////////////
//Helper's

#ifndef COUNTOF
#define COUNTOF(pMass) (sizeof(pMass) / sizeof(pMass[0]))
#endif//COUNTOF

template<class Type>
inline Type SLMaxUVal(const Type& rDummyVal)
{
  return (1 << (sizeof(rDummyVal) * CHAR_BIT)) - 1;
};

template<class Type>
inline Type SLMinUVal(const Type& rDummyVal)
{
  return 0;
};

template<class Type>
inline Type LowPart(const Type Val)
{
  const DWORD dwcBitSize = sizeof(Type) * CHAR_BIT / 2;
  return Val & ((static_cast<Type>(1) << dwcBitSize) - 1);
};

template<class Type>
inline Type HiPart(const Type Val)
{
  const DWORD dwcBitSize = sizeof(Type) * CHAR_BIT / 2;
  return (Val >> dwcBitSize) & ((static_cast<Type>(1) << dwcBitSize) - 1);
};

template<class Type> 
inline Type MakeType(const Type cLow, const Type cHi)
{
  const DWORD dwcBitSize = sizeof(Type) * CHAR_BIT / 2;
  const Type cLowPart = cLow & ((static_cast<Type>(1) << dwcBitSize) - 1);
  const Type cHiPart = cHi & ((static_cast<Type>(1) << dwcBitSize) - 1);
  return (cHiPart << dwcBitSize) | cLowPart;
};


_SL_END

#endif//__SLUTIL_H