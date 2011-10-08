/**********************************************************************
global's for mouse imp
**********************************************************************/

#ifndef __MIGLOBAL_H
#define __MIGLOBAL_H

#pragma once

/*
AHide behavior
1 MosueActivity (WM_MOUSEMOVE) - emcAHWndActivity

  1.1 try find wnd in "ProcessWndList"
  1.2 if find - look on "is wnd open"
  1.3 if not - open window and bring to top of "some wnd's" (not activate for not "focus switching" behavior)

2 Keyboard activisation (WM_ACTIVATE) - emcAHWndActive

  2.1 try find wnd in "ProcessWndList"
  2.2 if find - look wnd in "is wnd open"
  2.3 if not - open wnd and bring to top
  2.4 lock wnd (wnd active flag) for not remove by AhtoHide timer (OpenWndList)

3 WndShow (WM_SHOWWINDOW and other) - emcAHWndShowHide

  3.1 try find in "ProcessWndList"
  3.2 if not find - try process by WnbdPatternList
  3.3 if passed - insert to ProcessWndList, open, not lock by "wnd active" flag and insert to (OpenWndList)

4 WndHide (WM_SHOWWINDOW and other) - emcAHWndShowHide

  4.1 try find in "ProcessWndList"
  4.2 if find - remove "wnd close" state and unlock all state's

5 WndDestroy (WM_DESTROY) - emcAHWndDestroy

  5.1 try find in "ProcessWndList"
  5.2 if find - remove from "ProcessWndList"
*/

//shirnk working types
enum ShrinkTypeEnum
{
  //shrik wnd
  estShrink = 0,
  //only activate wnd
  estActivateWnd,
  //numb
  estLast
};

//try icon click mode
enum TrayIconClickEnum
{
  etiTrayIconClickSwitchDirectScroll,
  etiTrayIconClickSwitchAutoHide,
  etiTrayIconClickLast,
};

//enum's, used for identofy action with cpcGlobalIntMessage message - in wParam
enum GlbMsgEnum
{
  //hide wnd, in that passed message
  egmHideWnd = 0,
  //show window
  egmShowWnd,
  //try uninstall subclassing - sended from exit host app
  egmUnInstallSubClassing,
  //try uninstall subclassing - posted from exit host while egmUnInstallSubClassing failed
  egmDelayUnInstallSubClassing,
  //go off from HookForward mode
  egmOffHookForwarding,
  //get app name to SharedInfo::cpGetAppNameResultBuff mass
  egmGetAppName
};

//msg's for host app

enum MsgConstEnum
{
  //message, resiverd from tray icon ty main host wnd
  emcToHostTrayIconNotifyMsg = WM_APP + 100,
  //msg, notifyed main host wnd by chng mode (LPARAM is tray icon enum)
  emcChngModeSeondToMainHost,
  //wnd resive mouse activity (auto hide if present in "ProcessWndList")
  //LPARAM - passed wnd
  emcAHWndActivity,
  //wnd destroy - remove from ProcessWndList
  //LPARAM - passed wnd
  emcAHWndDestroy,
  //wnd resive activation (by key or other) - "lock" wnd auto hide
  //LPARAM - passed wnd
  //wparam - active/inactive (BOOL)
  emcAHWndActive,
  //reread wnd pattern list (send from cfg app)
  //lParam != FALSE - read cfg from reg
  emcReReadWndPattList,
  //wnd hided - lParam - HWND
  emcAHWndHide,
  //wnd showed - lParam - HWND, wParam - TRUE if wnd is active
  emcAHWndShow,
  //get info about pased process id
  //LPARAM is process id,
  emcGetAppNameById,
  //app exited - need delete app info from AppInfoList (in host)
  //LPARAM is process id
  emcAppExited,
  //need save cfg by host application (chnged by CfgApp)
  emcNeedSaveCfg,
  //new app started - sended from hook to host dll
  //LAPRAM is handle of file mapping (in page file) routed to host process (if FALSE != pCfgMem->bNotSendModuleFileName)
  //WPARAM is id of process, what started
  emcNewAppStarted,
  //registration info entered in cfg application
  emcRegInfoEnter,
};

//messages, sended to cfg app
enum EnumCfgMsg
{
  //get help if from dlg (ised internaly in CfgApp in it's help system
  //on this msg wnd can return help topic id, associated with this wnd
  ecmGetHelpId = WM_APP + 100,
  //app should show itself (used for goto from "popup find" mode)
  ecmShowApp
};

