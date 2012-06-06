/**********************************************************************
main functionaly
**********************************************************************/

#include "StdAfx.h"


#include "MIProHst.h"


#include "..\\SLibWin\\SLWDbg.h"

#include "resource.h"

#include "..\\SLib\\SLPrc.h"

#include "..\\SLibWin\\SLWRes.h"

#include "..\\Slib\\SLAutoPtr.h"

#include "..\\SLibWin\\SLWSplash.h"

#include "..\\Slib\\SLCfgReg.h"

#include "..\\Slib\\SLResMem.h"

#include "..\\MImpSrv\MImpSrvCtl.h"

namespace help
{
//# include "..\\help\\MIPROHLP.H"
};

//tool's dll
namespace ToolResDll
{
#include "..\\MImpUtl\\resource.h"
};

#include "..\\PSApiAdd\\PSAPI.H"

#include <TLHELP32.H>


//info for scall process's wnd's
struct __CScanProcessInfo
{
  //pointer to "this"
  CApp* pThis;
  //looked process id
  DWORD dwProcessId;
};

//////////////////////////////////////////////////////////////////////
//global's

//rgn in range
static inline DWORD MyRnd(const DWORD dwcFrom, const DWORD dwcTo)
{
  const DWORD dwTickVal = ::GetTickCount();
  const DWORD dwcTick = (LOWORD(dwTickVal) ^ HIWORD(dwTickVal)) % 2048;
  const DWORD dwcRangeSize = dwcTo - dwcFrom;
  const DWORD dwcResSize = dwcTick * dwcRangeSize / 2048;
  return dwcResSize + dwcFrom;
};


//////////////////////////////////////////////////////////////////////
//main init

CApp* g_pApp = 0;

//func names
static LPCSTR const cpcGetWkSetFunc = "GetProcessWorkingSetSize";
typedef BOOL (WINAPI *GetWkSetFuncType)(HANDLE, PSIZE_T, PSIZE_T);
static LPCSTR const cpcSetWkSetFunc = "SetProcessWorkingSetSize";
typedef BOOL (WINAPI *SetWkSetFuncType)(HANDLE, SIZE_T, SIZE_T);

//name of relocation info res type
static LPCSTR const cpcRelInfoResTypeName = "bin";

//name of RegisterServiceProcess func
static LPCTSTR const cpcRegServProcFunc = _T("RegisterServiceProcess");
typedef DWORD (WINAPI *RegSrvProcType)(DWORD dwProcId, DWORD dwRegType);
//module of RegisterServiceProcess
static LPCTSTR const cpcKernelModuleName = _T("KERNEL32.DLL");
LPTSTR const cpcCmdKeyQuickTour = _T("MIProCfg.exe /QuickTour");
//cfg's
static LPCTSTR const cpcCfgValidMouseKey = _T("ValidMouseKey");
static LPCTSTR const cpcCfgScrollReduction = _T("ScrollReduction");
static LPCTSTR const cpcCfgScollReductionKey = _T("ScollReductionKey");
static LPCTSTR const cpcCfgScrollInv = _T("ScrollInv");
static LPCTSTR const cpcCfgMoveDistanceSumm = _T("Summ");
static LPCTSTR const cpcCfgMoveDistanceSaveSumm = _T("SaveSumm");
static LPCTSTR const cpcCfgAHideEnabled = _T("AHideEnabled");
static LPCTSTR const cpcCfgwAutoHideTimerTime = _T("AHideTime");
static LPCTSTR const cpcCfgwAutoOpenTimerTime = _T("AOpenTime");
static LPCTSTR const cpcCfgCursorVisualise = _T("UseCursor");
static LPCTSTR const cpcCfgDSTempDis = _T("DSDisKey");
static LPCTSTR const cpcCfgASProd = _T("AShrinkProd");
static LPCTSTR const cpcCfgIEScrMinMove = _T("IEMoveStart");
static LPCTSTR const cpcCfgIEScrConv = _T("IEConvVal");
static LPCTSTR const cpcCfgTrayIcon = _T("TIcon");
static LPCTSTR const cpcCfgTrayIconAnim = _T("TIconAnim");
static LPCTSTR const cpcCfgSplashShow = _T("SSplash");
static LPCTSTR const cpcCfgBDScrolling = _T("BDScroll");
static LPCTSTR const cpcCfgScrollNormRed = _T("NRed");
static LPCTSTR const cpcCfgControlType = _T("ScrType");
static LPCTSTR const cpcCfgScrollEnabled = _T("ScrEnab");
static LPCTSTR const cpcCfgTotalTime = _T("TotalTime");
//"data imported from MImp" flag
static LPCTSTR const cpcCfgDataImported = "IOk";

//menu command's for scroll control type
static const UINT cpcScrollControlCommands[] =
{
  ID_TRAY_ENABLE_CTYPE_STANDARD,
  ID_TRAY_ENABLE_CTYPE_TOGGLE,
  ID_TRAY_ENABLE_CTYPE_MIX,
};

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
  //create app instance
  //try init
  //if init ok - start
  //finit
  //delete instance

  int iRes = 0;

  g_pApp = new CApp(hInst);
  bool bNewInited = false;
  if(false != g_pApp->Init(bNewInited))
  {
    g_pApp->Run();
  };
  if(false != bNewInited)
  {
    g_pApp->Finit();
  };
  delete g_pApp;
  g_pApp = 0;

  return iRes;
};


//////////////////////////////////////////////////////////////////////
//CApp

CApp::CApp(HINSTANCE hInitInstance)
  :hInst(hInitInstance),
  hHookLib(0),
  pCfgMem(0),
  hCfgMap(0),
  pCryptSharedMem(0),
  hCryptSharedMap(0),
  uicGlbMsg(::RegisterWindowMessage(cpcGlobalIntMessage)),
  dwLastLookAppTime(::GetTickCount()),
  dwTrayAnimWaitTime(MyRnd(eccTrayAnimOpenMinTime, eccTrayAnimOpenMaxTime)),
  bTrayAnimEyesClosed(false),
  bTrayIconPresent(false),

  ulcTrayDClickTime(::GetDoubleClickTime()),
  lcTrayDClickShiftX(::GetSystemMetrics(SM_CXDOUBLECLK)),
  lcTrayDClickShiftY(::GetSystemMetrics(SM_CYDOUBLECLK)),
  bTrayFirstClickFlag(false),
  ulTrayFirstClickTimerTime(0),

  pProcessFindNameFunc(0),
  pProcessCloseAllFunc(0),

  uiTimerId(0),

  dwSaveCfgSummTime(0)
{
};

CApp::~CApp()
{
};

void CApp::Run()
{
  MSG Msg;
  while(FALSE != GetMessage(&Msg, 0, 0, 0))
  {
    TranslateMessage(&Msg);
    DispatchMessage(&Msg);
  };
};

