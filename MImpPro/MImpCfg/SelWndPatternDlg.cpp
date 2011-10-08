/**********************************************************************
WndPattern finder dlg
**********************************************************************/

#include "stdafx.h"


#include "SelWndPatternDlg.h"


#include "AutoHidePatterns.h"

#include "..\\MIGlobal.h"

#include "MImpCfg.h"

#include "..\\SLibExc\\SLCfgStr.h"

#include "MImpCfgMainDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static LPCTSTR const cpcWndPosEnt = _T("FWndPos");
static const DWORD dwcWndPosVer = 2;

static const UINT cpcInitTypeCombo[] = 
{
  IDS_STR_ST_SHRINK,
  IDS_STR_ST_ACTIVATE
};

/////////////////////////////////////////////////////////////////////////////
// CSelWndPatternDlg dialog


CSelWndPatternDlg::CSelWndPatternDlg(CWnd* pParent, __CWndPatternInfo* const cpInitCtrlInfo)
	:CDialog(CSelWndPatternDlg::IDD, pParent),
  cpCtrlInfo(cpInitCtrlInfo),

  bCapturePresent(false),
  hCurrFindWnd(0),
  hLastLookingWnd(0),
  dwLastLookingProcess(0),

  bPopUpFind(false),

  dwFlashCount(0)
{
	//{{AFX_DATA_INIT(CSelWndPatternDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

static inline void GetTextCtrlInt(const HWND hcDlg, const UINT uicCtrlId, CString& rStr)
{
  const HWND hcCtrl = ::GetDlgItem(hcDlg, uicCtrlId);
  _ASSERT(0 != hcCtrl);
  //alloc buff
  const DWORD dwcLen = ::GetWindowTextLength(hcCtrl) + 1;
  LPSTR const cpBuff = rStr.GetBuffer(dwcLen);
  //copy data
  ::GetWindowText(hcCtrl, cpBuff, dwcLen);
  //release buffer
  rStr.ReleaseBuffer();
};

static inline void SyncDataInt(CDataExchange* pDX, const UINT uicEditCtrlId, const UINT uicCheckCtrlId, CString& rStr)
{
  const HWND hcDlg = pDX->m_pDlgWnd->m_hWnd;
  if(FALSE != pDX->m_bSaveAndValidate)
  {
    //data saved - get info - is check - empty string
    //else - get from edit

    //check box state
    if(BST_CHECKED == ::SendDlgItemMessage(hcDlg, uicCheckCtrlId, BM_GETCHECK, 0, 0))
    {
      //checked - clear
      rStr.Empty();
    }
    else
    {
      //not checked - get text
      GetTextCtrlInt(hcDlg, uicEditCtrlId, rStr);
    };
  }
  else
  {
    //data putted to ctrl's - enable/disable edit if string empty and check/uncheck check box
    //set edit ctrl text

    //easy
    const bool bcEmpty = 0 == ::_tcslen(rStr);
    //check/disable edit
    ::SendDlgItemMessage(hcDlg, uicCheckCtrlId, BM_SETCHECK, (false != bcEmpty) ? BST_CHECKED : BST_UNCHECKED, 0);
    ::SendDlgItemMessage(hcDlg, uicEditCtrlId, EM_SETREADONLY, false != bcEmpty, 0);
    //set text for edit ctrl
    ::SetDlgItemText(hcDlg, uicEditCtrlId, rStr);
  };
};

void CSelWndPatternDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelWndPatternDlg)
	DDX_Control(pDX, IDC_SEL_HOTKEY, SelHotKey);
	//}}AFX_DATA_MAP

  SyncDataInt(pDX, IDC_APP_EDIT, IDC_APP_NAME_ALL_CHECK, cpCtrlInfo->AppName);
  SyncDataInt(pDX, IDC_CLASS_EDIT, IDC_WND_CLASS_ALL_CHECK, cpCtrlInfo->ClassName);
  SyncDataInt(pDX, IDC_TITLE_EDIT, IDC_WND_NAME_ALL_CHECK, cpCtrlInfo->TitleName);

  //type transfer
  if(FALSE == pDX->m_bSaveAndValidate)
  {
    int iSelInd = 0;
    const DWORD dwcNumb = TypeCombo.GetCount();
    for(DWORD dwCount = 0; dwcNumb > dwCount; dwCount++)
    {
      if(TypeCombo.GetItemData(dwCount) == cpCtrlInfo->dwPattType)
      {
        iSelInd = dwCount;
        break;
      };
    };
    TypeCombo.SetCurSel(iSelInd);
  }
  else
  {
    cpCtrlInfo->dwPattType = estShrink;
    const int icInd = TypeCombo.GetCurSel();
    if(0 <= icInd)
    {
      cpCtrlInfo->dwPattType = TypeCombo.GetItemData(icInd);
    };
  };
}


