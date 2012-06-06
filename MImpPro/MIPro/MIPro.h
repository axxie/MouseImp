/**********************************************************************
mouse imp hok dll
**********************************************************************/

#ifndef __APP_H
#define __APP_H

#pragma once

#include "..\Include\MIGlobal.h"

#include "resource.h"

#include "..\\SLib\\SLDbg.h"

#include "..\\Slib\\SLCont.h"

#include "..\\Slib\\SLWndProcThunk.h"

#include "xmlParser.h"

#include <ZMOUSE.H>


class CApp;
extern CApp* pApp;

using sl::CSLSList;
using sl::CSLWndProcThunk;
using sl::CSLHashSet;

class CApp;
//type of callback func used for "call dispatch table"
typedef LRESULT (CApp::*HookProcClassType)(int nCode, WPARAM wParam, LPARAM lParam);


//////////////////////////////////////////////////////////////////////
//global's

inline DWORD MyAddDiv(const DWORD dwcFirst, const DWORD dwcSecond, const DWORD dwcThird)
{
  DWORD dwRes = -1;
  if(0 != dwcThird)
  {
    LONGLONG llRes = static_cast<LONGLONG>(dwcFirst);
    llRes += static_cast<LONGLONG>(dwcSecond);
    llRes /= static_cast<LONGLONG>(dwcThird);
    dwRes = static_cast<DWORD>(llRes);
  };
  return dwRes;
};

inline DWORD MyMulDiv(const DWORD dwcFirst, const DWORD dwcSecond, const DWORD dwcThird)
{
  DWORD dwRes = -1;
  if(0 != dwcThird)
  {
    LONGLONG llRes = static_cast<LONGLONG>(dwcFirst);
    llRes *= static_cast<LONGLONG>(dwcSecond);
    llRes /= static_cast<LONGLONG>(dwcThird);
    dwRes = static_cast<DWORD>(llRes);
  };
  return dwRes;
};

inline LONG MyMulDivLongMiss(const LONG lcFirst, const LONG lcSecond, LONG& rlMissVal, const LONG lcThird)
{
  LONG lRes = -1;
  if(0 != lcThird)
  {
    const LONGLONG cllThirdVal = static_cast<LONGLONG>(lcThird);
    const LONGLONG llMultVal =
      static_cast<LONGLONG>(lcFirst)
      * static_cast<LONGLONG>(lcSecond)
      + static_cast<LONGLONG>(rlMissVal);
    rlMissVal = static_cast<LONG>(llMultVal % cllThirdVal);
    const LONGLONG cllResVal = llMultVal / cllThirdVal;
    lRes = static_cast<LONG>(cllResVal);
  };
  return lRes;
};

inline LONG MyMulDivLong(const LONG lcFirst, const LONG lcSecond, const LONG lcThird)
{
  LONG lRes = -1;
  if(0 != lcThird)
  {
    LONGLONG llRes = static_cast<LONGLONG>(lcFirst) * static_cast<LONGLONG>(lcSecond);
    const LONGLONG cllThirdVal = static_cast<LONGLONG>(lcThird);
    llRes /= cllThirdVal;
    lRes = static_cast<LONG>(llRes);
  };
  return lRes;
};

////get center of rect
inline POINT GetRectCenter(const RECT& rcRect)
{
  const POINT cRec = {(rcRect.right + rcRect.left) / 2, (rcRect.bottom + rcRect.top) / 2};
  return cRec;
};

////look on "drag present" situation in direction's

inline bool DragPresentX(const POINT& rcFrom, const POINT& rcTo)
{
  const int icDrag = ::GetSystemMetrics(SM_CXDRAG);
  return
    rcFrom.x - icDrag > rcTo.x
    || rcFrom.x + icDrag <= rcTo.x;
};

inline bool DragPresentY(const POINT& rcFrom, const POINT& rcTo)
{
  const int icDrag = ::GetSystemMetrics(SM_CYDRAG);
  return
    rcFrom.y - icDrag > rcTo.y
    || rcFrom.y + icDrag <= rcTo.y;
};

inline bool DragPresent(const POINT& rcFrom, const POINT& rcTo)
{
  return
    false != DragPresentX(rcFrom, rcTo)
    || false != DragPresentY(rcFrom, rcTo);
};

//mouse btn's for click emulate
//and stop mouse control
enum MouseBtnEnum
{
  embLeft = 0,
  embMidle,
  embRight
};

//info for emulate mouse btn click's
struct StopMouseMsg
{
  //down message (for mouse_event)
  UINT uiDownMsg;
  //up msg (for mouse_event)
  UINT uiUpMsg;
  //skip flag for up/down skip mask
  UINT uiSkipMask;
  //virtual code for key
  UINT uiVirtKey;
};

extern const struct StopMouseMsg cpcStopMouseNCInfo[];

//////////////////////////////////////////////////////////////////////
//CSubClassInfo - infor for subclassed wnd

class CSubClassInfo;
typedef CSLWndProcThunk<CSubClassInfo> ThunkBaseType;

class CSubClassInfo : protected ThunkBaseType
{
protected:

  ////old proc
  WNDPROC pOldProc;
  ////class state flag holder
  DWORD dwFlags;

public:

  ////controlled wnd
  const HWND hcWnd;

  enum ClassStateEnum
  {
    //is thunk in "dummy" mode
    ecsIsDummy = 0,
    //is wnd closed (shrinked)
    ecsIsShrinked,
    //is object need delete
    ecsDelObject,
    //need unlock dll - seded from UnSubclass and used in WndProc
    ecsUnLockModule,
    //number
    ecsLast
  };

  //count of recursion call - used for not uninstal while call on some function present
  WORD wWndProcCallCount;

  //draw operation for draw edge's
  WORD wDrawEdgeType;
  //rect of drawed "close margin"
  RECT CloseMargin;

public:

  inline CSubClassInfo(const HWND hcInitWnd);

  //flag work
  inline bool FlagTest(const ClassStateEnum cFlgNumb) const;
  inline void FlagSet(const ClassStateEnum cFlagNumb, const bool bcState);

  //install thunk
  inline bool ThunkInstall(const HWND hcWnd);
  //remove thunk - return false if thunk goto DummyProc
  inline bool ThunkRemove(const HWND hcWnd);
  //reinstall thunk (must be already installed)
  inline void ThunkReInstall(const HWND hcWnd);
  //remove thunk force (used from destroy wnd)
  inline void ThunkRemoveTidy(const HWND hcWnd);

  ////for hash set
  inline bool operator==(const HWND hcCompareWnd) const;

protected:

  //switch to "DefWindowProc" instead of "CallwindowProc" (XxxDef funcs)
  inline void SwitchWndToDef();
  inline void SwitchDummyWndToDef();

  //draw margin on closed wnd
  inline void DrawCloseMargin(const HWND hcWnd);
  //actual wnd proc
  LRESULT WndProcA(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);
  LRESULT WndProcW(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);
  LRESULT WndProcADef(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);
  LRESULT WndProcWDef(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);
  //dummy wnd proc - used if thunk can't be unibstalled
  LRESULT DummyWndProcA(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);
  LRESULT DummyWndProcW(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);
  LRESULT DummyWndProcADef(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);
  LRESULT DummyWndProcWDef(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);
};

//////////////////////////////////////////////////////////////////////
//CApp

class CApp
{
protected:

  //subclassed have access
  friend CSubClassInfo;

  enum ClassConstEnum
  {
    //size of mass for GetKeyboardState call
    eccStateKeyMassSize = 256,
    //thumb min size under WinNT (const from CoolControl's from codeGuru)
    eccThumbMinSizeUnderNT = 8,
    //def's for calc "AddReduct" values
    eccSBAddReductDefRange = 100,
    eccSBAddReductDefPage = 10
  };

  //handle of dll module
  HINSTANCE hInstance;

  //instance of global shared cfg
  CMISharedInfo* pCfgMem;
  HANDLE hCfgMap;

  //number of registered global message
  const UINT uicGlbMsg;

  //list of wnd's memed from create in CBT hook
  typedef CSLSList<HWND> MemWndListType;
  typedef MemWndListType::iterator MemWndListIterType;
  MemWndListType MemWndList;

  ////hash set with subclasser info
  typedef CSLHashSet<HWND, CSubClassInfo> SubClassHashSetType;
  typedef SubClassHashSetType::iterator SubClassHashSetIterType;
  SubClassHashSetType SubClassHashSet;

  //number of lock's
  LONG lLockCount;

  //is data desrypted
  BOOL bIsDataDecrypted;

  ////mem hook install count from last process from CBT hook
  LONG lMemHookInstallCount;

  //handle for "common control's" dll
  HMODULE hCommCtrlModule;
  typedef BOOL (WINAPI* FlatSB_GetScrollPropType)(HWND, UINT, LPINT);
  FlatSB_GetScrollPropType pFlatSB_GetScrollProp;
  typedef BOOL (WINAPI* FlatSB_GetScrollInfoType)(HWND, int, LPSCROLLINFO);
  FlatSB_GetScrollInfoType pFlatSB_GetScrollInfo;

  XMLNode xMainNode;
  XMLResults xRes;

  bool m_IsHostProcess;

public:

public:

  CApp(HINSTANCE hInitInstance);
  ~CApp();

  //init application
  bool Init();
  //finti app
  void Finit();

  //hook function
  LRESULT MouseHookFunc(int nCode, WPARAM wParam, LPARAM lParam);
  //CBTProc hook
  LRESULT CBTHookFunc(int nCode, WPARAM wParam, LPARAM lParam);
  //dummy hook func - used for "clear" hook dispatch function
  LRESULT DummyHookFunc(int nCode, WPARAM wParam, LPARAM lParam);

  //analyser helper for start dll - return true if need drill down for child's (some process)
  inline bool AnalyseStartWndHelper(const HWND hcWnd);

  void SetIsHostProcess();

protected:

  //mouse event forwarding helper
  inline void MouseEventReForvarder(const MOUSEHOOKSTRUCT* const cpcInfo, const UINT uicClientEvent, const UINT uicNonClientEvent) const;

  //install all subclassing/send data to host
  void InstallSubClassingSendDataToHost();
  ////install sublcassing for process from hook's
  //process chng of CSharedNotClearedInfo::lHookInstallCount
  inline void InstallAllFromHook();

  //try unlock module
  inline void TryUnLock(const HWND hcWnd, CSubClassInfo* const cpInfo);

  //install/remove subclassing
  inline bool SubClassWnd(const HWND hcWnd);
  inline void UnSubClass(const HWND hcWnd, const bool bcFromDelWnd, const bool bcCanUnload);

  //restore cursor for passed wnd
  void RestoreCursorInWnd(const HWND hcWnd, const POINT& rcPnt) const;

