/**********************************************************************
Main cfg dialog
**********************************************************************/

#include "stdafx.h"


#include "MImpCfgMainDlg.h"


#include "MImpCfg.h"

#include "..\\SlibExc\\SLDlgMem.h"

#include "..\\SlibExc\\SLResMem.h"

#include "..\\SlibExc\\SLThunk.h"

#include "ScrollPage.h"

#include "General.h"

#include "AutoHidePatterns.h"

#include "OdometerDlg.h"

#include "..\\SLibExc\\SLCfgStr.h"

#include "..\\SLibExc\\SLCfgReg.h"

#include "..\\SLibExc\\SLAutoPtr.h"

#include "AppConst.h"

#include "Aboutpage.h"

#include "RegDlg.h"

//help topi's
namespace help
{
  #include "..\\help\\MIPROHLP.H"
};



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static LPCTSTR const cpcWndPosEnt = _T("CWndPos");
static const DWORD dwcWndPosVer = 0;


//////////////////////////////////////////////////////////////////////
//global section

struct __CURLInitInfo
{
  LPCTSTR pcUrl;
  UINT uiRes;
};

//mass of logo bmp's
const static __CURLInitInfo cpcLogoBmpMass[] = 
{
  {cpcURLHomePage, IDR_LOGO1_PNG},
  {cpcURLHomePage, IDR_LOGO2_PNG},
};


//perform scale of passed wnd (and it ctrl's)
static void ScaleWnd(const HWND hcWnd, const float fcScale)
{
  //scan wnd ctrl's and scale it's size 
  //scale wnd

  if(1.0 != fcScale)
  {
    //wnd
    CRect WndRect;
    GetWindowRect(hcWnd, &WndRect);
    const LONG lcWidth = static_cast<LONG>(static_cast<float>(WndRect.Width()) * fcScale);
    const LONG lcHeigt = static_cast<LONG>(static_cast<float>(WndRect.Height()) * fcScale);
    ::SetWindowPos(hcWnd, 0, 0, 0, lcWidth, lcHeigt, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
    //ctrl's 
    for(HWND hCtrl = ::GetWindow(hcWnd, GW_CHILD); 0 != hCtrl; hCtrl = ::GetWindow(hCtrl, GW_HWNDNEXT))
    {
      CRect WndRect;
      ::GetWindowRect(hCtrl, &WndRect);
      POINT* const cpPntPtr = reinterpret_cast<POINT*>(&WndRect);
      ::ScreenToClient(hcWnd, cpPntPtr);
      ::ScreenToClient(hcWnd, cpPntPtr + 1);
      //appply scale
      WndRect.bottom = static_cast<LONG>(static_cast<float>(WndRect.bottom) * fcScale);
      WndRect.top = static_cast<LONG>(static_cast<float>(WndRect.top) * fcScale);
      WndRect.left = static_cast<LONG>(static_cast<float>(WndRect.left) * fcScale);
      WndRect.right = static_cast<LONG>(static_cast<float>(WndRect.right) * fcScale);
      //chng 
      ::SetWindowPos(hCtrl, 0, WndRect.left, WndRect.top, WndRect.Width(), WndRect.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
    };
  };
};

/////////////////////////////////////////////////////////////////////////////
// CMImpCfgMainDlg dialog

CMImpCfgMainDlg::CMImpCfgMainDlg()
	: CDialog(CMImpCfgMainDlg::IDD, 0),
  fScale(1.0),
  dwCurrChngLogo(0),
  LogoBmp(true),
  TitleBmp(false)
{
	//{{AFX_DATA_INIT(CMImpCfgMainDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CMImpCfgMainDlg::ChngLogoProcess()
{
  //get current bmp and set to LogoBmp
  //increment and process bmp index chng

  //curr bmp
  const UINT uicBmpId = cpcLogoBmpMass[dwCurrChngLogo].uiRes;
  LogoBmp.LoadBmpFromRes(uicBmpId);
  LogoBmp.SetUrl(cpcLogoBmpMass[dwCurrChngLogo].pcUrl);

  //chng 
  dwCurrChngLogo++;
  dwCurrChngLogo %= COUNTOF(cpcLogoBmpMass);
};

void CMImpCfgMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMImpCfgMainDlg)
	DDX_Control(pDX, IDC_CHILD_TAB, ChildTab);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMImpCfgMainDlg, CDialog)
	//{{AFX_MSG_MAP(CMImpCfgMainDlg)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY(TCN_SELCHANGING, IDC_CHILD_TAB, OnSelchangingChildTab)
	ON_NOTIFY(TCN_SELCHANGE, IDC_CHILD_TAB, OnSelchangeChildTab)
	ON_BN_CLICKED(IDAPPLY, OnApply)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
  ON_MESSAGE(ecmGetHelpId, OnGetHelptopicId)
  ON_MESSAGE(ecmShowApp, OnActivateApp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMImpCfgMainDlg message handlers

BOOL CMImpCfgMainDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

  //init and set bmp's for logo and title
  SLCHECK(FALSE != LogoBmp.SubclassDlgItem(IDC_LOGO_BMP_STATIC, this));
  SLCHECK(FALSE != TitleBmp.SubclassDlgItem(IDC_TITLE_LOGO_STATIC, this));
  TitleBmp.LoadBmpFromRes(IDR_TITLE_LOGO_PNG);
  //first bmp
  ChngLogoProcess();

  //get size of logo bmp and calc scale factor
  {
    //bmp size
    const CSize BmpSize(LogoBmp.GetBmpSize());
    //bmp holder size
    CRect HolderRect;
    LogoBmp.GetClientRect(&HolderRect);
    const CSize BmpHolderSize(HolderRect.Size());
    //factor
    const float fcHScale = static_cast<float>(BmpSize.cx) / static_cast<float>(BmpHolderSize.cx);
    const float fcVScale = static_cast<float>(BmpSize.cy) / static_cast<float>(BmpHolderSize.cy);
    fScale = max(fcHScale, fcVScale);
  };

  //scale "this" dlg
  ScaleWnd(m_hWnd, fScale);

  //init "chng logo" timer
  SLCHECK(eccLogoChngTimerId == SetTimer(eccLogoChngTimerId, eccLogoChngTimerTime, 0));

	
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
  {
    HICON hBig = ::LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME)); 
    _ASSERT(0 != hBig);
    SetIcon(hBig, TRUE);
    HICON hSmall = reinterpret_cast<HICON>(::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 16, 16, LR_SHARED));
    _ASSERT(0 != hSmall);
    SetIcon(hSmall, FALSE);
  };


  //child's
  {
    using namespace help;
    InsertDlg(new COdometerDlg(this), Productivity);
    InsertDlg(new CScrollPage(this), DirectScroll);
    InsertDlg(new CAutoShrinkPatterns(this), AutoShrink);
    InsertDlg(new CGeneralPage(this), Options);
    InsertDlg(new CAboutPage(this), Contact);
  };

  //sel first dlg in child tab
  LRESULT lRes = 0;
  OnSelchangeChildTab(0, &lRes);

  //load dlg pos
  {
    using sl::CSLInCfgStr;
    CSLInCfgStr Cfg(true, cpcCfgKeyName);
    Cfg.SetEnt(cpcWndPosEnt, dwcWndPosVer);
    Cfg.LoadWndPos(m_hWnd);
  };

  //reg info
  ReadRegInfo();
      
	
	return TRUE;
}

