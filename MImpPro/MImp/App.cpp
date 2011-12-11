/**********************************************************************
mouse imp hok dll
**********************************************************************/

#include "StdAfx.h"


#include "App.h"


#include "..\\SLibWin\\SLWDbg.h"

#include <limits.h>

#include "..\\Slib\\SLExcHandler.h"

#include "..\\slib\\SLcont.h"


//////////////////////////////////////////////////////////////////////
//global's

//app instance
CApp* pApp = 0;

//hook func's dispatch table
//inited in init
//cleared in finit

//mouse hook
HookProcClassType pMouseFunc = 0;
//CallWnd hook
HookProcClassType pCBTFunc = 0;


//string's
static LPCTSTR const cpcScrollBarClassName = _T("ScrollBar");
static LPCTSTR const cpcComCtrlModuleName = _T("COMCTL32.DLL");
static LPCTSTR const cpcFlatSB_GetScrollInfoName = _T("FlatSB_GetScrollInfo");
static LPCTSTR const cpcFlatSB_GetScrollPropName = _T("FlatSB_GetScrollProp");
static LPCTSTR const cpcIEWndClassName = _T("Internet Explorer_Server");
static LPCTSTR const cpcWinAmpPLClassName = _T("Winamp PE");
//static LPCTSTR const cpcOffice2007ClassName = _T("NetUIHWND");
static LPCTSTR const cpcWord2007ClassName = _T("_WwG");
static LPCTSTR const cpcExcel2007ClassName = _T("EXCEL7");
static LPCTSTR const cpcPowerPoint2007ClassName = _T("paneClassDC");
static LPCTSTR const cpcOutlook2007ClassName = _T("AfxWndW");

static LPCTSTR const cpcOpera7WndClassName = _T("FRAMES2");
static LPCTSTR const cpcOpera9WndClassName = _T("OperaWindowClass");
static LPCTSTR const cpcMozillaWndClassName = _T("MozillaWindowClass");

// BEGIN.TormozIT - 1C8 fan
static LPCTSTR const cpcV8WindowWndClassName = _T("V8Window");
static LPCTSTR const cpcV8GridWndClassName = _T("V8Grid");
static LPCTSTR const cpcV8FormElementWndClassName = _T("V8FormElement");
// END.TormozIT - 1C8 fan

//info for non client messages (for stop scroll control mode)
const struct StopMouseMsg cpcStopMouseNCInfo[] =
{
  {MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP, MK_LBUTTON, VK_LBUTTON},
  {MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_MIDDLEUP, MK_MBUTTON, VK_MBUTTON},
  {MOUSEEVENTF_RIGHTDOWN, MOUSEEVENTF_RIGHTUP, MK_RBUTTON, VK_RBUTTON}
};


//info for link VK_XXX and MK_XXX
struct __MK2VKInfo
{
  //virt key
  UINT uiVKey;
  //key flag
  UINT uiFlag;
};

static const __MK2VKInfo cpcLinkKeysInfo[] =
{
  {VK_LBUTTON, MK_LBUTTON},
  {VK_MBUTTON, MK_MBUTTON},
  {VK_RBUTTON, MK_RBUTTON}
};

static inline bool FintNotTransparent(HWND& rhWnd, const LONG lcCursorX, const LONG lcCursorY, DWORD_PTR& rdwHitTest)
{
  ////dril from passed wnd until find not "transparent" wnd or wnd not belong to "process of start wnd"
  bool bRes = false;

  //wnd process
  DWORD dwPrc = 0;
  ::GetWindowThreadProcessId(rhWnd, &dwPrc);
  //scan
  while(true)
  {
    //check transparent
    SafeSendMsg(rhWnd, WM_NCHITTEST, 0, MAKEWPARAM(lcCursorX, lcCursorY), &rdwHitTest);
    if(HTTRANSPARENT != rdwHitTest)
    {
      //find not transparent wnd
      bRes = true;
      break;
    };

    //next wnd
    rhWnd = GetParentWndForDrillUp(rhWnd);
    if(0 == rhWnd)
    {
      //wnd top
      break;
    };

    //check process
    DWORD dwWndProcess = 0;
    ::GetWindowThreadProcessId(rhWnd, &dwWndProcess);
    if(dwWndProcess != dwPrc)
    {
      //not some process
      break;
    };
  };
  return bRes;
};

//activate passed wnd
static inline void ClickActivateWnd(const HWND hcWnd, const MOUSEHOOKSTRUCT* const cpcInfo)
{
  //process id
  DWORD dwProcess = 0;
  ::GetWindowThreadProcessId(hcWnd, &dwProcess);

  //if wnd child - setnd WM_PARENTNOTIFY

  const bool bcNeedSend = 0 != (WS_CHILD & ::GetWindowLong(hcWnd, GWL_STYLE));
  HWND hTopParent = hcWnd;
  for(HWND hcNotifyWnd = ::GetParent(hcWnd); 0 != hcNotifyWnd; hcNotifyWnd = ::GetParent(hcNotifyWnd))
  {
    DWORD dwWndProc = 0;
    ::GetWindowThreadProcessId(hcNotifyWnd, &dwWndProc);
    if(dwWndProc != dwProcess)
    {
      break;
    };
    //send
    if(false != bcNeedSend)
    {
      POINT ParentCur = cpcInfo->pt;
      ::ScreenToClient(hcNotifyWnd, &ParentCur);
      SafeSendMsg(hcNotifyWnd, WM_PARENTNOTIFY, MAKEWPARAM(WM_LBUTTONDOWN, 0), MAKELPARAM(ParentCur.x, ParentCur.y));
    };
    hTopParent = hcNotifyWnd;
  };

  ////process WM_MOUSEACTIVATE
  DWORD_PTR dwRes = MA_ACTIVATE;
  SafeSendMsg(hcWnd, WM_MOUSEACTIVATE, reinterpret_cast<WPARAM>(hTopParent), MAKELPARAM(cpcInfo->wHitTestCode, WM_LBUTTONDOWN), &dwRes);
  if(MA_ACTIVATE == dwRes || MA_ACTIVATEANDEAT == dwRes)
  {
    //activate wnd
    ::SetFocus(hcWnd);
    ::SetActiveWindow(hcWnd);
  };
};

//////////////////////////////////////////////////////////////////////
//CSubClassInfo

LRESULT CSubClassInfo::WndProcA(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
#define CallWindowProc_(WndProc, hWnd, uiMsg, wParam, lParam) CallWindowProcA(WndProc, hWnd, uiMsg, wParam, lParam)
#include "App_WndProc.txt"
#undef CallWindowProc_
};

LRESULT CSubClassInfo::WndProcW(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
#define CallWindowProc_(WndProc, hWnd, uiMsg, wParam, lParam) CallWindowProcW(WndProc, hWnd, uiMsg, wParam, lParam)
#include "App_WndProc.txt"
#undef CallWindowProc_
};

LRESULT CSubClassInfo::WndProcADef(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
#define CallWindowProc_(WndProc, hWnd, uiMsg, wParam, lParam) DefWindowProcA(hWnd, uiMsg, wParam, lParam)
#include "App_WndProc.txt"
#undef CallWindowProc_
};

LRESULT CSubClassInfo::WndProcWDef(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
#define CallWindowProc_(WndProc, hWnd, uiMsg, wParam, lParam) DefWindowProcW(hWnd, uiMsg, wParam, lParam)
#include "App_WndProc.txt"
#undef CallWindowProc_
};

LRESULT CSubClassInfo::DummyWndProcA(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
#define CallWindowProc_(WndProc, hWnd, uiMsg, wParam, lParam) CallWindowProcA(WndProc, hWnd, uiMsg, wParam, lParam)
#include "App_WndDummyProc.txt"
#undef CallWindowProc_
};

LRESULT CSubClassInfo::DummyWndProcW(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
#define CallWindowProc_(WndProc, hWnd, uiMsg, wParam, lParam) CallWindowProcW(WndProc, hWnd, uiMsg, wParam, lParam)
#include "App_WndDummyProc.txt"
#undef CallWindowProc_
};

LRESULT CSubClassInfo::DummyWndProcADef(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
#define CallWindowProc_(WndProc, hWnd, uiMsg, wParam, lParam) DefWindowProcA(hWnd, uiMsg, wParam, lParam)
#include "App_WndDummyProc.txt"
#undef CallWindowProc_
};

LRESULT CSubClassInfo::DummyWndProcWDef(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
#define CallWindowProc_(WndProc, hWnd, uiMsg, wParam, lParam) DefWindowProcW(hWnd, uiMsg, wParam, lParam)
#include "App_WndDummyProc.txt"
#undef CallWindowProc_
};

//////////////////////////////////////////////////////////////////////
//CApp


CApp::CApp(HINSTANCE hInitInstance)
  :hInstance(hInitInstance),
  pCfgMem(0),
  hCfgMap(0),
  uicGlbMsg(::RegisterWindowMessage(cpcGlobalIntMessage)),
  lLockCount(0),
  bIsDataDecrypted(FALSE),

  lMemHookInstallCount(0),

  hCommCtrlModule(0),
  pFlatSB_GetScrollProp(0),
  pFlatSB_GetScrollInfo(0),

  MemWndList(100)
{
};

CApp::~CApp()
{
};

//start dll enum helper's

static BOOL CALLBACK StartDllEnumChildWndProc(HWND hWnd, LPARAM lParam)
{
  if(false != reinterpret_cast<CApp*>(lParam)->AnalyseStartWndHelper(hWnd))
  {
    ::EnumChildWindows(hWnd, StartDllEnumChildWndProc, lParam);
  };
  return TRUE;
};

static BOOL CALLBACK StartDllEnumWndProc(HWND hWnd, LPARAM lParam)
{
  if(false != reinterpret_cast<CApp*>(lParam)->AnalyseStartWndHelper(hWnd))
  {
    ::EnumChildWindows(hWnd, StartDllEnumChildWndProc, lParam);
  };
  return TRUE;
};

