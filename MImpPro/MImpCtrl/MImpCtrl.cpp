// MImpCtrl.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#include "resource.h"

#include "cpl.h"

#include "..\Include\MIGlobal.h"

#include "..\\Slib\\SLFileMap.h"

#include "..\\Slib\\SLUtil.h"

#include "..\\Slib\\SLCfgReg.h"

#include "..\\Slib\\SLPrc.h"


////run imp cfg
static inline void RunCfg()
{
  using sl::CSLFileMap;
  CSLFileMap Map;
  if
    (
    false == Map.OpenExist(cpcSharedInfoName, sizeof(CMISharedInfo), CSLFileMap::eomRead)
    || false == TryShowCfgApp(reinterpret_cast<CMISharedInfo*>(Map.GetPtr()))
    )
  {
    //try run exist
    using sl::CSLCfgReg;
    CSLCfgReg Reg(cpcCfgInstallDirPath, false);
    TCHAR cpPath[MAX_PATH];
    DWORD dwLen = 0;
    if
      (
      false != Reg.IsPresent(cpcCfgInstallDirEnt)
      && false != Reg.ReadStr(cpcCfgInstallDirEnt, cpPath, COUNTOF(cpPath))
      )
    {
      //attach cfg module name to path
      TCHAR cpFullName[MAX_PATH];
      ::wsprintf(cpFullName, _T("%s\\%s"), cpPath, cpcCfgAppFileName);
      using sl::CSLProcess;
      CSLProcess Prc;
      Prc.Open(cpFullName);
    };
  };
};

////check host for running state
static inline bool HostRuned()
{
  bool bRes = false;

  using sl::CSLFileMap;
  CSLFileMap Map;
  if(false != Map.OpenExist(cpcSharedInfoName, sizeof(CMISharedInfo), CSLFileMap::eomRead))
  {
    const CMISharedInfo* const cpcCfgMem = reinterpret_cast<const CMISharedInfo*>(Map.GetPtr());
    if(FALSE != ::IsWindow(cpcCfgMem->common64.hMainHostWnd))
    {
      bRes = true;
    };
  };

  return bRes;
};

//mem res module
static HMODULE hMemMod = 0;

BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
  switch(dwReason)
  {
  case DLL_PROCESS_ATTACH:
    hMemMod = reinterpret_cast<HMODULE>(hModule);
    break;

  default:
    break;
  };

  return TRUE;
}

LONG APIENTRY CPlApplet(HWND hwndCPl,UINT uMsg,LONG lParam1,LONG lParam2)
{
  LPCPLINFO lpCPlInfo;
  LONG lRes = 0;

  switch(uMsg)
  {
  case CPL_INIT:      // first message, sent once
    lRes = TRUE;
    break;

  case CPL_GETCOUNT:  // second message, sent once
    lRes = 1;
    break;

  case CPL_NEWINQUIRE:
    {
      NEWCPLINFO* const cpInfo = reinterpret_cast<NEWCPLINFO*>(lParam2);
      cpInfo->dwSize = sizeof(*cpInfo);
      cpInfo->lData = 1;
      cpInfo->hIcon = ::LoadIcon(hMemMod, MAKEINTRESOURCE(IDR_STATE_RUN_ICON));
      ::LoadString(hMemMod, IDS_NAME, cpInfo->szName, COUNTOF(cpInfo->szName));
      ::LoadString(hMemMod, IDS_DESCR, cpInfo->szInfo, COUNTOF(cpInfo->szInfo));
    };
    break;

  case CPL_INQUIRE: // third message, sent once per application
    lpCPlInfo = reinterpret_cast<LPCPLINFO>(lParam2);
    lpCPlInfo->lData = 1;
    lpCPlInfo->idIcon = IDR_STATE_RUN_ICON;
    lpCPlInfo->idName = IDS_NAME;
    lpCPlInfo->idInfo = IDS_DESCR;
    break;

  case CPL_DBLCLK:
    RunCfg();
    break;

  case CPL_STOP:      // sent once per application before CPL_EXIT
    break;

  case CPL_EXIT:    // sent once before FreeLibrary is called
    break;

  default:
    lRes = TRUE;
    break;
  };

  return lRes;
};