BEGIN_MESSAGE_MAP(CSelWndPatternDlg, CDialog)
	//{{AFX_MSG_MAP(CSelWndPatternDlg)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_CAPTURECHANGED()
	ON_WM_KEYUP()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_APP_NAME_ALL_CHECK, OnAppNameAllCheck)
	ON_BN_CLICKED(IDC_WND_CLASS_ALL_CHECK, OnWndClassAllCheck)
	ON_BN_CLICKED(IDC_WND_NAME_ALL_CHECK, OnWndNameAllCheck)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_FIND_BTN, OnFindBtn)
	//}}AFX_MSG_MAP
  ON_CONTROL(STN_CLICKED, IDC_CAPTURE_STATIC, OnCaptureStatic)
  ON_MESSAGE(WM_HOTKEY, OnHotKey)
  ON_MESSAGE(ecmShowApp, OnShowAppMsg)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelWndPatternDlg message handlers

void CSelWndPatternDlg::OnCaptureStatic() 
{
  ////start find wnd
  //hide main wnd (if specified)
	//set capture
  //get "start wnd info"	

  //main wnd
  if(false != IsHideMainWnd())
  {
    AfxGetMainWnd()->ShowWindow(SW_HIDE);
  };

  //capture
  SetCapture();
  bCapturePresent = true;

  //parm's for selection border
  dwFlashCount = 0;

  SetTimer(eccFTimerId, eccFTimerTime, 0);

  //start
  hCurrFindWnd = 0;
  hLastLookingWnd = 0;
  dwLastLookingProcess = 0;
  //set finder icon
  SyncFinderIconWithState();
}

void CSelWndPatternDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
  if(false != bCapturePresent)
  {
    FindStopDraw();	
  }
  else
  {
	  CDialog::OnLButtonUp(nFlags, point);
  };
}

void CSelWndPatternDlg::DrawFeedBack()
{
  if(0 != hCurrFindWnd && FALSE != ::IsWindow(hCurrFindWnd))
  {
    //border size
    const DWORD dwcSelBorderWidth = ::GetSystemMetrics(SM_CXSIZEFRAME);
    const DWORD dwcSelBorderHeight = ::GetSystemMetrics(SM_CYSIZEFRAME);


    HDC hDC = ::GetWindowDC(hCurrFindWnd);
    if(0 != hDC)
    {
      RECT Rect;
      ::GetWindowRect(hCurrFindWnd, &Rect);
      Rect.right -= Rect.left + 1;
      Rect.left = 0;
      Rect.bottom -= Rect.top + 1;
      Rect.top = 0;

      //left 
      RECT DrawRect;
      DrawRect.left = Rect.left;
      DrawRect.right = DrawRect.left + dwcSelBorderWidth;
      DrawRect.bottom = Rect.bottom;
      DrawRect.top = Rect.top + dwcSelBorderHeight;
      ::InvertRect(hDC, &DrawRect);
      //top
      DrawRect.left = Rect.left;
      DrawRect.right = Rect.right - dwcSelBorderWidth;
      DrawRect.top = Rect.top;
      DrawRect.bottom = DrawRect.top + dwcSelBorderHeight;
      ::InvertRect(hDC, &DrawRect);
      //right
      DrawRect.left = Rect.right - dwcSelBorderWidth;
      DrawRect.right = Rect.right;
      DrawRect.top = Rect.top;
      DrawRect.bottom = Rect.bottom - dwcSelBorderHeight;
      ::InvertRect(hDC, &DrawRect);
      //down
      DrawRect.left = Rect.left + dwcSelBorderWidth;
      DrawRect.right = Rect.right;
      DrawRect.top = Rect.bottom - dwcSelBorderHeight;
      DrawRect.bottom = Rect.bottom;
      ::InvertRect(hDC, &DrawRect);

      ::ReleaseDC(hCurrFindWnd, hDC);
      dwFlashCount++;
    };
  };
};