enum GlobalConstEnum
{
  //internal version
  eccInternalVer = 9,
  //valid key's for mouse processing
  egcValidMouseKeysMask = MK_LBUTTON | MK_MBUTTON | MK_RBUTTON,
  //default ScrollReduction
  egcMagnScrollReduction = 0x10000,
  egcDefScrollReduction = egcMagnScrollReduction * 4,
  //sub reduction values
  egcAddReductMax = egcMagnScrollReduction * 32,
  egcAddReductMin = egcMagnScrollReduction / 32,
  //number of precent's
  egcNumberPerCentValue = 100,
  //scroll reduction margin's
  egcMarginScrollReductionPsMin = egcAddReductMin * egcNumberPerCentValue / egcMagnScrollReduction,
  egcMarginScrollReductionPsMax = egcAddReductMax * egcNumberPerCentValue / egcMagnScrollReduction,
  //key for scroll reduction (LeftButton)
  egcDecScollReductionKey = VK_LBUTTON,
  //size of "window class" text
  egcWndClassTextLen = 100,
  //default "AutoHide" timer time
  egcDefAutoHideTimerTime = 1000,
  //AutoHide timer range's
  egcMinAutoHideTimerTime = 250,
  egcMaxAutoHideTimerTime = 10000,
  //magnifer for AutoHide timer (from val to sec's)
  egcAutoHideTimerMagnifer = 1000,
  ////const's for IE scroll
  //magnifer for convert dwIESCrollConvMove
  egcIEScrollConvMoveMagn = 1024,
  ////default's for IE scroll prms's
  egcIEScrollMinMoveDef = 2,
  egcIEScrollConvMoveDef = 1024 * 10,
  ////margin's for dwIESCrollConvMove (ie move sens) in PS
  egcIEScrollConvMoveMin = 5,
  egcIEScrollConvMoveMax = 20000,
  //ie vert/hor multipler
  egcIEScrollHorVertCoef = 20,
  //shift/delta conversion for "wheel scrolling" emulation
  egcWheelScrollingSBPointPerDelta = 8,
  //time for save cfg in host app (mili sec)
  egcHostSaveCfgTime = 1000 * 60 * 70,
  //max wnd proc recursion count (used for break recursion and forward all to "DefWindowProc")
  egcWndProcMaxRecursionCount = 100,
};

//mode of hook proc
enum HookModeEnum
{
  ehmNone = 0,
  //button hold down for scroll mode (not move perform)
  ehmScrollPress,
  //mouse moved for scroll - all movind process scroll
  ehmScrollProcess,
  //bidirection scrolling
  ehmBDScrollProcess,
  //btn press down in IE4 (not move present)
  ehmScrollIEPress,
  //mouse move for scroll in IE4 - all move process scroll
  ehmScrollIEProcess,
  //bidirection IE scrolling
  ehmBDScrollIEProcess,
  //process pager ctrl
  ehmScrollPagerPress,
  ehmScrollPagerProcess
};

////scroll control types
enum ScrollControlEnum
{
  //standard
  escStandard,
  //toggle mode with eat click's
  escToggleWithEat,
  //mix mode (escStandard + escToggleWithEat)
  escScrollModeMix,
};

//mouse keys processing states
enum MouseProcessStateEnum
{
  //not any mode state present before
  //on press work key ScrollControlEnum analyse will perform
  empMouseProcessNone = 0,
  //state for escStandard mode
  empMouseProcessDirect,
  //toggle states
  empMouseProcessToggleScroll,
  empMouseProcessToggleSlipPressed,
  //mix mode alayze
  empMouseProcessMixLook,
  //last state
  empMouseProcessLast,
};

//infor for enumlate scroll in IE
struct CSCrollIEProcessInfo
{
  //wnd
  HWND hWnd;
  //last "scroll" point
  POINT PntLastStart;
  //point of "start scroll"
  POINT PntStartScroll;
  //mem for magn misses
  POINT LastScrollMagnMiss;
  //last point with present scroll (used with DB scrolling)
  POINT LastScrollPnt;
  //remainder from scroll conversion
  LONG lRemand;
  //scroll in hor direction
  bool bHorScroll;
  //locked scrolling
  bool bLockedScrollDirection;
  //locked "whell delta" (used for WinAmp scrolling)
  bool bLockedWheelDelta;
};

////info for scroll pager ctrl
struct CScrollPagerCtrlInfo
{
  //last looking pnt
  POINT PntLastScroll;
  //mem for magn misses
  POINT LastScrollMagnMiss;
  //is pager horisontal
  bool bHorProcess;
  //paget ctrl
  HWND hPagerCtrl;
  //paget ctrl id
  UINT uiPagerCtrlId;
  //child wnd of pager ctrl
  HWND hChildWnd;
  //client rect of pager ctrl
  RECT ClientRect;
  //current scrolling pos
  LONG lCurrPos;
  //pos diff valud from wndt scroll and from "child driven" scroll
  LONG lStartPos;
};

//info for ehmScrollProcess and ehmScrollPress mode
struct CScrollProcessInfo
{
  //used for calc "mouse shift" while process scroll of scroll bar
  POINT LastProcessLookPnt;
  //used for chng scrolling direction (scrolling direction vector chng)
  POINT LastScrollPnt;
  //mem for magn misses
  POINT LastScrollMagnMiss;
  //last "emulated move" point
  POINT LastEmulateThumbPnt;
  //rect of scroll bar "work area"
  RECT WorkArea;
  //curent scroll in hor direction
  bool bHorProcess;
  ////infor for scroll procedure (about one scrolling direction)
  struct CScrollDirInfo
  {
    //is flat SB processed
    bool bFlatSB;
    //is control wnd captured
    BOOL bIsCaptured;
    //is process ctrl SB_CTRL/(SB_HROZ/SB_VERT)
    bool bIsCtrl;
    //ctrl wnd (zero if no scroll in this direction present)
    HWND hWnd;
    //scrolling reduction for this direction
    DWORD dwScrollReduction;
  };
  //hor info
  CScrollDirInfo HInfo;
  //vert info
  CScrollDirInfo VInfo;
};