void CApp::InstallSubClassingSendDataToHost()
{
  //subclass wnd's from this process
  ::EnumWindows(StartDllEnumWndProc, reinterpret_cast<LPARAM>(this));

  if(FALSE == pCfgMem->bNotSendModuleFileName)
  {
    //app name
    //try create file mapping for app name
    //get app name to file mapping
    //route mapping to host process

    //file mapping
    HANDLE hMap = ::CreateFileMapping
      (
      reinterpret_cast<HANDLE>(0xFFFFFFFF),
      0,
      PAGE_READWRITE,
      0,
      MAX_PATH,
      0
      );
    if(0 != hMap)
    {
      //try map memory
      LPSTR const cpMem = reinterpret_cast<LPSTR>(::MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0));
      if(0 != cpMem)
      {
        //try get app name (full)
        ::GetModuleFileName(::GetModuleHandle(0), cpMem, MAX_PATH);
        //unmap
        ::UnmapViewOfFile(cpMem);

        //try open host process
        DWORD dwHostProc = 0;
        ::GetWindowThreadProcessId(pCfgMem->hMainHostWnd, &dwHostProc);
        HANDLE hHProc = ::OpenProcess(PROCESS_DUP_HANDLE, FALSE, dwHostProc);
        if(0 != hHProc)
        {
          //dest handle
          HANDLE hDestMap = 0;
          //duplicate mapping and send to host
          const BOOL bcDupOk = ::DuplicateHandle
            (
            ::GetCurrentProcess(),
            hMap,
            hHProc,
            &hDestMap,
            0,
            FALSE,
            DUPLICATE_SAME_ACCESS
            );
          if(FALSE != bcDupOk)
          {
            ::PostMessage(pCfgMem->hMainHostWnd, emcNewAppStarted, ::GetCurrentProcessId(), reinterpret_cast<LPARAM>(hDestMap));
          };
          //close host process handle
          ::CloseHandle(hHProc);
        };
      };
      //close "this" mapiing
      ::CloseHandle(hMap);
    };
  }
  else
  {
    //simple pass process id to host app
    ::PostMessage(pCfgMem->hMainHostWnd, emcNewAppStarted, ::GetCurrentProcessId(), 0);
  };
};

bool CApp::Init()
{
  ////init app module class
  //get app name
  //try attach to "SharedInfo"
  //create event object for protect hook function

  bool bRes = true;

  //init hook dispatch func's
  if(false != bRes)
  {
    pMouseFunc = &CApp::MouseHookFunc;
    pCBTFunc = &CApp::CBTHookFunc;
  };

  //shared info
  if(false != bRes)
  {
    bRes = false;

    hCfgMap = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, cpcSharedInfoName);
    if(0 != hCfgMap)
    {
      LPVOID const cpPtr = ::MapViewOfFile(hCfgMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
      if(0 != cpPtr)
      {
        pCfgMem = reinterpret_cast<CMISharedInfo*>(cpPtr);
        bRes = true;
      };
    };
  };

  //init (if ok)
  if(false != bRes)
  {
    InstallSubClassingSendDataToHost();
  };

  return bRes;
};

void CApp::Finit()
{
  //del shared cfg mem
  //delete event

  //remove from host's process list
  if(0 != pCfgMem && FALSE != ::IsWindow(pCfgMem->hMainHostWnd))
  {
    ::PostMessage(pCfgMem->hMainHostWnd, emcAppExited, 0, ::GetCurrentProcessId());
  };

  //clear hook func dispatch table
  pMouseFunc = &CApp::DummyHookFunc;
  pCBTFunc = &CApp::DummyHookFunc;

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

  //free common control's module
  if(0 != hCommCtrlModule)
  {
    ::FreeLibrary(hCommCtrlModule);
    hCommCtrlModule = 0;
  };
};

LRESULT CApp::ProcessGlbMsg(const HWND hcWnd, WPARAM wParam, LPARAM lParam, CSubClassInfo* const cpInfo, const bool bcFullProcess)
{
  LRESULT lRes = 0;
  switch(wParam)
  {
  case egmHideWnd:
    if(false != bcFullProcess && false == cpInfo->FlagTest(CSubClassInfo::ecsIsShrinked))
    {
      CalcWndSizeForAS(hcWnd, cpInfo->CloseMargin, cpInfo->wDrawEdgeType);
      //close
      HRGN hRgn = ::CreateRectRgn
        (
        cpInfo->CloseMargin.left,
        cpInfo->CloseMargin.top,
        cpInfo->CloseMargin.right,
        cpInfo->CloseMargin.bottom
        );
      cpInfo->FlagSet(CSubClassInfo::ecsIsShrinked, true);
      if(FALSE == ::SetWindowRgn(hcWnd, hRgn, TRUE))
      {
        //if set not "ok" - free object
        cpInfo->FlagSet(CSubClassInfo::ecsIsShrinked, false);
        ::DeleteObject(hRgn);
      }
      else
      {
        //open ok - update wnd
        TryUpdateEntireWnd(hcWnd);
      };
    };
    break;

  case egmShowWnd:
    if(false != bcFullProcess && false != cpInfo->FlagTest(CSubClassInfo::ecsIsShrinked))
    {
      TryOpenWnd(hcWnd, cpInfo);
    };
    break;

  case egmDelayUnInstallSubClassing:
    TryOpenWnd(hcWnd, cpInfo);
    UnSubClass(hcWnd, false, false);
    break;

  case egmUnInstallSubClassing:
    TryOpenWnd(hcWnd, cpInfo);
    UnSubClass(hcWnd, false, true);
    break;

  case egmOffHookForwarding:
    pCfgMem->bMouseHookInForwardMode = FALSE;
    break;

  case egmGetAppName:
    ::GetModuleFileName(0, pCfgMem->cpGetAppNameResultBuff, COUNTOF(pCfgMem->cpGetAppNameResultBuff));
    break;

  default:
    break;
  };
  return lRes;
};

LRESULT CApp::DummyHookFunc(int nCode, WPARAM wParam, LPARAM lParam)
{
  return ::CallNextHookEx(0, nCode, wParam, lParam);
};

LRESULT CApp::CBTHookFunc(int nCode, WPARAM wPrm, LPARAM lPrm)
{
	//OA
	if (nCode < 0)
		return ::CallNextHookEx(pCfgMem->hCBTHook, nCode, wPrm, lPrm);

  InstallAllFromHook();

  //process wnd's from mem list
  switch(nCode)
  {
    //process looking "mem list" only for following event's
  case HCBT_ACTIVATE:
  case HCBT_CREATEWND:
  case HCBT_DESTROYWND:
  case HCBT_MINMAX:
  case HCBT_MOVESIZE:
  case HCBT_SETFOCUS:
  case HCBT_SYSCOMMAND:
    {
      //process scan on cbt func
      MemWndListIterType Iter = MemWndList.IterBegin();
      while(false != MemWndList.IterIsOk(Iter))
      {
        const HWND hcWnd = *(MemWndList.IterGet(Iter));
        //if not process from cbt - remove all
        BOOL bRemove = FALSE;
        if(FALSE != pCfgMem->bProcessAddWndFromCBTHook && FALSE != ::IsWindow(hcWnd))
        {
          //if wnd visible
          if(FALSE != ::IsWindowVisible(hcWnd))
          {
            bRemove = TRUE;
            if(FALSE != AHideCheckWndStyle(hcWnd))
            {
              //try subclass
              if(false != SubClassWnd(hcWnd))
              {
                AShowSensor(hcWnd, true);
              };
            };
          };
        }
        else
        {
          bRemove = TRUE;
        };

        //remove
        if(FALSE != bRemove)
        {
          MemWndList.IterRemove(Iter);
          if(false == MemWndList.IterIsOk(Iter))
          {
            //break
            break;
          }
          else
          {
            //next
            continue;
          };
        };

        //next
        Iter = MemWndList.IterNext(Iter);
      };
    };
    break;

  default:
    break;
  };

  //process new wnd creating
  switch(nCode)
  {
  case HCBT_CREATEWND:
    //mem only if alloved
    if(FALSE != pCfgMem->bProcessAddWndFromCBTHook)
    {
		//OA - commented this out
      //MemWndList.Push(reinterpret_cast<HWND>(wPrm));
    };
    break;

  default:
    break;
  };
  //OA - changed to return 0 instead of next hook
	return 0;
  //return ::CallNextHookEx(pCfgMem->hCBTHook, nCode, wPrm, lPrm);
};