  ////process for "global" message's
  //used for "exec some in WndProcess address space" (for example - hide wnd (for Win98 bug))
  LRESULT ProcessGlbMsg(const HWND hcWnd, WPARAM wParam, LPARAM lParam, CSubClassInfo* const cpInfo, const bool bcFullProcess);
  //try opend wnd - used before remove sublcassing or destroy wnd
  inline void TryOpenWnd(const HWND hcWnd, CSubClassInfo* const cpInfo);
  //update entire wnd area (client/nonclient) - used form Shrink wnd
  inline void TryUpdateEntireWnd(const HWND hcWnd);

  //set cursor for scroll visualisation
  inline void SetCursorForScroll();

  //send msg - (actualy "post message")
  inline LRESULT SendWndMsg(const HWND hcWnd, const UINT uicMsg, WPARAM wParam, LPARAM lParam) const;

  //auto hide move sensor
  inline void AHideMoveSensor(const HWND hcWnd);
  //process show wnd (for looking by host "is wnd new")
  inline void AHideSensor(const HWND hcWnd);
  inline void AShowSensor(const HWND hcWnd, const bool bcNotLookStyle = false);
  //sensor for destroy window
  inline void AHideDestroySensor(const HWND hcWnd);
  //activation (keyboard) sensor
  inline void AHideActivateSensor(const HWND hcWnd, const bool bcActive);

  //move sensor (calc mouse move dist summ)
  inline void MouseMoveDistSensor(const POINT& rcCurrPnt) const;
  //store "save" distance between passed pnt's
  inline void CalcSaveDistance(const POINT& rcPnt, const POINT& rcToPnt) const;
  //calc dist from two passed pnt's and stopre to resule
  inline void CalcPointDistance(const POINT& rcFrom, const POINT& rcTo, LONGLONG& rResult) const;

  //set process mode and send "mode icon" to host app
  inline void ChngHookMode(const UINT uicNewMode);
  inline void ChngHookModeRefresh();

  //now drill up in wnd hierarchy and try find wnd process (and set process mode and all it's data)
  UINT DrillChildUp(const HWND hcInitStart, const UINT uicCurrKeyFlag, const MOUSEHOOKSTRUCT* const cpMsg);
  ////drill for child's for passed wnd (down)
  //used from DrillChildUp
  void AnalyseForDrillChildDown(const HWND hcAnalyseWnd, const MOUSEHOOKSTRUCT* const cpMsg, LONG& rlHSbDist, bool& rbHFind, LONG& rlVSbDist, bool& rbVFind, const POINT& rcClickPoint);

  //////////////////////////////////////////////////////////////////////
  //mouse event's hooker's (called from hook proc) -
  //in this level realised looking for "is key stay pressed"

  //mouse down event
  inline bool MouseDownEvent(const UINT uicAddKey, const MOUSEHOOKSTRUCT* const cpMsg, const bool bcIsEatMsg, const MouseBtnEnum ecStartBtn);
  //mouse up event
  //uicMouseBtnNumb - whar key is enumlated - from MouseBtnEnum enum (look in source)
  //bcNotClientMode - select from "client"/"not client" pair's
  inline bool MouseUpEvent(const UINT uicDelKey, const MOUSEHOOKSTRUCT* const cpMsg, const UINT uicMouseBtnNumb, const bool bcIsEatMsg);
  //mouse move event
  inline bool MouseMoveEvent(const MOUSEHOOKSTRUCT* const cpMsg);

  //////////////////////////////////////////////////////////////////////
  //mouse event's section handler's - if msg catched - return true - need eat msg by hook

  //mouse key's already tested (for not duplication) and striped (for unneded)
  //mouse btn down (client)
  bool MouseBtnDown(const UINT uicCurrKeyFlag, const MOUSEHOOKSTRUCT* const cpMsg);
  //mouse btn up (client)
  bool MouseBtnUp(const UINT uicCurrKeyFlag, const MOUSEHOOKSTRUCT* const cpMsg, const UINT uicMouseBtnNumb);
  //mouse btn move (all - client and not client)
  bool MouseMoveAll(const UINT uicCurrKeyFlag, const MOUSEHOOKSTRUCT* const cpMsg);
  //process chng coord's while "slip" present
  void MouseSlipNewCoords(const UINT uicCurrKeyFlag, const MOUSEHOOKSTRUCT* const cpMsg);
  //abort mouse control - called when some wnd activated
  void MouseControlAbort(const bool bcAbortForward = false);

  //////////////////////////////////////////////////////////////////////
  //IE scroll section
  //start scroll IE
  bool IEScrollStart(const MOUSEHOOKSTRUCT* const cpMsg);
  bool IEScrollStartBD(const MOUSEHOOKSTRUCT* const cpMsg);
  //do scroll IE
  bool IEScrollMove(const MOUSEHOOKSTRUCT* const cpMsg);
  bool IEScrollMoveBD(const MOUSEHOOKSTRUCT* const cpMsg);
  //stop scroll IE
  inline bool IEScrollStop(const MOUSEHOOKSTRUCT* const cpMsg, const bool bcAbort);
  inline bool IEScrollStopBD(const MOUSEHOOKSTRUCT* const cpMsg, const bool bcAbort);
  inline void IEScrollSlipChng(const MOUSEHOOKSTRUCT* const cpMsg);
  //scrolling workers
  inline bool IEScrollStartInt(const MOUSEHOOKSTRUCT* const cpMsg, CSCrollIEProcessInfo& rScrollInfo, const bool bcRealyFirstStart, const bool bcHorStart);
  inline bool IEScrollMoveInt(const MOUSEHOOKSTRUCT* const cpMsg, CSCrollIEProcessInfo& rScrollInfo);
  inline bool IEScrollStopInt(const MOUSEHOOKSTRUCT* const cpMsg, CSCrollIEProcessInfo& rScrollInfo, const bool bcRealyAbort);

  //////////////////////////////////////////////////////////////////////
  //pager ctrl section

  inline bool PagerScrollStart(const MOUSEHOOKSTRUCT* const cpMsg);
  bool PagerScrollMove(const MOUSEHOOKSTRUCT* const cpMsg);
  inline bool PagerScrollStop(const MOUSEHOOKSTRUCT* const cpMsg, const bool bcAbort);
  inline void PagerScrollSlipChng(const MOUSEHOOKSTRUCT* const cpMsg);

  static inline bool CheckRealKey(const int icVKKey);

  //////////////////////////////////////////////////////////////////////
  //ScrollBar ctrl section (actual working)

  //start scrolling
  void SBScrollStart(const MOUSEHOOKSTRUCT* const cpMsg);
  void SBScrollStartBD(const MOUSEHOOKSTRUCT* const cpMsg);
  //helper
  inline void SBScrollStartInt(CScrollProcessInfo& rInfo, CScrollProcessInfo::CScrollDirInfo& rDirInfo, const bool bcIsHor, const POINT& rcPnt);
  //do scrolling
  void SBScrollMove(const MOUSEHOOKSTRUCT* const cpMsg);
  void SBScrollMoveBD(const MOUSEHOOKSTRUCT* const cpMsg);
  //helper
  inline void SBScrollMoveInt(CScrollProcessInfo& rInfo, CScrollProcessInfo::CScrollDirInfo& rDirInfo, const bool bcIsHor, const POINT& rcPnt);
  //stop scrolling
  void SBScrollStop(const MOUSEHOOKSTRUCT* const cpMsg, const bool bcAbort);
  void SBScrollStopBD(const MOUSEHOOKSTRUCT* const cpMsg, const bool bcAbort);
  ////chng coord's while "toggle slip" present
  inline void SBScrollSlipChngBD(const MOUSEHOOKSTRUCT* const cpMsg);
  inline void SBScrollSlipChng(const MOUSEHOOKSTRUCT* const cpMsg);
  //helper
  inline void SBScrollStopInt(CScrollProcessInfo& rInfo, CScrollProcessInfo::CScrollDirInfo& rDirInfo, const bool bcIsHor, const POINT& rcPnt, const bool bcAbort);
  //calc cpecial mouse pnt conversion for standard scroll bar's (from screen to "window coner" relative
  inline void SBScrollConvert(const HWND hcWnd, POINT& rcPnt, const bool bcIsFlatSB);
  //scalc shift for two passed point's (apply ScrollReReduction)
  //after call rFromPnt - "new point" (reminder apply)
  //rToPnt - calculated shift
  //dwcAddReduct - additional reduction - used for "subturning"
  inline void SBScrollCalcShift(POINT& rFromPnt, POINT& rToPnt, POINT& rMemMissPnt, const bool bcReduct, const DWORD dwcAddReduct) const;
  //calc "AddReduct" value for scroll bar's control
  //all "smart handling" removed now
  inline DWORD SBScrollCalcAddReduct(const HWND hcWnd, const bool bcIsCtrl, const bool bcIsHor, const bool bcIsFlat) const;
  inline DWORD ScrollGetCurrentReduction() const;

  //get info about scroll bar
  //return thumb box size in rThumbRect and scrollbale area in rScrollRect (all in screen coord's)
  //hwWnd - scroll bar wnd, bcIsCtrl - true if it's scroll bar ctrl, bcIsHor - true if look on horisontal scroll bar
  inline void SBGetInfo(const HWND hcWnd, const bool bcIsCtrl, const bool bcIsHor, RECT& rThumbRect, RECT& rScrollRect);
  //get info for Flat SB (some as SBGetInfo)
  inline void FlatSBGetInfo(const HWND hcWnd, const bool bcIsHor, RECT& rThumbRect, RECT& rScrollRect);
  //test scroll bar for Range > PageSize condition
  inline bool SCTestRangeGreatPage(const HWND hcWnd, const bool bcIsCtrl, const bool bcIsHor);
  inline bool FSBTestRangeGreatPage(const HWND hcWnd, const bool bcIsHor, const SCROLLINFO& rcInfo);
  //get min thumb size (depend on OS)
  static inline LONG GetMinThumbSize(const bool bcIsHor);
};


/**********************************************************************
implementation
**********************************************************************/

//////////////////////////////////////////////////////////////////////
//CSubClassInfo

inline CSubClassInfo::CSubClassInfo(const HWND hcInitWnd)
  :hcWnd(hcInitWnd),
  dwFlags(0),
  pOldProc(0),
  wWndProcCallCount(0)
{
};

inline bool CSubClassInfo::FlagTest(const ClassStateEnum cFlagNumb) const
{
  _ASSERT(ecsLast > cFlagNumb);
  return 0 != (dwFlags & (1 << cFlagNumb));
};

inline void CSubClassInfo::FlagSet(const ClassStateEnum cFlagNumb, const bool bcState)
{
  _ASSERT(ecsLast > cFlagNumb);
  if(false != bcState)
  {
    //set
    dwFlags |= 1 << cFlagNumb;
  }
  else
  {
    //remove
    dwFlags &= ~(1 << cFlagNumb);
  };
};