void CMImpCfgMainDlg::InsertDlg(CChildDlg* const cpDlg, const DWORD dwcHelpId)
{
  TCITEM Item;
  Item.mask = TCIF_PARAM | TCIF_TEXT;
  Item.lParam = reinterpret_cast<LPARAM>(cpDlg);

  using sl::CSLResMem;
  CSLResMem ResMem;
  ResMem.Load(AfxGetResourceHandle(), cpDlg->GetResId(), RT_DIALOG);

  using sl::CSLDlgTmpl;
  CSLDlgTmpl Tmpl;
  Tmpl.Attach(ResMem.GetPtr());

  CString Title(Tmpl.GetTitle());
  Item.pszText = Title.LockBuffer();

  //insert
  ChildTab.InsertItem(ChildTab.GetItemCount(), &Item);

  //set help id
  cpDlg->SetHelpTopicId(dwcHelpId);
};

int CMImpCfgMainDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  //create
  //put to shared mem

  //creaet
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

  //put
  theApp.pCfgMem->hCfgAppWnd = m_hWnd;
	
	return 0;
}

void CMImpCfgMainDlg::OnDestroy() 
{
  //save wnd pos
  //remove from shared mem 
  //delete all child's 
  //kill "ChngLogo" timer
  //parent destroy

  //pos
  {
    using sl::CSLOutCfgStr;
    CSLOutCfgStr Cfg(true, cpcCfgKeyName);
    Cfg.SetEnt(cpcWndPosEnt, dwcWndPosVer);
    Cfg.SaveWndPos(m_hWnd);
  };

  //clear
  theApp.pCfgMem->hCfgAppWnd = 0;	

  //child's
  {
    const int icNumb = ChildTab.GetItemCount();
    for(int iCount = 0; icNumb > iCount; iCount++)
    {
      TCITEM Item;
      Item.mask = TCIF_PARAM;
      ChildTab.GetItem(iCount, &Item);
      CChildDlg* const cpDlg = reinterpret_cast<CChildDlg*>(Item.lParam);
      //destroy wnd
      if(FALSE != ::IsWindow(cpDlg->m_hWnd))
      {
        cpDlg->DestroyWindow();
      };
      delete cpDlg;
    };
  };

  //timer
  KillTimer(eccLogoChngTimerId);

  //destroy
	CDialog::OnDestroy();	
}