LRESULT CApp::MouseHookFunc(int nCode, WPARAM wPrm, LPARAM lPrm)
{
  //OA - commented this out
  if (nCode < 0)
	  return ::CallNextHookEx(pCfgMem->hMouseHook, nCode, wPrm, lPrm);
  InstallAllFromHook();
  //LRESULT lRes = ::CallNextHookEx(pCfgMem->hMouseHook, nCode, wPrm, lPrm);
  LRESULT lRes = 0;
  //OA end
  if(0 <= nCode && 0 != lPrm)
  {
    //msg info
    const MOUSEHOOKSTRUCT* const cpcInfo = reinterpret_cast<const MOUSEHOOKSTRUCT*>(lPrm);
    //need eat msg flag
    bool bNeedEat = false;
    //is msg eated by application
    const bool bcIsEatMsgByApp = HC_NOREMOVE != nCode;

    //try process
    switch(wPrm)
    {
      //right
    case WM_NCRBUTTONDOWN:
      bNeedEat = MouseDownEvent(MK_RBUTTON, cpcInfo, bcIsEatMsgByApp, embRight);
      if(false == bNeedEat && FALSE != pCfgMem->bMouseHookInForwardMode)
      {
        bNeedEat = true;
        ::PostMessage(cpcInfo->hwnd, WM_NCRBUTTONDOWN, cpcInfo->wHitTestCode, MAKELPARAM(cpcInfo->pt.x, cpcInfo->pt.y));
      };
      break;

    case WM_NCRBUTTONUP:
      bNeedEat = MouseUpEvent(MK_RBUTTON, cpcInfo, embRight, bcIsEatMsgByApp);
      if(false == bNeedEat && FALSE != pCfgMem->bMouseHookInForwardMode)
      {
        bNeedEat = true;
        ::PostMessage(cpcInfo->hwnd, WM_NCRBUTTONUP, cpcInfo->wHitTestCode, MAKELPARAM(cpcInfo->pt.x, cpcInfo->pt.y));
      };
      break;

    case WM_RBUTTONDOWN:
      bNeedEat = MouseDownEvent(MK_RBUTTON, cpcInfo, bcIsEatMsgByApp, embRight);
      if(false == bNeedEat && FALSE != pCfgMem->bMouseHookInForwardMode)
      {
        POINT WndPt = cpcInfo->pt;
        ::ScreenToClient(cpcInfo->hwnd, &WndPt);
        bNeedEat = true;
        ::PostMessage(cpcInfo->hwnd, WM_RBUTTONDOWN, pCfgMem->uiCurrMouseKeys, MAKELPARAM(WndPt.x, WndPt.y));
      };
      break;

    case WM_RBUTTONUP:
      bNeedEat = MouseUpEvent(MK_RBUTTON, cpcInfo, embRight, bcIsEatMsgByApp);
      if(false == bNeedEat && FALSE != pCfgMem->bMouseHookInForwardMode)
      {
        POINT WndPt = cpcInfo->pt;
        ::ScreenToClient(cpcInfo->hwnd, &WndPt);
        bNeedEat = true;
        ::PostMessage(cpcInfo->hwnd, WM_RBUTTONUP, pCfgMem->uiCurrMouseKeys, MAKELPARAM(WndPt.x, WndPt.y));
      };
      break;

      //left
    case WM_NCLBUTTONDOWN:
      bNeedEat = MouseDownEvent(MK_LBUTTON, cpcInfo, bcIsEatMsgByApp, embLeft);
      if(false == bNeedEat && FALSE != pCfgMem->bMouseHookInForwardMode)
      {
        bNeedEat = true;
        ::PostMessage(cpcInfo->hwnd, WM_NCLBUTTONDOWN, cpcInfo->wHitTestCode, MAKELPARAM(cpcInfo->pt.x, cpcInfo->pt.y));
      };
      break;

    case WM_NCLBUTTONUP:
      bNeedEat = MouseUpEvent(MK_LBUTTON, cpcInfo, embLeft, bcIsEatMsgByApp);
      if(false == bNeedEat && FALSE != pCfgMem->bMouseHookInForwardMode)
      {
        bNeedEat = true;
        ::PostMessage(cpcInfo->hwnd, WM_NCLBUTTONUP, cpcInfo->wHitTestCode, MAKELPARAM(cpcInfo->pt.x, cpcInfo->pt.y));
      };
      break;

    case WM_LBUTTONDOWN:
      bNeedEat = MouseDownEvent(MK_LBUTTON, cpcInfo, bcIsEatMsgByApp, embLeft);
      if(false == bNeedEat && FALSE != pCfgMem->bMouseHookInForwardMode)
      {
        POINT WndPt = cpcInfo->pt;
        ::ScreenToClient(cpcInfo->hwnd, &WndPt);
        bNeedEat = true;
        ::PostMessage(cpcInfo->hwnd, WM_LBUTTONDOWN, pCfgMem->uiCurrMouseKeys, MAKELPARAM(WndPt.x, WndPt.y));
      };
      break;

    case WM_LBUTTONUP:
      bNeedEat = MouseUpEvent(MK_LBUTTON, cpcInfo, embLeft, bcIsEatMsgByApp);
      if(false == bNeedEat && FALSE != pCfgMem->bMouseHookInForwardMode)
      {
        POINT WndPt = cpcInfo->pt;
        ::ScreenToClient(cpcInfo->hwnd, &WndPt);
        bNeedEat = true;
        ::PostMessage(cpcInfo->hwnd, WM_LBUTTONUP, pCfgMem->uiCurrMouseKeys, MAKELPARAM(WndPt.x, WndPt.y));
      };
      break;

      //middle
    case WM_NCMBUTTONDOWN:
      bNeedEat = MouseDownEvent(MK_MBUTTON, cpcInfo, bcIsEatMsgByApp, embMidle);
      if(false == bNeedEat && FALSE != pCfgMem->bMouseHookInForwardMode)
      {
        bNeedEat = true;
        ::PostMessage(cpcInfo->hwnd, WM_NCMBUTTONDOWN, cpcInfo->wHitTestCode, MAKELPARAM(cpcInfo->pt.x, cpcInfo->pt.y));
      };
      break;

    case WM_NCMBUTTONUP:
      bNeedEat = MouseUpEvent(MK_MBUTTON, cpcInfo, embMidle, bcIsEatMsgByApp);
      if(false == bNeedEat && FALSE != pCfgMem->bMouseHookInForwardMode)
      {
        bNeedEat = true;
        ::PostMessage(cpcInfo->hwnd, WM_NCMBUTTONUP, cpcInfo->wHitTestCode, MAKELPARAM(cpcInfo->pt.x, cpcInfo->pt.y));
      };
      break;

    case WM_MBUTTONDOWN:
      bNeedEat = MouseDownEvent(MK_MBUTTON, cpcInfo, bcIsEatMsgByApp, embMidle);
      if(false == bNeedEat && FALSE != pCfgMem->bMouseHookInForwardMode)
      {
        POINT WndPt = cpcInfo->pt;
        ::ScreenToClient(cpcInfo->hwnd, &WndPt);
        bNeedEat = true;
        ::PostMessage(cpcInfo->hwnd, WM_MBUTTONDOWN, pCfgMem->uiCurrMouseKeys, MAKELPARAM(WndPt.x, WndPt.y));
      };
      break;

    case WM_MBUTTONUP:
      bNeedEat = MouseUpEvent(MK_MBUTTON, cpcInfo, embMidle, bcIsEatMsgByApp);
      if(false == bNeedEat && FALSE != pCfgMem->bMouseHookInForwardMode)
      {
        POINT WndPt = cpcInfo->pt;
        ::ScreenToClient(cpcInfo->hwnd, &WndPt);
        bNeedEat = true;
        ::PostMessage(cpcInfo->hwnd, WM_MBUTTONUP, pCfgMem->uiCurrMouseKeys, MAKELPARAM(WndPt.x, WndPt.y));
      };
      break;

      //move
    case WM_MOUSEMOVE:
      AHideMoveSensor(cpcInfo->hwnd);
      MouseMoveDistSensor(cpcInfo->pt);
      bNeedEat = MouseMoveEvent(cpcInfo);
      if(false == bNeedEat && FALSE != pCfgMem->bMouseHookInForwardMode)
      {
        POINT WndPt = cpcInfo->pt;
        ::ScreenToClient(cpcInfo->hwnd, &WndPt);
        bNeedEat = true;
        ::PostMessage(cpcInfo->hwnd, WM_MOUSEMOVE, pCfgMem->uiCurrMouseKeys, MAKELPARAM(WndPt.x, WndPt.y));
      };
      break;

    case WM_NCMOUSEMOVE:
      AHideMoveSensor(cpcInfo->hwnd);
      MouseMoveDistSensor(cpcInfo->pt);
      bNeedEat = MouseMoveEvent(cpcInfo);
      if(false == bNeedEat && FALSE != pCfgMem->bMouseHookInForwardMode)
      {
        bNeedEat = true;
        ::PostMessage(cpcInfo->hwnd, WM_NCMOUSEMOVE, cpcInfo->wHitTestCode, MAKELPARAM(cpcInfo->pt.x, cpcInfo->pt.y));
      };
      break;

      //default
    default:
      break;
    };

    //try reforward mosue event
	//OA pCfgMem->bReForwardMouseEvent = FALSE;
    if(FALSE != pCfgMem->bReForwardMouseEvent)
    {
      //stop reforward
      pCfgMem->bReForwardMouseEvent = FALSE;
      //process reforward
      bNeedEat = true;
      switch(wPrm)
      {
        //right
      case WM_NCRBUTTONDOWN:
      case WM_RBUTTONDOWN:
        MouseEventReForvarder(cpcInfo, WM_RBUTTONDOWN, WM_NCRBUTTONDOWN);
        break;

      case WM_RBUTTONUP:
      case WM_NCRBUTTONUP:
        MouseEventReForvarder(cpcInfo, WM_RBUTTONUP, WM_NCRBUTTONUP);
        break;

        //left
      case WM_NCLBUTTONDOWN:
      case WM_LBUTTONDOWN:
        MouseEventReForvarder(cpcInfo, WM_LBUTTONDOWN, WM_NCLBUTTONDOWN);
        break;

      case WM_NCLBUTTONUP:
      case WM_LBUTTONUP:
        MouseEventReForvarder(cpcInfo, WM_LBUTTONUP, WM_NCLBUTTONUP);
        break;

        //middle
      case WM_NCMBUTTONDOWN:
      case WM_MBUTTONDOWN:
        MouseEventReForvarder(cpcInfo,WM_MBUTTONDOWN , WM_NCMBUTTONDOWN);
        break;

      case WM_NCMBUTTONUP:
      case WM_MBUTTONUP:
        MouseEventReForvarder(cpcInfo, WM_MBUTTONUP, WM_NCMBUTTONUP);
        break;

        //default
      default:
        bNeedEat = false;
        break;
      };
    };

    //if msg eated - not return to WndProc
    lRes = false != bNeedEat;

    //now try set cursor (if need)
    SetCursorForScroll();
  };

  return lRes;
};