inline bool CSubClassInfo::ThunkInstall(const HWND hcWnd)
{
  bool bRes = false;

  if(FALSE == ::IsWindowUnicode(hcWnd))
  {
    InitThunk(&CSubClassInfo::WndProcA, this);
    pOldProc = reinterpret_cast<WNDPROC>(::SetWindowLongPtrA(hcWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(GetThunk())));
  }
  else
  {
    InitThunk(&CSubClassInfo::WndProcW, this);
    pOldProc = reinterpret_cast<WNDPROC>(::SetWindowLongPtrW(hcWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(GetThunk())));
  };
  bRes = 0 != pOldProc;

  return bRes;
};

inline bool CSubClassInfo::ThunkRemove(const HWND hcWnd)
{
  bool bRes = false;

  //some proc as "this"
  bool bSetToDummy = true;
  if
    (
    //wnd proc not chnaged after install subclasing
    reinterpret_cast<LONG_PTR>(GetThunk()) == ::GetWindowLongPtr(hcWnd, GWLP_WNDPROC)
    )
  {
    //remove subclassing
    bRes = 0 != ::SetWindowLongPtr(hcWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(pOldProc));
    bSetToDummy = false == bRes;
  };

  //if need - set wnd proc to dummy func
  if(false != bSetToDummy)
  {
    //goto dummy
    if(FALSE == ::IsWindowUnicode(hcWnd))
    {
      InitThunk(&CSubClassInfo::DummyWndProcA, this);
    }
    else
    {
      InitThunk(&CSubClassInfo::DummyWndProcW, this);
    };
    bRes = false;
  };

  return bRes;
};

inline void CSubClassInfo::ThunkReInstall(const HWND hcWnd)
{
  //thunk shold already present
  _ASSERT(0 != pOldProc);
  if(FALSE == ::IsWindowUnicode(hcWnd))
  {
    InitThunk(&CSubClassInfo::WndProcA, this);
  }
  else
  {
    InitThunk(&CSubClassInfo::WndProcW, this);
  };
};

inline bool CSubClassInfo::operator==(const HWND hcCompareWnd) const
{
  return hcWnd == hcCompareWnd;
};

inline void CSubClassInfo::ThunkRemoveTidy(const HWND hcWnd)
{
  ::SetWindowLongPtr(hcWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(pOldProc));
};

inline void CSubClassInfo::SwitchWndToDef()
{
  if(FALSE == ::IsWindowUnicode(hcWnd))
  {
    InitThunk(&CSubClassInfo::WndProcADef, this);
  }
  else
  {
    InitThunk(&CSubClassInfo::WndProcWDef, this);
  };
};

inline void CSubClassInfo::SwitchDummyWndToDef()
{
  if(FALSE == ::IsWindowUnicode(hcWnd))
  {
    InitThunk(&CSubClassInfo::DummyWndProcADef, this);
  }
  else
  {
    InitThunk(&CSubClassInfo::DummyWndProcWDef, this);
  };
};

inline void CSubClassInfo::DrawCloseMargin(const HWND hcWnd)
{
  HDC hDC = ::GetWindowDC(hcWnd);
  ////save dc state
  const int icState = ::SaveDC(hDC);
  ////draw
  ::DrawEdge(hDC, &CloseMargin, EDGE_SUNKEN, wDrawEdgeType);
  ////restore state
  ::RestoreDC(hDC, icState);
  ////kill
  ::ReleaseDC(hcWnd, hDC);
};


//////////////////////////////////////////////////////////////////////
//CApp

inline bool CApp::MouseDownEvent(const UINT uicAddKey, const MOUSEHOOKSTRUCT* const cpMsg, const bool bcIsEatMsg, const MouseBtnEnum ecStartBtn)
{
  //apply valid mask on passed key's
  //look on "skip mask" - if some - skip msg, else - process

  //apply mask on passed key's
  //if some key's already present - it's "process" mode (eat msg)
  //if result not null and old null - process "btn down mouse event"

  bool bRes = false;

  //wrk prm's
  const UINT uicValidKey = uicAddKey & egcValidMouseKeysMask;
  if(0 != (pCfgMem->uiSkipDownFlag & uicValidKey))
  {
    //skip this mouse motion - if this msg is eat by application
    if(false != bcIsEatMsg)
    {
      pCfgMem->uiSkipDownFlag &= ~uicValidKey;
    };
  }
  else
  {
    //process mouse key's
    //is this "actual processing" - remove some "skip flag's" from uiSkipUpFlag
    //becouse processed down message should have coresponding up message (not skip)
    pCfgMem->uiSkipUpFlag &= ~uicValidKey;

    const bool bcNoOtherPresentBefore = 0 == pCfgMem->uiCurrMouseKeys;
    pCfgMem->uiCurrMouseKeys |= uicValidKey;
    bRes = 0 != pCfgMem->uiCurrWorkKeys;
    const UINT uicNewWorkKey = (pCfgMem->uiCurrWorkKeys | uicAddKey) & pCfgMem->common64.uiValidMouseKeys & egcValidMouseKeysMask;
    if
      (
      //not some pressed before
      false != bcNoOtherPresentBefore
      //not work pressed before
      && false == bRes
      )
    {
      if(0 == uicNewWorkKey)
      {
        //this is first key press, but this is not work key - some work should be stoped
        switch(pCfgMem->ulControlState)
        {
          //stop control for all toggle mode's
          //abort control and not eat key
        case empMouseProcessToggleScroll:
        case empMouseProcessToggleSlipPressed:
          MouseControlAbort(true);
          pCfgMem->bReForwardMouseEvent = TRUE;
          bRes = false;
          break;

        default:
          break;
        };
        pCfgMem->ulControlState = empMouseProcessNone;
      }
      else
      {
        //now start work on selected mode
        switch(pCfgMem->ulControlState)
        {
        case empMouseProcessDirect:
          break;

        case empMouseProcessToggleScroll:
          //goto slip mode
          pCfgMem->SlipStartPnt = cpMsg->pt;
          pCfgMem->ulControlState = empMouseProcessToggleSlipPressed;
          bRes = true;
          break;

        case empMouseProcessToggleSlipPressed:
          bRes = true;
          break;

          //nether will present
        case empMouseProcessMixLook:
          break;

        case empMouseProcessNone:
        default:
          //mem "start" key's
          pCfgMem->dwMouseControlStartKey = ecStartBtn;
          //try start work here
          switch(pCfgMem->common64.dwControlType)
          {
          default:
          case escStandard:
            //process mouse event's
            if(false != MouseBtnDown(pCfgMem->uiCurrMouseKeys, cpMsg))
            {
              pCfgMem->ulControlState = empMouseProcessDirect;
              bRes = true;
            };
            break;
            
          case escToggleWithEat:
            //process mouse event's
            if(false != MouseBtnDown(pCfgMem->uiCurrMouseKeys, cpMsg))
            {
              pCfgMem->ulControlState = empMouseProcessToggleScroll;
              bRes = true;
            };
            break;
          case escScrollModeMix:
            if(false != MouseBtnDown(pCfgMem->uiCurrMouseKeys, cpMsg))
            {
              pCfgMem->ulControlState = empMouseProcessMixLook;
              pCfgMem->SlipStartPnt = cpMsg->pt;
              bRes = true;
            };
            break;
          };
          break;
        };
        //if work started - em key states and eat
        if(false != bRes)
        {
          pCfgMem->uiCurrWorkKeys = uicNewWorkKey;
        };
      };
    };
    //if "start work" present - add "current key's" to "work key mask"
    if(0 != pCfgMem->uiCurrWorkKeys || 0 != pCfgMem->uiCurrWorkKeysMask)
    {
      pCfgMem->uiCurrWorkKeysMask |= uicValidKey;
    };
    //mouse input captured
    bRes =
      //work present while
      false != bRes 
      //some present in "work key mask"
      || 0 != pCfgMem->uiCurrWorkKeysMask;
  };
  return bRes;
};

inline bool CApp::MouseUpEvent(const UINT uicDelKey, const MOUSEHOOKSTRUCT* const cpMsg, const UINT uicMouseBtnNumb, const bool bcIsEatMsg)
{
  //apply valid mask on passed key's
  //look on "skip mask" - if some - skip msg, else - process

  //apply "valid mask" on "passed key"
  //mem old
  //apply to current (as "now present")
  //if current not empty - eat
  //if old not null and new - null - apply "button up" event

  bool bRes = false;
  //valid
  const UINT uicValid = uicDelKey & egcValidMouseKeysMask;
  if(0 != (pCfgMem->uiEatUpFlag & uicValid))
  {
    //skip all "eated" mouse and not process
    pCfgMem->uiEatUpFlag &= ~uicValid;
    bRes = true;
  }
  else
  {
    //if some "up" event present - remove this flag's from "down skip flag"
    //becouse all "up" event siganled - "end of down/up" cycle ended
    pCfgMem->uiSkipDownFlag &= ~uicValid;
    if(0 != (uicValid & pCfgMem->uiSkipUpFlag))
    {
      //eat this msg - only if eat by application
      if(false != bcIsEatMsg)
      {
        pCfgMem->uiSkipUpFlag &= ~uicValid;
      };
    }
    else
    {
      ////actual processing for msg
      pCfgMem->uiCurrMouseKeys &= (~uicDelKey);
      //look on "work" key's
      bRes = 0 != pCfgMem->uiCurrWorkKeys;
      pCfgMem->uiCurrWorkKeys &= ((~uicDelKey) & pCfgMem->common64.uiValidMouseKeys & egcValidMouseKeysMask);
      
      if(false != bRes && 0 == pCfgMem->uiCurrWorkKeys)
      {
        //work keys chnaged
        bool bOffNow = true;
        bool bProcessMouseUp = false;
        switch(pCfgMem->ulControlState)
        {
        case empMouseProcessDirect:
          //work now
          bProcessMouseUp = true;
          break;
          
        case empMouseProcessToggleScroll:
          bOffNow = false;
          break;
          
        case empMouseProcessToggleSlipPressed:
          //look on motion distance
          if(false != DragPresent(pCfgMem->SlipStartPnt, cpMsg->pt))
          {
            //slip drag present - goto toggle
            MouseSlipNewCoords(pCfgMem->uiCurrMouseKeys, cpMsg);
            pCfgMem->ulControlState = empMouseProcessToggleScroll;
            bOffNow = false;
            ChngHookModeRefresh();
          }
          else
          {
            //drag not present - interpret as unpress
            bProcessMouseUp = true;
          };
          break;
          
        case empMouseProcessMixLook:
          //goto "toggle mode" (drag not detected)
          pCfgMem->ulControlState = empMouseProcessToggleScroll;
          bOffNow = false;
          ChngHookModeRefresh();
          break;
          
        case empMouseProcessNone:
        default:
          break;
        };
        //off control state
        if(false != bOffNow)
        {
          pCfgMem->ulControlState = empMouseProcessNone;
        };
        //process mouse btn up
        if(false != bProcessMouseUp)
        {
          MouseBtnUp(pCfgMem->uiCurrMouseKeys, cpMsg, uicMouseBtnNumb);
        };
      };
      ////mouse input captured
      bRes =
        //until some present in "work key mask"
        0 != pCfgMem->uiCurrWorkKeysMask;
      //clear "work key mask"
      pCfgMem->uiCurrWorkKeysMask &= ~uicValid;
    };
  };

  return bRes;
};