void CSelWndPatternDlg::StopFeedBack()
{
  ////remove border at last wnd
  //invert border at last wnd

  if(0 != (dwFlashCount & 1))
  {
    DrawFeedBack();
  };
  dwFlashCount = 0;
};

void CSelWndPatternDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
  if(false != bCapturePresent)
  {
    //get mouse pos
    //find wnd
    //if not some as last - chng

	  POINT Pnt;
    ::GetCursorPos(&Pnt);
    const HWND hcCurrWnd = ::WindowFromPoint(Pnt);
    if(hcCurrWnd != hLastLookingWnd)
    {
      hLastLookingWnd = hcCurrWnd;
      //try drill up for "AutoHideWindow"
      HWND hFindWnd = 0;
      if(false != AHideDrillForWindow(hLastLookingWnd, hFindWnd) && hFindWnd != hCurrFindWnd)
      {
        //stop feedback on old
        StopFeedBack();
        //chng new
        hCurrFindWnd = hFindWnd;
        //get data
        SyncDataForFindWnd();
        DrawFeedBack();
      };
    };
    //set cursor 
    ::SetCursor(AfxGetApp()->LoadCursor(IDC_WND_FIND_CUR));
  }
  else
  {
    CDialog::OnMouseMove(nFlags, point);
  };
}

void CSelWndPatternDlg::SyncDataForFindWnd()
{
  //wnd info
  //app

  //get "wnd process"
  DWORD dwWndProcess = 0;
  ::GetWindowThreadProcessId(hCurrFindWnd, &dwWndProcess);
  if(dwLastLookingProcess != dwWndProcess)
  {
    dwLastLookingProcess = dwWndProcess;
    //zero current
    CMISharedInfo& rInfo = *(theApp.pCfgMem);
    *(rInfo.cpGetAppNameResultBuff) = '\0';
    ::SendMessage(rInfo.hMainHostWnd, emcGetAppNameById, 0, dwWndProcess);
    cpCtrlInfo->AppName = rInfo.cpGetAppNameResultBuff;
  };
  //title
  const LONG lcTitleLen = ::GetWindowTextLength(hCurrFindWnd) + 1;
  LPTSTR const cpTitleBuff = cpCtrlInfo->TitleName.GetBuffer(lcTitleLen);
  ::GetWindowText(hCurrFindWnd, cpTitleBuff, lcTitleLen);
  cpCtrlInfo->TitleName.ReleaseBuffer();
  //class
  TCHAR cpClass[egcWndClassTextLen];
  SLCHECK(0 != ::GetClassName(hCurrFindWnd, cpClass, COUNTOF(cpClass)));
  cpCtrlInfo->ClassName = cpClass;
  //update
  UpdateData(FALSE);
};

BOOL CSelWndPatternDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
  BOOL bRes = TRUE;
  if(false != bCapturePresent)
  {
    ::SetCursor(AfxGetApp()->LoadCursor(IDC_WND_FIND_CUR));
  }
  else
  {
    bRes = CDialog::OnSetCursor(pWnd, nHitTest, message);
  }
  return bRes;
}

void CSelWndPatternDlg::OnCaptureChanged(CWnd *pWnd) 
{
  if(false != bCapturePresent)
  {
    FindStopDraw();
  };
	
	CDialog::OnCaptureChanged(pWnd);
}

void CSelWndPatternDlg::SyncFinderIconWithState()
{
  HICON hIcon = AfxGetApp()->LoadIcon((false == bCapturePresent) ? IDI_WND_FIND_ICON : IDI_WND_EMPTY_ICON);
  _ASSERT(0 != hIcon);
  SendDlgItemMessage(IDC_CAPTURE_STATIC, STM_SETICON, reinterpret_cast<WPARAM>(hIcon));
};