//global shared struct
//used for ctore status/cfg and inited by zero's (stored cfg from registry)

struct CMISharedInfo
{
  //handle of "main host wnd"
  HWND hMainHostWnd;
  //handle of main window of cfg application (used for start only one instance)
  HWND hCfgAppWnd;

  //tray icon mode (one of TrayIconClickEnum)
  DWORD dwTrayIconClickMode;

  //is OS require Win2000 pathc???
  //inited on host start
  //used in Scroll hanlders
  BOOL bWin2000PatchRequired;

  //is hook module should not send "module filename" to host module
  BOOL bNotSendModuleFileName;

  //hook for mouse
  HHOOK hMouseHook;
  //hook for CallWndProc
  HHOOK hCBTHook;

  //skip flag's for mosue btn's - used for skip mouse event's in emulate mouse click's
  UINT uiSkipUpFlag;
  UINT uiSkipDownFlag;
  //"eat down" flag's for mouse btn's - used for eat mouse input while working with IE scroll
  UINT uiEatUpFlag;
  //flag for "reforward" msg - used for reforward msg's from "toggle mode" while "non working button" pressed
  BOOL bReForwardMouseEvent;

  //current mosue key flags's (pressed on "this" time)
  UINT uiCurrMouseKeys;
  //current "worked" key's - if this not null - all mouse input eated
  //first chng from/to null call "process start/stop"
  UINT uiCurrWorkKeys;
  //mask for "work" key's - while some in this mask present - mouse input captured
  //this mask or'ed while some present in uiCurrWorkKeys
  //this mask cleared (and'ed) while key up'ed and uiCurrWorkKeys is zero
  UINT uiCurrWorkKeysMask;
  //mouse key's available for hooking
  UINT uiValidMouseKeys;
  //hook mode (HookModeEnum)
  LONG lHookMode;
  //scroll shift reduction
  LONG lScrollReduction;
  //scroll reduction virtual key
  UINT uiScollReductionKey;
  //"inverse" scrilling
  BOOL bScrollInv;
  //scrolling normal reduction
  DWORD dwScrollNormReduction;
  //from what start of control present
  POINT StartMsg;
  //if need set cursor
  BOOL bCursorNeedSet;
  //id of cursor, seted (from LoadCursor)
  UINT uiCursorName;
  //not use cursor visualisation (if FALSE)
  BOOL bCursorVisualise;
  //control type (from ScrollControlEnum)
  DWORD dwControlType;
  BOOL bScrollDisabled;

  //control states (from MouseProcessStateEnum)
  ULONG ulControlState;
  //control states data
  //point of start slip - used for mem poing and detect "slip processing" or simple "unpress process"
  POINT SlipStartPnt;

  //moving distance section
  //conversion from "point distance" to "metric distance" look in "odometer" page in "cfg application"
  //move distance summ (by cursor)
  LONGLONG llMoveDistanceSumm;
  //move distance "save" summ
  LONGLONG llMoveDistanceSaveSumm;
  //lasr looking pnt for calc llMoveDistanceSumm
  POINT MoveDistanseLastLookPnt;

  //Autohide data section
  //auto hide enabled
  BOOL bAHideEnabled;
  //last looking wnd
  HWND hAHideLastLookingWnd;
  //last "drilling down" window's
  HWND hAHideLastActiveSendWnd;
  //auto hide "mouse inactivity" timer timer
  DWORD dwAutoHideTimerTime;
  //delay time before open wnd (0 if delay disabled)
  DWORD dwAutoOpenTimerTime;

  //////////////////////////////////////////////////////////////////////
  //section for get app name from hook module

  //buffer for get app name message call
  CHAR cpGetAppNameResultBuff[MAX_PATH];

  union
  {
    //scroll process and press mode
    CScrollProcessInfo ScrollProcessInfo;
    //scroll process for IE
    CSCrollIEProcessInfo ScrollIEProcessInfo;
    //infor for pager ctrl
    CScrollPagerCtrlInfo ScrollPagerCtrlInfo;
  };

  //key for temporary disable DirectScroll
  UINT uiDirectScrollTmpDisableKey;

  //productivity for AutoShrink
  struct
  {
    //time count for "total work time" - sec
    LONGLONG llTotalTime;
    //summ of ("wnd open time" multiply ScrSq/WndSq)
    LONGLONG llWndSqTime;
  } ASOpenProd;

  //whis wnd resive notification msg's from scroll
  //used for additional process in subclassed wnd's
  //inited ad scroll start
  //cleared in scrollstop
  HWND hSubClassScrollWnd;

  //from what mouse btn mouse control started (scroll bar ctrl)
  //used for identify "need unpress emulate"
  DWORD dwMouseControlStartKey;

  ////prm's for scroll in IE
  //min moving size - if movine more then his size conversion to WM_MOUSEWHELL performed
  DWORD dwIEScrollMinMove;
  //rediction for scroll (convert from WM_MOUSEMOVE to WM_MOUSEWHELL)
  //stored as magnifed to egcIEScrollConvMoveMagn
  DWORD dwIESCrollConvMove;

  //for mem cursor while start scroll
  HCURSOR hMemWndCursor;

