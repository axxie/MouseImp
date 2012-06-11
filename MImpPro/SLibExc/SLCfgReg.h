/**********************************************************************
registry cfg store/restore class
**********************************************************************/
#ifndef __SLCFGREG_H
#define __SLCFGREG_H

#pragma once


#include "SLUtil.h"

#include "SLDbg.h"


_SL_BEG

//////////////////////////////////////////////////////////////////////
//CSLCfgReg

class CSLCfgReg
{
protected:

  //registry key
  HKEY hKey;
  
public:
  
public:

  //construct
  inline CSLCfgReg();
  inline CSLCfgReg(LPCTSTR const cpcKeyName, HKEY hFromKey);
  inline CSLCfgReg(LPCTSTR const cpcKeyName, const bool bcPerUser);
  //destruct
  inline ~CSLCfgReg();

  //open
  //rbIsCreate used as "IsRegEntryCreated" flag - if error present - this value set to true
  inline bool Open(LPCTSTR const cpcKeyName, HKEY hFromKey);
  inline bool Open(LPCTSTR const cpcKeyName, HKEY hFromKey, bool& rbIsCreate);
  inline bool Open(LPCTSTR const cpcKeyName, const bool bcPerUser);
  inline bool Open(LPCTSTR const cpcKeyName, const bool bcPerUser, bool& rbIsCreate);
  //close
  inline void Close();
  //is key open
  inline bool IsOk() const;
  //delete value
  inline bool DelVal(LPCTSTR const cpcEntryName) const;
  //read binary buffer 
  inline bool ReadBuff(LPCTSTR const cpcEntryName, BYTE* const cpBuff, const DWORD dwcLen) const;
  inline bool ReadBuff(LPCTSTR const cpcEntryName, void* const cpBuff, const DWORD dwcLen) const;
  //write buffer
  inline bool WriteBuff(LPCTSTR const cpcEntryName, const BYTE* const cpBuff, const DWORD dwcLen) const;
  inline bool WriteBuff(LPCTSTR const cpcEntryName, const void* const cpBuff, const DWORD dwcLen) const;
  //read/write string
  inline bool ReadStr(LPCTSTR const cpcEntryName, LPTSTR const cpBuff, const DWORD dwcBuffLen) const;
  inline bool WriteStr(LPCTSTR const cpcEntryName, LPCTSTR const cpBuff, const DWORD dwcBuffLen = 0) const;
  //get size of value
  inline bool GetValSize(LPCTSTR const cpcEntryName, DWORD& rdwLen) const;
  //is value present in registry
  inline bool IsPresent(LPCTSTR const cpcEntryName) const;

  //get val's
  inline void GetVal(LPCTSTR const cpcEntryName, bool& rbVal, const bool bcDefVal) const;
  inline void GetVal(LPCTSTR const cpcEntryName, LONGLONG& rllVal, const LONGLONG llDefVal) const;
  inline void GetVal(LPCTSTR const cpcEntryName, DWORD& rdwVal, const DWORD dwcDefVal) const;
  inline void GetVal(LPCTSTR const cpcEntryName, LONG& rlVal, const LONG lcDefVal) const;
  inline void GetVal(LPCTSTR const cpcEntryName, int& riVal, const int icDefVal) const;
  inline void GetVal(LPCTSTR const cpcEntryName, UINT& riVal, const UINT uicDefVal) const;
  inline void GetVal(LPCTSTR const cpcEntryName, LPTSTR const cpBuff, const DWORD dwcBuffLen, LPCTSTR const cpcDefVal);

  //put val's
  inline bool PutVal(LPCTSTR const cpcEntryName, const bool bcVal) const;
  inline bool PutVal(LPCTSTR const cpcEntryName, const LONGLONG llcVal) const;
  inline bool PutVal(LPCTSTR const cpcEntryName, const DWORD dwcVal) const;
  inline bool PutVal(LPCTSTR const cpcEntryName, const int icVal) const;
  inline bool PutVal(LPCTSTR const cpcEntryName, const UINT uicVal) const;
  inline bool PutVal(LPCTSTR const cpcEntryName, LPCTSTR const cpcVal, const DWORD dwcValLen) const;

  //for pass to API func's
  inline operator HKEY() const;
  
protected:
  //check entry for passed type (or if entry not exist - return true)
  inline bool CheckEntryType(LPCTSTR const cpcEntryName, const DWORD dwcType) const;
};


//////////////////////////////////////////////////////////////////////
//CSLCfgRegEnum

class CSLCfgRegEnum
{
protected:

  ////controlled key
  HKEY hKey;

  ////current iterate number
  DWORD dwCurrIndex;

  ////name of looked key 
  TCHAR cpName[MAX_PATH];
  
public:
  
public:

