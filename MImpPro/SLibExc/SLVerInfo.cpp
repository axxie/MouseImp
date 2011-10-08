/**********************************************************************
Version information getter
**********************************************************************/

#define STRICT
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#pragma hdrstop


#include <tchar.h>


#include "SLVerInfo.h"


//for version service
#pragma comment(lib, "VERSION.LIB")

_SL_BEG

//////////////////////////////////////////////////////////////////////
//CSLVerInfo

bool CSLVerInfo::QueryValue(LPCTSTR const cpcSecName, BYTE** ppBuff, DWORD* const cpdwBuffSize)
{
  bool bRes = false;

  //not inited
  SLPTRRD(pVerData);

  //make block name
  TCHAR BlockName[64];
  wsprintf(BlockName, _T("\\StringFileInfo\\%08lx\\%s\0"), dwLangId, cpcSecName);
  UINT uiBlockSize = 0;
  if(0 != ::VerQueryValue(pVerData, BlockName, reinterpret_cast<VOID**>(ppBuff), &uiBlockSize))
  {
    *cpdwBuffSize = uiBlockSize;
    bRes = true;
  };

  return bRes;
};

bool CSLVerInfo::Open(LPCTSTR const cpcFileName, const DWORD dwcWantLanId)
{
  bool bRes = false;

  Close();

  DWORD dwDummy = 0;
  const DWORD dwcVerSize = ::GetFileVersionInfoSize(const_cast<LPTSTR>(cpcFileName), &dwDummy);
  if(0 != dwcVerSize)
  {
    pVerData = new BYTE[dwcVerSize];
    if(0 != ::GetFileVersionInfo(const_cast<LPTSTR>(cpcFileName), dwDummy, dwcVerSize, pVerData))
    {
      DWORD* pTransTable = 0;
      UINT uiTableSize = 0;
      if(FALSE != ::VerQueryValue(pVerData, _T("\\VarFileInfo\\Translation"), reinterpret_cast<LPVOID*>(&pTransTable), &uiTableSize))
      {
        //set to first fonr char set
        dwLangId = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));
        //try find passed LangId
        if(0 != dwcWantLanId)
        {
          for(UINT uiCount = 0; uiTableSize > uiCount; uiCount++)
          {
            const DWORD dwCurrLang = MAKELONG(HIWORD(pTransTable[uiCount]), LOWORD(pTransTable[uiCount]));
            if(dwCurrLang == dwcWantLanId)
            {
              //some present - break cycle
              dwLangId = dwCurrLang;
              break;
            };
          };
        };
        bRes = true;
      };
    };
  };
  return bRes;
};

bool CSLVerInfo::GetFixedInfo(VS_FIXEDFILEINFO** const cppInfoAddr)
{
  //not inited
  SLPTRRD(pVerData);

  bool bRes = false;

  UINT uiSize = 0;
  *cppInfoAddr = 0;
  if(0 != ::VerQueryValue(pVerData, _T("\\"), reinterpret_cast<VOID**>(cppInfoAddr), &uiSize))
  {
    bRes = true;
  };

  return bRes;
}

LPCTSTR CSLVerInfo::GetFileDescription()
{
  return QueryStringValue(_T("FileDescription"));
};

LPCTSTR CSLVerInfo::GetFileVersion()
{
  return QueryStringValue(_T("FileVersion"));
};

LPCTSTR CSLVerInfo::GetInternalName()
{
  return QueryStringValue(_T("InternalName"));
};

LPCTSTR CSLVerInfo::GetCompanyName()
{
  return QueryStringValue(_T("CompanyName"));
};

LPCTSTR CSLVerInfo::GetLegalCopyright()
{
  return QueryStringValue(_T("LegalCopyright"));
};

LPCTSTR CSLVerInfo::GetOriginalFilename()
{
  return QueryStringValue(_T("OriginalFilename"));
};

LPCTSTR CSLVerInfo::GetProductName()
{
  return QueryStringValue(_T("ProductName"));
};

LPCTSTR CSLVerInfo::GetProductVersion()
{
  return QueryStringValue(_T("ProductVersion"));
};

_SL_END
