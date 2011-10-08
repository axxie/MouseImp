/**********************************************************************
ConfigStream implementation
**********************************************************************/

#define STRICT
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#pragma hdrstop


#include <tchar.h>


#include "SLCfgStr.h"


#include "SLVerInfo.h"


#pragma comment(lib, "advapi32.lib")


_SL_BEG

////////////////////////////////////////////////////////////////////////////////
//CSLCfgRegBase

CSLCfgRegBase::CSLCfgRegBase(LPCTSTR const cpcSecName, const bool bcPerUser, const bool bcForWrite, HMODULE hModule)
  :hKey(0)
{
  CSLVerInfo Info(hModule);
  LPCTSTR const cpcVerCompName = Info.GetCompanyName();
  LPCTSTR const cpcVerSoftName = Info.GetProductName();

  LPCTSTR const cpcCompName = (0 != cpcVerCompName && 0 != _tcslen(cpcVerCompName)) 
    ? cpcVerCompName 
    : _T("Underground");
  LPCTSTR const cpcSoftName = (0 != cpcVerSoftName && 0 != _tcslen(cpcVerSoftName)) 
    ? cpcVerSoftName 
    : _T("SomeApp");

  CHAR Buff[64];
  if(0 != cpcSecName && 0 != _tcslen(cpcSecName))
  {
    wsprintf(Buff, _T("SOFTWARE\\%s\\%s\\%s\0"), cpcCompName, cpcSoftName, cpcSecName);
  }
  else
  {
    wsprintf(Buff, _T("SOFTWARE\\%s\\%s\0"), cpcCompName, cpcSoftName);
  };
  Open((false != bcPerUser) ? HKEY_CURRENT_USER : HKEY_LOCAL_MACHINE, Buff, bcForWrite);
};

bool CSLCfgRegBase::ReadSubKey(LPCTSTR const cpKeyName, LPTSTR const cpBuff, DWORD& rdwBuffSize)
{
  bool bRes = false;

  cpBuff[0] = '\0';

  DWORD dwType = 0;
  DWORD dwResSize = rdwBuffSize * sizeof(*cpBuff);
  if(0 != hKey && ERROR_SUCCESS == RegQueryValueEx(hKey, cpKeyName, 0, &dwType, reinterpret_cast<PBYTE>(cpBuff), &rdwBuffSize))
  {
    switch(dwType)
    {
    case REG_EXPAND_SZ:
    case REG_SZ:
      bRes = true;
      rdwBuffSize = dwResSize / sizeof(*cpBuff);
      break;

    default:
      break;
    };
  };

  return bRes;
};

//////////////////////////////////////////////////////////////////////
//CSLInCfgStr

void CSLInCfgStr::LoadWndPos(const HWND hcWnd)
{
  POINT Pnt;
  (*this)
    >> Pnt.x
    >> Pnt.y;
  if(false == fail())
  {
    //ajust on scr size
    const LONG lcXMax = ::GetSystemMetrics(SM_CXFULLSCREEN);
    const LONG lcYMax = ::GetSystemMetrics(SM_CYFULLSCREEN);
    ::SetWindowPos
      (
      hcWnd, 
      0, 
      (lcXMax < Pnt.x) ? 0 : Pnt.x, 
      (lcYMax < Pnt.y) ? 0 : Pnt.y, 
      0, 
      0, 
      SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE
      );
  };
};

void CSLInCfgStr::LoadWndPlace(const HWND hcWnd)
{
  WINDOWPLACEMENT Place;
  ZeroMemory(&Place, sizeof(Place));
  Place.length = sizeof(Place);
  (*this)
    >> Place.showCmd
    >> Place.rcNormalPosition.left
    >> Place.rcNormalPosition.top
    >> Place.rcNormalPosition.right
    >> Place.rcNormalPosition.bottom;
  if(false == fail())
  {
    //ajust on scr size
    const LONG lcXMax = ::GetSystemMetrics(SM_CXFULLSCREEN);
    const LONG lcYMax = ::GetSystemMetrics(SM_CYFULLSCREEN);
    if(lcXMax < Place.rcNormalPosition.left)
    {
      Place.rcNormalPosition.left = lcXMax - Place.rcNormalPosition.right - Place.rcNormalPosition.left;
      Place.rcNormalPosition.right = lcXMax;
    };
    if(lcYMax < Place.rcNormalPosition.top)
    {
      Place.rcNormalPosition.top = lcYMax - Place.rcNormalPosition.bottom - Place.rcNormalPosition.top;
      Place.rcNormalPosition.bottom = lcYMax;
    };

    //set place
    SLCHECK(FALSE != ::SetWindowPlacement(hcWnd, &Place));
  };  
};


_SL_END