inline bool CApp::MouseMoveEvent(const MOUSEHOOKSTRUCT* const cpMsg)
{
  //simple move

  bool bRes = false;
  
  switch(pCfgMem->ulControlState)
  {
  case empMouseProcessDirect:
    MouseMoveAll(pCfgMem->uiCurrMouseKeys, cpMsg);
    bRes = 0 != pCfgMem->uiCurrWorkKeysMask;
    break;
    
  case empMouseProcessToggleScroll:
    MouseMoveAll(pCfgMem->uiCurrMouseKeys, cpMsg);
    bRes = true;
    break;

  case empMouseProcessToggleSlipPressed:
    //"slip mode" - skip processing
    //refresh hook mode for visualise "sliping"
    if
      (
      IDC_SCR_NONE_OPEN_CUR != pCfgMem->uiCursorName
      && false != DragPresent(pCfgMem->SlipStartPnt, cpMsg->pt)
      )
    {
      ChngHookModeRefresh();
    };
    bRes = true;
    break;
    
  case empMouseProcessMixLook:
    //detect draw - fi any - goto "direct work" mode
    if(false != DragPresent(pCfgMem->SlipStartPnt, cpMsg->pt))
    {
      pCfgMem->ulControlState = empMouseProcessDirect;
      ChngHookModeRefresh();
    };
    bRes = true;
    break;
    
  case empMouseProcessNone:
  default:
    break;
  };

  //set control state now
  if(false == bRes)
  {
    pCfgMem->ulControlState = empMouseProcessNone;
  };

  return bRes;
};

inline void CApp::SBScrollConvert(const HWND hcWnd, POINT& rcPnt, const bool bcIsFlatSB)
{
  if(false == bcIsFlatSB)
  {
    RECT Rect;
    ::GetWindowRect(hcWnd, &Rect);
    rcPnt.x -= Rect.left;
    rcPnt.y -= Rect.top;
  }
  else
  {
    ::ScreenToClient(hcWnd, &rcPnt);
  };
};

inline void CApp::SBScrollCalcShift(POINT& rFromPnt, POINT& rToPnt, POINT& rMemMissPnt, const bool bcReduct, const DWORD dwcAddReduct) const
{
  //calc reduction value
  LONG lReductVal = (false != bcReduct)
    ? pCfgMem->common64.lScrollReduction
    : egcMagnScrollReduction;

  //apply "additional reduction"
  lReductVal = MyMulDiv(lReductVal, egcMagnScrollReduction, dwcAddReduct);

  //calc point chng
  POINT Dist;
  Dist.x = MyMulDivLongMiss(rFromPnt.x - rToPnt.x, egcMagnScrollReduction, rMemMissPnt.x, lReductVal);
  Dist.y = MyMulDivLongMiss(rFromPnt.y - rToPnt.y, egcMagnScrollReduction, rMemMissPnt.y, lReductVal);
  //new point
  rFromPnt.x = rToPnt.x;
  rFromPnt.y = rToPnt.y;
  //result
  rToPnt = Dist;
};

inline void CApp::ChngHookModeRefresh()
{
  ChngHookMode(pCfgMem->lHookMode);
};

inline void CApp::ChngHookMode(const UINT uicNewMode)
{
  //look for icon for tray and for cursor
  pCfgMem->lHookMode = uicNewMode;
  pCfgMem->bCursorNeedSet = TRUE;
  switch(pCfgMem->ulControlState)
  {
  case empMouseProcessToggleSlipPressed:
    //set clip cursor
    pCfgMem->bCursorNeedSet = true;
    pCfgMem->uiCursorName = IDC_SCR_NONE_OPEN_CUR;
    break;

  default:
  case empMouseProcessDirect:
  case empMouseProcessToggleScroll:
  case empMouseProcessMixLook:
    //set cursor depend on hook mode
    switch(pCfgMem->lHookMode)
    {
    case ehmScrollPress:
      if(0 == pCfgMem->ScrollProcessInfo.HInfo.hWnd)
      {
        pCfgMem->uiCursorName = IDC_SCR_VERT_OPEN_CUR;
      }
      else if(0 == pCfgMem->ScrollProcessInfo.VInfo.hWnd)
      {
        pCfgMem->uiCursorName = IDC_SCR_HOR_OPEN_CUR;
      }
      else
      {
        pCfgMem->uiCursorName = IDC_SCR_ALL_OPEN_CUR;
      };
      break;
      
    case ehmScrollProcess:
    case ehmBDScrollProcess:
      pCfgMem->uiCursorName = (false != pCfgMem->ScrollProcessInfo.bHorProcess) ? IDC_SCR_HOR_CLOSE_CUR : IDC_SCR_VERT_CLOSE_CUR;
      break;
      
    case ehmScrollIEPress:
      pCfgMem->uiCursorName = (false != pCfgMem->ScrollIEProcessInfo.bLockedScrollDirection)
        ? ((false != pCfgMem->ScrollIEProcessInfo.bHorScroll) ? IDC_SCR_HOR_OPEN_CUR : IDC_SCR_VERT_OPEN_CUR)
        : IDC_SCR_ALL_OPEN_CUR;
      break;
      
    case ehmScrollIEProcess:
    case ehmBDScrollIEProcess:
      pCfgMem->uiCursorName = (false == pCfgMem->ScrollIEProcessInfo.bHorScroll)
        ? IDC_SCR_VERT_CLOSE_CUR
        : IDC_SCR_HOR_CLOSE_CUR;
      break;
      
    case ehmScrollPagerPress:
      pCfgMem->uiCursorName = (false != pCfgMem->ScrollPagerCtrlInfo.bHorProcess)
        ? IDC_SCR_HOR_OPEN_CUR
        : IDC_SCR_VERT_OPEN_CUR;
      break;
      
    case ehmScrollPagerProcess:
      pCfgMem->uiCursorName = (false != pCfgMem->ScrollPagerCtrlInfo.bHorProcess)
        ? IDC_SCR_HOR_CLOSE_CUR
        : IDC_SCR_VERT_CLOSE_CUR;
      break;
      
    default:
      pCfgMem->bCursorNeedSet = FALSE;
      break;
    };
    break;
  };
};

inline void CApp::MouseMoveDistSensor(const POINT& rcCurrPnt) const
{
  CalcPointDistance(pCfgMem->MoveDistanseLastLookPnt, rcCurrPnt, pCfgMem->common64.llMoveDistanceSumm);
  pCfgMem->MoveDistanseLastLookPnt = rcCurrPnt;
};

inline void CApp::CalcPointDistance(const POINT& rcFrom, const POINT& rcTo, LONGLONG& rResult) const
{
  const LONGLONG llcHDist = static_cast<LONGLONG>(rcTo.x - rcFrom.x);
  const LONGLONG llcVDist = static_cast<LONGLONG>(rcTo.y - rcFrom.y);
  rResult += max(llcHDist, -llcHDist) + max(llcVDist, -llcVDist);
};

inline void CApp::CalcSaveDistance(const POINT& rcPnt, const POINT& rcToPnt) const
{
  CalcPointDistance(rcPnt, rcToPnt, pCfgMem->common64.llMoveDistanceSaveSumm);
};

inline void CApp::AHideMoveSensor(const HWND hcWnd)
{
  //look "already looked" wnd
  //if failed - try drill from this to up
  //if find - compare with "already active" wnd
  //if not some - notify HostWnd

  //already looked
  if(hcWnd != pCfgMem->hAHideLastLookingWnd)
  {
    pCfgMem->hAHideLastLookingWnd = hcWnd;
    //try drill
    HWND hFindActive = 0;
    if
      (
      false == AHideDrillForWindow(hcWnd, hFindActive)
      || hFindActive != pCfgMem->hAHideLastActiveSendWnd
      )
    {
      //active chnged
      pCfgMem->hAHideLastActiveSendWnd = hFindActive;
      //to host
      ::PostMessage(pCfgMem->common64.hMainHostWnd, emcAHWndActivity, 0, reinterpret_cast<LPARAM>(hFindActive));
    };
  };
};

inline void CApp::AShowSensor(const HWND hcWnd, const bool bcNotLookStyle)
{
  //if wnd show - send to host

  //to host
  if(false != bcNotLookStyle || FALSE != AHideCheckWndStyle(hcWnd))
  {
    ::PostMessage(pCfgMem->common64.hMainHostWnd, emcAHWndShow, CheckWndActive(hcWnd), reinterpret_cast<LPARAM>(hcWnd));
  };
};

inline void CApp::AHideSensor(const HWND hcWnd)
{
  //if wnd show - send to host

  //to host
  if(FALSE != AHideCheckWndStyle(hcWnd))
  {
    ::PostMessage(pCfgMem->common64.hMainHostWnd, emcAHWndHide, 0, reinterpret_cast<LPARAM>(hcWnd));
  };
};

inline void CApp::AHideDestroySensor(const HWND hcWnd)
{
  //to host
  if(FALSE != AHideCheckWndStyle(hcWnd))
  {
    ::PostMessage(pCfgMem->common64.hMainHostWnd, emcAHWndDestroy, 0, reinterpret_cast<LPARAM>(hcWnd));
  };
};

inline void CApp::AHideActivateSensor(const HWND hcWnd, const bool bcActive)
{
  //to host
  if(FALSE != AHideCheckWndStyle(hcWnd))
  {
    ::PostMessage(pCfgMem->common64.hMainHostWnd, emcAHWndActive, bcActive, reinterpret_cast<LPARAM>(hcWnd));
  };
};

inline LRESULT CApp::SendWndMsg(const HWND hcWnd, const UINT uicMsg, WPARAM wParam, LPARAM lParam) const
{
  //simple post message (nice work for mouse msg's)
  return ::PostMessage(hcWnd, uicMsg, wParam, lParam);
};

inline void CApp::SetCursorForScroll()
{
  if(FALSE != pCfgMem->common64.bCursorVisualise && FALSE != pCfgMem->bCursorNeedSet)
  {
    ::SetCursor(::LoadCursor(hInstance, MAKEINTRESOURCE(pCfgMem->uiCursorName)));
  };
};