void CApp::LoadCfg(bool& rbFirstStart)
{
  {
    using sl::CSLCfgReg;
    CSLCfgReg Reg;
    if(false != Reg.Open(cpcCfgKeyName, true, rbFirstStart))
    {
      BOOL bImpOk = FALSE;
      Reg.GetVal(cpcCfgDataImported, bImpOk, FALSE);
      //try import data from MImp cfg
      if(false != rbFirstStart || FALSE == bImpOk)
      {
        ////enum key's and try copy all data from MImp cfg to MImpPro cfg
        CSLCfgReg OldCfg;
        if(false != OldCfg.Open(cpcCfgImportKeyName, true))
        {
          ////enum now and copy all from old cfg to current
          bool bDoCycle = true;
          for(DWORD dwCount = 0; false != bDoCycle; dwCount++)
          {
            TCHAR cpKeyName[MAX_PATH];
            DWORD dwKeyNameSize = COUNTOF(cpKeyName);
            DWORD dwValType = 0;
            DWORD dwKeySize =0;
            bDoCycle = ERROR_SUCCESS == ::RegEnumValue(OldCfg, dwCount, cpKeyName, &dwKeyNameSize, 0, &dwValType, 0, &dwKeySize);
            DWORD dwNewKeyTestSize = 0;
            //key find and new not exist
            if(false != bDoCycle && false == Reg.GetValSize(cpKeyName, dwNewKeyTestSize))
            {
              ////alloc buff, get data and put to new cfg
              using sl::CSLAutoPtr;
              CSLAutoPtr<BYTE> pBuff = new BYTE[dwKeySize];
              if(ERROR_SUCCESS == ::RegQueryValueEx(OldCfg, cpKeyName, 0, 0, pBuff.GetPtr(), &dwKeySize))
              {
                ////put
                ::RegSetValueEx(Reg, cpKeyName, 0, dwValType, pBuff.GetPtr(), dwKeySize);
              };
            };
          };
          ////save "imported" falg
          Reg.PutVal(cpcCfgDataImported, TRUE);
        };
      };
      //val's
      Reg.GetVal(cpcCfgScrollEnabled, pCfgMem->common64.bScrollDisabled, TRUE);
      Reg.GetVal(cpcCfgControlType, pCfgMem->common64.dwControlType, escStandard);
      Reg.GetVal(cpcCfgValidMouseKey, pCfgMem->common64.uiValidMouseKeys, MK_RBUTTON);
      Reg.GetVal(cpcCfgScrollReduction, pCfgMem->common64.lScrollReduction, egcDefScrollReduction);
      Reg.GetVal(cpcCfgScollReductionKey, pCfgMem->common64.uiScollReductionKey, egcDecScollReductionKey);
      Reg.GetVal(cpcCfgScrollInv, pCfgMem->common64.bScrollInv, FALSE);
      Reg.GetVal(cpcCfgMoveDistanceSumm, pCfgMem->common64.llMoveDistanceSumm, 0);
      Reg.GetVal(cpcCfgMoveDistanceSaveSumm, pCfgMem->common64.llMoveDistanceSaveSumm, 0);
      Reg.GetVal(cpcCfgAHideEnabled, pCfgMem->common64.bAHideEnabled, TRUE);
      Reg.GetVal(cpcCfgwAutoHideTimerTime, pCfgMem->common64.dwAutoHideTimerTime, egcDefAutoHideTimerTime);
      Reg.GetVal(cpcCfgwAutoOpenTimerTime, pCfgMem->common64.dwAutoOpenTimerTime, 0);
      Reg.GetVal(cpcCfgCursorVisualise, pCfgMem->common64.bCursorVisualise, TRUE);
      Reg.GetVal(cpcCfgDSTempDis, pCfgMem->common64.uiDirectScrollTmpDisableKey, VK_CONTROL);
      Reg.GetVal(cpcCfgIEScrMinMove, pCfgMem->dwIEScrollMinMove, egcIEScrollMinMoveDef);
      Reg.GetVal(cpcCfgIEScrConv, pCfgMem->common64.dwIESCrollConvMove, egcIEScrollConvMoveDef);
      Reg.GetVal(cpcCfgTrayIcon, pCfgMem->common64.bEnableTrayIcon, TRUE);
      Reg.GetVal(cpcCfgTrayIconAnim, pCfgMem->common64.bEnableTrayIconAnim, TRUE);
      Reg.GetVal(cpcCfgSplashShow, pCfgMem->common64.bShowSplash, TRUE);
      Reg.GetVal(cpcCfgBDScrolling, pCfgMem->common64.bBDScrollingEnabled, FALSE);
      Reg.GetVal(cpcCfgScrollNormRed, pCfgMem->common64.dwScrollNormReduction, egcMagnScrollReduction);
      Reg.GetVal(cpcCfgTotalTime, pCfgMem->common64.llTotalRunTime, 0);
      //AS productivity
      Reg.ReadBuff(cpcCfgASProd, reinterpret_cast<BYTE*>(&(pCfgMem->common64.ASOpenProd)), sizeof(pCfgMem->common64.ASOpenProd));
    };
  };
};

void CApp::SaveCfg()
{
  {
    using sl::CSLCfgReg;
    CSLCfgReg Reg;
    Reg.Open(cpcCfgKeyName, true);
    //val's
    Reg.PutVal(cpcCfgScrollEnabled, pCfgMem->common64.bScrollDisabled);
    Reg.PutVal(cpcCfgControlType, pCfgMem->common64.dwControlType);
    Reg.PutVal(cpcCfgValidMouseKey, pCfgMem->common64.uiValidMouseKeys);
    Reg.PutVal(cpcCfgScrollReduction, pCfgMem->common64.lScrollReduction);
    Reg.PutVal(cpcCfgScollReductionKey, pCfgMem->common64.uiScollReductionKey);
    Reg.PutVal(cpcCfgScrollInv, pCfgMem->common64.bScrollInv);
    Reg.PutVal(cpcCfgMoveDistanceSumm, pCfgMem->common64.llMoveDistanceSumm),
    Reg.PutVal(cpcCfgMoveDistanceSaveSumm, pCfgMem->common64.llMoveDistanceSaveSumm),
    Reg.PutVal(cpcCfgAHideEnabled, pCfgMem->common64.bAHideEnabled);
    Reg.PutVal(cpcCfgwAutoHideTimerTime, pCfgMem->common64.dwAutoHideTimerTime);
    Reg.PutVal(cpcCfgwAutoOpenTimerTime, pCfgMem->common64.dwAutoOpenTimerTime);
    Reg.PutVal(cpcCfgCursorVisualise, pCfgMem->common64.bCursorVisualise);
    Reg.PutVal(cpcCfgDSTempDis, pCfgMem->common64.uiDirectScrollTmpDisableKey);
    Reg.PutVal(cpcCfgIEScrMinMove, pCfgMem->dwIEScrollMinMove);
    Reg.PutVal(cpcCfgIEScrConv, pCfgMem->common64.dwIESCrollConvMove);
    Reg.PutVal(cpcCfgTrayIcon, pCfgMem->common64.bEnableTrayIcon);
    Reg.PutVal(cpcCfgTrayIconAnim, pCfgMem->common64.bEnableTrayIconAnim);
    Reg.PutVal(cpcCfgSplashShow, pCfgMem->common64.bShowSplash);
    Reg.PutVal(cpcCfgBDScrolling, pCfgMem->common64.bBDScrollingEnabled);
    Reg.PutVal(cpcCfgScrollNormRed, pCfgMem->common64.dwScrollNormReduction);
    Reg.PutVal(cpcCfgTotalTime, pCfgMem->common64.llTotalRunTime);
    //AS productivity
    Reg.WriteBuff(cpcCfgASProd, reinterpret_cast<const BYTE*>(&(pCfgMem->common64.ASOpenProd)), sizeof(pCfgMem->common64.ASOpenProd));
  };
};

static LRESULT __stdcall CApp_MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return g_pApp->MainWndProc(hwnd, uMsg, wParam, lParam);
}

static VOID __stdcall CApp_TimerProcThunk(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    g_pApp->TimerThunkProc(hwnd, uMsg, idEvent, dwTime);
}

