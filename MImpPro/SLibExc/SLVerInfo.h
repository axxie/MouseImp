/**********************************************************************
Version information getter
**********************************************************************/

#ifndef __SLVERINFO_H
#define __SLVERINFO_H

#pragma once


#include "SLUtil.h"

#include "SLDbg.h"


_SL_BEG 

//////////////////////////////////////////////////////////////////////
//CSLVerInfo
//
//getter for version info from modules (VERSIONINFO in resources)
//
//Note : automaticaly attach "VERSION.DLL" (look #pragma comment(lib, "VERSION.LIB") in source)
//
//Example : look constructor of CSLCfgRegBase in SLCfgStr.cpp

class CSLVerInfo
{ 
protected:

  //buffer for VERSIONINFO data
  BYTE* pVerData; 
  //matched CharSet
  DWORD dwLangId;

public:

public:

  //construct empty
  inline CSLVerInfo();
  //construct and get VerInfo for passed module file name
  inline CSLVerInfo(LPCTSTR const cpcFileName, const DWORD dwcWantLagId = 0);
  //construct and get VerInfo for passed module instance (AfxGetInstanceHandle() for MFC)
  inline CSLVerInfo(HMODULE hModule, const DWORD dwcWantLagId = 0);
  //destruct and free attached VerInfo data
  inline ~CSLVerInfo();

  //get in ppBuff addr of buffer and it's size in cpdwBuffSize for cpcSecName section of VerInfo
  bool QueryValue(LPCTSTR const cpcSecName, BYTE** ppBuff, DWORD* const cpdwBuffSize);
  //get cpcSecName section content like LPCTSTR string
  inline LPCTSTR QueryStringValue(LPCTSTR const cpcSecName);

  //return in cppInfoAddr "fixed file info" like VS_FIXEDFILEINFO (permanent part of VerInfo)
  bool GetFixedInfo(VS_FIXEDFILEINFO** const cppInfoAddr);
  //get FileVersion from "fixed file info" like DWORD 
  inline DWORD GetFixedFileVersion();
  //get Product version from "fixed file info" like it part's
  inline DWORD GetProdVersionHiHi();
  inline DWORD GetProdVersionHiLow();
  inline DWORD GetProdVersionLowHi();
  inline DWORD GetProdVersionLowLow();
  //get file version from "fixed file info" like it part's
  inline DWORD GetFileVersionHiHi();
  inline DWORD GetFileVersionHiLow();
  inline DWORD GetFileVersionLowHi();
  inline DWORD GetFileVersionLowLow();

  //open VerInfo for passed module name
  bool Open(LPCTSTR const cpcFileName, const DWORD dwcWantLagId = 0);
  //open VerInfo for for passed module instance (AfxGetInstanceHandle() for MFC)
  inline bool Open(HMODULE hModule, const DWORD dwcWantLagId = 0);
  //close and free VerInfo buffer's
  inline void Close();

  //is Version info data present (for 
  inline bool IsOk() const;

  //Get predefined values from VerInfo (look func body's)
  LPCTSTR GetFileDescription();
  LPCTSTR GetFileVersion();
  LPCTSTR GetInternalName();
  LPCTSTR GetCompanyName();
  LPCTSTR GetLegalCopyright();
  LPCTSTR GetOriginalFilename();
  LPCTSTR GetProductName();
  LPCTSTR GetProductVersion();

protected:
}; 


/**********************************************************************
implementation
**********************************************************************/

inline CSLVerInfo::CSLVerInfo()
  :pVerData(0), 
  dwLangId(0)
{
};

inline CSLVerInfo::CSLVerInfo(HMODULE hModule, const DWORD dwcWantLagId)
  :pVerData(0), 
  dwLangId(0)
{
  SLCHECK(false != Open(hModule, dwcWantLagId));
};

inline CSLVerInfo::CSLVerInfo(LPCTSTR const cpcFileName, const DWORD dwcWantLagId)
  :pVerData(0), 
  dwLangId(0)
{
  SLCHECK(false != Open(cpcFileName, dwcWantLagId));
};

inline CSLVerInfo::~CSLVerInfo()
{
  Close();
};