bool CMImpCfgMainDlg::GetSelDlg(CChildDlg*& rpDlg)
{
  bool bRes = false;

  const int icSel = ChildTab.GetCurSel();
  if(0 <= icSel)
  {
    TCITEM Item;
    Item.mask = TCIF_PARAM;
    ChildTab.GetItem(icSel, &Item);
    rpDlg = reinterpret_cast<CChildDlg*>(Item.lParam);
    bRes = true;
  };

  return bRes;
};

void CMImpCfgMainDlg::OnSelchangingChildTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
  CChildDlg* pDlg = 0;
  if(false != GetSelDlg(pDlg))
  {
    pDlg->ShowWindow(SW_HIDE);
    pDlg->OnKillActive();
  };
	
	*pResult = 0;
}

void CMImpCfgMainDlg::OnSelchangeChildTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
  CChildDlg* pDlg = 0;
  if(false != GetSelDlg(pDlg))
  {
    //look "is present"
    if(FALSE == ::IsWindow(pDlg->m_hWnd))
    {
      SLCHECK(FALSE != pDlg->Create(this));

      pDlg->ModifyStyle
        (
        WS_BORDER | WS_CAPTION | DS_MODALFRAME | WS_POPUP | WS_DLGFRAME | WS_OVERLAPPED | WS_SYSMENU, 
        DS_CONTROL | WS_CHILD, 
        0
        );
      pDlg->ModifyStyleEx
        (
        WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME,
        0,
        0
        );
      pDlg->SetParent(this);
      CRect Rect;
      pDlg->GetWindowRect(&Rect);
      //move child to 
      Rect.OffsetRect(-Rect.left, -Rect.top);
      ChildTab.AdjustRect(FALSE, &Rect);

      //from tab to main dlg
      ChildTab.MapWindowPoints(this, &Rect);

      //get wnd before TabCtrl
      CWnd* const cpBeforeWnd = ChildTab.GetWindow(GW_HWNDPREV);
      pDlg->SetWindowPos
        (
        (0 == cpBeforeWnd) ? &wndTop : cpBeforeWnd, 
        Rect.left, 
        Rect.top, 
        Rect.Width(), 
        Rect.Height(), 
        SWP_FRAMECHANGED | SWP_SHOWWINDOW
        );

      //scale wnd
      ScaleWnd(pDlg->m_hWnd, fScale);
    }
    else
    {
      //show if present
      pDlg->ShowWindow(SW_SHOW);
    };
    pDlg->OnSetActive();
  };

  //set focus to first control
  HWND hCtrl = ::GetNextDlgTabItem(pDlg->m_hWnd, 0, FALSE);
  if(0 == hCtrl)
  {
    hCtrl = ::GetNextDlgTabItem(m_hWnd, 0, FALSE);
  };
  ::SetFocus(hCtrl);

	*pResult = 0;
}

void CMImpCfgMainDlg::OnOK()
{
  //save data first (apply)
  //call parent

  //apply
  OnApply();

  //parent
  CDialog::OnOK();
};

void CMImpCfgMainDlg::OnApply() 
{
  //send to host app "save cfg" message
  //scan all dlg's and transfer data (if dlg present)

  const int icNumb = ChildTab.GetItemCount();
  for(int iCount = 0; icNumb > iCount; iCount++)
  {
    TCITEM Item;
    Item.mask = TCIF_PARAM;
    SLCHECK(FALSE != ChildTab.GetItem(iCount, &Item));
    CChildDlg* const cpDlg = reinterpret_cast<CChildDlg*>(Item.lParam);
    if(FALSE != ::IsWindow(cpDlg->m_hWnd))
    {
      //dlg present =transfer data
      cpDlg->UpdateData(TRUE);
    };
  };
  //save
  ::SendMessage(theApp.pCfgMem->hMainHostWnd, emcNeedSaveCfg, 0, 0);
}

void CMImpCfgMainDlg::OnTimer(UINT nIDEvent) 
{
  switch(nIDEvent)
  {
    //"chng logo" timer
  case eccLogoChngTimerId:
    ChngLogoProcess();
    break;

  default:
    CDialog::OnTimer(nIDEvent);
    break;
  };
}