bool CApp::Init(bool& rNewInited)
{
  //splash on
  //alloc memory for app pattern's
  //try open global "SharedInfo" - if find - app laready present (init failed) - else - create new
  //try install hook
  //create main wnd
  //try icon
  //load Autohist pattern's list
  //splash off

  bool bRes = true;
  rNewInited = true;
  bool bFirstStart = false;
  UINT uiErrorId = 0;

  //shared info (open, create and load) and look "single copy" (by IsMainHostAppWndValid) rules
  if(false != bRes)
  {
    bRes = false;

    //size of shared data
    const DWORD dwcSharedInfoSize =
      sizeof(CMISharedInfo)
      + sizeof(CSharedNotClearedInfo);
    //try open exist
    bool bInstallNew = false;
    hCfgMap = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, cpcSharedInfoName);
    //flag
    const bool bcOpenExist = 0 != hCfgMap;
    if(0 == hCfgMap)
    {
      SECURITY_ATTRIBUTES secAttr;
      char secDesc[ SECURITY_DESCRIPTOR_MIN_LENGTH ];
      secAttr.nLength = sizeof(secAttr);
      secAttr.bInheritHandle = FALSE;
      secAttr.lpSecurityDescriptor = &secDesc;
      InitializeSecurityDescriptor(secAttr.lpSecurityDescriptor, SECURITY_DESCRIPTOR_REVISION);
      SetSecurityDescriptorDacl(secAttr.lpSecurityDescriptor, TRUE, 0, FALSE);

      PSECURITY_DESCRIPTOR pSD;
      ConvertStringSecurityDescriptorToSecurityDescriptor(
        "S:(ML;;NW;;;LW)", // this means "low integrity"
        SDDL_REVISION_1,
        &pSD,
        NULL);
      PACL pSacl = NULL;                  // not allocated
      BOOL fSaclPresent = FALSE;
      BOOL fSaclDefaulted = FALSE;
      GetSecurityDescriptorSacl(
        pSD,
        &fSaclPresent,
        &pSacl,
        &fSaclDefaulted);
      SetSecurityDescriptorSacl(secAttr.lpSecurityDescriptor, TRUE, pSacl, FALSE);

      //open failed - create new
      hCfgMap = ::CreateFileMapping(INVALID_HANDLE_VALUE, &secAttr, PAGE_READWRITE, 0, dwcSharedInfoSize, cpcSharedInfoName);
      bInstallNew = true;
    };
    if(0 != hCfgMap)
    {
      LPVOID const cpPtr = ::MapViewOfFile(hCfgMap, FILE_MAP_ALL_ACCESS, 0, 0, dwcSharedInfoSize);
      if(0 != cpPtr)
      {
        pCfgMem = reinterpret_cast<CMISharedInfo*>(cpPtr);
        bRes = true;
        if(false != bInstallNew)
        {
          CSharedNotClearedInfo* const cpNotClearInfo = GetNotClearInfo(pCfgMem);
          ZeroMemory(cpNotClearInfo, sizeof(*cpNotClearInfo));
        };
      };
    };
    //report error
    if(false == bRes)
    {
      uiErrorId = IDS_MSG_NOT_START;
    };
    //if "open exist file mapping" - look on "HostWnd" is valid - if ok - exit
    if(false != bRes && false != bcOpenExist)
    {
      rNewInited = FALSE == ::IsWindow(pCfgMem->common64.hMainHostWnd);
      bRes = false != rNewInited;
    };
    if(false != bRes)
    {
      //init cfg map
      ZeroMemory(pCfgMem, sizeof(CMISharedInfo));
      //set def's
      ::GetCursorPos(&(pCfgMem->MoveDistanseLastLookPnt));
      //load cfg from registry
      LoadCfg(bFirstStart);
      //load wnd patterns'
      AHReadWndPatternList(false, true);
      //load basic's for reg info
      LoadRegInfo(true);
    };
  };

  //OS-specific actions
  if(false != bRes)
  {
      OSVERSIONINFO OsVer;
      OsVer.dwOSVersionInfoSize = sizeof(OsVer);
      if(FALSE != ::GetVersionEx(&OsVer))
      {
          if(VER_PLATFORM_WIN32_NT == OsVer.dwPlatformId)
          {
              //WinNT - enable for all NT greater then 4
              pCfgMem->bWin2000PatchRequired = 4 < OsVer.dwMajorVersion;
              // Vista and higher - check integrity level and start launcher service if needed
              if (OsVer.dwMajorVersion >= 6)
              {
                  bRes = CheckIntegrityLevel();
              }
          }
          else if(VER_PLATFORM_WIN32_WINDOWS == OsVer.dwPlatformId)
          {
              //win95
              pCfgMem->bWin2000PatchRequired = FALSE;
          };
      };
  };


  //splash
  using slw::CSWSplash;
  CSWSplash Splash;
  if(false != bRes && (FALSE != pCfgMem->common64.bShowSplash || FALSE == pCfgMem->bCryptFirstLayerValid))
  {
    SLCHECK(FALSE != Splash.Create(IDB_SPLASH_BMP, hInst, ULONG_MAX));
  };

  //try init "process info" system
  if(false != bRes)
  {
    hPSApiModule = ::LoadLibrary(cpcPSApiModuleName);
    if(0 != hPSApiModule)
    {
      pProcessCloseAllFunc = &CApp::ProcessEnumCloseAllPSApi;
      pEnumProcessesFunc = reinterpret_cast<EnumProcessesType>(::GetProcAddress(hPSApiModule, cpcEnumProcessesName));
      pEnumProcessModulesFunc = reinterpret_cast<EnumProcessModulesType>(::GetProcAddress(hPSApiModule, cpcEnumProcessModulesName));
      pGetModuleBaseNameFunc = reinterpret_cast<GetModuleBaseNameType>(::GetProcAddress(hPSApiModule, cpcGetModuleBaseNameName));
      if
        (
        0 != pEnumProcessesFunc
        && 0 != pEnumProcessModulesFunc
        && 0 != pGetModuleBaseNameFunc
        )
      {
        pProcessFindNameFunc = &CApp::ProcessFindNamePSApi;
      };
    }
    else
    {
      hToolHelpModule = ::LoadLibrary(cpcToolHelpModuleName);
      if(0 != hToolHelpModule)
      {
        pProcessCloseAllFunc = &CApp::ProcessEnumCloseAllToolHelp;
        pProcess32First = reinterpret_cast<Process32FirstType>(::GetProcAddress(hToolHelpModule, cpcProcess32FirstName));
        pProcess32Next = reinterpret_cast<Process32NextType>(::GetProcAddress(hToolHelpModule, cpcProcess32NextName));
        pCreateToolhelp32Snapshot = reinterpret_cast<CreateToolhelp32SnapshotType>(::GetProcAddress(hToolHelpModule, cpcCreateToolhelp32SnapshotName));
        if
          (
          0 != pProcess32First
          && 0 != pProcess32Next
          && 0 != pCreateToolhelp32Snapshot
          )
        {
          pProcessFindNameFunc = &CApp::ProcessFindNameToolHelp;
        };
      };
    };

    //if find any "custom" process enum func - disable "process info" from hook module
    if(0 != pProcessFindNameFunc)
    {
      pCfgMem->bNotSendModuleFileName = TRUE;
    };
  };

  //main wnd
  if(false != bRes)
  {
    bRes = false;
    //register class
    WNDCLASS Class;
    Class.style = 0;
    Class.lpfnWndProc = CApp_MainWndProc;
    Class.cbClsExtra = 0;
    Class.cbWndExtra = 0;
    Class.hInstance = hInst;
    Class.hIcon = 0;//LoadIcon(0, IDI_APPLICATION);
    Class.hCursor = 0;//LoadCursor(0, IDC_ARROW);
    Class.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    Class.lpszMenuName = 0;
    Class.lpszClassName = cpcWndClass;
    if(0 != RegisterClass(&Class))
    {
      //create wnd
      const HWND hcWnd = ::CreateWindow
        (
        cpcWndClass,
        cpcWndTitle,
        WS_OVERLAPPED,
        0,
        0,
        0,
        0,
        0,
        0,
        hInst,
        0
        );
      if(0 != hcWnd)
      {
        this->hcWnd = hcWnd;
        //mem main wnd in shared cfg
        pCfgMem->common64.hMainHostWnd = hcWnd;
        bRes = true;
      };
    };
    //report error
    if(false == bRes)
    {
      uiErrorId = IDS_MSG_NOT_START;
    };
  };

  //get scr size
  if(false != bRes)
  {
    pCfgMem->ScrSize.x = ::GetSystemMetrics(SM_CXSCREEN);
    pCfgMem->ScrSize.y = ::GetSystemMetrics(SM_CYSCREEN);
  };

  //try icon
  if(false != bRes)
  {
    //ini tray icon data
    TrayIconData.cbSize = sizeof(TrayIconData);
    TrayIconData.hWnd = pCfgMem->common64.hMainHostWnd;
    TrayIconData.uID = 0;
    TrayIconData.uCallbackMessage = emcToHostTrayIconNotifyMsg;
    TrayIconData.hIcon = 0;
    SLCHECK(FALSE != ::LoadString(hInst, IDS_STR_TRAY_DEF, TrayIconData.szTip, COUNTOF(TrayIconData.szTip)));
  };

  //try register self as service process - for invisibility under Win95
  if(false != bRes)
  {
    RegSrvProcType pRegProc = reinterpret_cast<RegSrvProcType>(::GetProcAddress(::GetModuleHandle(cpcKernelModuleName), cpcRegServProcFunc));
    if(0 != pRegProc)
    {
      pRegProc(::GetCurrentProcessId(), 1);
    };
  };

  //start "processing" timer
  if(false != bRes)
  {
    uiTimerId = ::SetTimer(0, 0, eccTimerTime, CApp_TimerProcThunk);
    bRes = 0 != uiTimerId;
  };

  //try load library
  if(false != bRes)
  {
    bRes = false;
    hHookLib = ::LoadLibrary(cpcHookFundModule);
    if(0 != hHookLib)
    {
      bRes = true;
    };
    //report error
    if(false == bRes)
    {
      uiErrorId = IDS_MSG_NOT_START;
    };
  };

  //try locate "crypt" section in dll and decrypt it's content
  if(false != bRes)
  {
    LoadRegInfo(false);
  };


  //try look on hook versions
  if(false != bRes)
  {
    HookGetVerFuncType const cpFinc = reinterpret_cast<HookGetVerFuncType>(::GetProcAddress(hHookLib, cpcHookGetVerFunc));
    SetIsHostProcessFuncType const cpSetIsHost = reinterpret_cast<SetIsHostProcessFuncType>(::GetProcAddress(hHookLib, cpcSetIsHostProcessFunc));

    bRes =
      0 != cpFinc
      && GetVersionNumber() == cpFinc() 
      && 0 != cpSetIsHost;
    //version failed
    if(false == bRes)
    {
      uiErrorId = IDS_MSG_INV_VER;
    } else
	{
	  cpSetIsHost();
	}
  };

  //try install hook
  if(false != bRes)
  {
    bRes = false;
    //try get hook proc from dll
    FARPROC const cpMouseHook = ::GetProcAddress(hHookLib, cpMouseHookFunc);
    FARPROC const cpCBTHook = ::GetProcAddress(hHookLib, cpcCBTHookFunc);
    if(0 != cpMouseHook && 0 != cpCBTHook)
    {
      ////allow process from cbt hook - else first processing from cbt hook will remove memed list
      pCfgMem->bProcessAddWndFromCBTHook = TRUE;
#ifndef MIMP_DEBUG
      pCfgMem->hMouseHook = ::SetWindowsHookEx(WH_MOUSE, reinterpret_cast<HOOKPROC>(cpMouseHook), hHookLib, 0);
      pCfgMem->hCBTHook = ::SetWindowsHookEx(WH_CBT, reinterpret_cast<HOOKPROC>(cpCBTHook), hHookLib, 0);
#else
      STARTUPINFO si = {0};
      si.cb = sizeof(STARTUPINFO);
      PROCESS_INFORMATION pi = {0};

      BOOL bNotepadRes = CreateProcess(
          NULL,
          "notepad.exe",
          //"C:\\Program Files\\Internet Explorer\\IEXPLORE.EXE",
          NULL,
          NULL,
          FALSE,
          0,
          NULL,
          NULL,
          &si,
          &pi);
      MessageBox(NULL, "Wait for notepad", "Message", MB_OK);

      pCfgMem->hMouseHook = ::SetWindowsHookEx(WH_MOUSE, reinterpret_cast<HOOKPROC>(cpMouseHook), hHookLib, pi.dwThreadId);
      pCfgMem->hCBTHook = ::SetWindowsHookEx(WH_CBT, reinterpret_cast<HOOKPROC>(cpCBTHook), hHookLib, pi.dwThreadId);

#endif
      //dbg:
      if(0 != pCfgMem->hMouseHook && 0 != pCfgMem->hCBTHook)
      {
        bRes = true;
        pCfgMem->hHookPresentFlag = pCfgMem->common64.hMainHostWnd;
        //increment hook install count
        ::InterlockedIncrement(&(GetNotClearInfo(pCfgMem)->lHookInstallCount));
      };
      pCfgMem->bProcessAddWndFromCBTHook = false != bRes;
    };
    //report error
    if(false == bRes)
    {
      uiErrorId = IDS_MSG_NOT_START;
    };
  };

  //post message for scan
  if(false != bRes)
  {
    ::PostMessage(pCfgMem->common64.hMainHostWnd, emcReReadWndPattList, 0, FALSE);
  };

  //splash off
  Splash.WaitForEnd();

  SignalSharedInfoReady();

  //if need - show error
  if(false == bRes && 0 != uiErrorId)
  {
    MsgBox(uiErrorId, MB_OK | MB_ICONERROR);
  };

  //if it's first start - bring up cfg application
  if(false != bRes && false != bFirstStart)
  {
    StartCfgApp(true);
  };

  //try chng workset size
  if(false != bRes)
  {
    HMODULE hKrnl = ::GetModuleHandle(cpcKernelModuleName);
    GetWkSetFuncType const pGetWkFunc = reinterpret_cast<GetWkSetFuncType>(::GetProcAddress(hKrnl, cpcGetWkSetFunc));
    SetWkSetFuncType const pSetWkFunc = reinterpret_cast<SetWkSetFuncType>(::GetProcAddress(hKrnl, cpcSetWkSetFunc));
    if(0 != pGetWkFunc && 0 != pSetWkFunc)
    {
      SIZE_T dwHiSize = 0;
      SIZE_T dwLowSize = 0;
      if(FALSE != pGetWkFunc(::GetCurrentProcess(), &dwLowSize, &dwHiSize))
      {
        pSetWkFunc(::GetCurrentProcess(), dwLowSize, 2 * dwLowSize);
      };
    };
  };

  return bRes;
};