inline bool CSLVerInfo::Open(HMODULE hModule, const DWORD dwcWantLagId)
{
  bool bRes = false;
  TCHAR Buff[MAX_PATH];
  if
    (
    0 != GetModuleFileName(hModule, Buff, sizeof(Buff) / sizeof(Buff[0]))
    && false != Open(Buff, dwcWantLagId)
    )
  {
    bRes = true;
  };
  return bRes;
};

inline void CSLVerInfo::Close()
{
  delete pVerData;
  pVerData = 0;
};


inline DWORD CSLVerInfo::GetFixedFileVersion()
{
  DWORD dwRes = 0;
  //not inited
  SLPTRRD(pVerData);

  VS_FIXEDFILEINFO* pFixedInfo = 0;
  if(false != GetFixedInfo(&pFixedInfo))
  {
    dwRes = MAKELONG(pFixedInfo->dwFileVersionLS, pFixedInfo->dwFileVersionMS);
  };

  return dwRes;
};

inline DWORD CSLVerInfo::GetProdVersionHiHi()
{
  DWORD dwRes = 0;

  VS_FIXEDFILEINFO* pFixedInfo = 0;
  if(false != GetFixedInfo(&pFixedInfo))
  {
    dwRes = HIWORD(pFixedInfo->dwProductVersionMS);
  };

  return dwRes;
};

inline DWORD CSLVerInfo::GetProdVersionHiLow()
{
  DWORD dwRes = 0;

  VS_FIXEDFILEINFO* pFixedInfo = 0;
  if(false != GetFixedInfo(&pFixedInfo))
  {
    dwRes = LOWORD(pFixedInfo->dwProductVersionMS);
  };

  return dwRes;
};

inline DWORD CSLVerInfo::GetProdVersionLowHi()
{
  DWORD dwRes = 0;

  VS_FIXEDFILEINFO* pFixedInfo = 0;
  if(false != GetFixedInfo(&pFixedInfo))
  {
    dwRes = HIWORD(pFixedInfo->dwProductVersionLS);
  };

  return dwRes;
};

inline DWORD CSLVerInfo::GetProdVersionLowLow()
{
  DWORD dwRes = 0;

  VS_FIXEDFILEINFO* pFixedInfo = 0;
  if(false != GetFixedInfo(&pFixedInfo))
  {
    dwRes = LOWORD(pFixedInfo->dwProductVersionLS);
  };

  return dwRes;
};

inline DWORD CSLVerInfo::GetFileVersionHiHi()
{
  DWORD dwRes = 0;

  VS_FIXEDFILEINFO* pFixedInfo = 0;
  if(false != GetFixedInfo(&pFixedInfo))
  {
    dwRes = HIWORD(pFixedInfo->dwFileVersionMS);
  };

  return dwRes;
};

inline DWORD CSLVerInfo::GetFileVersionHiLow()
{
  DWORD dwRes = 0;

  VS_FIXEDFILEINFO* pFixedInfo = 0;
  if(false != GetFixedInfo(&pFixedInfo))
  {
    dwRes = LOWORD(pFixedInfo->dwFileVersionMS);
  };

  return dwRes;
};

inline DWORD CSLVerInfo::GetFileVersionLowHi()
{
  DWORD dwRes = 0;

  VS_FIXEDFILEINFO* pFixedInfo = 0;
  if(false != GetFixedInfo(&pFixedInfo))
  {
    dwRes = HIWORD(pFixedInfo->dwFileVersionLS);
  };

  return dwRes;
};

inline DWORD CSLVerInfo::GetFileVersionLowLow()
{
  DWORD dwRes = 0;

  VS_FIXEDFILEINFO* pFixedInfo = 0;
  if(false != GetFixedInfo(&pFixedInfo))
  {
    dwRes = LOWORD(pFixedInfo->dwFileVersionLS);
  };

  return dwRes;
};

inline LPCTSTR CSLVerInfo::QueryStringValue(LPCTSTR const cpcSecName)
{
  BYTE* pBuff = 0;
  LPCTSTR pcRes = 0;
  DWORD dwBuffSize = 0;
  if(false != QueryValue(cpcSecName, &pBuff, &dwBuffSize))
  {
    pcRes = reinterpret_cast<LPCTSTR>(pBuff);
  };
  return pcRes;
};


_SL_END

#endif  // __SLVERINFO_H
