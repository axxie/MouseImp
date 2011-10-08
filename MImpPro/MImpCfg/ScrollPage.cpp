/**********************************************************************
scroll prm's
**********************************************************************/

#include "stdafx.h"


#include "ScrollPage.h"


#include "AppConst.h"

#include "MImpCfg.h"

#include "..\\SlibExc\\SLDbg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


struct __CInitComboStr
{
  UINT uiResStr;
  UINT uiKey;
};

static const __CInitComboStr cpcInitKeyMass[] = 
{
  {IDS_STR_KEY_LBUTTON, MK_LBUTTON},
  {IDS_STR_KEY_RBUTTON, MK_RBUTTON},
  {IDS_STR_KEY_MBUTTON, MK_MBUTTON}
};

static const __CInitComboStr cpcInitSmartKeyMass[] = 
{
  {IDS_STR_KEY_LBUTTON, VK_LBUTTON},
  {IDS_STR_KEY_RBUTTON, VK_RBUTTON},
  {IDS_STR_KEY_MBUTTON, VK_MBUTTON},
  {IDS_STR_KEY_SHIFT, VK_SHIFT},
  {IDS_STR_KEY_ALT, VK_MENU},
  {IDS_STR_KEY_CTRL, VK_CONTROL}
};

static const __CInitComboStr cpcInitDisableKeyMass[] =
{
  {IDS_STR_KEY_SHIFT, VK_SHIFT},
  {IDS_STR_KEY_ALT, VK_MENU},
  {IDS_STR_KEY_CTRL, VK_CONTROL}
};

//control types
static const __CInitComboStr cpcInitControlType[] = 
{
  {IDS_STR_SCR_CTYPE_STANDARD, escStandard},
  {IDS_STR_SCR_CTYPE_TOGGLE, escToggleWithEat},
  {IDS_STR_SCR_CTYPE_MIX, escScrollModeMix},
};


/////////////////////////////////////////////////////////////////////////////
// CScrollPage property page

