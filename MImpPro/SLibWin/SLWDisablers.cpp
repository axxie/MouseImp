/**********************************************************************
disablers
**********************************************************************/

#define STRICT
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#pragma hdrstop

#include <tchar.h>


#include "SLWDisablers.h"


#include <crtdbg.h>


_SLW_BEG


static LPCTSTR const cpcEditClassName = _T("EDIT");

void EnableWnd(const HWND hcWnd, const BOOL bcEnable)
{
  TCHAR szClassName[50];
  GetClassName(hcWnd, szClassName, COUNTOF(szClassName));
  if(0 == _tcsicmp(cpcEditClassName, szClassName))
  {
    ::SendMessage(hcWnd, EM_SETREADONLY, !bcEnable, 0L);
  }
  else
  {
    ::EnableWindow(hcWnd, bcEnable);
  }
};

void EnableCtrl(const HWND hcDlg, const UINT uicCtrlId, const BOOL bcEnable)
{
  HWND hCtrl = ::GetDlgItem(hcDlg, uicCtrlId);
  if(0 != hCtrl)
  {
    EnableWnd(hCtrl, bcEnable);
  };
};

void EnableDlgWnd(const HWND hcWnd, const BOOL bcEnable)
{
  for(HWND hCtrl = ::GetWindow(hcWnd, GW_CHILD); 0 != hCtrl; hCtrl = ::GetWindow(hCtrl, GW_HWNDNEXT))
  {
    const LONG lcInd = ::GetWindowLong(hCtrl, GWL_ID);
    if(0xffff != lcInd && 0 != lcInd)
    {
      EnableWnd(hCtrl, bcEnable);
    };
  };
};

void EnableCtrlGrp(const HWND hcWnd, const UINT* const cpcCtrl, const BOOL bcEnable)
{
  for(const UINT* pcCtrl = cpcCtrl; 0 != *pcCtrl && -1 != *pcCtrl; pcCtrl++)
  {
    EnableCtrl(hcWnd, *pcCtrl, bcEnable);
  };
};

bool CheckEmptyCtrlGrp(const HWND hcWnd, const UINT* const cpcCtrl)
{
  //look on mass and get ctrl's from dlg
  //if find ctrl with 0 == ::GetWindowTextLenght - return false

  bool bRes = true;

  //scan mass
  for(const UINT* cpLook = cpcCtrl; 1 != *cpLook && 0 != *cpLook; cpLook++)
  {
    const HWND hCtrl = ::GetDlgItem(hcWnd, *cpLook);
    if(0 != hCtrl && 0 == ::GetWindowTextLength(hCtrl))
    {
      //find invalid ctrl - break
      bRes = false;
      ::SetFocus(hCtrl);
      break;
    };
  };

  return bRes;
};

void SLWComboControlInit(HINSTANCE hInst, HWND hDlg, const UINT uicCtrl, const DWORD dwcInitData, const __CSLWListBoxInitData* const cpcInitData, const DWORD dwcDayaSize)
{
  TCHAR cpBuff[1024];
  for(const __CSLWListBoxInitData* pcLook = cpcInitData; cpcInitData + dwcDayaSize > pcLook; pcLook++)
  {
    *cpBuff = '\0';
    const DWORD dwcLen = ::LoadString(hInst, pcLook->dwResId, cpBuff, COUNTOF(cpBuff));
    _ASSERT(0 != dwcLen);
    const DWORD dwcInd = ::SendDlgItemMessage(hDlg, uicCtrl, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(cpBuff));
    ::SendDlgItemMessage(hDlg, uicCtrl, CB_SETITEMDATA, dwcInd, pcLook->dwDataId);
    if(dwcInitData == pcLook->dwDataId)
    {
      ::SendDlgItemMessage(hDlg, uicCtrl, CB_SETCURSEL, dwcInd, 0);
    };
  };
};

bool SLWComboControlGetSel(HWND hDlg, const UINT uicCtrl, DWORD& rData)
{
  bool bRes = false;
  const int dwcInd = ::SendDlgItemMessage(hDlg, uicCtrl, CB_GETCURSEL, 0, 0);
  if(0 <= dwcInd)
  {
    rData = ::SendDlgItemMessage(hDlg, uicCtrl, CB_GETITEMDATA, dwcInd, 0);
    bRes = true;
  };
  return bRes;
};



_SLW_END