////calc difference between passed (looked) wnd and "clicked" wnd center
static inline bool TestWndRect(const HWND hcWnd, const POINT& rcClickWndCenter, LONG& rlCenterDist)
{
  bool bRes = false;

  //get rect of "looked" wnd
  RECT LookRect;
  GetWindowRect(hcWnd, &LookRect);

  //if scroll bar not "lefter" or "upper"
  if(rcClickWndCenter.x < LookRect.right || rcClickWndCenter.y < LookRect.bottom)
  {
    //center of "looked" wnd
    const POINT cLookPnt = GetRectCenter(LookRect);

    //calc diff from "click wnd center" and "look wnd center" (x * x + y * y)
    const LONG lcDiffX = cLookPnt.x - rcClickWndCenter.x;
    const LONG lcDiffY = cLookPnt.y - rcClickWndCenter.y;
    const LONG lcDiffPowTmp = lcDiffX * lcDiffX + lcDiffY * lcDiffY;
    const LONG lcDiffPow = (rcClickWndCenter.x > LookRect.right || rcClickWndCenter.y > LookRect.bottom)
      ? (lcDiffPowTmp + (1024 * 1024 * 16))
      : lcDiffPowTmp;

    //look on "is this diff smaller then passed"
    if(lcDiffPow < rlCenterDist)
    {
      rlCenterDist = lcDiffPow;
      bRes = true;
    };
  };

  return bRes;
};

void CApp::AnalyseForDrillChildDown(const HWND hcAnalyseWnd, const MOUSEHOOKSTRUCT* const cpMsg, LONG& rlHSbDist, bool& rbHFind, LONG& rlVSbDist, bool& rbVFind, const POINT& rcClickPoint)
{
  //easy
  CScrollProcessInfo& rProcessInfo = pCfgMem->ScrollProcessInfo;

  TCHAR cpBuff[egcWndClassTextLen];
  ::GetClassName(hcAnalyseWnd, cpBuff, COUNTOF(cpBuff));
  if(false != ASCompareClassSimple(cpBuff, cpcScrollBarClassName))
  {
    //it's scroll bar ctrl - look on it's style
    const LONG lcSibStyle = ::GetWindowLong(hcAnalyseWnd, GWL_STYLE);
    //is not "size box"
    if(0 == (SBS_SIZEBOX & lcSibStyle))
    {
      if(0 == (SBS_VERT & lcSibStyle))
      {
        //horisontal scroll bar
        if
          (
          //is finded near when some other
          false != TestWndRect(hcAnalyseWnd, rcClickPoint, rlHSbDist)
          //is finded sb scrollable
          && false != SCTestRangeGreatPage(hcAnalyseWnd, true, true)
          )
        {
          //get scrol info

          //mem find and break cycle
          rProcessInfo.HInfo.bIsCtrl = true;
          rProcessInfo.HInfo.hWnd = hcAnalyseWnd;
          rbHFind = true;
        };
      }
      else
      {
        //vertical scroll bar
        if
          (
          //is finded nearest
          false != TestWndRect(hcAnalyseWnd, rcClickPoint, rlVSbDist)
          //is Sb scrollable
          && false != SCTestRangeGreatPage(hcAnalyseWnd, true, false)
          )
        {
          //mem find and break cycle
          rProcessInfo.VInfo.bIsCtrl = true;
          rProcessInfo.VInfo.hWnd = hcAnalyseWnd;
          rbVFind = true;
        };
      };
    };
  };
};

//is user can work with wnd
static inline bool IsWndUserWork(const HWND hcWnd)
{
  return FALSE != ::IsWindowEnabled(hcWnd) && FALSE != ::IsWindowVisible(hcWnd);
};

//helper for analyse drill wnd down
static inline bool DrillDownAnalyseHelper(const HWND hcWnd, const DWORD dwcLookWndProcess)
{
  bool bRes = false;

  if(0 != hcWnd && false != IsWndUserWork(hcWnd))
  {
    DWORD dwWndProcess = 0;
    ::GetWindowThreadProcessId(hcWnd, &dwWndProcess);
    if(dwcLookWndProcess == dwWndProcess)
    {
      bRes = true;
    };
  };

  return bRes;
};