  inline CSLCfgRegEnum(HKEY hInitKey);

  ////iteration
  inline void Open();
  inline void Next();
  inline bool IsOk() const;
  inline LPCTSTR GetName() const;
  
protected:
  ////goto next val
  inline void InternalNext();
};





/**********************************************************************
implementation
**********************************************************************/

//////////////////////////////////////////////////////////////////////
//CSLCfgReg

inline CSLCfgReg::CSLCfgReg()
  :hKey(0)
{
};

inline CSLCfgReg::CSLCfgReg(LPCTSTR const cpcKeyName, HKEY hFromKey)
  :hKey(0)
{
  SLCHECK(false != Open(cpcKeyName, hFromKey));
};

inline CSLCfgReg::CSLCfgReg(LPCTSTR const cpcKeyName, const bool bcPerUser)
  :hKey(0)
{
  SLCHECK(false != Open(cpcKeyName, bcPerUser));
};

inline CSLCfgReg::~CSLCfgReg()
{
  Close();
};

inline bool CSLCfgReg::Open(LPCTSTR const cpcKeyName, HKEY hFromKey, bool& rbIsCreate)
{
  //close 
  //try open

  bool bRes = false;

  //close
  Close();
  //open
  DWORD dwIsCreate = 0;
  //if error - treat key as "created" 
  rbIsCreate = true;
  if(ERROR_SUCCESS == ::RegCreateKeyEx(hFromKey, cpcKeyName, 0, 0, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &hKey, &dwIsCreate))
  {
    bRes = true;
    rbIsCreate = REG_CREATED_NEW_KEY == dwIsCreate;
  };  

  return bRes;
};
  

inline bool CSLCfgReg::Open(LPCTSTR const cpcKeyName, const bool bcPerUser, bool& rbIsCreate)
{
  return Open(cpcKeyName, (false != bcPerUser) ? HKEY_CURRENT_USER : HKEY_LOCAL_MACHINE, rbIsCreate);
};

inline bool CSLCfgReg::Open(LPCTSTR const cpcKeyName, HKEY hFromKey)
{
  bool bIsCreate = false;
  return Open(cpcKeyName, hFromKey, bIsCreate);
};

inline bool CSLCfgReg::Open(LPCTSTR const cpcKeyName, const bool bcPerUser)
{
  bool bIsCreate = false;
  return Open(cpcKeyName, (false != bcPerUser) ? HKEY_CURRENT_USER : HKEY_LOCAL_MACHINE, bIsCreate);
};

inline void CSLCfgReg::Close()
{
  if(0 != hKey)
  {
    SLCHECK(ERROR_SUCCESS == ::RegCloseKey(hKey));
    hKey = 0;
  };
};

inline bool CSLCfgReg::IsOk() const
{
  return 0 != hKey;
};

inline void CSLCfgReg::GetVal(LPCTSTR const cpcEntryName, bool& rbVal, const bool bcDefVal) const
{
  _ASSERT(false != IsOk());
  _ASSERT(false != CheckEntryType(cpcEntryName, REG_BINARY));
  DWORD dwSize = sizeof(rbVal);
  if(ERROR_SUCCESS != ::RegQueryValueEx(hKey, cpcEntryName, 0, 0, reinterpret_cast<BYTE*>(&rbVal), &dwSize))
  {
    rbVal = bcDefVal;
  };
};

inline void CSLCfgReg::GetVal(LPCTSTR const cpcEntryName, int& riVal, const int icDefVal) const
{
  _ASSERT(false != IsOk());
  _ASSERT(false != CheckEntryType(cpcEntryName, REG_DWORD));
  DWORD dwSize = sizeof(riVal);
  if(ERROR_SUCCESS != ::RegQueryValueEx(hKey, cpcEntryName, 0, 0, reinterpret_cast<BYTE*>(&riVal), &dwSize))
  {
    riVal = icDefVal;
  };
};

inline void CSLCfgReg::GetVal(LPCTSTR const cpcEntryName, LONGLONG& rllVal, const LONGLONG llDefVal) const
{
  _ASSERT(false != IsOk());
  _ASSERT(false != CheckEntryType(cpcEntryName, REG_BINARY));
  DWORD dwSize = sizeof(rllVal);
  if(ERROR_SUCCESS != ::RegQueryValueEx(hKey, cpcEntryName, 0, 0, reinterpret_cast<BYTE*>(&rllVal), &dwSize))
  {
    rllVal = llDefVal;
  };
};