void CApp::LoadRegInfo(const bool bcFirstCheckOnly)
{
  pCfgMem->bCryptOk = TRUE;
  pCfgMem->bCryptFirstLayerValid = TRUE;
};

//////////////////////////////////////////////////////////////////////
//exit wnd enum helper's

static BOOL CALLBACK ExitEnumChildWndProc(HWND hWnd, LPARAM lParam)
{
  reinterpret_cast<CApp*>(lParam)->AnalyseExitWndProc(hWnd);
  ::EnumChildWindows(hWnd, ExitEnumChildWndProc, lParam);
  return TRUE;
};

static BOOL CALLBACK ExitEnumWndProc(HWND hWnd, LPARAM lParam)
{
  reinterpret_cast<CApp*>(lParam)->AnalyseExitWndProc(hWnd);
  ::EnumChildWindows(hWnd, ExitEnumChildWndProc, lParam);
  return TRUE;
};

void CApp::UninstallSubClassing()
{
  //stop add wnd's from cBT hook
  pCfgMem->bProcessAddWndFromCBTHook = FALSE;

  //Ahide - show all wnd's' cutternly in list
  bool bSomeOpen = false;
  for(ProcessWndListIterType Iter = ProcessWndList.IterBegin(); false != ProcessWndList.IterIsOk(Iter); Iter = ProcessWndList.IterNext(Iter))
  {
    __CAHWndInfo* const cpInfo = ProcessWndList.IterGet(Iter);
    if(false != cpInfo->bWndClosed)
    {
      bSomeOpen = true;
      AHOpenWnd(cpInfo, false, true, false);
    };
  };
  ProcessWndList.FreeAll();
  TimerLookingList.FreeAll();

  //now try unload all subclassing
  if(0 != pCfgMem)
  {
    ::EnumWindows(ExitEnumWndProc, reinterpret_cast<LPARAM>(this));
  };

  //clear "host present" flag
  pCfgMem->hHookPresentFlag = 0;
};

void CApp::Finit()
{
  //detach hook
  if(0 != pCfgMem)
  {
    if(0 != pCfgMem->hMouseHook)
    {
      ::UnhookWindowsHookEx(pCfgMem->hMouseHook);
      pCfgMem->hMouseHook = 0;
    };
    if(0 != pCfgMem->hCBTHook)
    {
      ::UnhookWindowsHookEx(pCfgMem->hCBTHook);
      pCfgMem->hCBTHook = 0;
    };
  };

  //save "close" time for app run
  if(0 != pCfgMem)
  {
    pCfgMem->common64.ASOpenProd.llTotalTime += static_cast<LONGLONG>(CalcTimeInSec(dwLastLookAppTime));
  };

  //tray icon's
  TrayIconData.uFlags = 0;
  if(false != bTrayIconPresent)
  {
    ::Shell_NotifyIcon(NIM_DELETE, &TrayIconData);
  };

  //msg's
  {
    MSG Msg;
    while(FALSE != ::PeekMessage(&Msg, 0, 0, 0, PM_REMOVE))
    {
      switch(Msg.message)
      {
        //info about new app - free mapping
      case emcNewAppStarted:
        ::CloseHandle(reinterpret_cast<HANDLE>(Msg.lParam));
        break;

      default:
        break;
      };
    };
  };

  //free lib
  if(0 != hHookLib)
  {
    ::FreeLibrary(hHookLib);
    hHookLib = 0;
  };

  //close "process info" system
  if(0 != pProcessCloseAllFunc)
  {
    (this->*pProcessCloseAllFunc)();
  };

  //save cfg's
  SaveCfg();

  //shared cfg
  if(0 != pCfgMem)
  {
    ::UnmapViewOfFile(pCfgMem);
    pCfgMem = 0;
  };
  if(0 != hCfgMap)
  {
    ::CloseHandle(hCfgMap);
    hCfgMap = 0;
  };

  //shared crypt mem
  if(0 != pCryptSharedMem)
  {
    ::UnmapViewOfFile(pCryptSharedMem);
    pCryptSharedMem = 0;
  };
  if(0 != hCryptSharedMap)
  {
    ::CloseHandle(hCryptSharedMap);
    hCryptSharedMap = 0;
  };
};

