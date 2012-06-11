/**********************************************************************
global's for mouse imp
**********************************************************************/

#ifndef __MIGLOBAL_H
#define __MIGLOBAL_H

#pragma once

#include "MIHostInterface.h"

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

enum GlobalConstEnum
{
  //valid key's for mouse processing
  egcValidMouseKeysMask = MK_LBUTTON | MK_MBUTTON | MK_RBUTTON,
  //key for scroll reduction (LeftButton)
  egcDecScollReductionKey = VK_LBUTTON,
  //size of "window class" text
//  egcWndClassTextLen = 100,
  //default "AutoHide" timer time
  egcDefAutoHideTimerTime = 1000,
  ////const's for IE scroll
  ////default's for IE scroll prms's
  egcIEScrollMinMoveDef = 2,
  egcIEScrollConvMoveDef = 1024 * 10,
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
  CMICommonPartWith64bit common64;

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
  //hook mode (HookModeEnum)
  LONG lHookMode;
  //from what start of control present
  POINT StartMsg;
  //if need set cursor
  BOOL bCursorNeedSet;
  //id of cursor, seted (from LoadCursor)
  UINT uiCursorName;

  //control states (from MouseProcessStateEnum)
  ULONG ulControlState;
  //control states data
  //point of start slip - used for mem poing and detect "slip processing" or simple "unpress process"
  POINT SlipStartPnt;

  //moving distance section
  //conversion from "point distance" to "metric distance" look in "odometer" page in "cfg application"
  //lasr looking pnt for calc llMoveDistanceSumm
  POINT MoveDistanseLastLookPnt;

  //Autohide data section
  //last looking wnd
  HWND hAHideLastLookingWnd;
  //last "drilling down" window's
  HWND hAHideLastActiveSendWnd;

  //////////////////////////////////////////////////////////////////////
  //section for get app name from hook module

  union
  {
    //scroll process and press mode
    CScrollProcessInfo ScrollProcessInfo;
    //scroll process for IE
    CSCrollIEProcessInfo ScrollIEProcessInfo;
    //infor for pager ctrl
    CScrollPagerCtrlInfo ScrollPagerCtrlInfo;
  };


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

  //size of screen - used for call AHide benefit and for call scroll bar "AddReduct" valud
  POINT ScrSize;

};

//shared info not cleared until init host
struct CSharedNotClearedInfo
{
  ////number of installed hook's - used for init scan wnd's from CBT hook
  //incremented on all hook install
  //and monitored on CBT hook and on chng - preform rescan for process wnd's and subclass all
  LONG lHookInstallCount;
};

//class of dlg's
LPCSTR const cpcWndDlgClass = "#32770";

//////////////////////////////////////////////////////////////////////
//func's

//simple compare class (without MFC processing)
inline bool ASCompareClassSimple(LPCSTR const cpcClass, LPCSTR const cpcPatt)
{
  return 0 == ::lstrcmpi(cpcClass, cpcPatt);
};

//simple compare class (without MFC processing)
inline bool ASCompareClassSimplePatt(LPCSTR const cpcClass, LPCSTR const cpcPatt)
{
	return NULL != ::strstr(cpcClass,cpcPatt);
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


//scan app name and extract entire name only (name, not ext, path and etc)
inline void ScanAppNameAndExtractName(LPCSTR& rpcName, LONG_PTR& rlInStrLenOutEndPos)
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
inline bool SafeSendMsg(const HWND hcWnd, const UINT uicMsg, WPARAM wParam, LPARAM lParam, PDWORD_PTR lRes, const bool bcLongWait = false)
{
  return 0 != ::SendMessageTimeout(hcWnd, uicMsg, wParam, lParam, SMTO_ABORTIFHUNG | SMTO_BLOCK, (false == bcLongWait) ? 1000 : 10000, lRes);
};

inline bool SafeSendMsg(const HWND hcWnd, const UINT uicMsg, WPARAM wParam, LPARAM lParam)
{
  DWORD_PTR dwRes = 0;
  return SafeSendMsg(hcWnd, uicMsg, wParam, lParam, &dwRes);
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

////get not cleared info from shared info
inline CSharedNotClearedInfo* GetNotClearInfo(CMISharedInfo* const cpInfo)
{
  return reinterpret_cast<CSharedNotClearedInfo*>(cpInfo + 1);
};

////prm's for scroll control
inline bool ScrollEnabled(CMISharedInfo* const cpInfo)
{
  return FALSE != cpInfo->common64.bScrollDisabled;
};
inline bool ScrollToggleMode(CMISharedInfo* const cpInfo)
{
  return escToggleWithEat == cpInfo->common64.dwControlType;
};
inline bool ScrollToggleWithEat(CMISharedInfo* const cpInfo)
{
  return true;
};

#endif//__MIGLOBAL_H