  //process, in what scroll start - used for check "deacivate" message to wnd
  DWORD dwScrollStartThread;

  //mouse hook on "post message" mode - used for avoid "last mouse message processed before process stop scroll message"
  BOOL bMouseHookInForwardMode;

  //is need process CBT hook - used for disable subclass new wnd's from CBT hook
  BOOL bProcessAddWndFromCBTHook;
  //is hook'd present - used as host wnd and tested as compare wnd class
  HWND hHookPresentFlag;

  ////data uncrypted and placed after this struct
  //first - "crypt" section
  //second - crypt section relocation's
  BOOL bCryptOk;
  //size of crypt section (align for crypt size)
  DWORD dwCryptSectSize;
  //size of crypt section relocation's (real size)
  DWORD dwCryptSectRelocRealSize;
  //is "first" protection layer valid (CRC on key)
  BOOL bCryptFirstLayerValid;
  BOOL bCryptPirated;

  //enable tray icon
  BOOL bEnableTrayIcon;
  //enable tray icon animation
  BOOL bEnableTrayIconAnim;

  //bidirection scrolling enabled
  BOOL bBDScrollingEnabled;

  //need show splash at start
  BOOL bShowSplash;

  //size of screen - used for call AHide benefit and for call scroll bar "AddReduct" valud
  POINT ScrSize;

  ////timer for total MouseImp runnign time 
  //incremented in host and used for show in "productivity" page
  LONGLONG llTotalRunTime;
};

//shared info not cleared until init host
struct CSharedNotClearedInfo
{
  ////number of installed hook's - used for init scan wnd's from CBT hook
  //incremented on all hook install
  //and monitored on CBT hook and on chng - preform rescan for process wnd's and subclass all
  LONG lHookInstallCount;
};

//name of shared info view (for open from hook dll)
LPCSTR const cpcSharedInfoName = "MImpProSharedInfo";
//shared info for handle crypted data
LPCSTR const cpcSharedCryptInfo = "MImpProSharedCInfo";

//name og mousr hook function (used for enhancing)
LPCSTR const cpMouseHookFunc = "MIHookFunc";
//hook proc for system activity detection
LPCSTR const cpcCBTHookFunc = "MIHookFunc1";
//name of hook module DLL file
LPCSTR const cpcHookFundModule = "MIPro.dll";
//util dll
LPCSTR const cpcUtilDllName = "MIProUtl.dll";
//name of host application
LPCSTR const cpcHostAppName = "MIProHst.exe";
//name of "cfg" application
LPCSTR const cpcCfgAppFileName = "MIProCfg.exe";
//application help file
LPCSTR const cpcHelpFileName = "MIPROHLP.HLP";
//name of reg key for store cfg
LPCSTR const cpcCfgKeyName = "SOFTWARE\\TV4 Studios\\MImpPro";
//name of cfg for MouseImp
LPCSTR const cpcCfgImportKeyName = "SOFTWARE\\TV4 Studios\\MouseImp";
//name of "base" entry for WndPattern's (count of pattern's)
LPCSTR const cpcCfgWPatternsMainName = "WPat";
//name of "store" entry for WndPattern's (actual data)
//each entry stored in "new line" separated format as "AppName\nClassName\nTitleName
LPCSTR const cpcCfgWPatternsEntryMask = "WPat%d";
//name of "instalation directory" entry
LPCTSTR const cpcCfgInstallDirPath = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\MIProHst.exe";
LPCSTR const cpcCfgInstallDirEnt = "Path";
//name of global message
LPCSTR const cpcGlobalIntMessage = "MImpProGlbMsg";
//name of enty in Run for start MouseImp on WndLoad - corresponding to setup.inf file
LPCSTR const cpcRunEntryName = "MImpPro";
//registration key name
LPCSTR const cpcCfgRegKey = "Reg";
//host wnd class/name
LPCSTR const cpcWndClass = "MImpProWndClass";
LPCSTR const cpcWndTitle = "MImpProWndTitle";
//name of "get version" function, imported from cpcHookFundModule
//function return GetVersionNumber()
LPCSTR const cpcHookGetVerFunc = "MIGetVer";
extern "C" typedef DWORD (*HookGetVerFuncType)();

//class of dlg's
LPCSTR const cpcWndDlgClass = "#32770";


//////////////////////////////////////////////////////////////////////
//exclude info for AutoShrink

struct __CShrinkExcludeInfo
{
  //wnd class name
  LPCSTR cpWndClass;
  //test wnd style
  DWORD dwStyle;
  //test wnd style mask
  DWORD dwStyleMask;
  //wnd border size
  UINT uiBorderSizeMetric;
  //wnd caption size
  UINT uiCaptionSizeMetric;
  //place for caption (for DrawEdge function)
  //beside from caption
  UINT uiEdgePos;
};

const __CShrinkExcludeInfo cpcExcludeInfo[] =
{
  {"MsoCommandBar", WS_POPUP, WS_POPUP | WS_CHILD, SM_CYSIZEFRAME, SM_CYSMCAPTION, BF_BOTTOM}
};


//////////////////////////////////////////////////////////////////////
//func's

////compare Wnd class for AShrink