UINT CApp::DrillChildUp(const HWND hcInitStart, const UINT uicCurrKeyFlag, const MOUSEHOOKSTRUCT* const cpMsg)
{
  //easy
  UINT uiRes = ehmNone;

  //for easy
  CScrollProcessInfo& rProcessInfo = pCfgMem->ScrollProcessInfo;
  DWORD dwWhileProcess = 0;
  ::GetWindowThreadProcessId(hcInitStart, &dwWhileProcess);

  //clear old
  rProcessInfo.HInfo.hWnd = 0;
  rProcessInfo.VInfo.hWnd = 0;

  //cycle
  bool bHorFind = false;
  bool bVertFind = false;
  LONG lHSbDist = LONG_MAX;
  LONG lVSbDist = LONG_MAX;
  bool bInDScroll = true;
  bool bVertFlatSB = false;
  bool bHorFlatSB = false;

  //drill from passed wnd "up"
  for(HWND hLookWnd = hcInitStart; (false == bHorFind || false == bVertFind) && 0 != hLookWnd; hLookWnd = GetParentWndForDrillUp(hLookWnd))
  {
    //look only on wnd's from "this" process
    DWORD dwPrcs = 0;
    ::GetWindowThreadProcessId(hLookWnd, &dwPrcs);
    if(dwWhileProcess != dwPrcs)
    {
      break;
    };

    //is wnd not visible - continue
    if(false == IsWndUserWork(hLookWnd))
    {
      continue;
    };

    //not in scroll bar analyse
    bInDScroll = false;

    //style
    const LONG lcLookStyle = ::GetWindowLong(hLookWnd, GWL_STYLE);

    //try look "is this wnd IE explorer wnd"
    TCHAR cpBuff[egcWndClassTextLen];
    ::GetClassName(hLookWnd, cpBuff, COUNTOF(cpBuff));
    ////find IE
    if(false != ASCompareClassSimple(cpBuff, cpcIEWndClassName))
    {
      //mem info
      CSCrollIEProcessInfo& rInfo = pCfgMem->ScrollIEProcessInfo;
      rInfo.hWnd = hLookWnd;
      rInfo.bLockedScrollDirection = false;
      rInfo.bLockedWheelDelta = false;
      //goto state
      uiRes = ehmScrollIEPress;
      break;
    }
    else if(false != ASCompareClassSimple(cpBuff, cpcWinAmpPLClassName))
    {
      ////WinAmp processing
      //mem info
      CSCrollIEProcessInfo& rInfo = pCfgMem->ScrollIEProcessInfo;
      rInfo.hWnd = hLookWnd;
      rInfo.bLockedScrollDirection = true; 
      rInfo.bHorScroll = false;
      rInfo.bLockedWheelDelta = true;
      //goto state
      uiRes = ehmScrollIEPress;
      break;
    }
    else if(
		false != ASCompareClassSimple(cpBuff, cpcWord2007ClassName)
		)
    {
      CSCrollIEProcessInfo& rInfo = pCfgMem->ScrollIEProcessInfo; // ScrollProcessInfo ???
      rInfo.hWnd = hLookWnd;
      rInfo.bLockedScrollDirection = true;
      rInfo.bHorScroll = false;
      rInfo.bLockedWheelDelta = true;
      //rInfo.bLockedWheelDelta = true;
      //goto state
      uiRes = ehmScrollIEPress; // ehmScrollPress ???
      break;
    }
    else if(
		false != ASCompareClassSimple(cpBuff, cpcExcel2007ClassName)
		)
    {
      CSCrollIEProcessInfo& rInfo = pCfgMem->ScrollIEProcessInfo;
      rInfo.hWnd = hLookWnd;
      rInfo.bLockedScrollDirection = true;
      rInfo.bHorScroll = false;
      rInfo.bLockedWheelDelta = false;
      //goto state
      uiRes = ehmScrollIEPress;
      break;
    }
    else if(
		false != ASCompareClassSimple(cpBuff, cpcPowerPoint2007ClassName)
		)
    {
      CSCrollIEProcessInfo& rInfo = pCfgMem->ScrollIEProcessInfo;
      rInfo.hWnd = hLookWnd;
      rInfo.bLockedScrollDirection = true;
      rInfo.bHorScroll = false;
      rInfo.bLockedWheelDelta = false;
      //goto state
      uiRes = ehmScrollIEPress;
      break;
    }
	else if(false != ASCompareClassSimple(cpBuff, cpcOpera7WndClassName) ||
		false != ASCompareClassSimple(cpBuff, cpcOpera9WndClassName))
	{
		////Mozilla processing
		//mem info
		CSCrollIEProcessInfo& rInfo = pCfgMem->ScrollIEProcessInfo;
		rInfo.hWnd = hLookWnd;
		rInfo.bLockedScrollDirection = false;
		rInfo.bHorScroll = false;
		rInfo.bLockedWheelDelta = true;
		//goto state
		uiRes = ehmScrollIEPress;
		break;
	}
	else if (
		false != ASCompareClassSimple(cpBuff, cpcMozillaWndClassName)
		)
	{
		 ////Mozilla processing
		//mem info
		CSCrollIEProcessInfo& rInfo = pCfgMem->ScrollIEProcessInfo;
		rInfo.hWnd = hLookWnd;
		rInfo.bLockedScrollDirection = true;
		rInfo.bHorScroll = false;
		rInfo.bLockedWheelDelta = true;
		//goto state
		uiRes = ehmScrollIEPress;
		break;
	}

	// BEGIN.TormozIT - 1C8 fan
	else if (false
		|| ASCompareClassSimple(cpBuff, cpcV8FormElementWndClassName)
		|| ASCompareClassSimple(cpBuff, cpcV8GridWndClassName)
		|| ASCompareClassSimple(cpBuff, cpcV8WindowWndClassName)
		)
	{
		//mem info
		CSCrollIEProcessInfo& rInfo = pCfgMem->ScrollIEProcessInfo;
		rInfo.hWnd = hLookWnd;
		rInfo.bLockedScrollDirection = true;
		rInfo.bHorScroll = false;
		rInfo.bLockedWheelDelta = true;
		//goto state
		uiRes = ehmScrollIEPress;
		break;
	}

	// END.TormozIT - 1C8 fan

	;/*
    else if(
		false != ASCompareClassSimple(cpBuff, cpcOutlook2007ClassName)
		)
    {
      CSCrollIEProcessInfo& rInfo = pCfgMem->ScrollIEProcessInfo;
      rInfo.hWnd = hLookWnd;
      rInfo.bLockedScrollDirection = true;
      rInfo.bHorScroll = false;
      rInfo.bLockedWheelDelta = false;
      //goto state
      uiRes = ehmScrollIEPress;
      break;
    };*/
//    //find Pager Ctrl
//    if(false != ASCompareClassSimple(cpBuff, WC_PAGESCROLLER))
//    {
//      const HWND hcChildWnd = ::GetWindow(hLookWnd, GW_CHILD);
//      if(0 != hcChildWnd)
//      {
//        const bool bcHor = PGS_HORZ == ((PGS_HORZ | PGS_VERT) & lcLookStyle);
//        //try query scroll size from child - if size chnged - scroll valid
//        NMPGCALCSIZE Info;
//        Info.dwFlag = (false != bcHor)
//          ? PGF_CALCWIDTH
//          : PGF_CALCHEIGHT;
//        Info.iHeight = 0;
//        Info.iWidth = 0;
//        NMHDR& rHdr = Info.hdr;
//        rHdr.code = PGN_CALCSIZE;
//        rHdr.hwndFrom = hLookWnd;
//        rHdr.idFrom = ::GetDlgCtrlID(hLookWnd);
//        if(false != SafeSendMsg(hcChildWnd, WM_NOTIFY, rHdr.code, reinterpret_cast<LPARAM>(&rHdr)))
//        {
//          //calc client rect
//          RECT PgCliRect;
//          ::GetClientRect(hLookWnd, &PgCliRect);
//          const int icCliSize = (false != bcHor)
//            ? PgCliRect.right
//            : PgCliRect.bottom;
//          //info size
//          int& riSize = (false != bcHor)
//            ? Info.iWidth
//            : Info.iHeight;
//          if(icCliSize < riSize)
//          {
//            //valid scroll for pager ctrl
//            CScrollPagerCtrlInfo& rScrollInfo = pCfgMem->ScrollPagerCtrlInfo;
//            rScrollInfo.PntLastScroll = cpMsg->pt;
//            rScrollInfo.ClientRect = PgCliRect;
//            rScrollInfo.bHorProcess = bcHor;
//            rScrollInfo.hChildWnd = hcChildWnd;
//            rScrollInfo.hPagerCtrl = hLookWnd;
//            rScrollInfo.uiPagerCtrlId = rHdr.idFrom;
//            uiRes = ehmScrollPagerPress;
//            break;
//          };
//        };
//      };
//    };

    //in scroll bar analyse
    bInDScroll = true;
    //try look if "look" wnd have one of "scroll bar" style
    if(0 != ((WS_VSCROLL | WS_HSCROLL) & lcLookStyle))
    {
      //some of scrol bar find - not need drill down and need mem info
      bHorFind =
        0 != (WS_HSCROLL & lcLookStyle)
        && false !=  SCTestRangeGreatPage(hLookWnd, false, true);
      rProcessInfo.HInfo.hWnd = (false != bHorFind)
        ? hLookWnd
        : 0;
      rProcessInfo.HInfo.bIsCtrl = false;
      if(false != bHorFind)
      {
        lHSbDist = 0;
      };

      bVertFind =
        0 != (WS_VSCROLL & lcLookStyle)
        && false !=  SCTestRangeGreatPage(hLookWnd, false, false);
      rProcessInfo.VInfo.hWnd = (false != bVertFind)
        ? hLookWnd
        : 0;
      rProcessInfo.VInfo.bIsCtrl = false;
      if(false != bVertFind)
      {
        lVSbDist = 0;
      };
      //we find wnd with "scroll bar" style - it's target
      if(false != bVertFind || false != bHorFind)
      {
        break;
      };
    };

    ////wnd not have standard scroll bar's - test for "flat scrollbar's"
    //try load module (if already loaded)
    if(0 == hCommCtrlModule)
    {
      //not already getted
      HMODULE hModule = ::GetModuleHandle(cpcComCtrlModuleName);
      if(0 != hModule)
      {
        //module present - load now
        hCommCtrlModule = ::LoadLibrary(cpcComCtrlModuleName);
        pFlatSB_GetScrollProp = reinterpret_cast<FlatSB_GetScrollPropType>(::GetProcAddress(hCommCtrlModule, cpcFlatSB_GetScrollPropName));
        if(0 != pFlatSB_GetScrollProp)
        {
          pFlatSB_GetScrollInfo = reinterpret_cast<FlatSB_GetScrollInfoType>(::GetProcAddress(hCommCtrlModule, cpcFlatSB_GetScrollInfoName));
        };
      };
    };
    //function present
    if(0 != pFlatSB_GetScrollInfo)
    {
      //try get info from wnd's
      SCROLLINFO HScrInfo;
      HScrInfo.cbSize = sizeof(HScrInfo);
      HScrInfo.fMask = SIF_RANGE | SIF_PAGE;
      SCROLLINFO VScrInfo;
      VScrInfo.cbSize = sizeof(VScrInfo);
      VScrInfo.fMask = SIF_RANGE | SIF_PAGE;
      const BOOL bcHValid = pFlatSB_GetScrollInfo(hLookWnd, SB_HORZ, &HScrInfo);
      const BOOL hcVValid = pFlatSB_GetScrollInfo(hLookWnd, SB_VERT, &VScrInfo);
      if(FALSE != bcHValid || FALSE != hcVValid)
      {
        //hor
        bHorFind =
          FALSE != bcHValid
          && false != FSBTestRangeGreatPage(hLookWnd, true, HScrInfo);
        rProcessInfo.HInfo.hWnd = (false != bHorFind)
          ? hLookWnd
          : 0;
        rProcessInfo.HInfo.bIsCtrl = 0;
        if(false != bHorFind)
        {
          lHSbDist = 0;
        };

        //vert
        bVertFind =
          FALSE != hcVValid
          && false !=  FSBTestRangeGreatPage(hLookWnd, false, VScrInfo);
        rProcessInfo.VInfo.hWnd = (false != bVertFind)
          ? hLookWnd
          : 0;
        rProcessInfo.VInfo.bIsCtrl = false;
        if(false != bVertFind)
        {
          lVSbDist = 0;
        };
        //we find wnd with "scroll bar" style - it's target
        if(false != bVertFind || false != bHorFind)
        {
          bVertFlatSB = bVertFind;
          bHorFlatSB = bHorFind;
          break;
        };
      };
    };

    //look cycle for wnd
    for(HWND hSibl = ::GetWindow(hLookWnd, GW_CHILD); 0 != hSibl; hSibl = ::GetWindow(hSibl, GW_HWNDNEXT))
    {
      //loon on "enabled" state
      if(false == IsWndUserWork(hSibl))
      {
        continue;
      };
      //try look on sibling wnd from whis and find by wnd class
      AnalyseForDrillChildDown(hSibl, cpMsg, lHSbDist, bHorFind, lVSbDist, bVertFind, cpMsg->pt);
    };

    //if wnd have caption - break drilling
    if(FALSE != AHideCheckWndStyle(hLookWnd, false))
    {
      break;
    };
  };

  //is not some not find - try drill down from "stack"
  if(false != bInDScroll && (false == bVertFind || false == bHorFind))
  {
    //stack for later drill down
    using sl::CSLStack;
    CSLStack<HWND> MemWndList;
    //start from "parent to clicked" wnd
    {
      HWND hStartWnd = hcInitStart;
      //drill up until not found parent with caption
      for(HWND hLookParentWnd = hStartWnd; false != DrillDownAnalyseHelper(hLookParentWnd, dwWhileProcess); hLookParentWnd = GetParentWndForDrillUp(hLookParentWnd))
      {
        hStartWnd = hLookParentWnd;
        //look on wnd - if caption find - break cycle
        if(FALSE != AHideCheckWndStyle(hLookParentWnd, false))
        {
          break;
        };
      };
      MemWndList.Push(hStartWnd);
    };
    while(false == MemWndList.IsEmpty())
    {
      //get "scan parent" wnd
      const HWND hcScanParent = *MemWndList.Top();
      MemWndList.Pop();
      //scan level
      for(HWND hSibl = ::GetWindow(hcScanParent, GW_CHILD); 0 != hSibl; hSibl = ::GetWindow(hSibl, GW_HWNDNEXT))
      {
        if
          (
          //look on "is valid for process"
          false == DrillDownAnalyseHelper(hSibl, dwWhileProcess)
          //or wnd have caption - not drill it down
          || FALSE != AHideCheckWndStyle(hSibl, false)
          )
        {
          continue;
        };
        //try look on sibling wnd from whis and find by wnd class
        AnalyseForDrillChildDown(hSibl, cpMsg, lHSbDist, bHorFind, lVSbDist, bVertFind, cpMsg->pt);
        //push "looked" wnd to stack for later processing
        MemWndList.Push(hSibl);
      };
    };
  };

  //if some fidn - goto ehmScrollPress
  if(false != bInDScroll && (false != bVertFind || false != bHorFind))
  {
    uiRes = ehmScrollPress;
    //mem "flat sb" also
    rProcessInfo.VInfo.bFlatSB = bVertFlatSB;
    rProcessInfo.HInfo.bFlatSB = bHorFlatSB;
  };

  return uiRes;
};