void CSelWndPatternDlg::FindStopDraw()
{
  //show wnd
  if(false != IsHideMainWnd())
  {
    AfxGetMainWnd()->ShowWindow(SW_SHOW);
  };
  //capture
  ReleaseCapture();
  bCapturePresent = false;
  //stop draw
  StopFeedBack();
  //restore icon img
  SyncFinderIconWithState();
  ////kill flash timer
  KillTimer(eccFTimerId);
};

void CSelWndPatternDlg::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
  bool bCatch = false;
  switch(nChar)
  {
  case VK_ESCAPE:
    if(false != bCapturePresent)
    {
      bCatch = true;
      FindStopDraw();
    };
    break;

  default:
    break;
  };	

  if(false == bCatch)
  {
    CDialog::OnKeyUp(nChar, nRepCnt, nFlags);
  };
}

void CSelWndPatternDlg::OnDestroy() 
{
  //pos and cfg
  //stop feedback
  //destroy icon's
  //destroy

  //cfg
  {
    using sl::CSLOutCfgStr;
    CSLOutCfgStr Cfg(true, cpcCfgKeyName);
    Cfg.SetEnt(cpcWndPosEnt, dwcWndPosVer);
    Cfg.SaveWndPos(m_hWnd);
    WORD wKey = 0;
    WORD wModifer = 0;
    SelHotKey.GetHotKey(wKey, wModifer);
    Cfg
      << SendDlgItemMessage(IDC_HIDE_CHECK, BM_GETCHECK, 0, 0)
      << wKey;
  };


  //feedback
  FindStopDraw();

  //stop popup find
  StopPopUpFind(true);

  //destroy
	CDialog::OnDestroy();
}

BOOL CSelWndPatternDlg::OnInitDialog() 
{
  //type combo
  TypeCombo.SubclassDlgItem(IDC_TYPE_COMBO, this);
  for(DWORD dwCount = 0; COUNTOF(cpcInitTypeCombo) > dwCount; dwCount++)
  {
    CString Str;
    SLCHECK(FALSE != Str.LoadString(cpcInitTypeCombo[dwCount]));
    const int icInd = TypeCombo.AddString(Str);
    TypeCombo.SetItemData(icInd, dwCount);
  };

	const BOOL bcRes = CDialog::OnInitDialog();
	
  //sync finder icon
  SyncFinderIconWithState();

  //rules for hot key
  SelHotKey.SetRules(HKCOMB_A | HKCOMB_C | HKCOMB_CA | HKCOMB_S | HKCOMB_SA | HKCOMB_SC | HKCOMB_SCA, 0);
	
  //pos and cfg
  {
    using sl::CSLInCfgStr;
    CSLInCfgStr Cfg(true, cpcCfgKeyName);
    Cfg.SetEnt(cpcWndPosEnt, dwcWndPosVer);
    Cfg.LoadWndPos(m_hWnd);
    UINT uiCheck = BST_CHECKED;
    WORD wHotKey = VK_F11;
    Cfg
      >> uiCheck
      >> wHotKey;
    SelHotKey.SetHotKey(wHotKey, 0);
    SendDlgItemMessage(IDC_HIDE_CHECK, BM_SETCHECK, uiCheck, 0);
  };


	return bcRes;
}

static inline void SyncCheckAndEditState(const HWND hcDlg, const UINT uicCheck, const UINT uicEdit)
{
  //get state of check and enable/disable edit

  const bool bcIsCheck = BST_CHECKED == ::SendDlgItemMessage(hcDlg, uicCheck, BM_GETCHECK, 0, 0);
  ::SendDlgItemMessage(hcDlg, uicEdit, EM_SETREADONLY, false != bcIsCheck, 0);
};

void CSelWndPatternDlg::OnAppNameAllCheck() 
{
	SyncCheckAndEditState(m_hWnd, IDC_APP_NAME_ALL_CHECK, IDC_APP_EDIT);
}

void CSelWndPatternDlg::OnWndClassAllCheck() 
{
	SyncCheckAndEditState(m_hWnd, IDC_WND_CLASS_ALL_CHECK, IDC_CLASS_EDIT);
}

void CSelWndPatternDlg::OnWndNameAllCheck() 
{
	SyncCheckAndEditState(m_hWnd, IDC_WND_NAME_ALL_CHECK, IDC_TITLE_EDIT);
}