#ifndef COUNTOF
#define COUNTOF(x) (sizeof(x) / sizeof(x[0]))
#endif//COUNTOF

//compare str helper
inline bool __StrCmpHelper(LPCSTR const cpcStartStr, const DWORD dwcStartLen, LPCSTR const cpcStr)
{
  bool bRes = true;
  for(DWORD dwCount = 0; dwcStartLen > dwCount && '\0' != cpcStr[dwCount]; dwCount++)
  {
    if(cpcStartStr[dwCount] != cpcStr[dwCount])
    {
      bRes = false;
      break;
    };
  };
  return bRes;
};

//scan until passed char not skiped
inline void __SkipUntil(LPCSTR& rcStr, const CHAR ccLook)
{
  while(0 != *rcStr)
  {
    const CHAR ccMem = *rcStr;
    rcStr++;
    if(ccMem == ccLook)
    {
      //all find - break
      break;
    };
  };
};

//compare string for digit and (next) colon or zero end
inline bool __CompareHelperDigit(LPCSTR const cpcStr, const CHAR ccChar)
{
  return
    ccChar == cpcStr[0]
    && (':' == cpcStr[1] || '\0' == cpcStr[1]);
};

//string compare helper (for digit group, separated by ':')
inline bool __StrCmpHelperDig(LPCSTR& rpcWnd, LPCSTR& rpcPatt, const bool bcExtraLook)
{
  bool bRes = true;

  //mem start
  LPCSTR const cpcStartWnd = rpcWnd;
  LPCSTR const cpcStartPatt = rpcPatt;

  //compare loop (until ':' not found
  while('\0' != *rpcWnd && '\0' != *rpcPatt)
  {
    if(*rpcWnd != *rpcPatt)
    {
      //compare failed
      bRes = false;
      break;
    };
    const CHAR ccMem = *rpcWnd;
    //next
    rpcWnd++;
    rpcPatt++;
    //try find colon
    if(':' == ccMem)
    {
      //break on
      break;
    };
  };

  //if string's not equal - and pattern not '0' - string's equal
  if
    (
    //need look extra
    false != bcExtraLook
    //prev compare failed
    && false == bRes
    //pattern and wnd not have '0'
    && false == __CompareHelperDigit(cpcStartPatt, '0')
    && false == __CompareHelperDigit(cpcStartWnd, '0')
    )
  {
    bRes = true;
    //try find "colon'ed" end in pattern and wnd info
    __SkipUntil(rpcPatt, ':');
    __SkipUntil(rpcWnd, ':');
  };

  return bRes;
};

const CHAR cpcMFCClassPrefix[] = {'A', 'f', 'x', ':'};
inline bool ASCompareClass(LPCSTR const cpcWnd, LPCSTR const cpcPatt)
{
  bool bRes = 0 == ::lstrcmpi(cpcWnd, cpcPatt);

  //if compare failed - try look on "MFC class"
  if(false == bRes)
  {
    //look on "MFC prefix before pattern and wnd class
    if(false != __StrCmpHelper(cpcMFCClassPrefix, COUNTOF(cpcMFCClassPrefix), cpcWnd) && false != __StrCmpHelper(cpcMFCClassPrefix, COUNTOF(cpcMFCClassPrefix), cpcPatt))
    {
      LPCSTR pcWnd = cpcWnd + COUNTOF(cpcMFCClassPrefix);
      LPCSTR pcPatt = cpcPatt + COUNTOF(cpcMFCClassPrefix);
      //look on "Afx:Inst:Style" format
      bRes =
        //instance
        false != __StrCmpHelperDig(pcWnd, pcPatt, false)
        //style
        && false != __StrCmpHelperDig(pcWnd, pcPatt, false);
      //look on "Afx:Inst:Stype:Cursor:Back:Icon" format
      if(false != bRes && '\0' != *pcWnd && '\0' != *pcPatt)
      {
        //compare end
        bRes =
          //cursor
          false != __StrCmpHelperDig(pcWnd, pcPatt, true)
          //back
          && false != __StrCmpHelperDig(pcWnd, pcPatt, false)
          //icon
          && false != __StrCmpHelperDig(pcWnd, pcPatt, true);
      };
    };
  };

  return bRes;
};

//simple compare class (without MFC processing)
inline bool ASCompareClassSimple(LPCSTR const cpcClass, LPCSTR const cpcPatt)
{
  return 0 == ::lstrcmpi(cpcClass, cpcPatt);
};

//compare wnd title for AShrink
inline bool ASCompareTitle(LPCTSTR const cpcWnd, LPCTSTR const cpcPatt)
{
  return 0 == ::lstrcmp(cpcWnd, cpcPatt);
};

//compare app and for ASrink
inline bool ASCompareAppName(LPCTSTR const cpcAppName, LPCTSTR const cpcPatt)
{
  return 0 == ::lstrcmpi(cpcAppName, cpcPatt);
};


////calc version number, based on eccInternalVer and sizeof(CMISharedInfo)
inline DWORD GetVersionNumber()
{
  return MAKELONG(eccInternalVer, sizeof(CMISharedInfo));
};