bool CApp::MouseBtnDown(const UINT uicCurrKeyFlag, const MOUSEHOOKSTRUCT* const cpMsg)
{
  //it's first start (not "already press" present)
  //need look on "point WND" and look - if some press action can process
  //if some action present - mem data and "eat" msg

  bool bRes = false;
  UINT uiNewHookMode = ehmNone;

  //look on wnd from "this" pnt
  const HWND hcPntWnd = ::WindowFromPoint(cpMsg->pt);
  //mem "start" wnd process
  pCfgMem->dwScrollStartThread = ::GetWindowThreadProcessId(cpMsg->hwnd, 0);
  if(false != ScrollEnabled(pCfgMem) && 0 != hcPntWnd)
  {
    //look on "IsDisableSScroll key not pressed"
    if(false == CheckRealKey(pCfgMem->uiDirectScrollTmpDisableKey))
    {
      //now drill up in wnd hierarchy and try find wnd process
      uiNewHookMode = DrillChildUp(hcPntWnd, uicCurrKeyFlag, cpMsg);
    };
  };


  //if some mode present - mem start prm's
  if(ehmNone != uiNewHookMode)
  {
    //mem prm's for forward msg's
    pCfgMem->StartMsg = cpMsg->pt;
    bRes = true;
  };

  //try mem cursor from "start" wnd
  pCfgMem->hMemWndCursor = ::GetCursor();

  //chng hook mode
  ChngHookMode(uiNewHookMode);


  return bRes;
};

bool CApp::MouseBtnUp(const UINT uicCurrKeyFlag, const MOUSEHOOKSTRUCT* const cpMsg, const UINT uicMouseBtnNumb)
{
  //stop hook processing (for avoid recurent calling in BtnUp sending with menu's)
  //look on curr mode and process button up

  //easy
  bool bRes = true;
  const UINT uicOldMode = pCfgMem->lHookMode;

  //stop processing
  ChngHookMode(ehmNone);

  //look on hook mode
  switch(uicOldMode)
  {
    //it's start scroll (scrool not process) - need forward message to "start wnd") and not eat "this" msg
    //for stop IE scroll too
  case ehmScrollPress:
  case ehmScrollIEPress:
    {
      _ASSERT(COUNTOF(cpcStopMouseNCInfo) > uicMouseBtnNumb);
      const StopMouseMsg& rcInfo = cpcStopMouseNCInfo[uicMouseBtnNumb];
      //emulate click
      pCfgMem->uiSkipUpFlag |= rcInfo.uiSkipMask;
      pCfgMem->uiSkipDownFlag |= rcInfo.uiSkipMask;
      mouse_event(rcInfo.uiDownMsg, 0, 0, 0, 0);
      mouse_event(rcInfo.uiUpMsg, 0, 0, 0, 0);
    };
    break;

  case ehmBDScrollIEProcess:
    bRes = IEScrollStopBD(cpMsg, false);
    break;

    //stop scroll for IE
  case ehmScrollIEProcess:
    bRes = IEScrollStop(cpMsg, false);
    break;

  case ehmScrollPagerProcess:
    bRes = PagerScrollStop(cpMsg, false);
    break;

  case ehmBDScrollProcess:
    //some scroll bar ctrl process present - need eat msg and process "scroll bar button up" event
    SBScrollStopBD(cpMsg, false);
    bRes = true;
    break;

  case ehmScrollProcess:
    //some scroll bar ctrl process present - need eat msg and process "scroll bar button up" event
    SBScrollStop(cpMsg, false);
    bRes = true;
    break;

  default:
    bRes = false;
    break;
  };

  ////set cursor for "stoped mouse pnt"
  //need espesialy for IE
  RestoreCursorInWnd(cpMsg->hwnd, cpMsg->pt);

  return bRes;
};

void CApp::RestoreCursorInWnd(const HWND hcStartWnd, const POINT& rcPnt) const
{
  DWORD_PTR dwHitTest = 0;
  HWND hWnd = hcStartWnd;
  if(false != FintNotTransparent(hWnd, rcPnt.x, rcPnt.y, dwHitTest))
  {
    //send "set cursor" for finede wnd as from "start" wnd for "mousemove"
    DWORD_PTR dwRes = 0;
    if
      (
      //se cursor failed
      false == SafeSendMsg(hWnd, WM_SETCURSOR, reinterpret_cast<WPARAM>(hcStartWnd), MAKELPARAM(dwHitTest, WM_MOUSEMOVE), &dwRes)
      //set sursor not handled by wnd proc
      || FALSE == dwRes
      )
    {
      ////get cursor from wnd class
      const DWORD_PTR dwcCursor = ::GetClassLongPtr(hWnd, GCLP_HCURSOR);
      HCURSOR hSetCursor = (0 != dwcCursor)
        ? reinterpret_cast<HCURSOR>(dwcCursor)
        : pCfgMem->hMemWndCursor;
      ::SetCursor(hSetCursor);
    };
  };
};

void CApp::MouseControlAbort(const bool bcAbortForward)
{
  bool bNeedChng = true;

  const HWND hcMemSubWnd = pCfgMem->hSubClassScrollWnd;
  switch(pCfgMem->lHookMode)
  {
    ////currently key is pressed - simple goto "none" mode
    //and for IE - too
  case ehmScrollPress:
  case ehmScrollIEPress:
    //chng to "none"
    break;

    //scroll process - send "btn up" msg to wnd
    //goto "none" mode
  case ehmScrollProcess:
    SBScrollStop(0, true);
    break;

  case ehmBDScrollProcess:
    SBScrollStopBD(0, true);
    break;

  case ehmBDScrollIEProcess:
    IEScrollStopBD(0, true);
    break;

    //abort scrol in IE
  case ehmScrollIEProcess:
    IEScrollStop(0, true);
    break;

  case ehmScrollPagerProcess:
    PagerScrollStop(0, true);
    break;

  default:
    bNeedChng = false;
    break;
  };

  //clear mouse state
  pCfgMem->ulControlState = empMouseProcessNone;

  //if need chng mode
  if(false != bNeedChng)
  {
    ChngHookMode(ehmNone);
    POINT Pnt;
    ::GetCursorPos(&Pnt);
    const HWND hcWnd = ::WindowFromPoint(Pnt);
    if(0 != hcWnd)
    {
      RestoreCursorInWnd(hcWnd, Pnt);
    };
    ////clear current key's
    pCfgMem->uiCurrWorkKeysMask = 0;
    pCfgMem->uiCurrWorkKeys = 0;
    pCfgMem->uiCurrMouseKeys = 0;
    //try stop forwarding
    if(false == bcAbortForward || FALSE == ::IsWindow(hcMemSubWnd))
    {
      pCfgMem->bMouseHookInForwardMode = FALSE;
    };
  };
};

void CApp::MouseSlipNewCoords(const UINT uicCurrKeyFlag, const MOUSEHOOKSTRUCT* const cpMsg)
{
  //look on curr mode
  switch(pCfgMem->lHookMode)
  {
  case ehmScrollProcess:
    SBScrollSlipChng(cpMsg);
    break;

  case ehmBDScrollProcess:
    SBScrollSlipChngBD(cpMsg);
    break;

  case ehmBDScrollIEProcess:
  case ehmScrollIEProcess:
    IEScrollSlipChng(cpMsg);
    break;

  case ehmScrollPagerProcess:
    PagerScrollSlipChng(cpMsg);
    break;

  default:
    break;
  };
};

bool CApp::MouseMoveAll(const UINT uicCurrKeyFlag, const MOUSEHOOKSTRUCT* const cpMsg)
{
  //it's move - press testing already tested
  //look on current move mode and process it's

  bool bRes = true;
  //look on curr mode
  switch(pCfgMem->lHookMode)
  {
    //btn pressed - look if mouse in "not drag rect" and if not in - process scroll
    //and for IE - too

  case ehmScrollPress:
    //look pnt in rect
    if(DragPresent(pCfgMem->StartMsg, cpMsg->pt))
    {
      //move out of not drag rect - start control of scroll bar
      if(FALSE != pCfgMem->bBDScrollingEnabled)
      {
        SBScrollStartBD(cpMsg);
        //switch to ehmBDScrollProcess mode
        ChngHookMode(ehmBDScrollProcess);
      }
      else
      {
        SBScrollStart(cpMsg);
        //switch to ehmScrollProcess mode
        ChngHookMode(ehmScrollProcess);
      };
      bRes = true;
      break;
    };
    bRes = true;
    break;

  case ehmScrollIEPress:
    if(false != DragPresent(pCfgMem->StartMsg, cpMsg->pt))
    {
      if
        (
        false == pCfgMem->ScrollIEProcessInfo.bLockedScrollDirection
        && FALSE != pCfgMem->bBDScrollingEnabled 
        )
      {
        bRes = IEScrollStartBD(cpMsg);
        ChngHookMode(ehmBDScrollIEProcess);
      }
      else
      {
        bRes = IEScrollStart(cpMsg);
        ChngHookMode(ehmScrollIEProcess);
      };
    };
    bRes = true;
    break;

  case ehmScrollPagerPress:
    {
      const bool bcDrag = (false != pCfgMem->ScrollPagerCtrlInfo.bHorProcess)
        ? DragPresentX(pCfgMem->StartMsg, cpMsg->pt)
        : DragPresentY(pCfgMem->StartMsg, cpMsg->pt);
      if(false != bcDrag)
      {
        bRes = PagerScrollStart(cpMsg);
        ChngHookMode(ehmScrollPagerProcess);
      };
    };
    break;

  case ehmScrollProcess:
    //scroll bar control already in process - forward to scroll bar ctlr
    SBScrollMove(cpMsg);
    bRes = true;
    break;

  case ehmBDScrollProcess:
    //scroll bar control already in process - forward to scroll bar ctlr
    SBScrollMoveBD(cpMsg);
    bRes = true;
    break;

  case ehmBDScrollIEProcess:
    IEScrollMoveBD(cpMsg);
    break;

  case ehmScrollIEProcess:
    bRes = IEScrollMove(cpMsg);
    break;

  case ehmScrollPagerProcess:
    bRes = PagerScrollMove(cpMsg);
    break;

  default:
    bRes = false;
    break;
  };

  return bRes;
};