LRESULT CMImpCfgMainDlg::OnGetHelptopicId(WPARAM, LPARAM)
{
  //try get current selected dlg
  //if ok - return help id from dlg, else - help for this page


  LRESULT lRes = 0;
  CChildDlg* pDlg = 0;
  if(false != GetSelDlg(pDlg))
  {
    lRes = pDlg->GetHelpTopicId();
  };

  return lRes;
};


void CMImpCfgMainDlg::PageNext(const bool bcPrev)
{
  //calc new page index (based on bcPrev) and ajust it
  //send TCN_SELCHANGE for dlg
  //if result ok - chng sel and send TCN_SELCHANGE is "sel chng" result

  //new index
  int iNewInd = ChildTab.GetCurSel();
  if(false != bcPrev)
  {
    //prev
    iNewInd--;
    if(0 > iNewInd)
    {
      iNewInd = ChildTab.GetItemCount() - 1;
    };
  }
  else
  {
    //forvard
    iNewInd++;
    if(ChildTab.GetItemCount() <= iNewInd)
    {
      iNewInd = 0;
    };
  };


  //fill notify info
  NMHDR Info;
  Info.hwndFrom = ChildTab.m_hWnd;
  Info.idFrom = ChildTab.GetDlgCtrlID();
  //notify
  Info.code = TCN_SELCHANGING;
  if(FALSE == SendMessage(WM_NOTIFY, Info.idFrom, reinterpret_cast<LPARAM>(&Info)))
  {
    //chng selection
    ChildTab.SetCurSel(iNewInd);
    //post selection process
    Info.code = TCN_SELCHANGE;
    SendMessage(WM_NOTIFY, Info.idFrom, reinterpret_cast<LPARAM>(&Info));
  };
};

BOOL CMImpCfgMainDlg::PreTranslateMessage(MSG* pMsg)
{
	//try pretranslate message from tab ctrl
  //if it's page down/page up - forward to Next/Prev page

  //easy
  bool bCatch = true;

  //pretranslate
  switch(pMsg->message)
  {
  case WM_KEYDOWN:
    bCatch = false;
    if(0 > ::GetKeyState(VK_CONTROL))
    {
      switch(pMsg->wParam)
      {
        //page up
      case VK_PRIOR:
        PageNext(true);
        bCatch = true;
        break;

      case VK_NEXT:
        PageNext(false);
        bCatch = true;
        break;

      default:
        break;
      };
    };
    break;

  default:
    bCatch = false;
    break;
  };

	
	return (false != bCatch)
    ? TRUE
    : CDialog::PreTranslateMessage(pMsg);
};

//send message helper
static BOOL CALLBACK NotifyWndEnumProc(HWND hWnd, LPARAM lParam)
{
  const HWND hcAvoidWnd = reinterpret_cast<HWND>(lParam);
  if(hcAvoidWnd != hWnd)
  {
    ::SendMessage(hWnd, ecmShowApp, 0, 0);
  };
  return TRUE;
};

LRESULT CMImpCfgMainDlg::OnActivateApp(WPARAM, LPARAM)
{
  ::EnumThreadWindows(::GetCurrentThreadId(), NotifyWndEnumProc, reinterpret_cast<LPARAM>(m_hWnd));
  ActivateWndToUser(m_hWnd);
  return 0;
};

void CMImpCfgMainDlg::ReadRegInfo()
{
};

void CMImpCfgMainDlg::OnRegInfo() 
{
}

BOOL CMImpCfgMainDlg::WndAllHideHelper(HWND hcWnd, LPARAM lParam)
{
  //if wnd currently shows - try hide
  const LONG lcStyle = ::GetWindowLong(hcWnd, GWL_STYLE);
  if(WS_POPUP == (WS_POPUP & lcStyle) && FALSE != ::IsWindowVisible(hcWnd))
  {
    HideWndStack.Push(hcWnd);
    ::ShowWindow(hcWnd, SW_HIDE);
  };
  return TRUE;
};

void CMImpCfgMainDlg::WndAllHide()
{
  //enu all wnd's, hide and push it's to "mem list
  using sl::CSLThunk;
  typedef CSLThunk<CMImpCfgMainDlg> ThunkType;
  ThunkType Thunk;
  Thunk.InitThunk(reinterpret_cast<ThunkType::TMFP>(&CMImpCfgMainDlg::WndAllHideHelper), this);
  ::EnumThreadWindows(::GetCurrentThreadId(), reinterpret_cast<WNDENUMPROC>(Thunk.GetThunk()), 0);
};
