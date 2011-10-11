/**********************************************************************
main functionaly
**********************************************************************/

#ifndef __APP_H
#define __APP_H

#pragma once

#include "..\Include\MIGlobal.h"

#include "..\\SLib\\SLThunk.h"

#include "..\\Slib\\SLCont.h"

#include "..\\SLib\\SLDbg.h"

#include "..\\SLib\\SLAutoPtr.h"

#include "..\\PSApiAdd\\PSApiAdd.h"

#include "..\\PSApiAdd\\ToolHelpAdd.h"


class CApp;
extern CApp* pApp;
struct __CAppPattInfo;
struct __CAHWndPatternInfo;

using sl::CSLThunk;
using sl::CSLHashSet;
using sl::__SLHashFunc;
using sl::CSLSList;
using sl::CSLAutoPtr;

//////////////////////////////////////////////////////////////////////
//CAutoHideWndInfo - info about "AutoHide" wnd

class __CAHWndInfo
{
protected:

public:

  //handle of wnd
  const HWND hcWnd;
  //size (vertical) of wnd in "open" state
  //"is window closed" flag (wnd is hided and lOpenVSize contain size of wnd)
  bool bWndClosed;
  //activisation "lock" flag (set in WM_ACTIVATE process and lock "hide wnd" by timer)
  bool bActivateLock;
  //is wnd "delay activated" flag
  bool bDelayActivateLook;
  //count for "timer elapse interval" (incremented by eccTimerTime on each timer elapsing and on greating pCfgMem->dwAutoHideTimerTime - hide close process)
  DWORD dwTimerLastLookCount;
  //time for "close" time (from ::GetTickCount()) - used for calc "wnd closed" time in AShrink productivity
  DWORD dwCloseWndTime;
  //shrink method - from ShrinkTypeEnum
  DWORD dwShrinkMethod;

public:

  explicit inline __CAHWndInfo(const HWND hcInitWnd);
  inline ~__CAHWndInfo();

  //init wnd info by pattern
  inline void Init(const __CAHWndPatternInfo* const cpcInitPatt);

  //for conainter's
  inline bool operator==(const HWND hcWnd);

protected:
};

//////////////////////////////////////////////////////////////////////
//__CAHWndPatternInfo - info for filter new activated wnd's

struct __CAHWndPatternInfo
{
  //app name - begin of attached dynamic addr
  LPSTR pApp;
  //class name
  LPSTR pClass;
  //title
  LPSTR pTitle;
  //shrink type
  DWORD dwType;
  //create and attach data
  explicit inline __CAHWndPatternInfo(LPSTR const cpAttachMem);
  //destroy
  inline ~__CAHWndPatternInfo();
  //build class helper (from constructor)
protected:
  static inline void FindDelimHelper(LPSTR const cpFrom, LPSTR& rpcTo);
};

//////////////////////////////////////////////////////////////////////
//information about process

struct __CProcessInfoStr
{
  //name of application
  LPSTR pAppName;
  //id of process
  DWORD dwProcessId;
  //init
  explicit inline __CProcessInfoStr(const DWORD dwcIintProcessId);
  //finid
  inline ~__CProcessInfoStr();
  //for conainter's
  inline bool operator==(const DWORD dwcCompProcess);
  //set app name
  inline void SetAppName(LPCSTR const cpcInitName, const DWORD dwcInitNameLen);
};


//////////////////////////////////////////////////////////////////////
//CApp

class CApp
{
protected:

  //const's
  enum ClassConstEnum
  {
    //timer
    eccTimerTime = 250,
    //null splash color
    eccNullSplashColor = RGB(0, 255, 0),
    //number milisec's in sec
    eccMilSecInSec = 1000,
    //tray animation time's
    eccTrayAnimOpenMinTime = 100,
    eccTrayAnimOpenMaxTime = 10000,
    eccTrayAnimCloseMinTime = 100,
    eccTrayAnimCloseMaxTime = 500,
  };

  //app instace
  HINSTANCE hInst;

  //global message
  const UINT uicGlbMsg;

  //hook module handle
  HINSTANCE hHookLib;

  //wnd proc thunk
  typedef CSLThunk<CApp> AppClassThunkType;
  AppClassThunkType WndProcThunk;

  //instance of global shared cfg
  CMISharedInfo* pCfgMem;
  HANDLE hCfgMap;
  //for store crypt data
  BYTE* pCryptSharedMem;
  HANDLE hCryptSharedMap;