bool CApp::PagerScrollMove(const MOUSEHOOKSTRUCT* const cpMsg)
{
  CScrollPagerCtrlInfo& rInfo = pCfgMem->ScrollPagerCtrlInfo;
  //move direction
  POINT MouseShift = cpMsg->pt;
  POINT LastMousePnt = rInfo.PntLastScroll;
  const bool bcReduct = CheckRealKey(pCfgMem->uiScollReductionKey);
  SBScrollCalcShift(LastMousePnt, MouseShift, rInfo.LastScrollMagnMiss, bcReduct, egcMagnScrollReduction);

  //move size
  LONG lMove = (false != rInfo.bHorProcess)
    ? MouseShift.x
    : MouseShift.y;
  if(FALSE != pCfgMem->bScrollInv)
  {
    lMove = -lMove;
  };

  bool bPosScroll = true;
  LONG lAbsMove = lMove;
  if(0 > lMove)
  {
    lAbsMove = -lMove;
    bPosScroll = false;
  };

  //need start scroll?
  if(rInfo.lStartPos < lAbsMove)
  {
    ////try get scroll size for look valid scrolling range
    NMPGCALCSIZE SizeInfo;
    SizeInfo.dwFlag = (false != rInfo.bHorProcess)
      ? PGF_CALCWIDTH
      : PGF_CALCHEIGHT;
    int& rScrollSize = (false != rInfo.bHorProcess)
      ? SizeInfo.iWidth
      : SizeInfo.iHeight;
    rScrollSize = 0;
    const LONG lcCliSize = (false != rInfo.bHorProcess)
      ? rInfo.ClientRect.right
      : rInfo.ClientRect.bottom;
    NMHDR& rScrHdr = SizeInfo.hdr;
    rScrHdr.hwndFrom = rInfo.hPagerCtrl;
    rScrHdr.idFrom = rInfo.uiPagerCtrlId;
    rScrHdr.code = PGN_CALCSIZE;
    SafeSendMsg(rInfo.hChildWnd, WM_NOTIFY, rScrHdr.idFrom, reinterpret_cast<LPARAM>(&SizeInfo));
    const LONG lcMaxPosSize = rScrollSize - lcCliSize;

    //prepare ScrInfo for query from child
    NMPGSCROLL ScrInfo;
    ScrInfo.fwKeys = 0;
    ScrInfo.rcParent = rInfo.ClientRect;

    if(false != rInfo.bHorProcess)
    {
      ScrInfo.iXpos = rInfo.lCurrPos;
      ScrInfo.iYpos = 0;
      ScrInfo.iDir = (false == bPosScroll)
        ? PGF_SCROLLLEFT
        : PGF_SCROLLRIGHT;
    }
    else
    {
      ScrInfo.iXpos = 0;
      ScrInfo.iYpos = rInfo.lCurrPos;
      ScrInfo.iDir = (false == bPosScroll)
        ? PGF_SCROLLUP
        : PGF_SCROLLDOWN;
    };
    ScrInfo.iScroll = lAbsMove;

    NMHDR& rHdr = ScrInfo.hdr;
    rHdr.code = PGN_SCROLL;
    rHdr.hwndFrom = rInfo.hPagerCtrl;
    rHdr.idFrom = rInfo.uiPagerCtrlId;
    //call child
    SafeSendMsg(rInfo.hChildWnd, WM_NOTIFY, rInfo.uiPagerCtrlId, reinterpret_cast<LPARAM>(&ScrInfo));
    //apply user chng in scroll val
    rInfo.lStartPos = ScrInfo.iScroll - lAbsMove;
    //chng pos in paget ctrl
    rInfo.lCurrPos += (false != bPosScroll)
      ? ScrInfo.iScroll
      : -ScrInfo.iScroll;
    //pos in valid range's
    if(0 > rInfo.lCurrPos)
    {
      rInfo.lCurrPos = 0;
    }
    else if(lcMaxPosSize < rInfo.lCurrPos)
    {
      rInfo.lCurrPos = lcMaxPosSize;
    };
    SafeSendMsg(rInfo.hPagerCtrl, PGM_SETPOS, 0, rInfo.lCurrPos);
    rInfo.PntLastScroll = LastMousePnt;
  };

  return true;
};

bool CApp::IEScrollMove(const MOUSEHOOKSTRUCT* const cpMsg)
{
  return IEScrollMoveInt(cpMsg, pCfgMem->ScrollIEProcessInfo);
};

bool CApp::IEScrollStartBD(const MOUSEHOOKSTRUCT* const cpMsg)
{
  const POINT& rcPnt = cpMsg->pt;
  const POINT& rcStartPnt = pCfgMem->StartMsg;
  SIZE Size;
  Size.cx = rcPnt.x - rcStartPnt.x;
  Size.cx = max(Size.cx, -Size.cx);
  Size.cy = rcPnt.y - rcStartPnt.y;
  Size.cy = max(Size.cy, -Size.cy);
  const bool bcHorProcess = Size.cx > Size.cy;

  return IEScrollStartInt(cpMsg, pCfgMem->ScrollIEProcessInfo, true, bcHorProcess);
};

bool CApp::IEScrollMoveBD(const MOUSEHOOKSTRUCT* const cpMsg)
{
  bool bRes = false;

  //easy
  CSCrollIEProcessInfo& rProcessInfo = pCfgMem->ScrollIEProcessInfo;

  ////look chng scroll direction
  //calc shift from this point to curr pnt
  bool bChngScrollDir = false;
  POINT LastShift;
  LastShift.x = rProcessInfo.LastScrollPnt.x - cpMsg->pt.x;
  LastShift.x = max(LastShift.x, -LastShift.x);
  LastShift.y = rProcessInfo.LastScrollPnt.y - cpMsg->pt.y;
  LastShift.y = max(LastShift.y, -LastShift.y);
  const bool bcWantHor = LastShift.y < LastShift.x;
  if(rProcessInfo.bHorScroll != bcWantHor)
  {
    //look - if chng out of "drag" range
    const LONG lcTriggerRange = ::GetSystemMetrics((false != rProcessInfo.bHorScroll) ? SM_CYDRAG : SM_CXDRAG);
    const LONG lcCurrRange = (false != rProcessInfo.bHorScroll) ? LastShift.y : LastShift.x;
    if(lcCurrRange > lcTriggerRange)
    {
      bChngScrollDir = true;
    };
  };

  if(false != bChngScrollDir)
  {
    //try chng direction
    IEScrollStopInt(cpMsg, rProcessInfo, false);
    rProcessInfo.bHorScroll = !rProcessInfo.bHorScroll;
    IEScrollStartInt(cpMsg, rProcessInfo, false, rProcessInfo.bHorScroll);
    //visualize chng
    ChngHookMode(ehmBDScrollIEProcess);
    bRes = true;
  }
  else
  {
    //not chng direction
    bRes = IEScrollMoveInt(cpMsg, rProcessInfo);
  };
  return bRes;
};

inline bool CApp::IEScrollStopBD(const MOUSEHOOKSTRUCT* const cpMsg, const bool bcAbort)
{
  return IEScrollStopInt(cpMsg, pCfgMem->ScrollIEProcessInfo, bcAbort);
};

void CApp::SBScrollStartBD(const MOUSEHOOKSTRUCT* const cpMsg)
{
  //if two of scrol bar's enabled - look on "most moved direction"
  //else - ctrl presented
  //get rect of thumb for scroll bar and emulate mouse pressing (WM_LBUTTONDOWN - in middle of thumb)
  //mem mouse pnt for later restoring
  //calc and store "saved distance"

  //for easy
  const POINT rcPnt = cpMsg->pt;
  CScrollProcessInfo& rProcessInfo = pCfgMem->ScrollProcessInfo;

  //mem "is wnd captured" flas
  const HWND hcCapturedWnd = ::GetCapture();
  rProcessInfo.HInfo.bIsCaptured = rProcessInfo.HInfo.hWnd == hcCapturedWnd;
  rProcessInfo.VInfo.bIsCaptured = rProcessInfo.VInfo.hWnd == hcCapturedWnd;

  if(0 != rProcessInfo.HInfo.hWnd && 0 != rProcessInfo.VInfo.hWnd)
  {
    //two sb present - look "most move" direction
    const POINT& rcStartPnt = pCfgMem->StartMsg;
    SIZE Size;
    Size.cx = (rcPnt.x > rcStartPnt.x)
      ? rcPnt.x - rcStartPnt.x
      : rcStartPnt.x - rcPnt.x;
    Size.cy = (rcPnt.y > rcStartPnt.y)
      ? rcPnt.y - rcStartPnt.y
      : rcStartPnt.y - rcPnt.y;
    rProcessInfo.bHorProcess = Size.cx > Size.cy;

    const DWORD dwcHRed = SBScrollCalcAddReduct(rProcessInfo.HInfo.hWnd, rProcessInfo.HInfo.bIsCtrl, true, rProcessInfo.HInfo.bFlatSB);
    const DWORD dwcVRed = SBScrollCalcAddReduct(rProcessInfo.VInfo.hWnd, rProcessInfo.VInfo.bIsCtrl, false, rProcessInfo.VInfo.bFlatSB);
    const DWORD dwcMidReduct = min(dwcHRed, dwcVRed);
    rProcessInfo.HInfo.dwScrollReduction = dwcMidReduct;
    rProcessInfo.VInfo.dwScrollReduction = dwcMidReduct;
  }
  else
  {
    //only one sb present
    rProcessInfo.bHorProcess = (0 != rProcessInfo.HInfo.hWnd);
    if(0 != rProcessInfo.HInfo.hWnd)
    {
      rProcessInfo.bHorProcess = true;
      rProcessInfo.HInfo.dwScrollReduction = SBScrollCalcAddReduct(rProcessInfo.HInfo.hWnd, rProcessInfo.HInfo.bIsCtrl, true, rProcessInfo.HInfo.bFlatSB);
    }
    else
    {
      rProcessInfo.bHorProcess = false;
      rProcessInfo.VInfo.dwScrollReduction = SBScrollCalcAddReduct(rProcessInfo.VInfo.hWnd, rProcessInfo.VInfo.bIsCtrl, false, rProcessInfo.VInfo.bFlatSB);
    };
  };

  SBScrollStartInt(rProcessInfo, (false != rProcessInfo.bHorProcess) ? rProcessInfo.HInfo : rProcessInfo.VInfo, rProcessInfo.bHorProcess, rcPnt);
};