CScrollPage::CScrollPage(CWnd* const cpParent) : CChildDlg(CScrollPage::IDD, cpParent)
{
  //{{AFX_DATA_INIT(CScrollPage)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}

CScrollPage::~CScrollPage()
{
}

void CScrollPage::DoDataExchange(CDataExchange* pDX)
{
  CChildDlg::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CScrollPage)
	DDX_Control(pDX, IDC_CONTROL_TYPE_COMBO, ControlTypeCombo);
	DDX_Control(pDX, IDC_DISABLE_KEY_COMBO, DisableKeyCombo);
  DDX_Control(pDX, IDC_REDUCT_KEY_COMBO, SmartKeyCombo);
  DDX_Control(pDX, IDC_CTRL_KEY_COMBO, CtrlKeyCombo);
	//}}AFX_DATA_MAP

  //direct transfer
  CMISharedInfo* const cpInfo = theApp.pCfgMem;

  //scroll inversion 
  DDX_Check(pDX, IDC_INV_CHECK, cpInfo->bScrollInv);
  //"use cursor"
  DDX_Check(pDX, IDC_USE_CURSOR_CHECK, cpInfo->bCursorVisualise);
  //bidirection
  if(false == theApp.CheckRegBoolCheckBox(m_hWnd, IDC_BD_SCROLL_CHECK, cpInfo->bBDScrollingEnabled, FALSE))
  {
    DDX_Check(pDX, IDC_BD_SCROLL_CHECK, cpInfo->bBDScrollingEnabled);
  };

  //reduction
  if(FALSE != pDX->m_bSaveAndValidate)
  {
    //save reduction
    {
      const DWORD dwcPsRedVal = min(max(GetDlgItemInt(IDC_RED_EDIT, 0, FALSE), egcMarginScrollReductionPsMin), egcMarginScrollReductionPsMax);
      //to cfg
      const DWORD dwcSetVal = (0 != dwcPsRedVal)
        ? egcNumberPerCentValue * egcMagnScrollReduction / dwcPsRedVal
        : 0;
      cpInfo->lScrollReduction = dwcSetVal;
    };
    //normal reduction
    {
      const DWORD dwcPsRedVal = min(max(GetDlgItemInt(IDC_NORM_RED_EDIT, 0, FALSE), egcMarginScrollReductionPsMin), egcMarginScrollReductionPsMax);
      //to cfg
      const DWORD dwcSetVal = dwcPsRedVal * egcMagnScrollReduction / egcNumberPerCentValue;
      cpInfo->dwScrollNormReduction = dwcSetVal;
    };
    //IE move sens
    {
      const DWORD dwcPSMoveSensVal = min(max(GetDlgItemInt(IDC_IE_MOVE_SENS_EDIT, 0, FALSE), egcIEScrollConvMoveMin), egcIEScrollConvMoveMax);
      const DWORD dwcMoveSensVal = (0 != egcNumberPerCentValue)
        ? dwcPSMoveSensVal * egcIEScrollConvMoveMagn / egcNumberPerCentValue
        : 0;
      cpInfo->dwIESCrollConvMove = dwcMoveSensVal;
    };
  }
  else
  {
    {
      //range for spin
      SendDlgItemMessage(IDC_RED_SPIN, UDM_SETRANGE, 0, MAKELONG(egcMarginScrollReductionPsMax, egcMarginScrollReductionPsMin));
      //load reduction
      const DWORD dwRedPsVal = (0 == cpInfo->lScrollReduction)
        ? 0
        : egcNumberPerCentValue * egcMagnScrollReduction / cpInfo->lScrollReduction;
      SetDlgItemInt(IDC_RED_EDIT, min(max(dwRedPsVal, egcMarginScrollReductionPsMin), egcMarginScrollReductionPsMax), FALSE);
    };
    //normal reduction
    {
      SendDlgItemMessage(IDC_NORM_RED_SPIN, UDM_SETRANGE, 0, MAKELONG(egcMarginScrollReductionPsMax, egcMarginScrollReductionPsMin));
      const DWORD dwRedPsVal = egcNumberPerCentValue * cpInfo->dwScrollNormReduction / egcMagnScrollReduction;
      SetDlgItemInt(IDC_NORM_RED_EDIT, min(max(dwRedPsVal, egcMarginScrollReductionPsMin), egcMarginScrollReductionPsMax), FALSE);
    };
    {
      //IE move sens
      SendDlgItemMessage(IDC_IE_MOVE_SENS_SPIN, UDM_SETRANGE, 0, MAKELONG(egcIEScrollConvMoveMax, egcIEScrollConvMoveMin));
      //load 
      const DWORD dwcMoveSendPs = (0 == cpInfo->dwIESCrollConvMove)
        ? 0
        : cpInfo->dwIESCrollConvMove * egcNumberPerCentValue / egcIEScrollConvMoveMagn;
      SetDlgItemInt(IDC_IE_MOVE_SENS_EDIT, min(max(dwcMoveSendPs, egcIEScrollConvMoveMin), egcIEScrollConvMoveMax));
    };
  };

  //more difficult transfer
  if(FALSE != pDX->m_bSaveAndValidate)
  {
    //save

    //ctrl key
    const int icKeyInd = CtrlKeyCombo.GetCurSel();
    if(0 <= icKeyInd)
    {
      cpInfo->uiValidMouseKeys = CtrlKeyCombo.GetItemData(icKeyInd);
    };
    //smart key
    const int icSmartKeyInd = SmartKeyCombo.GetCurSel();
    if(0 <= icSmartKeyInd)
    {
      cpInfo->uiScollReductionKey = SmartKeyCombo.GetItemData(icSmartKeyInd);
    };
    //disble key
    const int icDisKeyInd = DisableKeyCombo.GetCurSel();
    if(0 <= icDisKeyInd)
    {
      cpInfo->uiDirectScrollTmpDisableKey = DisableKeyCombo.GetItemData(icDisKeyInd);
    };
    //control type
    cpInfo->bScrollDisabled = BST_CHECKED == IsDlgButtonChecked(IDC_ENABLED_CHECK);
    const int icControlTypeInd = ControlTypeCombo.GetCurSel();
    if(0 <= icControlTypeInd)
    {
      cpInfo->dwControlType = ControlTypeCombo.GetItemData(icControlTypeInd);
    };
  }
  else
  {
    //load

    //ctrl kwy
    {
      const int icNumb = CtrlKeyCombo.GetCount();
      for(int iCount = 0; icNumb > iCount; iCount++)
      {
        if(CtrlKeyCombo.GetItemData(iCount) == cpInfo->uiValidMouseKeys)
        {
          CtrlKeyCombo.SetCurSel(iCount);
          break;
        };
      };
    };
    //smart kwy
    {
      const int icNumb = SmartKeyCombo.GetCount();
      for(int iCount = 0; icNumb > iCount; iCount++)
      {
        if(SmartKeyCombo.GetItemData(iCount) == cpInfo->uiScollReductionKey)
        {
          SmartKeyCombo.SetCurSel(iCount);
          break;
        };
      };
    };
    //disable key
    {
      const int icNumb = DisableKeyCombo.GetCount();
      for(int iCount = 0; icNumb > iCount; iCount++)
      {
        if(DisableKeyCombo.GetItemData(iCount) == cpInfo->uiDirectScrollTmpDisableKey)
        {
          DisableKeyCombo.SetCurSel(iCount);
          break;
        };
      };
    };
    //control type
    CheckDlgButton(IDC_ENABLED_CHECK, (FALSE != cpInfo->bScrollDisabled) ? BST_CHECKED : BST_UNCHECKED);
    {
      const int icNumb = ControlTypeCombo.GetCount();
      for(int iCount = 0; icNumb > iCount; iCount++)
      {
        if(ControlTypeCombo.GetItemData(iCount) == cpInfo->dwControlType)
        {
          ControlTypeCombo.SetCurSel(iCount);
          break;
        };
      };
    };
  };
}


