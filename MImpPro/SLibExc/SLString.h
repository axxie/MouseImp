/**********************************************************************
string impl
**********************************************************************/

#ifndef __SLSTRING_H
#define __SLSTRING_H

#pragma once

#include "SLUtil.h"

#include <crtdbg.h>

_SL_BEG

//////////////////////////////////////////////////////////////////////
//CSLString

class CSLString
{
protected:

  enum ClassConstEnum
  {
    eccBuffGrowAlign = 8,
    eccUseStrSize = sizeof(TCHAR),
    eccResLoadStartLen = 64,
    eccResLoadAddLen = 64,
  };

  struct __CBuffHdr
  {
    ULONG ulBuffLen;
    ULONG ulStrLen;
    ULONG ulRefCount;
    inline LPTSTR GetStr();
  };
  __CBuffHdr* pData;
  static ULONG NullData[];
  
public:
  
public:

  inline CSLString();
  inline CSLString(const TCHAR cChar);
  inline CSLString(LPCTSTR const cpcInitStr, const ULONG ulcInitStrLen = ULONG_MAX);
  inline CSLString(const CSLString& rInitStr);
  inline ~CSLString();

  inline ULONG GetLen() const;
  inline LPCTSTR GetPtr() const;
  inline bool IsEmpty() const;
  inline operator LPCTSTR() const;

  inline void AjustLen();
  inline void AjustLen(const ULONG ulcWantLen);

  inline TCHAR At(const ULONG ulcPos) const;

  inline void Clear();

  inline CSLString& operator=(const CSLString& rAssignStr);
  inline CSLString& operator=(LPCTSTR const cpcAssignStr);
  inline CSLString& operator=(const TCHAR cChar);
  inline CSLString& operator+=(const CSLString& rAssignStr);
  inline CSLString& operator+=(LPCTSTR const cpcAssignStr);

  inline LPTSTR GetBuff(const ULONG ulcWaitSize);

  inline bool LoadString(const UINT uicResId, HINSTANCE hInst = ::GetModuleHandle(0));
  
protected:
  static inline __CBuffHdr* GetNull();
  static inline __CBuffHdr* __AllocBuff(const ULONG ulcWantSize);
  inline __CBuffHdr* __ReallocBuff(const ULONG ulcWantSize, const bool bcCopyData);
  inline void __CopyBuff(__CBuffHdr* const cpTo, const ULONG ulcToPos, const ULONG ulcToLen, LPCTSTR const cpcFrom, const ULONG ulcFromPos);
  inline void __CopyBuffAndEnd(__CBuffHdr* const cpTo, const ULONG ulcToPos, const ULONG ulcToLen, LPCTSTR const cpcFrom, const ULONG ulcFromPos);
  static inline void __FreeBuff(__CBuffHdr* const cpBuff);
};

//////////////////////////////////////////////////////////////////////
//Globals

inline CSLString operator+(const TCHAR cFirst, const CSLString& rcStr)
{
  CSLString Res(cFirst);
  Res += rcStr;
  return Res;
};

inline CSLString operator+(LPCTSTR const cpcFirst, const CSLString& rcStr)
{
  CSLString Res(cpcFirst);
  Res += rcStr;
  return Res;
};


/**********************************************************************
implementation
**********************************************************************/

//////////////////////////////////////////////////////////////////////
//CSLString::__CBuffHdr

inline LPTSTR CSLString::__CBuffHdr::GetStr()
{
  return reinterpret_cast<LPTSTR>(this + 1);
};

//////////////////////////////////////////////////////////////////////
//CSLString

inline CSLString::CSLString()
  :pData(GetNull())
{
  pData->ulRefCount++;
};

inline CSLString::CSLString(const TCHAR cChar)
{
  pData = __AllocBuff(1);
  __CopyBuffAndEnd(pData, 0, 1, &cChar, 0);
};

inline CSLString::CSLString(LPCTSTR const cpcInitStr, const ULONG ulcInitStrLen)
{
  const ULONG ulcStrLen = (ULONG_MAX == ulcInitStrLen)
    ? ::lstrlen(cpcInitStr)
    : ulcInitStrLen;
  pData = __AllocBuff(ulcStrLen);
  __CopyBuffAndEnd(pData, 0, ulcStrLen, cpcInitStr, 0);
};

inline CSLString::CSLString(const CSLString& rInitStr)
  :pData(rInitStr.pData)
{
  pData->ulRefCount++;
};

inline CSLString::~CSLString()
{
  __FreeBuff(pData);
  pData = 0;
};

inline CSLString::__CBuffHdr* CSLString::GetNull()
{
  return reinterpret_cast<__CBuffHdr*>(NullData);
};

inline void CSLString::__CopyBuffAndEnd(__CBuffHdr* const cpTo, const ULONG ulcToPos, const ULONG ulcToLen, LPCTSTR const cpcFrom, const ULONG ulcFromPos)
{
  _ASSERT(cpTo->ulBuffLen > ulcToPos + ulcToLen);
  CopyMemory(cpTo->GetStr() + ulcToPos, cpcFrom + ulcFromPos, ulcToLen * eccUseStrSize);
  const ULONG ulcWillEnd = ulcToPos + ulcToLen;
  cpTo->ulStrLen = ulcWillEnd;
  cpTo->GetStr()[cpTo->ulStrLen] = '\0';
};