  //try icon data
  NOTIFYICONDATA TrayIconData;

  //information about process's - used for compare wnd by wnd pattern's
  typedef CSLHashSet<const DWORD, __CProcessInfoStr, 4> ProcessMapListType;
  ProcessMapListType ProcessMapList;
  typedef void (CApp::*ProcessFindNameFuncType)(const DWORD dwcProcessId);
  typedef void (CApp::*ProcessCloseAllFuncType)();
  ProcessFindNameFuncType pProcessFindNameFunc;
  ProcessCloseAllFuncType pProcessCloseAllFunc;
  //PSApi data section
  HINSTANCE hPSApiModule;
  EnumProcessesType pEnumProcessesFunc;
  EnumProcessModulesType pEnumProcessModulesFunc;
  GetModuleBaseNameType pGetModuleBaseNameFunc;
  //tool help
  HINSTANCE hToolHelpModule;
  Process32FirstType pProcess32First;
  Process32NextType pProcess32Next;
  CreateToolhelp32SnapshotType pCreateToolhelp32Snapshot;


  //////////////////////////////////////////////////////////////////////
  //AutoHide section

  //set of "hided" window's
  typedef CSLHashSet<const HWND, __CAHWndInfo, 4> ProcessWndListType;
  typedef ProcessWndListType::iterator ProcessWndListIterType;
  ProcessWndListType ProcessWndList;
  //list of wnd pattern's
  typedef CSLSList<LPSTR, __CAHWndPatternInfo> WndPatternListType;
  typedef WndPatternListType::iterator WndPatternListIterType;
  WndPatternListType WndPatternList;
  //list of "timer looked" wnd's
  typedef CSLSList<const HWND> TimerLookingListType;
  typedef TimerLookingListType::iterator TimerLookingListIterType;
  TimerLookingListType TimerLookingList;

  //set for use in "timer looking" procedure - for make "from mouse pnt to top" wnd list
  typedef CSLHashSet<HWND, HWND, 4> TimerActiveWndSetType;
  TimerActiveWndSetType TimerActiveWndSet;

  //thunk for looking timer proc
  AppClassThunkType TimerProcThunk;
  //timer id
  UINT uiTimerId;

  //last value for ::GetTickCount(), looked from WndOpen - used for calc "application running" time
  DWORD dwLastLookAppTime;

  //tray animation process
  DWORD dwTrayAnimWaitTime;
  //is tray icon in "closed" state
  bool bTrayAnimEyesClosed;
  //is tray icon present
  bool bTrayIconPresent;

  //save cfg summ time - used in timer proc for calc estimated for save cfg time
  DWORD dwSaveCfgSummTime;

  //tray click detection section
  const ULONG ulcTrayDClickTime;
  const LONG lcTrayDClickShiftX;
  const LONG lcTrayDClickShiftY;
  POINT TrayDClickFirst;
  bool bTrayFirstClickFlag;
  ULONG ulTrayFirstClickTimerTime;

public:

public:

  //init and create
  CApp(HINSTANCE hInitInstance);
  ~CApp();


  //init application
  bool Init(bool& rNewInited);
  //finti app
  void Finit();
  //start app and it's message cycle
  void Run();

  //analyser for exit wnd enum proc's
  inline void AnalyseExitWndProc(const HWND hcWnd);

protected:

  //process detection
  void ProcessFindNamePSApi(const DWORD dwcProcessId);
  void ProcessEnumCloseAllPSApi();
  void ProcessFindNameToolHelp(const DWORD dwcProcessId);
  void ProcessEnumCloseAllToolHelp();

  //try uninstall all hook's (calsed from close host wnd)
  void UninstallSubClassing();

  //timer thunk proc
  void TimerThunkProc(HWND hWnd, UINT uiMsg, UINT uiEvent, DWORD dwTime);
  //build timer active looking set
  inline void TimerBuildActiveSetHelper();

  //try load and process registration info
  void LoadRegInfo(const bool bcFirstCheckOnly);

  //processor for save cfg
  inline void TimerSaveCfgProcess();

  //brign help
  inline void BringHelp(const UINT uicCmd, const DWORD dwcData);

  //calc distance in second's from current time, move current to passed ref
  inline DWORD CalcTimeInSec(DWORD& rdwLastTime);