static inline BOOL CALLBACK EnumCfgAppExitProc(HWND hwnd, LPARAM lCloseWndProcess)
{
  //look on process of finded wnd
  //if match - try close it's

  DWORD dwWndProcess = 0;
  ::GetWindowThreadProcessId(hwnd, &dwWndProcess);
  if(dwWndProcess == static_cast<DWORD>(lCloseWndProcess))
  {
    //some process - need close
    CHAR cpClass[egcWndClassTextLen];
    cpClass[0] = '\0';
    ::GetClassName(hwnd, cpClass, COUNTOF(cpClass));
    if(false != ASCompareClassSimple(cpcWndDlgClass, cpClass))
    {
      //dlg
      ::PostMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDCANCEL, BN_CLICKED), 0);
    };
  };

  return TRUE;
};

void CApp::TrayIconMessageFunction(LPARAM lParam)
{
  switch(lParam)
  {
  case WM_RBUTTONDOWN:
    {
      using slw::CSLWMenu;
      CSLWMenu Menu(IDR_CONTEXT_MENU, hInst);
      CSLWMenu SubMenu(Menu.GetSubMenu(0));
      //default
      SubMenu.MarkDefault(ID_TRAY_CFG);
      //check enable/disable cfg
      ::CheckMenuItem(SubMenu, ID_TRAY_ENABLE_AUTOSHRINK, MF_BYCOMMAND | ((FALSE != pCfgMem->common64.bAHideEnabled) ? MF_CHECKED : MF_UNCHECKED));
      ::CheckMenuItem(SubMenu, ID_TRAY_ENABLE_BIDIRECTION, MF_BYCOMMAND | ((FALSE != pCfgMem->common64.bBDScrollingEnabled) ? MF_CHECKED : MF_UNCHECKED));
      ::CheckMenuItem(SubMenu, ID_TRAY_ENABLE_INVERCE, MF_BYCOMMAND | ((FALSE != pCfgMem->common64.bScrollInv) ? MF_CHECKED : MF_UNCHECKED));
      ::CheckMenuItem(SubMenu, ID_TRAY_ENABLE_DIRECTSCROLL, MF_BYCOMMAND | ((FALSE != pCfgMem->common64.bScrollDisabled) ? MF_CHECKED : MF_UNCHECKED));

      //control type
      ::CheckMenuRadioItem
        (
        SubMenu,
        ID_TRAY_ENABLE_CTYPE_STANDARD,
        ID_TRAY_ENABLE_CTYPE_MIX,
        (COUNTOF(cpcScrollControlCommands) > pCfgMem->common64.dwControlType) ? cpcScrollControlCommands[pCfgMem->common64.dwControlType] : ID_TRAY_ENABLE_CTYPE_STANDARD,
        MF_BYCOMMAND
        );
      //registered option's
      if(FALSE == pCfgMem->bCryptFirstLayerValid)
      {
        ::EnableMenuItem(SubMenu, ID_TRAY_ENABLE_BIDIRECTION, MF_BYCOMMAND | MF_GRAYED);
      };
      //switch result
      bool bSaveCfg = true;
      switch(SubMenu.TrackTray(pCfgMem->common64.hMainHostWnd))
      {
      case ID_TRAY_ENABLE_CTYPE_STANDARD:
        pCfgMem->common64.dwControlType = escStandard;
        break;

      case ID_TRAY_ENABLE_CTYPE_TOGGLE:
        pCfgMem->common64.dwControlType = escToggleWithEat;
        break;

      case ID_TRAY_ENABLE_DIRECTSCROLL:
        pCfgMem->common64.bScrollDisabled = !pCfgMem->common64.bScrollDisabled;
        TrayAnimProcess(true);
        break;

      case ID_TRAY_ENABLE_CTYPE_MIX:
        pCfgMem->common64.dwControlType = escScrollModeMix;
        break;

        //inv scrolling
      case ID_TRAY_ENABLE_INVERCE:
        pCfgMem->common64.bScrollInv = !pCfgMem->common64.bScrollInv;
        break;

        //flip bidirectional
      case ID_TRAY_ENABLE_BIDIRECTION:
        pCfgMem->common64.bBDScrollingEnabled = !pCfgMem->common64.bBDScrollingEnabled;
        break;

        //flip hide
      case ID_TRAY_ENABLE_AUTOSHRINK:
        bSaveCfg = false;
        AHideFlipSwitch();
        TrayAnimProcess(true);
        break;

        //cfg
      case ID_TRAY_CFG:
        bSaveCfg = false;
        StartCfgApp();
        break;

        //quit
      case ID_TRAY_EXIT:
        bSaveCfg = false;
        PostMessage(pCfgMem->common64.hMainHostWnd, WM_CLOSE, 0, 0);
        break;

      default:
        bSaveCfg = false;
        break;
      };
      if(false != bSaveCfg)
      {
        ::PostMessage(pCfgMem->common64.hMainHostWnd, emcNeedSaveCfg, 0, 0);
      };
    };
    break;

    //process left click
  case WM_LBUTTONDBLCLK:
  case WM_LBUTTONDOWN:
    if(false == bTrayFirstClickFlag)
    {
      //mem this click and start timer look
      bTrayFirstClickFlag = true;
      ulTrayFirstClickTimerTime = 0;
      ::GetCursorPos(&TrayDClickFirst);
    }
    else
    {
      //look for "click diff"
      POINT CursorPos;
      ::GetCursorPos(&CursorPos);
      bTrayFirstClickFlag = false;
      if
        (
        TrayDClickFirst.x - lcTrayDClickShiftX > CursorPos.x
        || TrayDClickFirst.x + lcTrayDClickShiftX < CursorPos.x
        || TrayDClickFirst.y - lcTrayDClickShiftY > CursorPos.y
        || TrayDClickFirst.y + lcTrayDClickShiftY < CursorPos.y
        )
      {
        //SClick
        TrayClickProcess();
      }
      else
      {
        //DClick
        StartCfgApp();
      };
    };
    break;

  default:
    break;
  };

};

void CApp::StartCfgApp(const bool bcLikeQuickTour)
{
  if(false != bcLikeQuickTour || false == TryShowCfgApp(pCfgMem))
  {
    //current cfg app handle is not valid - create new process

    //build bull cfg app name
    TCHAR cpFullName[MAX_PATH];
    GetFullBasedName(cpcCfgAppFileName, cpFullName, hInst);
    //try start process
    using sl::CSLProcess;
    CSLProcess CfgPrc;
    if(false == CfgPrc.Create(cpFullName, (false != bcLikeQuickTour) ? cpcCmdKeyQuickTour : 0, 0))
    {
      //can't start cfg process
      MsgBox(IDS_MSG_CFG_NOT_START, MB_OK | MB_ICONERROR);
    };
  };
};