BEGIN_MESSAGE_MAP(CScrollPage, CChildDlg)
  //{{AFX_MSG_MAP(CScrollPage)
	ON_BN_CLICKED(IDC_BD_SCROLL_CHECK, OnBdScrollCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScrollPage message handlers

BOOL CScrollPage::OnInitDialog() 
{
  //parent
  //init ctrl key's combo and img lsit
  //init smart key and it's img list

  //parent
  const BOOL bcRes = CChildDlg::OnInitDialog();

  //ctrl key's
  {
    for(DWORD dwCount = 0; COUNTOF(cpcInitKeyMass) > dwCount; dwCount++)
    {
      //fill
      CString Str;
      SLCHECK(FALSE != Str.LoadString(cpcInitKeyMass[dwCount].uiResStr));
      const int icInd = CtrlKeyCombo.InsertString(-1, Str);
      CtrlKeyCombo.SetItemData(icInd, cpcInitKeyMass[dwCount].uiKey);
    };
  };

  //smart key's
  {
    for(DWORD dwCount = 0; COUNTOF(cpcInitSmartKeyMass) > dwCount; dwCount++)
    {
      CString Str;
      SLCHECK(FALSE != Str.LoadString(cpcInitSmartKeyMass[dwCount].uiResStr));
      const int icInd = SmartKeyCombo.InsertString(-1, Str);
      SmartKeyCombo.SetItemData(icInd, cpcInitSmartKeyMass[dwCount].uiKey);
    };
  };

  //disble key
  {
    for(DWORD dwCount = 0; COUNTOF(cpcInitDisableKeyMass) > dwCount; dwCount++)
    {
      CString Str;
      SLCHECK(FALSE != Str.LoadString(cpcInitDisableKeyMass[dwCount].uiResStr));
      const int icInd = DisableKeyCombo.InsertString(-1, Str);
      DisableKeyCombo.SetItemData(icInd, cpcInitDisableKeyMass[dwCount].uiKey);
    };
  };

  //control type
  {
    for(DWORD dwCount = 0; COUNTOF(cpcInitControlType) > dwCount; dwCount++)
    {
      CString Str;
      SLCHECK(FALSE != Str.LoadString(cpcInitControlType[dwCount].uiResStr));
      const int icInd = ControlTypeCombo.InsertString(-1, Str);
      ControlTypeCombo.SetItemData(icInd, cpcInitControlType[dwCount].uiKey);
    };
  };
  
  //transfer
  UpdateData(FALSE);

  return bcRes;
}


void CScrollPage::OnBdScrollCheck() 
{
	theApp.CheckRegValueCheckBox(m_hWnd, IDC_BD_SCROLL_CHECK);	
}