  //scann all wnd's in system (from passed process (0 - all process's)) and look on "IsWnd can be AutoHided"
  void ScannProcessWnds(const DWORD dwcProcessId);

  //new app started - get info from message and set rescan for "new" applicaton (by it's new window (situation with "delayed hook installing"))
  void NewAppInfoCatch(const DWORD dwcProcessId, HANDLE hNameMap);

  //load cfg's from registry
  void LoadCfg(bool& rbFirstStart);
  //save cfg to registry
  void SaveCfg();

  //scann all wnd proc's
  //Enumwindows callback's
  static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
  BOOL EnumWindowsProcThis(HWND hwnd, const DWORD dwcProcessId);
  //analiser for enumed window
  void ScannAllAnaliseWnd(const HWND hcWnd, bool& rbDrillDown, const DWORD dwcProcessId);
  //EnumChildwindows callback's
  static BOOL CALLBACK EnumChildWindowsProc(HWND hwnd, LPARAM lParam);
  BOOL EnumChildWindowsProcThis(HWND hwnd);

  //app messagebox
  UINT MsgBox(const UINT uicResId, const UINT uicType) const;
  UINT MsgBox(LPCSTR const cpcMsg, const UINT uicType) const;

  //////////////////////////////////////////////////////////////////////
  //auto hide wnd section

  //wnd resive mouse activity
  inline void AHWndActivity(const HWND hcWnd);
  //activation helper (actual work)
  inline void AHWndActivityHelper(__CAHWndInfo* const cpInfo);
  //activation
  inline void AHWndActivisation(const HWND hcWnd, const bool bActivate);
  //wnd show on screen
  inline void AHWndHide(const HWND hcWnd);
  inline void AHWndShow(const HWND hcWnd, const BOOL bcActive);
  //wnd wnd destroy
  inline void AHWndDestroy(const HWND hcWnd);
  //timmer looking for "hide wnd"
  void AHTimerLooking();
  //close passed wnd (internal for all AShrink - actual open)
  inline void AHOpenWnd(__CAHWndInfo* const cpInfo, const bool bcActivateWnd, const bool bcNotTochWndPos = false, const bool bcPushToTimer = true);
  //calc benefit for passed wnd info - used in AHOpenWnd func
  inline void AHOpenCalcBenefit(__CAHWndInfo* const cpInfo);
  //open paseed wnd (internal for all AShrink - actual close)
  inline void AHCloseWnd(__CAHWndInfo* const cpInfo) const;
  //read wnd pattern's list and scann all wnd's in system for "is wnd can process patterns' list"
  void AHReadWndPatternList(const bool bcDoRescan, const bool bcReadCfg);
  //check window by Pattern's list
  //rFindSimpleProcessInPattenrs used for not drill down while scann "existing" wnd's
  bool AHCheckWndByPatternList(const HWND hcWnd, bool& rFindSimpleProcessInPattenrs, const __CAHWndPatternInfo*& rpcFindPatt);
  //invert auto hide "enabled" state (with all work)
  inline void AHideFlipSwitch();

  //try start cfg app
  void StartCfgApp(const bool bcLikeQuickTour = false);

  //tray animation processing
  void TrayAnimProcess(const bool bcForceCycle);
  //set tray icon
  void TrayIconSet(HICON hIcon, LPCTSTR const cpcTipText, const bool bcForceShow);
  //tray icon function
  void TrayIconMessageFunction(LPARAM lParam);
  //process "click" looking
  inline void TrayClickTimerSensor(const ULONG ulcTimerTime);
  inline void TrayClickProcess();
  inline bool TrayIconSpecialState() const;