inline bool CApp::IEScrollStop(const MOUSEHOOKSTRUCT* const cpMsg, const bool bcAbort)
{
  return IEScrollStopInt(cpMsg, pCfgMem->ScrollIEProcessInfo, bcAbort);
};

inline LONG CApp::GetMinThumbSize(const bool bcIsHor)
{
  const DWORD dwcVerMask = 1 << (sizeof(DWORD) * CHAR_BIT - 1);
  return (0 != (::GetVersion() & dwcVerMask))
    ? ::GetSystemMetrics((false != bcIsHor) ? SM_CXHSCROLL : SM_CYHSCROLL) / 2
    : eccThumbMinSizeUnderNT;
};

inline bool CApp::FSBTestRangeGreatPage(const HWND hcWnd, const bool bcIsHor, const SCROLLINFO& rcInfo)
{
  bool bRes = true;

  //look on "scroll info ragne's
  if(false != bRes)
  {
    _ASSERT(0 != ((SIF_RANGE | SIF_PAGE) & rcInfo.fMask));
    const int icSignPageSize = static_cast<int>(rcInfo.nPage);

    //min is zero based
    bRes =
      rcInfo.nMax > rcInfo.nMin
      && (rcInfo.nMax - rcInfo.nMin) >= icSignPageSize;
  };

  //look on geometry
  if(false != bRes)
  {
    RECT WndRect;
    ::GetWindowRect(hcWnd, &WndRect);
    RECT CliRect;
    ::GetClientRect(hcWnd, &CliRect);
    ::MapWindowPoints(hcWnd, 0, reinterpret_cast<LPPOINT>(&CliRect), 2);
    //look on "client/wnd" rect's threshold
    bRes = (false != bcIsHor)
        ? ::GetSystemMetrics(SM_CYHSCROLL) <= WndRect.bottom - CliRect.bottom
        : ::GetSystemMetrics(SM_CXVSCROLL) <= WndRect.right - CliRect.right;
    if(false != bRes)
    {
      if(false != bcIsHor)
      {
        //hor scroll in wnd
        INT iThumbSize = 0;
        pFlatSB_GetScrollProp(hcWnd, WSB_PROP_CXHTHUMB, &iThumbSize);
        INT iArrowSize = 0;
        pFlatSB_GetScrollProp(hcWnd, WSB_PROP_CXHSCROLL, &iArrowSize);
        bRes = (CliRect.right - CliRect.left) >= (2 * iArrowSize + iThumbSize);
      }
      else
      {
        //vert scroll in wnd
        INT iThumbSize = 0;
        pFlatSB_GetScrollProp(hcWnd, WSB_PROP_CYVTHUMB, &iThumbSize);
        INT iArrowSize = 0;
        pFlatSB_GetScrollProp(hcWnd, WSB_PROP_CYVSCROLL, &iArrowSize);
        bRes = (CliRect.bottom - CliRect.top) >= (2 * iArrowSize + iThumbSize);
      };
    };
  };

  return bRes;
};

inline bool CApp::SCTestRangeGreatPage(const HWND hcWnd, const bool bcIsCtrl, const bool bcIsHor)
{
  bool bRes = true;

  //look on "scroll info ragne's
  if(false != bRes)
  {
    const int icCode = (false != bcIsCtrl)
      ? SB_CTL
      : ((false != bcIsHor) ? SB_HORZ : SB_VERT);
    SCROLLINFO Info;
    Info.cbSize = sizeof(Info);
    Info.fMask = SIF_RANGE | SIF_PAGE;
    ::GetScrollInfo(hcWnd, icCode, &Info);
    const int icSignPageSize = static_cast<int>(Info.nPage);

    //min is zero based
    bRes =
      Info.nMax > Info.nMin
      && (Info.nMax - Info.nMin) >= icSignPageSize;
  };

  //look on geometry
  if(false != bRes)
  {
    RECT WndRect;
    if(false == bcIsCtrl)
    {
      RECT Rect;
      ::GetWindowRect(hcWnd, &Rect);
      ::GetClientRect(hcWnd, &WndRect);
      ::MapWindowPoints(hcWnd, 0, reinterpret_cast<LPPOINT>(&WndRect), 2);
      //try look on client/wnd rect's treshold
      bRes = (false != bcIsHor)
        ? ::GetSystemMetrics(SM_CYHSCROLL) <= Rect.bottom - WndRect.bottom
        : ::GetSystemMetrics(SM_CXVSCROLL) <= Rect.right - WndRect.right;
    }
    else
    {
      ::GetWindowRect(hcWnd, &WndRect);
    };
    if(false != bRes)
    {
      if(false != bcIsHor)
      {
        //hor scroll in wnd
        const int icThumbMinSize = GetMinThumbSize(true);
        const int icArrowSize = ::GetSystemMetrics(SM_CXHSCROLL);
        bRes = (WndRect.right - WndRect.left) >= (2 * icArrowSize + icThumbMinSize);
      }
      else
      {
        //vert scroll in wnd
        const int icThumbMinSize = GetMinThumbSize(false);
        const int icArrowSize = ::GetSystemMetrics(SM_CYHSCROLL);
        bRes = (WndRect.bottom - WndRect.top) >= (2 * icArrowSize + icThumbMinSize);
      };
    };
  };

  return bRes;
};

inline void CApp::TryUnLock(const HWND hcWnd, CSubClassInfo* const cpInfo)
{
  if
    (
    //nto recurive call
    0 == cpInfo->wWndProcCallCount
    //need process
    && (false != cpInfo->FlagTest(CSubClassInfo::ecsDelObject) || false != cpInfo->FlagTest(CSubClassInfo::ecsUnLockModule))
    )
  {
    //incr thread priprity
    HANDLE hCurrThrd = ::GetCurrentThread();
    const int icThrdPrior = ::GetThreadPriority(hCurrThrd);
    ::SetThreadPriority(hCurrThrd, THREAD_PRIORITY_HIGHEST);
    //try unlock
    if(false != cpInfo->FlagTest(CSubClassInfo::ecsUnLockModule))
    {
      cpInfo->FlagSet(CSubClassInfo::ecsUnLockModule, false);
#ifndef __RELEASE_CFG
      //if hook's present
      TCHAR cpClassBuff[egcWndClassTextLen];
      if
        (
        //first check
        0 != pCfgMem->hHookPresentFlag
        //check "host wnd class"
        && 0 != ::GetClassName(pCfgMem->common64.hMainHostWnd, cpClassBuff, COUNTOF(cpClassBuff))
        && false != ASCompareClassSimple(cpClassBuff, cpcWndClass)
        //check again (apssoble exit while PostThreadMessage time)
        && 0 != pCfgMem->hHookPresentFlag
        )
      {
        if(0 == ::InterlockedDecrement(&lLockCount))
        {
          ::FreeLibrary(hInstance);
        };
      };
#endif//__RELEASE_CFG
    };
    //del obj
    if(false != cpInfo->FlagTest(CSubClassInfo::ecsDelObject))
    {
      cpInfo->FlagSet(CSubClassInfo::ecsDelObject, false);
      ////remove object from hash set
      SubClassHashSet.Remove(hcWnd);
    };

    //restore prior
    ::SetThreadPriority(hCurrThrd, icThrdPrior);
  };
};

inline bool CApp::SubClassWnd(const HWND hcWnd)
{
	bool bRes = false;

  //try get wnd process id
  const DWORD dwcWndThrd = ::GetWindowThreadProcessId(hcWnd, 0);

  if(::GetCurrentThreadId() == dwcWndThrd)
  {
    ////only if subclass in some wnd
    //try get current subclass info from current wnd
    CSubClassInfo* pInfo = SubClassHashSet.Find(hcWnd);
    bool bTryLock = false;
    if(0 != pInfo)
    {
      ////subclasser present - reinstall
      if(false != pInfo->FlagTest(CSubClassInfo::ecsIsDummy))
      {
        pInfo->ThunkReInstall(hcWnd);
        pInfo->FlagSet(CSubClassInfo::ecsIsDummy, false);
        //clear del/unlock flag's
        pInfo->FlagSet(CSubClassInfo::ecsDelObject, false);
        pInfo->FlagSet(CSubClassInfo::ecsUnLockModule, false);
      };
      bRes = true;
    }
    else
    {
      ////subclasser not present - make new
      pInfo = SubClassHashSet.Add(hcWnd);
      //try install
      if(false != pInfo->ThunkInstall(hcWnd))
      {
        bRes = true;
        bTryLock = true;
      }
      else
      {
        ////remove
        SubClassHashSet.Remove(hcWnd);
      };
    };

    //lock
    if(false != bTryLock)
    {
      if(0 == lLockCount)
      {
        CHAR cpName[MAX_PATH];
        ::GetModuleFileName(hInstance, cpName, COUNTOF(cpName));
        ::LoadLibrary(cpName);
      };
      ::InterlockedIncrement(&lLockCount);
    };
  };

  return bRes;
};

inline void CApp::UnSubClass(const HWND hcWnd, const bool bcFromDelWnd, const bool bcCanUnload)
{
  CSubClassInfo* const cpInfo = SubClassHashSet.Find(hcWnd);
  if(0 != cpInfo)
  {
    bool bDelObj = false;
    bool bTryUnLock = false;
    if(false == bcFromDelWnd)
    {
      ////standard remove
      if(false != cpInfo->ThunkRemove(hcWnd))
      {
        //ok remove
        bDelObj = true;
        bTryUnLock = true;
      }
      else
      {
        //reemove goto dummy
        bTryUnLock = false;
        bDelObj = false;
        cpInfo->FlagSet(CSubClassInfo::ecsIsDummy, true);
      };
    }
    else
    {
      ////remove from del wnd
      cpInfo->ThunkRemoveTidy(hcWnd);
      bTryUnLock = true;
      bDelObj = true;
    };
    //set flag's to object (for delay unloading and unloking)
    cpInfo->FlagSet(CSubClassInfo::ecsDelObject, bDelObj);
    cpInfo->FlagSet(CSubClassInfo::ecsUnLockModule, false != bTryUnLock && false != bcCanUnload);
  };
};

inline bool CApp::AnalyseStartWndHelper(const HWND hcWnd)
{
  bool bRes = false;

  //get wnd process
  DWORD dwWndProc = 0;
  ::GetWindowThreadProcessId(hcWnd, &dwWndProc);
  if(::GetCurrentProcessId() == dwWndProc)
  {
    bRes = true;
    //mem to process in CBT hook
    MemWndList.Push(hcWnd);
  };

  return bRes;
};