inline void CSLCfgReg::GetVal(LPCTSTR const cpcEntryName, UINT& riVal, const UINT uicDefVal) const
{
  _ASSERT(false != IsOk());
  _ASSERT(false != CheckEntryType(cpcEntryName, REG_DWORD));
  DWORD dwSize = sizeof(riVal);
  if(ERROR_SUCCESS != ::RegQueryValueEx(hKey, cpcEntryName, 0, 0, reinterpret_cast<BYTE*>(&riVal), &dwSize))
  {
    riVal = uicDefVal;
  };
};

inline void CSLCfgReg::GetVal(LPCTSTR const cpcEntryName, DWORD& rdwVal, const DWORD dwcDefVal) const
{
  _ASSERT(false != IsOk());
  _ASSERT(false != CheckEntryType(cpcEntryName, REG_DWORD));
  DWORD dwSize = sizeof(rdwVal);
  if(ERROR_SUCCESS != ::RegQueryValueEx(hKey, cpcEntryName, 0, 0, reinterpret_cast<BYTE*>(&rdwVal), &dwSize))
  {
    rdwVal = dwcDefVal;
  };
};

inline void CSLCfgReg::GetVal(LPCTSTR const cpcEntryName, LONG& rlVal, const LONG lcDefVal) const
{
  _ASSERT(false != IsOk());
  _ASSERT(false != CheckEntryType(cpcEntryName, REG_DWORD));
  DWORD dwSize = sizeof(rlVal);
  if(ERROR_SUCCESS != ::RegQueryValueEx(hKey, cpcEntryName, 0, 0, reinterpret_cast<BYTE*>(&rlVal), &dwSize))
  {
    rlVal = lcDefVal;
  };
};

inline void CSLCfgReg::GetVal(LPCTSTR const cpcEntryName, LPTSTR const cpBuff, const DWORD dwcBuffLen, LPCTSTR const cpcDefVal)
{
  if(false == ReadStr(cpcEntryName, cpBuff, dwcBuffLen))
  {
    ::lstrcpyn(cpBuff, (0 == cpcDefVal) ? "" : cpcDefVal, dwcBuffLen);
  };
};

inline bool CSLCfgReg::PutVal(LPCTSTR const cpcEntryName, const bool bcVal) const
{
  _ASSERT(false != IsOk());
  return 
    ERROR_SUCCESS == ::RegSetValueEx(hKey, cpcEntryName, 0, REG_BINARY, reinterpret_cast<const BYTE*>(&bcVal), sizeof(bcVal));
};

inline bool CSLCfgReg::PutVal(LPCTSTR const cpcEntryName, const UINT uicVal) const
{
  _ASSERT(false != IsOk());
  return 
    ERROR_SUCCESS == ::RegSetValueEx(hKey, cpcEntryName, 0, REG_DWORD, reinterpret_cast<const BYTE*>(&uicVal), sizeof(uicVal));
};

inline bool CSLCfgReg::PutVal(LPCTSTR const cpcEntryName, const int icVal) const
{
  _ASSERT(false != IsOk());
  return 
    ERROR_SUCCESS == ::RegSetValueEx(hKey, cpcEntryName, 0, REG_DWORD, reinterpret_cast<const BYTE*>(&icVal), sizeof(icVal));
};

inline bool CSLCfgReg::PutVal(LPCTSTR const cpcEntryName, const LONGLONG llcVal) const
{
  _ASSERT(false != IsOk());
  return 
    ERROR_SUCCESS == ::RegSetValueEx(hKey, cpcEntryName, 0, REG_BINARY, reinterpret_cast<const BYTE*>(&llcVal), sizeof(llcVal));
};

inline bool CSLCfgReg::PutVal(LPCTSTR const cpcEntryName, const DWORD dwcVal) const
{
  _ASSERT(false != IsOk());
  return 
    ERROR_SUCCESS == ::RegSetValueEx(hKey, cpcEntryName, 0, REG_DWORD, reinterpret_cast<const BYTE*>(&dwcVal), sizeof(dwcVal));
};

inline bool CSLCfgReg::PutVal(LPCTSTR const cpcEntryName, LPCTSTR const cpcVal, const DWORD dwcValLen) const
{
  return WriteStr(cpcEntryName, cpcVal, dwcValLen);
};

inline bool CSLCfgReg::CheckEntryType(LPCTSTR const cpcEntryName, const DWORD dwcType) const
{
  //get type
  //compare

  bool bRes = true;
  DWORD dwType = 0;
  //get
  if(ERROR_SUCCESS == ::RegQueryValueEx(hKey, cpcEntryName, 0, &dwType, 0, 0))
  {
    bRes = dwcType == dwType;
  };
  return bRes;
};

inline bool CSLCfgReg::DelVal(LPCTSTR const cpcEntryName) const
{
  _ASSERT(false != IsOk());
  return 
    ERROR_SUCCESS == ::RegDeleteValue(hKey, cpcEntryName);
};