  //main wnd proc
  LRESULT MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

/**********************************************************************
implementation
**********************************************************************/

//////////////////////////////////////////////////////////////////////
//__CAHWndPatternInfo

inline __CAHWndPatternInfo::__CAHWndPatternInfo(LPSTR const cpAttachMem)
  :pApp(cpAttachMem),
  pClass(""),
  pTitle("")
{
  FindDelimHelper(pApp, pClass);
  FindDelimHelper(pClass, pTitle);
  //find type
  LPSTR pTypeEnd = 0;
  FindDelimHelper(pTitle, pTypeEnd);
  dwType = (0 == pTypeEnd)
    ? estShrink
    : *pTypeEnd - '0';
};

inline __CAHWndPatternInfo::~__CAHWndPatternInfo()
{
  delete pApp;
};

inline void __CAHWndPatternInfo::FindDelimHelper(LPSTR const cpFrom, LPSTR& rpTo)
{
  for(LPSTR pLook = cpFrom; 0 != pLook && '\0' != *pLook; pLook++)
  {
    if('\n' == *pLook)
    {
      *pLook = '\0';
      rpTo = pLook + 1;
      break;
    };
  };
};


//////////////////////////////////////////////////////////////////////
//__CAHWndInfo

inline __CAHWndInfo::__CAHWndInfo(const HWND hcInitWnd)
  :hcWnd(hcInitWnd),
  bWndClosed(false),
  bActivateLock(false),
  dwTimerLastLookCount(0),
  dwCloseWndTime(::GetTickCount()),
  dwShrinkMethod(estShrink),
  bDelayActivateLook(false)
{
};

inline __CAHWndInfo::~__CAHWndInfo()
{
};

inline void __CAHWndInfo::Init(const __CAHWndPatternInfo* const cpcInitPatt)
{
  dwShrinkMethod = cpcInitPatt->dwType;
};

inline bool __CAHWndInfo::operator==(const HWND hcCompWnd)
{
  return hcWnd == hcCompWnd;
};


//////////////////////////////////////////////////////////////////////
//__CProcessInfoStr

inline __CProcessInfoStr::__CProcessInfoStr(const DWORD dwcIintProcessId)
  :dwProcessId(dwcIintProcessId),
  pAppName(0)
{
};

inline __CProcessInfoStr::~__CProcessInfoStr()
{
  delete pAppName;
};

inline void __CProcessInfoStr::SetAppName(LPCSTR const cpcInitName, const DWORD dwcInitNameLen)
{
  //only once
  _ASSERT(0 == pAppName);
  pAppName = new CHAR[dwcInitNameLen + 1];
  ::lstrcpyn(pAppName, cpcInitName, dwcInitNameLen);
};

inline bool __CProcessInfoStr::operator==(const DWORD dwcCompProcess)
{
  return dwProcessId == dwcCompProcess;
};


//////////////////////////////////////////////////////////////////////
//CApp

inline void CApp::AHOpenWnd(__CAHWndInfo* const cpInfo, const bool bcActivateWnd, const bool bcNotTochWndPos, const bool bcPushToTimer)
{
  //if not open - open and bring to top of some wnd's
  //activate wnd (if needed)

  //open
  if(false != cpInfo->bWndClosed)
  {
    //present recursive process on activate call
    cpInfo->bWndClosed = false;

    //open wnd type
    switch(cpInfo->dwShrinkMethod)
    {
    case estActivateWnd:
      break;

    case estShrink:
    default:
      //open
      ::PostMessage(cpInfo->hcWnd, uicGlbMsg, egmShowWnd, 0);
      //process ASh productivity
      AHOpenCalcBenefit(cpInfo);
      break;
    };

    //if open ok and need set to timer
    if(false != bcPushToTimer)
    {
      //push to timer looking list
      TimerLookingList.Push(cpInfo->hcWnd);
      cpInfo->dwTimerLastLookCount = 0;
    };
  };
  //if need activate or open wnd ok - process chng wnd pos
  if(false == bcNotTochWndPos)
  {
    //bring/activate
    const UINT uicAddFlags = (false != bcActivateWnd)
      ? 0
      : SWP_NOACTIVATE;
    ::SetWindowPos(cpInfo->hcWnd, HWND_TOP, 0, 0, 0, 0, SWP_ASYNCWINDOWPOS | SWP_NOMOVE | SWP_NOSIZE | uicAddFlags);
  };
};

inline void CApp::AHOpenCalcBenefit(__CAHWndInfo* const cpInfo)
{
  //calc app runned time
  const DWORD dwcAppRunedTime = CalcTimeInSec(dwLastLookAppTime);
  //calc wnd closed time
  const DWORD dwcWndClosedTime = CalcTimeInSec(cpInfo->dwCloseWndTime);
  //calc wnd square to scr square
  RECT ShrinkRect;
  WORD wEdgeType = 0;
  CalcWndSizeForAS(cpInfo->hcWnd, ShrinkRect, wEdgeType);
  RECT WndRect;
  ::GetWindowRect(cpInfo->hcWnd, &WndRect);
  const DWORD dwcScrSq = pCfgMem->ScrSize.x * pCfgMem->ScrSize.y;
  const DWORD dwcWndSq =
    (WndRect.right - WndRect.left) * (WndRect.bottom - WndRect.top)
    - (ShrinkRect.right - ShrinkRect.left) * (ShrinkRect.bottom - ShrinkRect.top);
  //mem benefit
  pCfgMem->ASOpenProd.llTotalTime += static_cast<LONGLONG>(dwcAppRunedTime);
  pCfgMem->ASOpenProd.llWndSqTime += static_cast<LONGLONG>(dwcWndClosedTime * dwcWndSq / max(1, dwcScrSq));
};

inline void CApp::AHCloseWnd(__CAHWndInfo* const cpInfo) const
{
  //flag's
  //calc wnd "closed" size
  //close wnd

  //flag's
  _ASSERT(false == cpInfo->bWndClosed);
  cpInfo->bWndClosed = true;

  //close wnd type
  switch(cpInfo->dwShrinkMethod)
  {
  case estActivateWnd:
    break;

  case estShrink:
  default:
    ::PostMessage(cpInfo->hcWnd, uicGlbMsg, egmHideWnd, 0);
    //process ASh productivity
    cpInfo->dwCloseWndTime = ::GetTickCount();
    break;
  };
};

inline void CApp::AHWndActivityHelper(__CAHWndInfo* const cpInfo)
{
  //check "disabled" style
  const bool bcIsDisabled = FALSE == ::IsWindowEnabled(cpInfo->hcWnd);
  //open
  AHOpenWnd(cpInfo, false, bcIsDisabled);
};

inline void CApp::AHWndActivity(const HWND hcWnd)
{
  //try find already in WndActiveList
  //if find and closed - open, bring to top, and put to TimerLookingList

  //find
  __CAHWndInfo* const cpInfo = ProcessWndList.Find(hcWnd);
  if(0 != cpInfo)
  {
    //if currntly delay activation enabled and wnd not already in "delay activate"
    if(0 != pCfgMem->dwAutoOpenTimerTime)
    {
      if(false == cpInfo->bDelayActivateLook)
      {
        ////add wnd to activation queue
        cpInfo->bDelayActivateLook = true;
        cpInfo->dwTimerLastLookCount = 0;
        TimerLookingList.Push(cpInfo->hcWnd);
      };
    }
    else
    {
      ////process activation immediately
      AHWndActivityHelper(cpInfo);
    };
  };
};


inline void CApp::AHWndActivisation(const HWND hcWnd, const bool bActivate)
{
  //try find in ProcessWndList
  //on "Activate" - show and activate wnd (if not already open), lock wnd
  //on "Deactivate" unlock wnd

  //find
  __CAHWndInfo* const cpInfo = ProcessWndList.Find(hcWnd);
  if(0 != cpInfo)
  {
    if(false == bActivate)
    {
      //deactivate
      cpInfo->bActivateLock = false;
    }
    else
    {
      //activate
      cpInfo->bActivateLock = true;
      AHOpenWnd(cpInfo, false, true);
    };
  };
};

inline void CApp::AHWndShow(const HWND hcWnd, const BOOL bcActive)
{
  //find "already in ProcessWndList" info
  //wnd show -
  //  if not already in ProcessWndList - process wnd pattern's
  //  push wnd in TimerHide list

  //tri find
  __CAHWndInfo* pProcessInfo = ProcessWndList.Find(hcWnd);
  //show wnd
  //if not already - try look by "pattern's" list
  bool bAppFind = false;
  const __CAHWndPatternInfo* pcFindPatt = 0;
  if(0 == pProcessInfo && false != AHCheckWndByPatternList(hcWnd, bAppFind, pcFindPatt))
  {
    pProcessInfo = ProcessWndList.Add(hcWnd);
    pProcessInfo->Init(pcFindPatt);
  };

  //if wnd present in "already" list - push top HideTimer
  if(0 != pProcessInfo)
  {
    TimerLookingList.Push(hcWnd);
    //modify "active" flag
    pProcessInfo->bActivateLock = FALSE != bcActive;
  };
};

inline void CApp::AHWndHide(const HWND hcWnd)
{
  //find "already in ProcessWndList" info
  //wnd hide - open wnd (if not open already)

  //tri find
  __CAHWndInfo* pProcessInfo = ProcessWndList.Find(hcWnd);
  //if find in info list - open wnd
  if(0 != pProcessInfo && false != pProcessInfo->bWndClosed)
  {
    AHOpenWnd(pProcessInfo, false, true, false);
  };
};

inline void CApp::AHWndDestroy(const HWND hcWnd)
{
  //remove wnd from ProcessWndList
  ProcessWndList.Remove(hcWnd);
};

inline DWORD CApp::CalcTimeInSec(DWORD& rdwLastTime)
{
  //get curr
  const DWORD dwcCurr = ::GetTickCount();
  //calc distance (with "overrun" possibility)
  const DWORD dwcTimeDist = (dwcCurr >= rdwLastTime)
    ? dwcCurr - rdwLastTime
    : static_cast<DWORD>(-1) - (rdwLastTime - dwcCurr);
  rdwLastTime = dwcCurr;
  //conv to sec
  //retun
  return dwcTimeDist / eccMilSecInSec;
};

inline void CApp::BringHelp(const UINT uicCmd, const DWORD dwcData)
{
  CHAR cpBuff[MAX_PATH];
  GetFullBasedName(cpcHelpFileName, cpBuff, hInst);
  ::WinHelp
    (
    (FALSE != ::IsWindow(pCfgMem->hCfgAppWnd)) ? pCfgMem->hCfgAppWnd : pCfgMem->hMainHostWnd,
    cpBuff,
    uicCmd,
    dwcData
    );
};

inline void CApp::TimerSaveCfgProcess()
{
  dwSaveCfgSummTime += eccTimerTime;
  if(egcHostSaveCfgTime < dwSaveCfgSummTime)
  {
    //time elapsed
    dwSaveCfgSummTime = 0;
    SaveCfg();
  };
};

inline void CApp::AnalyseExitWndProc(const HWND hcWnd)
{
  DWORD dwRes = 0;
  if(false == SafeSendMsg(hcWnd, uicGlbMsg, egmUnInstallSubClassing, 0, &dwRes, true))
  {
    //send failed - post uninstall request
    ::PostMessage(hcWnd, uicGlbMsg, egmDelayUnInstallSubClassing, 0);
  };
};

inline void CApp::TimerBuildActiveSetHelper()
{
  TimerActiveWndSet.FreeAll();
  POINT Pnt;
  ::GetCursorPos(&Pnt);
  for(HWND hWnd = ::WindowFromPoint(Pnt); 0 != hWnd; hWnd = GetParentWndForDrillUp(hWnd))
  {
    //look only on "AHide style wnd's"
    if(FALSE != AHideCheckWndStyle(hWnd))
    {
      TimerActiveWndSet.Add(hWnd);
    };
  };
};

inline void CApp::TrayClickTimerSensor(const ULONG ulcTimerTime)
{
  if(false != bTrayFirstClickFlag)
  {
    ulTrayFirstClickTimerTime += ulcTimerTime;
    if(ulcTrayDClickTime <= ulTrayFirstClickTimerTime)
    {
      //click present, but second click/dclick not present - ok tray icon click
      bTrayFirstClickFlag = false;
      TrayClickProcess();
    };
  };
};

inline void CApp::TrayClickProcess()
{
  bool bProcessTrayChng = true;
  switch(pCfgMem->dwTrayIconClickMode)
  {
  case etiTrayIconClickSwitchDirectScroll:
    //flip direct scroll cfg
    pCfgMem->bScrollDisabled = !pCfgMem->bScrollDisabled;
    break;

  case etiTrayIconClickSwitchAutoHide:
    AHideFlipSwitch();
    break;

  default:
    bProcessTrayChng = false;
    break;
  };

  if(false != bProcessTrayChng)
  {
    //notify tray icon chng
    TrayAnimProcess(true);
  };
};

inline bool CApp::TrayIconSpecialState() const
{
  bool bRes = true;

  switch(pCfgMem->dwTrayIconClickMode)
  {
  case etiTrayIconClickSwitchDirectScroll:
    bRes = false != pCfgMem->bScrollDisabled;
    break;

  case etiTrayIconClickSwitchAutoHide:
    bRes = false != pCfgMem->bAHideEnabled;
    break;

  default:
    break;
  };

  return bRes;
};

inline void CApp::AHideFlipSwitch()
{
  pCfgMem->bAHideEnabled = !pCfgMem->bAHideEnabled;
  ::PostMessage(pCfgMem->hMainHostWnd, emcNeedSaveCfg, 0, 0);
  ::PostMessage(pCfgMem->hMainHostWnd, emcReReadWndPattList, 0, 0);
};

#endif//__APP_H