inline void CApp::TryOpenWnd(const HWND hcWnd, CSubClassInfo* const cpInfo)
{
  if(false != cpInfo->FlagTest(CSubClassInfo::ecsIsShrinked))
  {
    cpInfo->FlagSet(CSubClassInfo::ecsIsShrinked, false);
    if(0 == SetWindowRgn(hcWnd, 0, TRUE))
    {
      //failed
      cpInfo->FlagSet(CSubClassInfo::ecsIsShrinked, true);
    }
    else
    {
      TryUpdateEntireWnd(hcWnd);
    };
  };
};

inline void CApp::TryUpdateEntireWnd(const HWND hcWnd)
{
  ::InvalidateRect(hcWnd, 0, TRUE);
  ::PostMessage(hcWnd, WM_NCPAINT, 1, 0);
};

void CApp::FlatSBGetInfo(const HWND hcWnd, const bool bcIsHor, RECT& rThumbRect, RECT& rScrollRect)
{
  //get scroll bar wnd rect
  //it's wnd with scroll bar's - calc client and apply "scroll bar at bottom of client and have Height/Width as ::GetSystemMetrics(SM_CYHSCROLL/SM_CXVSCROLL)
  ::GetClientRect(hcWnd, &rScrollRect);
  if(false != bcIsHor)
  {
    //horisontal scroll bar - it's at bottom of wnd
    INT iSBSize = 0;
    pFlatSB_GetScrollProp(hcWnd, WSB_PROP_CYHSCROLL, &iSBSize);
    rScrollRect.top = rScrollRect.bottom;
    rScrollRect.bottom = rScrollRect.top + iSBSize;
  }
  else
  {
    //vertical scroll bar - it's at right of wnd
    INT iSBSize = 0;
    pFlatSB_GetScrollProp(hcWnd, WSB_PROP_CXVSCROLL, &iSBSize);
    rScrollRect.left = rScrollRect.right;
    rScrollRect.right = rScrollRect.left + iSBSize;
  };
  //convert from client rect to wnd rect
  ::MapWindowPoints(hcWnd, 0, reinterpret_cast<POINT*>(&rScrollRect), 2);

  //now look on arraow sizes and decrement from scroll bar rect
  if(false != bcIsHor)
  {
    //hor scroll bar
    INT iArrSize = 0;
    pFlatSB_GetScrollProp(hcWnd, WSB_PROP_CXHSCROLL, &iArrSize);
    rScrollRect.left += iArrSize;
    rScrollRect.right -= iArrSize;
  }
  else
  {
    //vert scroll bar
    INT iArrSize = 0;
    pFlatSB_GetScrollProp(hcWnd, WSB_PROP_CYVSCROLL, &iArrSize);
    rScrollRect.top += iArrSize;
    rScrollRect.bottom -= iArrSize;
  };

  ////get scroll info
  SCROLLINFO ScrollInfo;
  ScrollInfo.cbSize = sizeof(ScrollInfo);
  ScrollInfo.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
  //scroll bar type
  const int icScrollBarType = (false != bcIsHor) ? SB_HORZ : SB_VERT;
  pFlatSB_GetScrollInfo(hcWnd, icScrollBarType, &ScrollInfo);
  const int icScrollRange = ScrollInfo.nMax - ScrollInfo.nMin + 1;

  ////now try get thumb rect
  INT iThumbSize = 0;
  if(0 == ScrollInfo.nPage)
  {
    //"standard thumb" size
    pFlatSB_GetScrollProp(hcWnd, (false != bcIsHor) ? WSB_PROP_CXHTHUMB : WSB_PROP_CYVTHUMB, &iThumbSize);
  }
  else
  {
    //min thumb size
    const int icMinThumbSize = GetMinThumbSize(bcIsHor);
    //proportional thumb - ThumbWidth from proportion PageSize / Range = ThumbSize / ScrollBarSize
    const int icScrollBarWidth = (false != bcIsHor)
      ? rScrollRect.right - rScrollRect.left
      : rScrollRect.bottom - rScrollRect.top;
    iThumbSize = max(::MyMulDiv(ScrollInfo.nPage, icScrollBarWidth, icScrollRange), static_cast<DWORD>(icMinThumbSize));
  };

  ////thumb pos
  if(false != bcIsHor)
  {
    //horisontal scroll bar
    rThumbRect.top = rScrollRect.top;
    rThumbRect.bottom = rScrollRect.bottom;
    if(icScrollRange == static_cast<int>(ScrollInfo.nPage))
    {
      //scroll range equal to page size (thumb not scrollable)
      rThumbRect.left = rScrollRect.left;
      rThumbRect.right = rScrollRect.right - 1;
    }
    else
    {
      rThumbRect.left = rScrollRect.left + ::MyMulDiv(ScrollInfo.nPos - ScrollInfo.nMin, rScrollRect.right - rScrollRect.left - iThumbSize, icScrollRange - ScrollInfo.nPage);
      rThumbRect.right = rThumbRect.left + iThumbSize;
    };
  }
  else
  {
    //vertical scroll bar
    rThumbRect.left = rScrollRect.left;
    rThumbRect.right = rScrollRect.right;
    if(icScrollRange == static_cast<int>(ScrollInfo.nPage))
    {
      //scroll range equal to page size (thumb not scrollable)
      rThumbRect.top = rScrollRect.top;
      rThumbRect.bottom = rScrollRect.bottom - 1;
    }
    else
    {
      rThumbRect.top = rScrollRect.top + ::MyMulDiv(ScrollInfo.nPos - ScrollInfo.nMin, rScrollRect.bottom - rScrollRect.top - iThumbSize, icScrollRange - ScrollInfo.nPage);
      rThumbRect.bottom = rThumbRect.top + iThumbSize;
    };
  };
};

void CApp::SBGetInfo(const HWND hcWnd, const bool bcIsCtrl, const bool bcIsHor, RECT& rThumbRect, RECT& rScrollRect)
{
  //if std scroll bar - calc entire scroll bar width/height as width/height of client area and put to scr coord's
  //else (scrol bar ctrl) - look on wnd size
  //for either type - break available arrow size
  //sb:get scroll info and try calc thumb size based on Range and PageSize - (if page size 0 - use ::GetSystemMetrics(SM_CXTHUMB)) and apply ::GetSystemMetrics(SM_CXEDGE) as edges

  //get scroll bar wnd rect
  if(false != bcIsCtrl)
  {
    //it's scroll bar ctrl - direct call
    ::GetWindowRect(hcWnd, &rScrollRect);
  }
  else
  {
    //it's wnd with scroll bar's - calc client and apply "scroll bar at bottom of client and have Height/Width as ::GetSystemMetrics(SM_CYHSCROLL/SM_CXVSCROLL)
    ::GetClientRect(hcWnd, &rScrollRect);
    if(false != bcIsHor)
    {
      //horisontal scroll bar - it's at bottom of wnd
      rScrollRect.top = rScrollRect.bottom;
      rScrollRect.bottom = rScrollRect.top + ::GetSystemMetrics(SM_CYHSCROLL);
    }
    else
    {
      //vertical scroll bar - it's at right of wnd
      rScrollRect.left = rScrollRect.right;
      rScrollRect.right = rScrollRect.left + ::GetSystemMetrics(SM_CXVSCROLL);
    };
    //convert from client rect to wnd rect
    ::MapWindowPoints(hcWnd, 0, reinterpret_cast<POINT*>(&rScrollRect), 2);
  };

  //now look on arraow sizes and decrement from scroll bar rect
  if(false != bcIsHor)
  {
    //hor scroll bar
    const int icSize = ::GetSystemMetrics(SM_CXHSCROLL);
    rScrollRect.left += icSize;
    rScrollRect.right -= icSize;
  }
  else
  {
    //vert scroll bar
    const int icSize = ::GetSystemMetrics(SM_CYVSCROLL);
    rScrollRect.top += icSize;
    rScrollRect.bottom -= icSize;
  };

  //now try get thumb rect
  //sb:get info about scroll bar
  //break on hor/vert scroll bar
  //  look on ScrollInfo::PageSize - if zero - user ::GetSystemMetrics(SM_CXTHUMB/SM_CYTHUMB)
  //  apply 3d edges to thumb size
  //  sb:calc thumb pos as proportional from MinRect - ThumbPos - MaxRect

  //get scroll info
  SCROLLINFO ScrollInfo;
  ScrollInfo.cbSize = sizeof(ScrollInfo);
  ScrollInfo.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
  //scroll bar type
  const int icScrollBarType = (false != bcIsCtrl)
    ? SB_CTL
    : ((false != bcIsHor) ? SB_HORZ : SB_VERT);
  ::GetScrollInfo(hcWnd, icScrollBarType, &ScrollInfo);
  const int icScrollRange = ScrollInfo.nMax - ScrollInfo.nMin + 1;

  int iThumbSize = 0;
  if(0 == ScrollInfo.nPage)
  {
    //"standard thumb" size
    iThumbSize = ::GetSystemMetrics((false != bcIsHor) ? SM_CXHTHUMB : SM_CYVTHUMB);
  }
  else
  {
    //min thumb size
    const int icMinThumbSize = GetMinThumbSize(bcIsHor);
    //proportional thumb - ThumbWidth from proportion PageSize / Range = ThumbSize / ScrollBarSize
    const int icScrollBarWidth = (false != bcIsHor)
      ? rScrollRect.right - rScrollRect.left
      : rScrollRect.bottom - rScrollRect.top;
    iThumbSize = max(::MyMulDiv(ScrollInfo.nPage, icScrollBarWidth, icScrollRange), static_cast<DWORD>(icMinThumbSize));
  };

  //calc thumb rect pos frm proportion
  //sb:calc shift of humb (from start of scroll bar) from proportion ThumbCenterWidth / TumbCenter = Range / Pos
  //  where ThumbCenterWidth - is a width of scroll bar witout tumb width
  //  actual thumb cener point is ScrollBarTop(Left) + TumbCenter + TumbWidth / 2
  if(false != bcIsHor)
  {
    //horisontal scroll bar
    rThumbRect.top = rScrollRect.top;
    rThumbRect.bottom = rScrollRect.bottom;
    if(icScrollRange == static_cast<int>(ScrollInfo.nPage))
    {
      //scroll range equal to page size (thumb not scrollable)
      rThumbRect.left = rScrollRect.left;
      rThumbRect.right = rScrollRect.right - 1;
    }
    else
    {
      rThumbRect.left = rScrollRect.left + ::MyMulDiv(ScrollInfo.nPos - ScrollInfo.nMin, rScrollRect.right - rScrollRect.left - iThumbSize, icScrollRange - ScrollInfo.nPage);
      rThumbRect.right = rThumbRect.left + iThumbSize;
    };
  }
  else
  {
    //vertical scroll bar
    rThumbRect.left = rScrollRect.left;
    rThumbRect.right = rScrollRect.right;
    if(icScrollRange == static_cast<int>(ScrollInfo.nPage))
    {
      //scroll range equal to page size (thumb not scrollable)
      rThumbRect.top = rScrollRect.top;
      rThumbRect.bottom = rScrollRect.bottom - 1;
    }
    else
    {
      rThumbRect.top = rScrollRect.top + ::MyMulDiv(ScrollInfo.nPos - ScrollInfo.nMin, rScrollRect.bottom - rScrollRect.top - iThumbSize, icScrollRange - ScrollInfo.nPage);
      rThumbRect.bottom = rThumbRect.top + iThumbSize;
    };
  };
};