inline void CSLString::__CopyBuff(__CBuffHdr* const cpTo, const ULONG ulcToPos, const ULONG ulcToLen, LPCTSTR const cpcFrom, const ULONG ulcFromPos)
{
  _ASSERT(cpTo->ulBuffLen > ulcToPos + ulcToLen);
  CopyMemory(cpTo->GetStr() + ulcToPos, cpcFrom + ulcFromPos, ulcToLen * eccUseStrSize);
  const ULONG ulcWillEnd = ulcToPos + ulcToLen;
  if(ulcWillEnd > cpTo->ulStrLen)
  {
    cpTo->ulStrLen = ulcWillEnd;
    cpTo->GetStr()[cpTo->ulStrLen] = '\0';
  };
};

inline CSLString::__CBuffHdr* CSLString::__ReallocBuff(const ULONG ulcWantSize, const bool bcCopyData)
{
  __CBuffHdr* pRes = pData;
  if(1 < pData->ulRefCount || ulcWantSize >= pData->ulBuffLen)
  {
    pRes = __AllocBuff(ulcWantSize);
    //copy 
    if(false != bcCopyData)
    {
      __CopyBuff(pRes, 0, pData->ulStrLen, pData->GetStr(), 0);
    };
    __FreeBuff(pData);
    pData = pRes;
  };
  return pRes;
};

inline CSLString::__CBuffHdr* CSLString::__AllocBuff(const ULONG ulcWantSize)
{
  const ULONG ulcUseSize = SLAlignUp(ulcWantSize + 1, eccBuffGrowAlign);
  __CBuffHdr* const cpRes = reinterpret_cast<__CBuffHdr*>(new BYTE[sizeof(__CBuffHdr) + ulcUseSize * eccUseStrSize]);
  cpRes->ulRefCount = 1;
  cpRes->ulStrLen = 0;
  cpRes->ulBuffLen = ulcUseSize;
  return cpRes;
};

inline ULONG CSLString::GetLen() const
{
  return pData->ulStrLen;
};

inline LPCTSTR CSLString::GetPtr() const
{
  return pData->GetStr();
};

inline CSLString::operator LPCTSTR() const
{
  return pData->GetStr();
};

inline bool CSLString::IsEmpty() const
{
  return 0 == pData->ulStrLen;
};

inline void CSLString::__FreeBuff(__CBuffHdr* const cpBuff)
{
  if(1 == cpBuff->ulRefCount && GetNull() != cpBuff)
  {
    delete cpBuff;
  };
  cpBuff->ulRefCount--;
};

inline CSLString& CSLString::operator=(const CSLString& rAssignStr)
{
  __FreeBuff(pData);
  pData = rAssignStr.pData;
  pData->ulRefCount++;
  return *this;
};

inline CSLString& CSLString::operator=(const TCHAR cChar)
{
  pData = __ReallocBuff(1, false);
  __CopyBuffAndEnd(pData, 0, 1, &cChar, 0);
  return *this;
};

inline CSLString& CSLString::operator=(LPCTSTR const cpcAssignStr)
{
  const ULONG ulcStrSize = ::lstrlen(cpcAssignStr);
  pData = __ReallocBuff(ulcStrSize, false);
  __CopyBuffAndEnd(pData, 0, ulcStrSize, cpcAssignStr, 0);
  return *this;
};

inline CSLString& CSLString::operator+=(const CSLString& rAssignStr)
{
  pData = __ReallocBuff(pData->ulStrLen + rAssignStr.pData->ulStrLen, true);
  __CopyBuffAndEnd(pData, pData->ulStrLen, rAssignStr.pData->ulStrLen, rAssignStr.pData->GetStr(), 0);
  return *this;
};

inline CSLString& CSLString::operator+=(LPCTSTR const cpcAssignStr)
{
  const ULONG ulcStrSize = ::lstrlen(cpcAssignStr);
  pData = __ReallocBuff(pData->ulStrLen + ulcStrSize, true);
  __CopyBuffAndEnd(pData, pData->ulStrLen, ulcStrSize, cpcAssignStr, 0);
  return *this;
};

inline LPTSTR CSLString::GetBuff(const ULONG ulcWaitSize)
{
  pData = __ReallocBuff(ulcWaitSize, true);
  pData->ulStrLen = ulcWaitSize - 1;
  return pData->GetStr();
};

inline bool CSLString::LoadString(const UINT uicResId, HINSTANCE hInst)
{
  bool bRes = false;

  ULONG ulResUseLen = eccResLoadStartLen;
  while(true)
  {
    pData = __ReallocBuff(ulResUseLen, false);
    const ULONG ulcLoadRes = ::LoadString(hInst, uicResId, pData->GetStr(), pData->ulBuffLen - 1);
    if(0 == ulcLoadRes)
    {
      //str not find
      break;
    };
    if(ulcLoadRes < pData->ulBuffLen - 1)
    {
      //load ok
      pData->GetStr()[ulcLoadRes] = '\0';
      AjustLen(ulcLoadRes);
      bRes = true;
      break;
    };
    ulResUseLen += eccResLoadAddLen;
  };

  return bRes;
};

inline void CSLString::Clear()
{
  if(GetNull() != pData)
  {
    if(1 < pData->ulRefCount)
    {
      __FreeBuff(pData);
      pData = GetNull();
      pData->ulRefCount++;
    }
    else
    {
      pData->ulStrLen = 0;
      *pData->GetStr() = '\0';
    };
  };
};

inline TCHAR CSLString::At(const ULONG ulcPos) const
{
  _ASSERT(ulcPos < pData->ulStrLen);
  return pData->GetStr()[ulcPos];
};

inline void CSLString::AjustLen()
{
  pData->ulStrLen = ::lstrlen(pData->GetStr());
};

inline void CSLString::AjustLen(const ULONG ulcWantLen)
{
  //to big string len
  _ASSERT(pData->ulBuffLen > ulcWantLen);
  pData->ulStrLen = ulcWantLen;
};


_SL_END
#endif//__SLSTRING_H