void CApp::SBScrollMoveBD(const MOUSEHOOKSTRUCT* const cpMsg)
{
  //easy
  CScrollProcessInfo& rProcessInfo = pCfgMem->ScrollProcessInfo;

  ////look chng scroll direction
  //calc shift from this point to curr pnt
  bool bChngScrollDir = false;
  if(0 != rProcessInfo.HInfo.hWnd && 0 != rProcessInfo.VInfo.hWnd)
  {
    POINT LastShift;
    LastShift.x = rProcessInfo.LastScrollPnt.x - cpMsg->pt.x;
    LastShift.x = max(LastShift.x, -LastShift.x);
    LastShift.y = rProcessInfo.LastScrollPnt.y - cpMsg->pt.y;
    LastShift.y = max(LastShift.y, -LastShift.y);
    const bool bcWantHor = LastShift.y < LastShift.x;
    if(rProcessInfo.bHorProcess != bcWantHor)
    {
      //look - if chng out of "drag" range
      const LONG lcTriggerRange = ::GetSystemMetrics((false != rProcessInfo.bHorProcess) ? SM_CYDRAG : SM_CXDRAG);
      const LONG lcCurrRange = (false != rProcessInfo.bHorProcess) ? LastShift.y : LastShift.x;
      if(lcCurrRange > lcTriggerRange)
      {
        bChngScrollDir = true;
      };
    };
  };

  if(false != bChngScrollDir)
  {
    //try chng direction
    SBScrollStopInt(rProcessInfo, (false != rProcessInfo.bHorProcess) ? rProcessInfo.HInfo : rProcessInfo.VInfo, rProcessInfo.bHorProcess, cpMsg->pt, false);
    rProcessInfo.bHorProcess = !rProcessInfo.bHorProcess;
    SBScrollStartInt(rProcessInfo, (false != rProcessInfo.bHorProcess) ? rProcessInfo.HInfo : rProcessInfo.VInfo, rProcessInfo.bHorProcess, cpMsg->pt);
    //visualize chng
    ChngHookMode(ehmBDScrollProcess);
  }
  else
  {
    //not chng direction
    SBScrollMoveInt(rProcessInfo, (false != rProcessInfo.bHorProcess) ? rProcessInfo.HInfo : rProcessInfo.VInfo, rProcessInfo.bHorProcess, cpMsg->pt);
  };
};

void CApp::SBScrollStopBD(const MOUSEHOOKSTRUCT* const cpMsg, const bool bcAbort)
{
  //easy
  CScrollProcessInfo& rProcessInfo = pCfgMem->ScrollProcessInfo;
  const bool bcIsHor = rProcessInfo.bHorProcess;

  //emulate "MouseButtonUp" in last thumb center pnt
  SBScrollStopInt(rProcessInfo, (false != bcIsHor) ? rProcessInfo.HInfo : rProcessInfo.VInfo, bcIsHor, cpMsg->pt, bcAbort);
};

void CApp::SBScrollStart(const MOUSEHOOKSTRUCT* const cpMsg)
{
  //if two of scrol bar's enabled - look on "most moved direction"
  //else - ctrl presented
  //get rect of thumb for scroll bar and emulate mouse pressing (WM_LBUTTONDOWN - in middle of thumb)
  //mem mouse pnt for later restoring
  //calc and store "saved distance"

  //for easy
  const POINT rcPnt = cpMsg->pt;
  CScrollProcessInfo& rProcessInfo = pCfgMem->ScrollProcessInfo;

  //mem "is wnd captured" flas
  const HWND hcCapturedWnd = ::GetCapture();
  rProcessInfo.HInfo.bIsCaptured = rProcessInfo.HInfo.hWnd == hcCapturedWnd;
  rProcessInfo.VInfo.bIsCaptured = rProcessInfo.VInfo.hWnd == hcCapturedWnd;

  if(0 != rProcessInfo.HInfo.hWnd && 0 != rProcessInfo.VInfo.hWnd)
  {
    //two sb present - look "most move" direction
    const POINT& rcStartPnt = pCfgMem->StartMsg;
    SIZE Size;
    Size.cx = rcPnt.x - rcStartPnt.x;
    Size.cx = max(Size.cx, -Size.cx);
    Size.cy = rcPnt.y - rcStartPnt.y;
    Size.cy = max(Size.cy, -Size.cy);
    rProcessInfo.bHorProcess = Size.cx > Size.cy;
  }
  else
  {
    //only one sb present
    rProcessInfo.bHorProcess = (0 != rProcessInfo.HInfo.hWnd);
  };
  if(0 != rProcessInfo.bHorProcess)
  {
    rProcessInfo.bHorProcess = true;
    rProcessInfo.HInfo.dwScrollReduction = SBScrollCalcAddReduct(rProcessInfo.HInfo.hWnd, rProcessInfo.HInfo.bIsCtrl, true, rProcessInfo.HInfo.bFlatSB);
  }
  else
  {
    rProcessInfo.bHorProcess = false;
    rProcessInfo.VInfo.dwScrollReduction = SBScrollCalcAddReduct(rProcessInfo.VInfo.hWnd, rProcessInfo.VInfo.bIsCtrl, false, rProcessInfo.VInfo.bFlatSB);
  };

  SBScrollStartInt(rProcessInfo, (false != rProcessInfo.bHorProcess) ? rProcessInfo.HInfo : rProcessInfo.VInfo, rProcessInfo.bHorProcess, rcPnt);
};

void CApp::SBScrollMove(const MOUSEHOOKSTRUCT* const cpMsg)
{
  //easy
  CScrollProcessInfo& rProcessInfo = pCfgMem->ScrollProcessInfo;

  //not chng direction
  SBScrollMoveInt(rProcessInfo, (false != rProcessInfo.bHorProcess) ? rProcessInfo.HInfo : rProcessInfo.VInfo, rProcessInfo.bHorProcess, cpMsg->pt);
};

void CApp::SBScrollStop(const MOUSEHOOKSTRUCT* const cpMsg, const bool bcAbort)
{
  //easy
  CScrollProcessInfo& rProcessInfo = pCfgMem->ScrollProcessInfo;
  const bool bcIsHor = rProcessInfo.bHorProcess;

  //emulate "MouseButtonUp" in last thumb center pnt
  SBScrollStopInt(rProcessInfo, (false != bcIsHor) ? rProcessInfo.HInfo : rProcessInfo.VInfo, bcIsHor, cpMsg->pt, bcAbort);
};

bool CApp::IEScrollStart(const MOUSEHOOKSTRUCT* const cpMsg)
{
  bool bRes = false;
  CSCrollIEProcessInfo& rInfo = pCfgMem->ScrollIEProcessInfo;
  if(false == rInfo.bLockedScrollDirection)
  {
    const POINT& rcPnt = cpMsg->pt;
    const POINT& rcStartPnt = pCfgMem->StartMsg;
    SIZE Size;
    Size.cx = rcPnt.x - rcStartPnt.x;
    Size.cx = max(Size.cx, -Size.cx);
    Size.cy = rcPnt.y - rcStartPnt.y;
    Size.cy = max(Size.cy, -Size.cy);
    const bool bcHorProcess = Size.cx > Size.cy;

    bRes = IEScrollStartInt(cpMsg, rInfo, true, bcHorProcess);
  }
  else
  {
    bRes = IEScrollStartInt(cpMsg, rInfo, true, rInfo.bHorScroll);
  };
  return bRes;
};


//////////////////////////////////////////////////////////////////////
//main init proc

extern "C" BOOL WINAPI DllMain(HANDLE hDllHandle, DWORD dwReason, LPVOID lpReserved)
{
  BOOL bRes = TRUE;

  switch(dwReason)
  {
  case DLL_PROCESS_ATTACH:
#ifdef __REL_CFG
    //dbg:
    sl::SLExcInstall("\\rpt.txt");
    //dbg:
#endif//__REL_CFG

    //try create and init app instance - if failed - not init dll
    pApp = new CApp(reinterpret_cast<HINSTANCE>(hDllHandle));
    if(false == pApp->Init())
    {
      pApp->Finit();
      delete pApp;
      pApp = 0;
      bRes = FALSE;
    };
    break;

  case DLL_PROCESS_DETACH:
    //try finit/delete app
    pApp->Finit();
    delete pApp;
    pApp = 0;

#ifdef __REL_CFG
    //dbg:
    sl::SLExcUnInstall();
    //dbg:
#endif//__REL_CFG
    break;

  default:
    break;
  };
  return bRes;
};

extern "C" LRESULT CALLBACK MIHookFunc1(int nCode, WPARAM wParam, LPARAM lParam)
{
#ifdef __REL_CFG
  __try
  {
#endif//__REL_CFG
    return (pApp->*pCBTFunc)(nCode, wParam, lParam);
#ifdef __REL_CFG
  }
  __except(sl::SLExceptionFilter(GetExceptionInformation()), EXCEPTION_CONTINUE_SEARCH)
  {
  };
  return 0;
#endif//__REL_CFG
};

extern "C" LRESULT CALLBACK MIHookFunc(int nCode, WPARAM wParam, LPARAM lParam)
{
#ifdef __REL_CFG
  __try
  {
#endif//__REL_CFG
    return (pApp->*pMouseFunc)(nCode, wParam, lParam);
#ifdef __REL_CFG
  }
  __except(sl::SLExceptionFilter(GetExceptionInformation()), EXCEPTION_CONTINUE_SEARCH)
  {
  };
  return 0;
#endif//__REL_CFG
};

//get version number for hook module
extern "C" DWORD MIGetVer()
{
  return GetVersionNumber();
};