void CSelWndPatternDlg::OnTimer(UINT nIDEvent) 
{
  switch(nIDEvent)
  {
  case eccPopUpTimerId:
    if(eccPopUpFlashNumber >= dwFlashCount)
    {
      //flash wnd
      DrawFeedBack();
    }
    else
    {
      //flash complete - stop popup find
      StopPopUpFind(false);
    };
    break;

  case eccFTimerId:
    if(eccFlashNumb >= dwFlashCount)
    {
      DrawFeedBack();
    };
    break;

  default:
    CDialog::OnTimer(nIDEvent);
    break;
  };
}

void CSelWndPatternDlg::OnFindBtn() 
{
  bPopUpFind = false;
  //register hot key's
  WORD wHotKey = 0;
  WORD wModifer = 0;
  SelHotKey.GetHotKey(wHotKey, wModifer);

  DWORD dwScanCode = ::MapVirtualKey(wHotKey, 0);
  if(0 != dwScanCode)
  {
    //show info to user
    if(false == theApp.bPopUpInstShowed)
    {
      TCHAR cpKeyName[100];
      ::GetKeyNameText(dwScanCode << 16, cpKeyName, COUNTOF(cpKeyName));
      //get key text
      CString Str;
      Str.Format(IDS_MSG_POP_FIND_INSTR, cpKeyName);
      AfxMessageBox(Str, MB_OK | MB_ICONINFORMATION);
      theApp.bPopUpInstShowed = true;
    };

    //try reg hot key
    if(FALSE != ::RegisterHotKey(m_hWnd, eccWndCatchHotKeyId, 0, wHotKey))
    {
      bPopUpFind = true;
      PopUpHide();
    };
  };
  if(false == bPopUpFind)
  {
    AfxMessageBox(IDS_MSG_REG_HOTKEY_FAILED, MB_OK | MB_ICONERROR);
  };
};

//show/hidw wnd helper's - FALSE == lParam if show wnd
static BOOL CALLBACK HideWndProc(HWND hWnd, LPARAM lParam)
{
  DWORD dwWndProc = 0;
  ::GetWindowThreadProcessId(hWnd, &dwWndProc);
  if(::GetCurrentProcessId() == dwWndProc)
  {
    //show/hide
    ::ShowWindow(hWnd, (FALSE != lParam) ? SW_HIDE : SW_SHOW);
  };
  return TRUE;
};

void CSelWndPatternDlg::PopFindShow()
{
  static_cast<CMImpCfgMainDlg*>(AfxGetMainWnd())->WndAllShow();
  ActivateWndToUser(m_hWnd);
};

void CSelWndPatternDlg::PopUpHide()
{
  static_cast<CMImpCfgMainDlg*>(AfxGetMainWnd())->WndAllHide();
};

void CSelWndPatternDlg::StopPopUpFind(const bool bcOnlyStop)
{
  if(false != bPopUpFind)
  {
    bPopUpFind = false;
    //stop timer
    KillTimer(eccPopUpTimerId);
    //unreg hot key
    ::UnregisterHotKey(m_hWnd, eccWndCatchHotKeyId);
    //show wnd
    PopFindShow();
    //sync data
    if(false == bcOnlyStop)
    {
      SyncDataForFindWnd();
    };
  };
};

LPARAM CSelWndPatternDlg::OnHotKey(WPARAM wParam, LPARAM lParam)
{
  switch(wParam)
  {
  case eccWndCatchHotKeyId:
    ////start timer proc for select wnd under cursor
    if(false != bPopUpFind)
    {
      POINT Pnt;
      ::GetCursorPos(&Pnt);
      const HWND hcFromWnd = ::WindowFromPoint(Pnt);
      if(0 != hcFromWnd && false != AHideDrillForWindow(hcFromWnd, hCurrFindWnd))
      {
        dwFlashCount = 0;
        SetTimer(eccPopUpTimerId, eccPopUpTimerTime, 0);
      }
      else
      {
        hCurrFindWnd = 0;
        StopPopUpFind(false);
      };
    };
    break;

  default:
    Default();
    break;
  };

  return 0;
};

LPARAM CSelWndPatternDlg::OnShowAppMsg(WPARAM wParam, LPARAM lParam)
{
  //stop popup find
  StopPopUpFind(true);
  return 0;
};