inline void CApp::SBScrollStartInt(CScrollProcessInfo& rInfo, CScrollProcessInfo::CScrollDirInfo& rDirInfo, const bool bcIsHor, const POINT& rcPnt)
{
  //clear magn misses
  rInfo.LastScrollMagnMiss.x = 0;
  rInfo.LastScrollMagnMiss.y = 0;
  //get scroll bar rect's
  RECT ThumbRect;
  if(false != rDirInfo.bFlatSB)
  {
    FlatSBGetInfo(rDirInfo.hWnd, bcIsHor, ThumbRect, rInfo.WorkArea);
  }
  else
  {
    SBGetInfo(rDirInfo.hWnd, rDirInfo.bIsCtrl, bcIsHor, ThumbRect, rInfo.WorkArea);
  };

  //emulate start of scroll (mouse down in center of thumb)
  POINT& rCurrPnt = rInfo.LastEmulateThumbPnt;
  rCurrPnt.x = (ThumbRect.right + ThumbRect.left) / 2;
  rCurrPnt.y = (ThumbRect.bottom + ThumbRect.top) / 2;
  rInfo.LastProcessLookPnt = rcPnt;
  rInfo.LastScrollPnt = rcPnt;
  //send msg
  const HWND hWnd = rDirInfo.hWnd;
  const bool bcIsCtrl = rDirInfo.bIsCtrl;

  //win2000 around
  if(FALSE != pCfgMem->bWin2000PatchRequired)
  {
    BYTE cpMapTbl[eccStateKeyMassSize];
    ::GetKeyboardState(cpMapTbl);
    const BYTE cMask = 1 << (CHAR_BIT - 1);
    cpMapTbl[VK_LBUTTON] |= cMask;
    ::SetKeyboardState(cpMapTbl);
  };


  if(false != bcIsCtrl || FALSE != rDirInfo.bIsCaptured)
  {
    //emulate click at middle of thumb in scroll bar ctrl
    POINT WndPnt(rCurrPnt);
    ::ScreenToClient(hWnd, &WndPnt);
    SendWndMsg(hWnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELONG(WndPnt.x, WndPnt.y));
  }
  else
  {
    //emulate WM_NCLBUTTONDOWN at middle of thumb
    SendWndMsg
      (
      hWnd,
      WM_NCLBUTTONDOWN,
      (false != rInfo.bHorProcess) ? HTHSCROLL : HTVSCROLL,
      MAKELONG(rCurrPnt.x, rCurrPnt.y)
      );
  };


  ////mem "additional process" wnd
  pCfgMem->hSubClassScrollWnd = (false != bcIsCtrl)
    ? ::GetParent(hWnd)
    : hWnd;

  //subclass and goto forwarding mode
  // OA this function causes Windows Vista applications to fail
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx (&osvi);
	bool bIsWindowsVistaorLater = (osvi.dwMajorVersion > 5);

	if (!bIsWindowsVistaorLater)
		pCfgMem->bMouseHookInForwardMode = false != SubClassWnd(pCfgMem->hSubClassScrollWnd);

  //store "save" distance - from mouse pnt to thumb center - for start of scroll
  CalcSaveDistance(rcPnt, rCurrPnt);
};

inline void CApp::SBScrollMoveInt(CScrollProcessInfo& rProcessInfo, CScrollProcessInfo::CScrollDirInfo& rDirInfo, const bool bcIsHor, const POINT& rcPnt)
{
  //for easy

  //call mouse shift
  const bool bcReduct = CheckRealKey(pCfgMem->common64.uiScollReductionKey);
  POINT MouseShift = rcPnt;
  SBScrollCalcShift(rProcessInfo.LastProcessLookPnt, MouseShift, rProcessInfo.LastScrollMagnMiss, bcReduct, rDirInfo.dwScrollReduction);
  int iMouseShift = (false != bcIsHor)
    ? MouseShift.x
    : MouseShift.y;
  //move direction (by InvFlag)
  if(FALSE != pCfgMem->common64.bScrollInv)
  {
    iMouseShift = -iMouseShift;
  };

  //center of thumb
  const RECT& rcWorkRect = rProcessInfo.WorkArea;
  bool bDoScroll = false;
  POINT& rThumbCenter = rProcessInfo.LastEmulateThumbPnt;
  //apply shift
  if(false != bcIsHor)
  {
    const LONG lcMem = rThumbCenter.x;
    rThumbCenter.x += iMouseShift;
    //hor scroll bar
    if(rcWorkRect.left > rThumbCenter.x)
    {
      //on left
      rThumbCenter.x = rcWorkRect.left;
    }
    else if(rcWorkRect.right < rThumbCenter.x)
    {
      //on right
      rThumbCenter.x = rcWorkRect.right;
    };
    bDoScroll = lcMem != rThumbCenter.x;
  }
  else
  {
    const LONG lcMem = rThumbCenter.y;
    rThumbCenter.y += iMouseShift;
    //vert scroll bar
    if(rcWorkRect.top > rThumbCenter.y)
    {
      //on top
      rThumbCenter.y = rcWorkRect.top;
    }
    else if(rcWorkRect.bottom < rThumbCenter.y)
    {
      //on bottom
      rThumbCenter.y = rcWorkRect.bottom;
    };
    bDoScroll = lcMem != rThumbCenter.y;
  };

  //shift significan
  if(false != bDoScroll)
  {
    //mem curr pnt as "last looked" wnd
    rProcessInfo.LastScrollPnt = rcPnt;
    //do mouse move emulation
    const HWND hcWnd = rDirInfo.hWnd;
    const bool bcIsCtrl = rDirInfo.bIsCtrl;

    if(false == bcIsCtrl)
    {
      //standard scroll bar
      //calc "specical mouse pnt"
      POINT WndPnt(rThumbCenter);
      SBScrollConvert(hcWnd, WndPnt, rDirInfo.bFlatSB);
      SendWndMsg
        (
        hcWnd,
        WM_MOUSEMOVE,
        MK_LBUTTON,
        MAKELONG(WndPnt.x, WndPnt.y)
        );
    }
    else
    {
      //scroll bar ctrl
      //calc "specical mouse pnt"
      POINT WndPnt(rThumbCenter);
      ::ScreenToClient(hcWnd, &WndPnt);
      SendWndMsg
        (
        hcWnd,
        WM_MOUSEMOVE,
        MK_LBUTTON,
        MAKELONG(WndPnt.x, WndPnt.y)
        );
    };
  }
  else
  {
    //mem only "direction corrdinate"
    if(false != bcIsHor)
    {
      rProcessInfo.LastScrollPnt.x = rcPnt.x;
    }
    else
    {
      rProcessInfo.LastScrollPnt.y = rcPnt.y;
    };
  };
};

inline void CApp::SBScrollStopInt(CScrollProcessInfo& rProcessInfo, CScrollProcessInfo::CScrollDirInfo& rDirInfo, const bool bcIsHor, const POINT& rcPnt, const bool bcAbort)
{
  //win2000 around
  if(FALSE != pCfgMem->bWin2000PatchRequired)
  {
    BYTE cpMapTbl[eccStateKeyMassSize];
    ::GetKeyboardState(cpMapTbl);
    const BYTE cMask = 1 << (CHAR_BIT - 1);
    cpMapTbl[VK_LBUTTON] &= ~cMask;
    ::SetKeyboardState(cpMapTbl);
  };

  //center of thumb
  POINT rThumbCenter = rProcessInfo.LastEmulateThumbPnt;
  //do mouse move emulation
  const HWND hcWnd = rDirInfo.hWnd;
  const bool bcIsCtrl = rDirInfo.bIsCtrl;
  if(false == bcIsCtrl)
  {
    //std scroll bar
    POINT WndPnt(rThumbCenter);
    SBScrollConvert(hcWnd, WndPnt, rDirInfo.bFlatSB);
    SendWndMsg
      (
      hcWnd,
      WM_LBUTTONUP,
      0,
      MAKELONG(WndPnt.x, WndPnt.y)
      );
    SendWndMsg
      (
      hcWnd,
      WM_NCMOUSEMOVE,
      (false != bcIsHor) ? HTHSCROLL : HTVSCROLL,
      MAKELPARAM(rThumbCenter.x, rThumbCenter.y)
      );
  }
  else
  {
    //scroll bar ctrl
    POINT WndPnt(rThumbCenter);
    ::ScreenToClient(hcWnd, &WndPnt);
    SendWndMsg
      (
      hcWnd,
      WM_LBUTTONUP,
      0,
      MAKELONG(WndPnt.x, WndPnt.y)
      );
  };

  ////send "stop forwarding" and zero hSubClassScrollWnd
  SendWndMsg(pCfgMem->hSubClassScrollWnd, uicGlbMsg, egmOffHookForwarding, 0);
  pCfgMem->hSubClassScrollWnd = 0;

  //store "saved" dist - from mouse pnt to center of thumb (last "emulate" center point)
  if(false == bcAbort)
  {
    CalcSaveDistance(rThumbCenter, rcPnt);
  };
};

inline DWORD CApp::ScrollGetCurrentReduction() const
{
  return MyMulDiv(egcMagnScrollReduction, pCfgMem->common64.dwScrollNormReduction, egcMagnScrollReduction);
};

inline DWORD CApp::SBScrollCalcAddReduct(const HWND hcWnd, const bool bcIsCtrl, const bool bcIsHor, const bool bcIsFlat) const
{
  return ScrollGetCurrentReduction();
};

inline void CApp::InstallAllFromHook()
{
  CSharedNotClearedInfo* const cpNotClearInfo = GetNotClearInfo(pCfgMem);
  if(lMemHookInstallCount != cpNotClearInfo->lHookInstallCount)
  {
    //mem
    lMemHookInstallCount = cpNotClearInfo->lHookInstallCount;
    //call install
    InstallSubClassingSendDataToHost();
  };
};


inline void CApp::SBScrollSlipChngBD(const MOUSEHOOKSTRUCT* const cpMsg)
{
  CScrollProcessInfo& rInfo = pCfgMem->ScrollProcessInfo;
  rInfo.LastProcessLookPnt = cpMsg->pt;
  rInfo.LastScrollPnt = cpMsg->pt;
  rInfo.LastScrollMagnMiss.x = 0;
  rInfo.LastScrollMagnMiss.y = 0;
};