void CApp::TrayAnimProcess(const bool bcForceCycle)
{
  if(FALSE != pCfgMem->common64.bEnableTrayIcon || FALSE == pCfgMem->bCryptFirstLayerValid)
  {
    ////need process tray icon
    //look on "residual" time for tray animation
    //if it's great then "timer" time - decrement on "timer" time
    //else - switch to "other" state, set tray icon, gen new "anim" random value

    //residual time
    if(false != bcForceCycle || false == bTrayIconPresent || eccTimerTime >= dwTrayAnimWaitTime)
    {
      //do switch
      if(false == bcForceCycle && false != bTrayIconPresent)
      {
        bTrayAnimEyesClosed = !bTrayAnimEyesClosed;
        //new time (bTrayAnimEyesClosed in new state)
        dwTrayAnimWaitTime = (false != bTrayAnimEyesClosed)
          ? MyRnd(eccTrayAnimCloseMinTime, eccTrayAnimCloseMaxTime)
          : MyRnd(eccTrayAnimOpenMinTime, eccTrayAnimOpenMaxTime);
      }
      else
      {
        //show new icon or "force" cycle
        bTrayAnimEyesClosed = false;
      };

      //look on "tray icon special disable" (DirectScroll) state
      const bool bcTrayIconSpecialDistabled = false == TrayIconSpecialState();

      //set icon
      const UINT uiOpenEyeIcon = (false != bcTrayIconSpecialDistabled)
        ? IDI_TRAY_MAIN_3_ICON
        : IDI_TRAY_MAIN_1_ICON;
      const UINT uicRes = (FALSE == pCfgMem->common64.bEnableTrayIconAnim)
        ? uiOpenEyeIcon
        : ((false != bTrayAnimEyesClosed) ? IDI_TRAY_MAIN_2_ICON : uiOpenEyeIcon);
      const HICON hcIcon = ::LoadIcon(hInst, MAKEINTRESOURCE(uicRes));
      _ASSERT(0 != hcIcon);
      TrayIconSet(hcIcon, 0, false == bTrayIconPresent);
      bTrayIconPresent = true;
    }
    else
    {
      //calc timer time
      dwTrayAnimWaitTime -= eccTimerTime;
    };
  }
  else
  {
    //need hide tray icon
    if(false != bTrayIconPresent)
    {
      ::Shell_NotifyIcon(NIM_DELETE, &TrayIconData);
      bTrayIconPresent = false;
    };
  };
};

void CApp::NewAppInfoCatch(const DWORD dwcProcessId, HANDLE hNameMap)
{
  __CProcessInfoStr* pNewAppInfo = 0;
  if(FALSE != pCfgMem->bNotSendModuleFileName)
  {
    //get name from mapping
    //insert name to process list
    //close mapping
    //do rescan

    //name
    LPCSTR const cpcAppName = reinterpret_cast<LPCSTR>(::MapViewOfFile(hNameMap, FILE_MAP_READ, 0, 0, 0));
    if(0 != cpcAppName)
    {
      //extract short file name
      LPCSTR pPtr = cpcAppName;
      LONG_PTR lLen = ::lstrlen(pPtr);
      ScanAppNameAndExtractName(pPtr, lLen);
      //insert info (if present already - chng)
      pNewAppInfo = ProcessMapList.Find(dwcProcessId);
      if(0 == pNewAppInfo)
      {
        //if info not have already - insert
        pNewAppInfo = ProcessMapList.Add(dwcProcessId);
      };
      pNewAppInfo->SetAppName(pPtr, lLen + 1);

      //close
      ::CloseHandle(hNameMap);
    };
  }
  else
  {
    //processing for "special name getters"

    //if some app already present - flush such info
    if(0 != ProcessMapList.Find(dwcProcessId))
    {
      ProcessMapList.Remove(dwcProcessId);
    };
  };
};


LRESULT CApp::MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  LRESULT lRes = 0;
  bool bCatched = true;

  switch(uMsg)
  {
  case emcNewAppStarted:
    NewAppInfoCatch(wParam, reinterpret_cast<HANDLE>(lParam));
    break;

    //chng scr size
  case WM_DISPLAYCHANGE:
    pCfgMem->ScrSize.x = LOWORD(lParam);
    pCfgMem->ScrSize.y = HIWORD(lParam);
    break;

    //reg info entered from cfg application
  case emcRegInfoEnter:
    LoadRegInfo(false);
    break;

    //quit
  case WM_DESTROY:
  case WM_QUERYENDSESSION:
    //send quit message to main cfg dlg (if present)
    if(FALSE != ::IsWindow(pCfgMem->common64.hCfgAppWnd))
    {
      DWORD dwCfgProcess = 0;
      ::GetWindowThreadProcessId(pCfgMem->common64.hCfgAppWnd, &dwCfgProcess);
      //enum wnd's in system from cfg process and if find - close it's
      ::EnumWindows(EnumCfgAppExitProc, dwCfgProcess);
    };
    //del timer
    if(0 != uiTimerId)
    {
      ::KillTimer(0, uiTimerId);
      uiTimerId = 0;
    };
    ////remove subclassing
    UninstallSubClassing();
    //wait short time for exit all subclassing
    pCfgMem->hHookPresentFlag = 0;
    //post msg for quit
    PostQuitMessage(0);
    bCatched = false;
    break;

  case emcAppExited:
    ProcessMapList.Remove(lParam);
    break;

    //auto hide
  case emcAHWndActive:
    AHWndActivisation(reinterpret_cast<HWND>(lParam), FALSE != wParam);
    break;

    //save cfg
  case emcNeedSaveCfg:
    SaveCfg();
    break;

    //app name queried from CFG app
  case emcGetAppNameById:
    {
      const __CProcessInfoStr* pInfo = ProcessMapList.Find(lParam);
      //return to caller
      if(0 != pInfo)
      {
        lstrcpy(pCfgMem->common64.cpGetAppNameResultBuff, pInfo->pAppName);
        lRes = TRUE;
      };
    };
    break;

  case emcAHWndActivity:
    AHWndActivity(reinterpret_cast<HWND>(lParam));
    break;

  case emcAHWndShow:
    AHWndShow(reinterpret_cast<HWND>(lParam), wParam);
    break;

  case emcAHWndHide:
    AHWndHide(reinterpret_cast<HWND>(lParam));
    break;

  case emcAHWndDestroy:
    AHWndDestroy(reinterpret_cast<HWND>(lParam));
    break;

  case emcReReadWndPattList:
    AHReadWndPatternList(true, FALSE != lParam);
    break;

    //tray message
    //wParam - tray icon id
    //lParam - mouse message code
  case emcToHostTrayIconNotifyMsg:
    TrayIconMessageFunction(lParam);
    break;

  default:
    bCatched = false;
    break;
  };

  return (false != bCatched)
    ? lRes
    : DefWindowProc(hwnd, uMsg, wParam, lParam);
};

void CApp::AHTimerLooking()
{
  //Autoshrink timer work only while lHookMode is in NoNe
  //scan "timer looking" wnd list and increment looking time count
  //try find wnd in ProcessWndList - if not find or wnd not valid wnd - remove from timer list
  //if wnd locked - continue
  //try build "FromMouseToTop" set (if not already) and try find wnd on it
  //if find - zero "WndLastLooking" timer count
  //else - increment WndLastLooking timer count - if more then specified - hide wnd
  //if not find - remove and hide wnd

  if(FALSE != pCfgMem->common64.bAHideEnabled && ehmNone == pCfgMem->lHookMode)
  {
    //FromMouseToTop wnd set
    bool bSetBuilded = false;

    //scan timer list and look on wnd's info
    TimerLookingListIterType Iter = TimerLookingList.IterBegin();
    while(false != TimerLookingList.IterIsOk(Iter))
    {
      const HWND hcLookWnd = *(TimerLookingList.IterGet(Iter));
      __CAHWndInfo* const cpInfo = ProcessWndList.Find(hcLookWnd);

      //if wnd not in "ProcessWndList" - remove and goto next
      bool bRemove = false;
      if(FALSE == ::IsWindow(hcLookWnd) || 0 == cpInfo)
      {
        bRemove = true;
        ProcessWndList.Remove(hcLookWnd);
      };

      //now analyse
      if(false == bRemove)
      {
        if(false == cpInfo->bDelayActivateLook)
        {
          ////wnd open and activated
          //try find - is mouse pnt in wnd
          if(false == cpInfo->bActivateLock)
          {
            //now build "FromMouseToTop" set (if not already)
            if(false == bSetBuilded)
            {
              TimerBuildActiveSetHelper();
              bSetBuilded = true;
            };

            //look on "mouse in wnd" situation
            if(0 != TimerActiveWndSet.Find(cpInfo->hcWnd))
            {
              //mouse in wnd - zero count
              cpInfo->dwTimerLastLookCount = 0;
            }
            else
            {
              //mouse not in wnd - increment count and look on "is wnd need hide"
              cpInfo->dwTimerLastLookCount += eccTimerTime;
              if(pCfgMem->common64.dwAutoHideTimerTime < cpInfo->dwTimerLastLookCount)
              {
                cpInfo->dwTimerLastLookCount = 0;
                //hide
                if(false == cpInfo->bWndClosed)
                {
                  AHCloseWnd(cpInfo);
                };
                bRemove = true;
              };
            };
          };
        }
        else
        {
          ////wnd currently open - try process "delay activation"
          //now build "FromMouseToTop" set (if not already)
          if(false == bSetBuilded)
          {
            TimerBuildActiveSetHelper();
            bSetBuilded = true;
          };

          ////try find wnd in "activate" set
          if(0 != TimerActiveWndSet.Find(cpInfo->hcWnd))
          {
            //mouse in wnd - increment activation timer if time elapsed - activate wnd
            cpInfo->dwTimerLastLookCount += eccTimerTime;
            if(pCfgMem->common64.dwAutoOpenTimerTime <= cpInfo->dwTimerLastLookCount)
            {
              //delay activisation time elapsed - activate wnd now
              cpInfo->bDelayActivateLook = false;
              bRemove = true;
              AHWndActivityHelper(cpInfo);
            };
          }
          else
          {
            //mouse not in wnd - remove from "delay activation"
            cpInfo->bDelayActivateLook = false;
            bRemove = true;
          };
        };
      };

      if(false != bRemove)
      {
        //remove
        TimerLookingList.IterRemove(Iter);
        if(false != TimerLookingList.IterIsOk(Iter))
        {
          //next
          continue;
        }
        else
        {
          //all end
          break;
        };
      };

      Iter = TimerLookingList.IterNext(Iter);
    };
  };
};