inline LONG GetWndBorderSize(const HWND hcWnd, const DWORD dwcStyle, const bool bcHor)
{
  LONG lRes = 0;

  if(0 != (WS_BORDER & dwcStyle))
  {
    if(0 != (WS_THICKFRAME & dwcStyle))
    {
      lRes = ::GetSystemMetrics((false != bcHor) ? SM_CXSIZEFRAME : SM_CYSIZEFRAME);
    }
    else
    {
      lRes = ::GetSystemMetrics((false != bcHor) ? SM_CXFIXEDFRAME : SM_CYFIXEDFRAME);
    };
  };
  return lRes;
};

//scan app name and extract entire name only (name, not ext, path and etc)
inline void ScanAppNameAndExtractName(LPCSTR& rpcName, LONG& rlInStrLenOutEndPos)
{
  //try find last '\/' at start of name
  //try find last '.'

  //'/\'
  {
    for(LPCSTR pScan = rpcName + rlInStrLenOutEndPos - 1; rpcName <= pScan; pScan--)
    {
      if('/' == *pScan || '\\' == *pScan)
      {
        pScan++;
        rlInStrLenOutEndPos -= pScan - rpcName;
        rpcName = pScan;
        break;
      };
    };
  };
};

//try fins exclude info for some wnd
inline const __CShrinkExcludeInfo* FindExcludeInfo(const HWND hcWnd)
{
  const __CShrinkExcludeInfo* pRes = 0;

  CHAR cpClass[egcWndClassTextLen];
  bool bClassNameValid = false;
  const DWORD dwcStyle = ::GetWindowLong(hcWnd, GWL_STYLE);
  for(DWORD dwCount = 0; COUNTOF(cpcExcludeInfo) > dwCount; dwCount++)
  {
    const __CShrinkExcludeInfo* const cpcInfo = cpcExcludeInfo + dwCount;
    if(cpcInfo->dwStyle == (cpcInfo->dwStyleMask & dwcStyle))
    {
      //compare by class
      if(false == bClassNameValid)
      {
        bClassNameValid = true;
        ::GetClassName(hcWnd, cpClass, COUNTOF(cpClass));
      };
      if(false != ASCompareClass(cpClass, cpcInfo->cpWndClass))
      {
        pRes = cpcInfo;
        break;
      };
    };
  };

  return pRes;
};

//calc size for WndClose process (AShrink)
//rwEdgeType - edge type for draw by ::DrawEdge
//return clised wnd rect in rWndRect (in wnd rect's)
inline void CalcWndSizeForAS(const HWND hcWnd, RECT& rWndRect, WORD& rwEdgeType)
{
  BOOL bRes = FALSE;
  //try find wnd in "exclusion" list
  const __CShrinkExcludeInfo* const cpcExcInfo = FindExcludeInfo(hcWnd);
  if(0 != cpcExcInfo)
  {
    bRes = TRUE;
    //mem prm's
    ::GetWindowRect(hcWnd, &rWndRect);
    ::MapWindowPoints(0, hcWnd, reinterpret_cast<POINT*>(&rWndRect), 2);

    rWndRect.right = rWndRect.right - rWndRect.left;
    rWndRect.left = 0;
    rWndRect.top = 0;
    rWndRect.bottom =
      ::GetSystemMetrics(cpcExcInfo->uiBorderSizeMetric)
      + ::GetSystemMetrics(cpcExcInfo->uiCaptionSizeMetric);
    rwEdgeType = cpcExcInfo->uiEdgePos;
  };

  //normal processing
  if(FALSE == bRes)
  {
    //style
    const LONG lcStyleEx = ::GetWindowLong(hcWnd, GWL_EXSTYLE);
    const LONG lcStyle = ::GetWindowLong(hcWnd, GWL_STYLE);
    const BOOL bcToolWnd = WS_EX_TOOLWINDOW & lcStyleEx;

    //const's
    const LONG lcCaptionSize = ::GetSystemMetrics((FALSE == bcToolWnd) ? SM_CYCAPTION : SM_CYSMCAPTION);
    const LONG lcHBordSize = GetWndBorderSize(hcWnd, lcStyle, true);
    const LONG lcVBordSize = GetWndBorderSize(hcWnd, lcStyle, false);
    const LONG lcHEdge = ::GetSystemMetrics(SM_CXEDGE);
    const LONG lcVEdge = ::GetSystemMetrics(SM_CYEDGE);

    //wnd dim's
    RECT WndRect = {0, 0, 0, 0};
    RECT CliRect = {0, 0, 0, 0};
    ::GetWindowRect(hcWnd, &WndRect);
    ::GetClientRect(hcWnd, &CliRect);
    ::MapWindowPoints(hcWnd, 0, reinterpret_cast<POINT*>(&CliRect), 2);
    const LONG lcLeftSize = CliRect.left - WndRect.left;
    const LONG lcTopSize = CliRect.top - WndRect.top;
    const LONG lcRightSize = WndRect.right - CliRect.right;
    const LONG lcBottomSize = WndRect.bottom - CliRect.bottom;

    ////try find max size
    //top
    LONG lLookSize = lcTopSize;
    rWndRect.left = 0;
    rWndRect.top = 0;
    rWndRect.right = WndRect.right - WndRect.left;
    rWndRect.bottom = CliRect.top - WndRect.top;
    rWndRect.bottom = max(lcVBordSize + lcVEdge, min(rWndRect.bottom, lcCaptionSize + lcVBordSize));
    rwEdgeType = BF_BOTTOM;
    //left
    if(lcLeftSize > lLookSize)
    {
      lLookSize = lcLeftSize;
      rWndRect.left = 0;
      rWndRect.top = 0;
      rWndRect.right = CliRect.left - WndRect.left;
      rWndRect.bottom = WndRect.bottom - WndRect.top;
      rWndRect.right = max(lcHBordSize + lcHEdge, min(rWndRect.right, lcCaptionSize + lcHBordSize));
      rwEdgeType = BF_RIGHT;
    };
    //right
    if(lcRightSize > lLookSize)
    {
      lLookSize = lcRightSize;
      rWndRect.right = WndRect.right - WndRect.left;
      const LONG lcCapSize = WndRect.right - CliRect.right;
      rWndRect.left = rWndRect.right - max(lcHBordSize + lcHEdge, min(lcCapSize, lcCaptionSize + lcHBordSize));
      rWndRect.top = 0;
      rWndRect.bottom = WndRect.bottom - WndRect.top;
      rwEdgeType = BF_LEFT;
    };
    //bottom
    if(lcBottomSize > lLookSize)
    {
      rWndRect.left = 0;
      rWndRect.right = WndRect.right - WndRect.left;
      const LONG lcCapSize = WndRect.bottom - CliRect.bottom;
      rWndRect.bottom = WndRect.bottom - WndRect.top;
      rWndRect.top = rWndRect.bottom - max(lcVBordSize + lcVEdge, min(lcCapSize, lcCaptionSize + lcVBordSize));
      rwEdgeType = BF_TOP;
    };
  };
};