inline void CApp::SBScrollSlipChng(const MOUSEHOOKSTRUCT* const cpMsg)
{
  SBScrollSlipChngBD(cpMsg);
};

inline bool CApp::CheckRealKey(const int icVKKey)
{
  int iUseKey = icVKKey;
  switch(icVKKey)
  {
  case VK_LBUTTON:
    iUseKey = (FALSE != ::GetSystemMetrics(SM_SWAPBUTTON))
      ? VK_RBUTTON
      : VK_LBUTTON;
    break;

  case VK_RBUTTON:
    iUseKey = (FALSE != ::GetSystemMetrics(SM_SWAPBUTTON))
      ? VK_LBUTTON
      : VK_RBUTTON;
    break;

  default:
    break;
  };

  const SHORT scRes = ::GetAsyncKeyState(iUseKey);
  const USHORT scMask = (1 << (sizeof(SHORT) * CHAR_BIT - 1));
  return 0 != (scMask & scRes);
};

inline void CApp::PagerScrollSlipChng(const MOUSEHOOKSTRUCT* const cpMsg)
{
  CScrollPagerCtrlInfo& rInfo = pCfgMem->ScrollPagerCtrlInfo;
  rInfo.PntLastScroll = cpMsg->pt;
  rInfo.LastScrollMagnMiss.x = 0;
  rInfo.LastScrollMagnMiss.y = 0;
};

inline bool CApp::IEScrollStartInt(const MOUSEHOOKSTRUCT* const cpMsg, CSCrollIEProcessInfo& rScrollInfo, const bool bcRealyFirstStart, const bool bcHorStart)
{
  //nothing in scroll - mem prm's

  rScrollInfo.LastScrollMagnMiss.x = 0;
  rScrollInfo.LastScrollMagnMiss.y = 0;
  rScrollInfo.PntLastStart = cpMsg->pt;
  rScrollInfo.PntStartScroll = cpMsg->pt;
  rScrollInfo.LastScrollPnt = cpMsg->pt;
  rScrollInfo.lRemand = 0;
  rScrollInfo.bHorScroll = bcHorStart;

  //emulate "unpress" with "eat" mouse input
  if(false != bcRealyFirstStart)
  {
    const StopMouseMsg& rcInfo = cpcStopMouseNCInfo[pCfgMem->dwMouseControlStartKey];
    pCfgMem->uiEatUpFlag |= rcInfo.uiSkipMask;
    mouse_event(rcInfo.uiUpMsg, 0, 0, 0, 0);

    //focus on IE window
    SetFocus(rScrollInfo.hWnd);
    //dbg:SafeSendMsg(rScrollInfo.hWnd, WM_SYSCOMMAND, SC_RESTORE, MAKEWPARAM(0, -1));
  };

  //calc move benefit
  {
    RECT WndRect;
    ::GetWindowRect(rScrollInfo.hWnd, &WndRect);
    POINT ClickPnt = {cpMsg->pt.x, WndRect.right};
    CalcSaveDistance(ClickPnt, cpMsg->pt);
  };

  return true;
};

inline bool CApp::IEScrollMoveInt(const MOUSEHOOKSTRUCT* const cpMsg, CSCrollIEProcessInfo& rScrollInfo)
{
  //calc dist from last "looking" to "curr"
  const bool bcReduct = CheckRealKey(pCfgMem->common64.uiScollReductionKey);
  POINT MouseShift = cpMsg->pt;
  SBScrollCalcShift(rScrollInfo.PntLastStart, MouseShift, rScrollInfo.LastScrollMagnMiss, bcReduct, egcMagnScrollReduction);

  //mouse shift
  int iMouseShift = (false != rScrollInfo.bHorScroll)
    ? MouseShift.x
    : MouseShift.y;
  if(FALSE != pCfgMem->common64.bScrollInv)
  {
    iMouseShift = -iMouseShift;
  };

  if(false == rScrollInfo.bHorScroll)
  {
    ////vert scroll throught "WHEEL"
    if(false == rScrollInfo.bLockedWheelDelta)
    {
      //ie vertical scrolling
      //calc mouse shift
      const int icCalcMouseShift = rScrollInfo.lRemand + iMouseShift * pCfgMem->common64.dwIESCrollConvMove;
      //actual wheel shift
      const int icTmpDelVal = egcIEScrollConvMoveMagn * pCfgMem->dwIEScrollMinMove;
      const int icTmpCalcVal = icCalcMouseShift / icTmpDelVal;
      const int icActualWheelShift = icTmpCalcVal * pCfgMem->dwIEScrollMinMove;
      //remander for shift
      rScrollInfo.lRemand = icCalcMouseShift - icActualWheelShift * egcIEScrollConvMoveMagn;
      //do scroll
      SafeSendMsg(rScrollInfo.hWnd, WM_MOUSEWHEEL, MAKEWPARAM(0, -icActualWheelShift), MAKELPARAM(rScrollInfo.PntStartScroll.x, rScrollInfo.PntStartScroll.y));
      rScrollInfo.LastScrollPnt = cpMsg->pt;
    }
    else
    {
      //winamp vertical scrolling

      //call mouse shift
      const bool bcReduct = CheckRealKey(pCfgMem->common64.uiScollReductionKey);
      POINT MouseShift = cpMsg->pt;
      SBScrollCalcShift
        (
        rScrollInfo.LastScrollPnt,
        MouseShift, 
        rScrollInfo.LastScrollMagnMiss, 
        CheckRealKey(pCfgMem->common64.uiScollReductionKey), 
        ScrollGetCurrentReduction()
        );
      int iMouseShift = MouseShift.y;
      //move direction (by InvFlag)
      if(FALSE != pCfgMem->common64.bScrollInv)
      {
        iMouseShift = -iMouseShift;
      };
      iMouseShift += rScrollInfo.lRemand;
      const int lcRealDeltaMult = iMouseShift / egcWheelScrollingSBPointPerDelta;
      rScrollInfo.lRemand = iMouseShift - lcRealDeltaMult * egcWheelScrollingSBPointPerDelta;
      if(0 != lcRealDeltaMult)
      {
		  // Oleg: added " / 2 " to increase scrolling precision in Office 2007 applications.
		  // " / 4 " would be the same as a infinitely stopless mouse wheel
        const LONG lcDeltaVal = WHEEL_DELTA * lcRealDeltaMult / 2;
        SafeSendMsg(rScrollInfo.hWnd, WM_MOUSEWHEEL, MAKEWPARAM(0, -lcDeltaVal), MAKELPARAM(rScrollInfo.PntStartScroll.x, rScrollInfo.PntStartScroll.y));
        rScrollInfo.LastScrollPnt = cpMsg->pt;
      };
    };
  }
  else
  {
    ////hor scroll throught "arrow clicks"

    const LONG lcUseShiftVal = rScrollInfo.lRemand + iMouseShift * pCfgMem->common64.dwIESCrollConvMove;
    const LONG lcDivVal = 20 * egcIEScrollConvMoveMagn;
    const LONG lcRealShiftVal = lcUseShiftVal / lcDivVal;
    rScrollInfo.lRemand = lcUseShiftVal - lcRealShiftVal * lcDivVal;
    
    if(0 != lcRealShiftVal)
    {
      const ULONG ulcScrollCode = (0 > lcRealShiftVal)
        ? SB_LINELEFT
        : SB_LINERIGHT;
      const LONG lcSendNumb = max(-lcRealShiftVal, lcRealShiftVal);
      for(LONG lSendCount = 0; lcSendNumb > lSendCount; lSendCount++)
      {
        SafeSendMsg(rScrollInfo.hWnd, WM_HSCROLL, MAKEWPARAM(ulcScrollCode, 0), 0);
      };
      rScrollInfo.LastScrollPnt = cpMsg->pt;
    };
  };

  return true;
};

inline bool CApp::IEScrollStopInt(const MOUSEHOOKSTRUCT* const cpMsg, CSCrollIEProcessInfo& rScrollInfo, const bool bcRealyAbort)
{
  if(false == bcRealyAbort)
  {
    //IEScrollMoveInt(cpMsg, rScrollInfo);
    //calc move benefit
    {
      RECT WndRect;
      ::GetWindowRect(rScrollInfo.hWnd, &WndRect);
      POINT ClickPnt = {cpMsg->pt.x, WndRect.right};
      CalcSaveDistance(ClickPnt, cpMsg->pt);
    };
  };
  return true;
};

inline void CApp::IEScrollSlipChng(const MOUSEHOOKSTRUCT* const cpMsg)
{
  CSCrollIEProcessInfo& rInfo = pCfgMem->ScrollIEProcessInfo;
  rInfo.PntLastStart = cpMsg->pt;
  rInfo.LastScrollMagnMiss.x = 0;
  rInfo.LastScrollMagnMiss.y = 0;
};

inline bool CApp::PagerScrollStop(const MOUSEHOOKSTRUCT* const cpMsg, const bool bcAbort)
{
  return true;
};

inline bool CApp::PagerScrollStart(const MOUSEHOOKSTRUCT* const cpMsg)
{
  //get prm's

  CScrollPagerCtrlInfo& rInfo = pCfgMem->ScrollPagerCtrlInfo;
  rInfo.LastScrollMagnMiss.x = 0;
  rInfo.LastScrollMagnMiss.y = 0;
  DWORD_PTR dwRes = 0;
  SafeSendMsg(rInfo.hPagerCtrl, PGM_GETPOS, 0, 0, &dwRes);
  rInfo.lCurrPos = dwRes;
  rInfo.lStartPos = 0;

  return true;
};

inline void CApp::MouseEventReForvarder(const MOUSEHOOKSTRUCT* const cpcInfo, const UINT uicClientEvent, const UINT uicNonClientEvent) const
{
  //try find wnd from mouse pnt
  HWND hWnd = ::WindowFromPoint(cpcInfo->pt);
  if(0 == hWnd)
  {
    hWnd = cpcInfo->hwnd;
  };
  //look on hit test for such wnd
  DWORD_PTR dwHitTestRes = WM_NCHITTEST;
  SafeSendMsg(hWnd, WM_NCHITTEST, 0, MAKELPARAM(cpcInfo->pt.x, cpcInfo->pt.y), &dwHitTestRes);
  //forward msg
  if(HTCLIENT == dwHitTestRes)
  {
    //client forward
    POINT WndPt = cpcInfo->pt;
    ::ScreenToClient(hWnd, &WndPt);
    SafeSendMsg(hWnd, uicClientEvent, pCfgMem->uiCurrMouseKeys, MAKELPARAM(WndPt.x, WndPt.y));
  }
  else
  {
    //non client forward
    SafeSendMsg(hWnd, uicNonClientEvent, dwHitTestRes, MAKELPARAM(cpcInfo->pt.x, cpcInfo->pt.y));
  };
};

#endif//__APP_H
