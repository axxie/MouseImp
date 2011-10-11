/**********************************************************************
AboutDlg
**********************************************************************/

#include "stdafx.h"


#include "General.h"


#include "..\Include\MIGlobal.h"

#include "MImpCfg.h"

#include <RegStr.h>

#include "..\\SlibExc\\SLCfgReg.h"

#include "..\\Slibwin\\SLWDisablers.h"


//tray icon click combo init data
using slw::__CSLWListBoxInitData;
static const __CSLWListBoxInitData cpcTrayIconComboInitData[] =
{
  {IDS_STR_TICON_CMODE_NONE, etiTrayIconClickLast},
  {IDS_STR_TICON_CMODE_DIRECTSCROLL, etiTrayIconClickSwitchDirectScroll},
  {IDS_STR_TICON_CMODE_AUTOHIDE, etiTrayIconClickSwitchAutoHide},
};

/////////////////////////////////////////////////////////////////////////////
// CGeneralPage property page

CGeneralPage::CGeneralPage(CWnd* const cpParent) 
  :CChildDlg(CGeneralPage::IDD, cpParent)
{
	//{{AFX_DATA_INIT(CGeneralPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CGeneralPage::~CGeneralPage()
{
}

static inline void SyncTIconCfg(const HWND hcWnd)
{
  using slw::EnableCtrl;
  const BOOL bcEnabled = BST_UNCHECKED != ::IsDlgButtonChecked(hcWnd, IDC_TICON_ENABLE_CHECK);
  EnableCtrl(hcWnd, IDC_TICON_ENABLE_ANIM_CHECK, FALSE != bcEnabled);
  EnableCtrl(hcWnd, IDC_TRAY_CLICK_COMBO, FALSE != bcEnabled);
};

void CGeneralPage::DoDataExchange(CDataExchange* pDX)
{
	CChildDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGeneralPage)
	//}}AFX_DATA_MAP

  //info
  CMISharedInfo* const cpInfo = theApp.pCfgMem;

  //tray anim
  if(false == theApp.CheckRegBoolCheckBox(m_hWnd, IDC_TICON_ENABLE_CHECK, cpInfo->bEnableTrayIcon, TRUE))
  {
    DDX_Check(pDX, IDC_TICON_ENABLE_CHECK, cpInfo->bEnableTrayIcon);
  };
  //tray icon animation
  DDX_Check(pDX, IDC_TICON_ENABLE_ANIM_CHECK, cpInfo->bEnableTrayIconAnim);
  SyncTIconCfg(m_hWnd);
  //splash icon
  if(false == theApp.CheckRegBoolCheckBox(m_hWnd, IDC_SPLASH_SHOW_CHECK, cpInfo->bShowSplash, TRUE))
  {
    DDX_Check(pDX, IDC_SPLASH_SHOW_CHECK, cpInfo->bShowSplash);
  };

  //load on wndstart
  {
    using sl::CSLCfgReg;
    CSLCfgReg Reg;
    if(false != Reg.Open(REGSTR_PATH_RUN, false))
    {
      if(FALSE != pDX->m_bSaveAndValidate)
      {
        //save - if checked - save in Run path for Host app
        //else - remove key
        if(BST_CHECKED == SendDlgItemMessage(IDC_RUN_AT_START_CHECK, BM_GETCHECK))
        {
          //save
          //make path to host app
          TCHAR cpHostName[MAX_PATH];
          GetFullBasedName(cpcHostAppName, cpHostName, AfxGetInstanceHandle());
          Reg.WriteStr(cpcRunEntryName, cpHostName, -1);
        }
        else
        {
          //remove
          Reg.DelVal(cpcRunEntryName);
        };
      }
      else
      {
        //load - look on "key exist"
        //try find val
        DWORD dwSize = 0;
        //set check box
        SendDlgItemMessage(IDC_RUN_AT_START_CHECK, BM_SETCHECK, (false != Reg.IsPresent(cpcRunEntryName)) ? BST_CHECKED : BST_UNCHECKED);
      };
    };
  };

  //tray icon mode
  if(FALSE != pDX->m_bSaveAndValidate)
  {
    //save
    using slw::SLWComboControlGetSel;
    SLWComboControlGetSel(m_hWnd, IDC_TRAY_CLICK_COMBO, theApp.pCfgMem->dwTrayIconClickMode);
  }
  else
  {
    //load
    using slw::SLWComboControlInit;
    SLWComboControlInit(AfxGetResourceHandle(), m_hWnd, IDC_TRAY_CLICK_COMBO, theApp.pCfgMem->dwTrayIconClickMode, cpcTrayIconComboInitData, COUNTOF(cpcTrayIconComboInitData));
  };
}


BEGIN_MESSAGE_MAP(CGeneralPage, CChildDlg)
	//{{AFX_MSG_MAP(CGeneralPage)
	ON_BN_CLICKED(IDC_SPLASH_SHOW_CHECK, OnSplashShowCheck)
	ON_BN_CLICKED(IDC_TICON_ENABLE_CHECK, OnTrayIconCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGeneralPage message handlers

void CGeneralPage::OnSplashShowCheck() 
{
	theApp.CheckRegValueCheckBox(m_hWnd, IDC_SPLASH_SHOW_CHECK);
}

void CGeneralPage::OnTrayIconCheck() 
{
	theApp.CheckRegValueCheckBox(m_hWnd, IDC_TICON_ENABLE_CHECK);
  SyncTIconCfg(m_hWnd);
}