//check "is window have needed style for AutoHide"
inline BOOL AHideCheckWndStyle(const HWND hcWnd, const bool bcLookOnCaptionSize = true)
{
  BOOL bRes = FALSE;
  //try find wnd in "exclusion" list
  const __CShrinkExcludeInfo* const cpcExcInfo = FindExcludeInfo(hcWnd);
  if(0 != cpcExcInfo)
  {
    bRes = TRUE;
  };

  if(FALSE == bRes)
  {
    const LONG lcStyle = ::GetWindowLong(hcWnd, GWL_STYLE);
    if(0 == (WS_POPUP | WS_CHILD) & lcStyle)
    {
      //overlapped window - have caption as default
      bRes = TRUE;
    }
    else if(0 != (WS_POPUP & lcStyle))
    {
      //for pop up's we look for caption style or for distance between top wnd and it's client area
      bRes = WS_CAPTION == (WS_CAPTION & lcStyle);
      //caption apsent
      if(false != bcLookOnCaptionSize && FALSE == bRes)
      {
        //look distance between wnd top wnd client top
        const LONG lcHorBord = GetWndBorderSize(hcWnd, lcStyle, true) + ::GetSystemMetrics(SM_CXEDGE);
        const LONG lcVertBord = GetWndBorderSize(hcWnd, lcStyle, false) + ::GetSystemMetrics(SM_CYEDGE);
        RECT WndRect;
        RECT CliRect;
        ::GetWindowRect(hcWnd, &WndRect);
        ::GetClientRect(hcWnd, &CliRect);
        ::MapWindowPoints(hcWnd, 0, reinterpret_cast<POINT*>(&CliRect), 2);
        //client rect smaller when wnd rect (non client present)
        bRes =
          WndRect.top + lcHorBord < CliRect.top
          || WndRect.bottom - lcHorBord > CliRect.bottom
          || WndRect.left + lcVertBord < CliRect.left
          || WndRect.right - lcVertBord > CliRect.right;
      };
    }
    else
    {
      //for child's look on Caption style
      //(???) possible not good idea AutoHide for child's
      bRes = WS_CAPTION == (WS_CAPTION & lcStyle);
    };
  };

  return bRes;
};

//get parent window while drilling up
inline HWND GetParentWndForDrillUp(const HWND hcWnd)
{
  const HWND hcRes = ::GetParent(hcWnd);
  return (0 == hcRes)
    ? ::GetWindow(hcWnd, GW_OWNER)
    : hcRes;
};
//drilling for window up while find "AutoHide" window
inline bool AHideDrillForWindow(const HWND hcFromWnd, HWND& rFindWnd)
{
  //drill while wnd exist and have "valid" style's

  bool bRes = false;

  rFindWnd = 0;
  for(HWND hLook = hcFromWnd; 0 != hLook; hLook = GetParentWndForDrillUp(hLook))
  {
    if(FALSE != AHideCheckWndStyle(hLook))
    {
      bRes = true;
      rFindWnd = hLook;
      break;
    };
  };

  return bRes;
};

//change name in passed buffer and set it's to passed name
inline void ChangeNameInFullName(LPCSTR const cpcNewName, LPTSTR const cpResMass, const DWORD dwcLen)
{
  //find end '\\' or '/'
  DWORD dwFindPos = 0;
  for(DWORD dwCount = 0; dwcLen > dwCount; dwCount++)
  {
    const DWORD dwcLookPos = dwcLen - 1 - dwCount;
    const TCHAR cLook = cpResMass[dwcLookPos];
    if('\\' == cLook || '/' == cLook)
    {
      //find char
      dwFindPos = dwcLookPos + 1;
      break;
    };
  };
  //copy passed name to buffer from finded pos
  ::lstrcpy(cpResMass + dwFindPos, cpcNewName);
};