inline bool CSLCfgReg::ReadBuff(LPCTSTR const cpcEntryName, void* const cpBuff, const DWORD dwcLen) const
{
  return ReadBuff(cpcEntryName, reinterpret_cast<BYTE*>(cpBuff), dwcLen);
};

inline bool CSLCfgReg::ReadBuff(LPCTSTR const cpcEntryName, BYTE* const cpBuff, const DWORD dwcLen) const
{
  _ASSERT(false != IsOk());
  _ASSERT(false != CheckEntryType(cpcEntryName, REG_BINARY));
  DWORD dwReadLen = dwcLen;
  return 
    ERROR_SUCCESS == ::RegQueryValueEx(hKey, cpcEntryName, 0, 0, cpBuff, &dwReadLen);
};

inline bool CSLCfgReg::WriteBuff(LPCTSTR const cpcEntryName, const void* const cpBuff, const DWORD dwcLen) const
{
  return WriteBuff(cpcEntryName, reinterpret_cast<const BYTE*>(cpBuff), dwcLen);
};

inline bool CSLCfgReg::WriteBuff(LPCTSTR const cpcEntryName, const BYTE* const cpBuff, const DWORD dwcLen) const
{
  _ASSERT(false != IsOk());
  _ASSERT(false != CheckEntryType(cpcEntryName, REG_BINARY));
  return ERROR_SUCCESS == ::RegSetValueEx(hKey, cpcEntryName, 0, REG_BINARY, cpBuff, dwcLen);
};

inline bool CSLCfgReg::ReadStr(LPCTSTR const cpcEntryName, LPTSTR const cpBuff, const DWORD dwcBuffLen) const
{
  _ASSERT(false != IsOk());
  _ASSERT(false != CheckEntryType(cpcEntryName, REG_SZ));
  DWORD dwLen = dwcBuffLen;
  return 
    ERROR_SUCCESS == ::RegQueryValueEx(hKey, cpcEntryName, 0, 0, reinterpret_cast<BYTE*>(cpBuff), &dwLen);
};

inline bool CSLCfgReg::WriteStr(LPCTSTR const cpcEntryName, LPCTSTR const cpBuff, const DWORD dwcBuffLen) const
{
  _ASSERT(false != IsOk());

  //size for string
  const DWORD dwcLen = (static_cast<DWORD>(-1) == dwcBuffLen || 0 == dwcBuffLen)
    ? ::lstrlen(cpBuff) + 1
    : dwcBuffLen;
  return ERROR_SUCCESS == ::RegSetValueEx(hKey, cpcEntryName, 0, REG_SZ, reinterpret_cast<const BYTE*>(cpBuff), dwcLen);
};

inline bool CSLCfgReg::IsPresent(LPCTSTR const cpcEntryName) const
{
  bool bRes = false;
  _ASSERT(false != IsOk());
  DWORD dwLen = 0;
  if
    (
    ERROR_SUCCESS == ::RegQueryValueEx(hKey, cpcEntryName, 0, 0, 0, &dwLen)
    && 0 < dwLen
    )
  {
    bRes = true;
  };
  return bRes;
};

inline bool CSLCfgReg::GetValSize(LPCTSTR const cpcEntryName, DWORD& rdwLen) const
{
  _ASSERT(false != IsOk());
  return ERROR_SUCCESS == ::RegQueryValueEx(hKey, cpcEntryName, 0, 0, 0, &rdwLen);
};

inline CSLCfgReg::operator HKEY() const
{
  return hKey;
};

//////////////////////////////////////////////////////////////////////
//CSLCfgRegEnum

inline CSLCfgRegEnum::CSLCfgRegEnum(HKEY hInitKey)
  :dwCurrIndex(ULONG_MAX),
  hKey(hInitKey)
{
};

inline void CSLCfgRegEnum::Open()
{
  dwCurrIndex = 0;
  InternalNext();
};

inline void CSLCfgRegEnum::Next()
{
  InternalNext();
};

inline bool CSLCfgRegEnum::IsOk() const
{
  return ULONG_MAX != dwCurrIndex;
};

inline LPCTSTR CSLCfgRegEnum::GetName() const
{
  _ASSERT(false != IsOk());
  return cpName;
};

inline void CSLCfgRegEnum::InternalNext()
{
  FILETIME LastTime;
  DWORD dwNameSize = ARRAYSIZE(cpName);
  const LONG lcRes = ::RegEnumKeyEx
    (
    hKey,
    dwCurrIndex,
    cpName,
    &dwNameSize,
    0,
    0,
    0,
    &LastTime
    );
  dwCurrIndex = (ERROR_SUCCESS == lcRes)
    ? ULONG_MAX
    : dwCurrIndex + 1;
};


_SL_END

#endif//__SLCFGREG_H