void CApp::AHReadWndPatternList(const bool bcDoRescan, const bool bcReadCfg)
{
  //empty current
  //get from registry
  //scann all wnd's in system for "is wnd can be processing by wnd pattern"

  if(false != bcReadCfg)
  {
    //empty
    WndPatternList.FreeAll();

    //open cfg reg key
    using sl::CSLCfgReg;
    CSLCfgReg Reg;
    if(false != Reg.Open(cpcCfgKeyName, true))
    {
      //number of pattern's
      DWORD dwNumb = 0;
      Reg.GetVal(cpcCfgWPatternsMainName, dwNumb, 0);
      //iterate
      for(DWORD dwCount = 0; dwNumb > dwCount; dwCount++)
      {
        //entry name
        TCHAR cpBuff[20];
        wsprintf(cpBuff, cpcCfgWPatternsEntryMask, dwCount);
        //try open entry val
        DWORD dwEntryLen = 0;
        if(false != Reg.GetValSize(cpBuff, dwEntryLen))
        {
          //alloc buffer
          using sl::CSLAutoPtr;
          CSLAutoPtr<CHAR> pData = new CHAR[dwEntryLen];
          //try get data
          if(false != Reg.ReadStr(cpBuff, pData.GetPtr(), dwEntryLen))
          {
            //insert to list
            WndPatternList.Push(pData.Detach());
          };
        };
      };
    };
  };

  //start scan all wnd's (by apply pattern's list)
  if(false != bcDoRescan)
  {
    ScannProcessWnds(0);
  };
};

void CApp::ScannProcessWnds(const DWORD dwcProcessId)
{
  __CScanProcessInfo Info = {this, dwcProcessId};
  ::EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&Info));
};

void CApp::ScannAllAnaliseWnd(const HWND hcWnd, bool& rbDrillDown, const DWORD dwcProcessId)
{
  //look on passed wnd process (if some as passed (or passed null))
  //if ok - look on passed wnd style
  //if style ok - process by wnd pattern's
  //try find "already" in "process wnd list"
  //if "neeing" chnaged (not->yes or yes->not) - apply insert/remove from autohide process

  //easy
  bool bDoNext = true;

  //process
  DWORD dwWndProc = 0;
  ::GetWindowThreadProcessId(hcWnd, &dwWndProc);
  if(false != bDoNext)
  {
    if(0 != dwcProcessId)
    {
      bDoNext = dwcProcessId == dwWndProc;
      rbDrillDown = bDoNext;
    }
    else
    {
      rbDrillDown = true;
    };
  };


  //style
  if(false != bDoNext && FALSE != AHideCheckWndStyle(hcWnd))
  {
    const BOOL bcVisible = ::IsWindowVisible(hcWnd);
    //process by pattern's
    const __CAHWndPatternInfo* pcPatt = 0;
    bool bSomeProcFind = false;
    const bool bcFindInPattern = AHCheckWndByPatternList(hcWnd, bSomeProcFind, pcPatt);
    //"aleady"
    __CAHWndInfo* pInfo = ProcessWndList.Find(hcWnd);
    //look on "state chnged" situation (need some doing)
    if((0 != pInfo) != bcFindInPattern)
    {
      if(0 != pInfo)
      {
        //need remove
        //open wnd
        if(FALSE != bcVisible)
        {
          AHOpenWnd(pInfo, false, true, false);
        };
        //remove from wnd list
        ProcessWndList.Remove(hcWnd);
      }
      else
      {
        //need insert
        //to process list
        pInfo = ProcessWndList.Add(hcWnd);
        pInfo->Init(pcPatt);
        if(FALSE != bcVisible)
        {
          //check new find wnd for "is it's "active"
          pInfo->bActivateLock = false != CheckWndActive(hcWnd);
          //to timer list
          TimerLookingList.Push(hcWnd);
        };
      };
    }
    else if(false != bcFindInPattern && 0 != pInfo && pInfo->dwShrinkMethod != pcPatt->dwType)
    {
      ////pattern not deleted/removed - only type chnaged
      //open wnd and chng shrink type
      if(FALSE != bcVisible)
      {
        AHOpenWnd(pInfo, false, true, true);
      };
      pInfo->dwShrinkMethod = pcPatt->dwType;
    };
  };
};

BOOL CApp::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
  const __CScanProcessInfo* const cpcInfo = reinterpret_cast<const __CScanProcessInfo*>(lParam);
  return cpcInfo->pThis->EnumWindowsProcThis(hwnd, cpcInfo->dwProcessId);
};

BOOL CApp::EnumWindowsProcThis(HWND hwnd, const DWORD dwcProcessId)
{
  //pass wnd to analiser proc
  //if "need drilling" - drill down

  bool bDrillDown = false;
  ScannAllAnaliseWnd(hwnd, bDrillDown, dwcProcessId);
  if(false != bDrillDown)
  {
    ::EnumChildWindows(hwnd, EnumChildWindowsProc, reinterpret_cast<LPARAM>(this));
  };

  return TRUE;
};

BOOL CApp::EnumChildWindowsProc(HWND hwnd, LPARAM lParam)
{
  return reinterpret_cast<CApp*>(lParam)->EnumChildWindowsProcThis(hwnd);
};

BOOL CApp::EnumChildWindowsProcThis(HWND hwnd)
{
  //pass wnd to analiser proc
  //if "need drilling" - drill down

  bool bDrillDown = false;
  ScannAllAnaliseWnd(hwnd, bDrillDown, 0);
  if(false != bDrillDown)
  {
    ::EnumChildWindows(hwnd, EnumChildWindowsProc, reinterpret_cast<LPARAM>(this));
  };

  return TRUE;
};