//build in passed mass cpResMass (MAX_PATH len) full name of cpcShortName application, based on path to hBaseModule
inline void GetFullBasedName(LPCTSTR const cpcShortName, LPTSTR const cpResMass, const HMODULE hBaseModule)
{
  //get based module name
  const DWORD dwcLen = ::GetModuleFileName(hBaseModule, cpResMass, MAX_PATH);
  //set to path
  ChangeNameInFullName(cpcShortName, cpResMass, dwcLen);
};

//check wnd - if it's child or some wnd is active
inline bool CheckWndActive(const HWND hcWnd)
{
  bool bRes = false;

  //get active wnd and try drill up until find passed wnd or "other" process find
  DWORD dwWndProc = 0;
  ::GetWindowThreadProcessId(hcWnd, &dwWndProc);
  const DWORD dwcPrc = ::GetCurrentProcessId();
  HWND hLookWnd = ::GetActiveWindow();
  while(0 != hLookWnd)
  {
    //is active find
    if(hcWnd == hLookWnd)
    {
      //passed wnd have active child or is active
      bRes = true;
      break;
    };
    //compare wnd process
    DWORD dwWndPrc = 0;
    ::GetWindowThreadProcessId(hLookWnd, &dwWndPrc);
    if(dwWndPrc != dwcPrc)
    {
      //other process
      break;
    };
    //try get next wnd
    hLookWnd = GetParentWndForDrillUp(hLookWnd);
  };
  return bRes;
};

//send to "critical" wnd (what can be hung)
inline bool SafeSendMsg(const HWND hcWnd, const UINT uicMsg, WPARAM wParam, LPARAM lParam, DWORD* lRes, const bool bcLongWait = false)
{
  return 0 != ::SendMessageTimeout(hcWnd, uicMsg, wParam, lParam, SMTO_ABORTIFHUNG | SMTO_BLOCK, (false == bcLongWait) ? 1000 : 10000, lRes);
};

inline bool SafeSendMsg(const HWND hcWnd, const UINT uicMsg, WPARAM wParam, LPARAM lParam)
{
  DWORD dwRes = 0;
  return SafeSendMsg(hcWnd, uicMsg, wParam, lParam, &dwRes);
};

////activate wnd and bring to user
inline void ActivateWndToUser(const HWND hcWnd)
{
  ::SetForegroundWindow(hcWnd);
};

////try show cfg application (return false if app not present)
inline bool TryShowCfgApp(CMISharedInfo* const cpCfgMem)
{
  bool bRes = false;
  if(FALSE != ::IsWindow(cpCfgMem->hCfgAppWnd))
  {
    bRes = true;
    ::PostMessage(cpCfgMem->hCfgAppWnd, ecmShowApp, 0, 0);
  };
  return bRes;
};

//calc size of section of crypted data
inline DWORD GetCryptDataSize(const CMISharedInfo* const cpcCfgMem)
{
  return cpcCfgMem->dwCryptSectRelocRealSize + cpcCfgMem->dwCryptSectSize;
};
//get crypt section addr from addr base and from shared info
inline BYTE* GetCryptSectAddr(const CMISharedInfo* const cpcCfgMem, BYTE* const cpBaseSect)
{
  return cpBaseSect;
};
//get relocation info from base crypt data
inline BYTE* GetCryptRelocSectAddr(const CMISharedInfo* const cpcCfgMem, BYTE* const cpBaseSect)
{
  return cpBaseSect + cpcCfgMem->dwCryptSectSize;
};

//try exit from host app (with wait)
inline void TryExitFromHostApp(const CMISharedInfo* const cpcCfgMem)
{
  if(FALSE != ::IsWindow(cpcCfgMem->hMainHostWnd))
  {
    //try open process
    HANDLE hProc = 0;
    DWORD dwWndProc = 0;
    if(0 != ::GetWindowThreadProcessId(cpcCfgMem->hMainHostWnd, &dwWndProc))
    {
      hProc = ::OpenProcess(SYNCHRONIZE, FALSE, dwWndProc);
    };
    //post close to wnd
    ::PostMessage(cpcCfgMem->hMainHostWnd, WM_CLOSE, 0, 0);
    //try wait for exit from host process
    if(0 != hProc)
    {
      ::WaitForSingleObject(hProc, 10 * 1000);
      ::CloseHandle(hProc);
    };
  };
};

////get not cleared info from shared info
inline CSharedNotClearedInfo* GetNotClearInfo(CMISharedInfo* const cpInfo)
{
  return reinterpret_cast<CSharedNotClearedInfo*>(cpInfo + 1);
};

////prm's for scroll control
inline bool ScrollEnabled(CMISharedInfo* const cpInfo)
{
  return FALSE != cpInfo->bScrollDisabled;
};
inline bool ScrollToggleMode(CMISharedInfo* const cpInfo)
{
  return escToggleWithEat == cpInfo->dwControlType;
};
inline bool ScrollToggleWithEat(CMISharedInfo* const cpInfo)
{
  return true;
};

#endif//__MIGLOBAL_H