bool CApp::AHCheckWndByPatternList(const HWND hcWnd, bool& rFindSimpleProcessInPattenrs, const __CAHWndPatternInfo*& rpcFindPatt)
{
  //get wnd info and try find some in Pattern's list
  rpcFindPatt = 0;

  //easy
  bool bRes = false;
  rFindSimpleProcessInPattenrs = false;

  //only is passed is wnd and AShrink enabled
  if(FALSE != pCfgMem->common64.bAHideEnabled && FALSE != ::IsWindow(hcWnd))
  {
    //wnd info
    //app name
    __CProcessInfoStr* pAppInfo = 0;
    {
      DWORD dwProcessId = 0;
      ::GetWindowThreadProcessId(hcWnd, &dwProcessId);
      //find already
      pAppInfo = ProcessMapList.Find(dwProcessId);
      if(0 == pAppInfo && 0 != pProcessFindNameFunc)
      {
        (this->*pProcessFindNameFunc)(dwProcessId);
        pAppInfo = ProcessMapList.Find(dwProcessId);
      };
    };
    if(0 != pAppInfo)
    {
      //title
      using sl::CSLAutoPtr;
      const DWORD dwcTextLen = ::GetWindowTextLength(hcWnd) + 1;
      CSLAutoPtr<CHAR> pText(new CHAR[dwcTextLen]);
      *pText = '\0';
      ::GetWindowText(hcWnd, pText.GetPtr(), dwcTextLen);
      //class
      TCHAR cpClass[egcWndClassTextLen];
      *cpClass = '\0';
      if(0 != ::GetClassName(hcWnd, cpClass, COUNTOF(cpClass)))
      {
        //cycle and look
        for(WndPatternListIterType Iter = WndPatternList.IterBegin(); false != WndPatternList.IterIsOk(Iter); Iter = WndPatternList.IterNext(Iter))
        {
          const __CAHWndPatternInfo* const cpcPattern = WndPatternList.IterGet(Iter);
          //compare app
          if(0 == *(cpcPattern->pApp) || false != ASCompareAppName(cpcPattern->pApp, pAppInfo->pAppName))
          {
            rFindSimpleProcessInPattenrs = true;
            //class
            if(0 == *(cpcPattern->pClass) || false != ASCompareClass(cpcPattern->pClass, cpClass))
            {
              //title
              if(0 == *(cpcPattern->pTitle) || false != ASCompareTitle(cpcPattern->pTitle, pText))
              {
                bRes = true;
                rpcFindPatt = cpcPattern;
                break;
              };
            };
          };
        };
      };
    };
  };

  return bRes;
};

UINT CApp::MsgBox(LPCSTR const cpcMsg, const UINT uicType) const
{
  CHAR cpTitle[50];
  SLCHECK(0 != ::LoadString(hInst, IDS_STR_MSG_TITLE, cpTitle, COUNTOF(cpTitle)));
  return ::MessageBox((0 == pCfgMem) ? 0 : pCfgMem->common64.hMainHostWnd, cpcMsg, cpTitle, uicType | MB_SETFOREGROUND | MB_TOPMOST);
};

UINT CApp::MsgBox(const UINT uicResId, const UINT uicType) const
{
  using slw::SLWMessageBox;
  return SLWMessageBox((0 == pCfgMem) ? 0 : pCfgMem->common64.hMainHostWnd, uicResId, IDS_STR_MSG_TITLE, uicType | MB_SETFOREGROUND | MB_TOPMOST, hInst);
};

void CApp::TimerThunkProc(HWND hWnd, UINT uiMsg, UINT_PTR, DWORD)
{
  AHTimerLooking();
  TrayClickTimerSensor(eccTimerTime);
  TrayAnimProcess(false);
  pCfgMem->common64.llTotalRunTime += eccTimerTime;
  TimerSaveCfgProcess();
};

void CApp::TrayIconSet(HICON hIcon, LPCTSTR const cpcTipText, const bool bcForceShow)
{
  //look on "already" icon (for avoid flicking)
  //fill data
  //set flag
  //chng tray icon data

  //already
  if(hIcon != TrayIconData.hIcon)
  {
    //fill
    TrayIconData.hIcon = hIcon;
    TrayIconData.uFlags = NIF_ICON;
    if(0 != cpcTipText)
    {
      ::lstrcpyn(TrayIconData.szTip, cpcTipText, COUNTOF(TrayIconData.szTip));
      TrayIconData.uFlags |= NIF_TIP;
    };

    //chng
    if(false != bcForceShow || FALSE == ::Shell_NotifyIcon(NIM_MODIFY, &TrayIconData))
    {
      TrayIconData.uFlags |= NIF_MESSAGE | NIF_TIP;
      ::Shell_NotifyIcon(NIM_ADD, &TrayIconData);
    };
  };
};

void CApp::ProcessFindNamePSApi(const DWORD dwcProcessId)
{
  HANDLE hProcess = ::OpenProcess
    (
    PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
    FALSE,
    dwcProcessId
    );
  if(0 != hProcess)
  {
    //getmain module handle of finded process
    HMODULE hMainProcessModule = 0;
    DWORD dwResMainProcessModuleSize = 0;
    if(FALSE != pEnumProcessModulesFunc(hProcess, &hMainProcessModule, sizeof(hMainProcessModule), &dwResMainProcessModuleSize))
    {
      TCHAR cpModuleName[MAX_PATH];
      DWORD dwModuleLen = pGetModuleBaseNameFunc(hProcess, hMainProcessModule, cpModuleName, sizeof(cpModuleName));
      if(0 != dwModuleLen)
      {
        __CProcessInfoStr* pNewAppInfo = ProcessMapList.Find(dwcProcessId);
        if(0 == pNewAppInfo)
        {
          pNewAppInfo = ProcessMapList.Add(dwcProcessId);
        };
        pNewAppInfo->SetAppName(cpModuleName, dwModuleLen + 1);
      };
    };

    ::CloseHandle(hProcess);
  };
};

void CApp::ProcessEnumCloseAllPSApi()
{
  ::FreeLibrary(hPSApiModule);
  hPSApiModule = 0;
};

void CApp::ProcessFindNameToolHelp(const DWORD dwcProcessId)
{
  HANDLE hSnapShort = pCreateToolhelp32Snapshot
    (
    TH32CS_SNAPPROCESS,
    dwcProcessId
    );
  if(reinterpret_cast<HANDLE>(-1) != hSnapShort)
  {
    //enum processes info
    PROCESSENTRY32 ProcessInfo;
    ZeroMemory(&ProcessInfo, sizeof(ProcessInfo));
    ProcessInfo.dwSize = sizeof(ProcessInfo);
    BOOL bRes = pProcess32First(hSnapShort, &ProcessInfo);
    if(FALSE != bRes)
    {
      ProcessMapList.FreeAll();
    };
    while(FALSE != bRes)
    {
      //process module name
      LPCTSTR pcNamePos = ProcessInfo.szExeFile;
      LONG_PTR lLen = ::lstrlen(pcNamePos);
      ScanAppNameAndExtractName(pcNamePos, lLen);
      //if info not have already - insert
      __CProcessInfoStr* pNewAppInfo = ProcessMapList.Add(ProcessInfo.th32ProcessID);
      pNewAppInfo->SetAppName(pcNamePos, lLen + 1);

      bRes = pProcess32Next(hSnapShort, &ProcessInfo);
    };

    ::CloseHandle(hSnapShort);
  };
};

void CApp::ProcessEnumCloseAllToolHelp()
{
  ::FreeLibrary(hToolHelpModule);
  hToolHelpModule = 0;
};


DWORD GetProcessIntegrityLevel()
{
    HANDLE hToken;
    HANDLE hProcess;

    DWORD dwLengthNeeded;
    DWORD dwError = ERROR_SUCCESS;

    PTOKEN_MANDATORY_LABEL pTIL = NULL;
    DWORD dwIntegrityLevel = -1;

    hProcess = GetCurrentProcess();
    if (OpenProcessToken(hProcess, TOKEN_QUERY, &hToken)) 
    {
        // Get the Integrity level.
        if (!GetTokenInformation(hToken, TokenIntegrityLevel, 
            NULL, 0, &dwLengthNeeded))
        {
            dwError = GetLastError();
            if (dwError == ERROR_INSUFFICIENT_BUFFER)
            {
                pTIL = (PTOKEN_MANDATORY_LABEL)LocalAlloc(0, 
                    dwLengthNeeded);
                if (pTIL != NULL)
                {
                    if (GetTokenInformation(hToken, TokenIntegrityLevel, 
                        pTIL, dwLengthNeeded, &dwLengthNeeded))
                    {
                        dwIntegrityLevel = *GetSidSubAuthority(pTIL->Label.Sid, 
                            (DWORD)(UCHAR)(*GetSidSubAuthorityCount(pTIL->Label.Sid)-1));
                    }
                    LocalFree(pTIL);
                }
            }
        }
        CloseHandle(hToken);
    }

    return dwIntegrityLevel;
}

bool CApp::CheckIntegrityLevel()
{
    DWORD processIntegrityLevel = GetProcessIntegrityLevel();

    if (processIntegrityLevel == -1)
    {
        return true;
    }

    if (processIntegrityLevel < SECURITY_MANDATORY_HIGH_RID)
    {
        StartMImpSrv();
        return false;
    }

    return true;
}

