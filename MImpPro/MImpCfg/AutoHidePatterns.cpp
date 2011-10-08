/**********************************************************************
WndPattern cfg dlg
**********************************************************************/

#include "stdafx.h"


#include "AutoHidePatterns.h"


#include "MImpCfg.h"

#include "..\\SlibExc\\SLAutoPtr.h"

#include "SelWndPatternDlg.h"

#include "..\\Slibwin\\SLWDisablers.h"

#include "..\\SLibExc\\SLCfgReg.h"

#include "AppConst.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


enum ColConstEnum
{
  eccAppCol = 0,
  eccClassCol,
  eccTitleCol
};

static const CListColInfo cpcInitListInfo[] = 
{
  {IDS_COL_APP, eccAppCol, 100},
  {IDS_COL_CLASS, eccClassCol, 200},
  {IDS_COL_TITLE, eccTitleCol, 100},
  {-1, -1, -1}
};

/////////////////////////////////////////////////////////////////////////////
// CAutoShrinkPatterns property page


CAutoShrinkPatterns::CAutoShrinkPatterns(CWnd* const cpParent) 
  :CChildDlg(CAutoShrinkPatterns::IDD, cpParent),
  bSomePatternsChnged(false),
  bEnableChnged(false)
{
	//{{AFX_DATA_INIT(CAutoShrinkPatterns)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CAutoShrinkPatterns::~CAutoShrinkPatterns()
{
}

void CAutoShrinkPatterns::DoDataExchange(CDataExchange* pDX)
{
	CChildDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoShrinkPatterns)
	DDX_Control(pDX, IDC_WND_PAT_LIST, WndPatList);
	//}}AFX_DATA_MAP

  //easy
  CMISharedInfo* const cpInfo = theApp.pCfgMem;

  //disabled
  DDX_Check(pDX, IDC_ENABLED_CHECK, cpInfo->bAHideEnabled);

  //hide time
  if(FALSE != pDX->m_bSaveAndValidate)
  {
    //save
    float fVal = 0;
    DDX_Text(pDX, IDC_HIDE_TIME_EDIT, fVal);
    const ULONG ulcRealVal = static_cast<ULONG>(fVal * egcAutoHideTimerMagnifer);
    cpInfo->dwAutoHideTimerTime = (0 == ulcRealVal)
      ? 0
      : min(egcMaxAutoHideTimerTime, max(egcMinAutoHideTimerTime, ulcRealVal));
  }
  else
  {
    //load
    float fVal = static_cast<float>(cpInfo->dwAutoHideTimerTime) / static_cast<float>(egcAutoHideTimerMagnifer);
    DDX_Text(pDX, IDC_HIDE_TIME_EDIT, fVal);
    SendDlgItemMessage(IDC_HIDE_TIME_SPIN, UDM_SETRANGE, 0, MAKELONG(egcMaxAutoHideTimerTime / egcAutoHideTimerMagnifer, egcMinAutoHideTimerTime / egcAutoHideTimerMagnifer));
  };
  ////open time
  if(FALSE != pDX->m_bSaveAndValidate)
  {
    ////save
    if(BST_CHECKED != IsDlgButtonChecked(IDC_DELAY_OPEN_CHECK))
    {
      cpInfo->dwAutoOpenTimerTime = 0;
    }
    else
    {
      float fVal = 0;
      DDX_Text(pDX, IDC_DELAY_OPEN_EDIT, fVal);
      cpInfo->dwAutoOpenTimerTime = min(egcMaxAutoHideTimerTime, max(egcMinAutoHideTimerTime, static_cast<DWORD>(fVal * egcAutoHideTimerMagnifer)));
    };
  }
  else
  {
    ////load
    const DWORD dwcVal = min(egcMaxAutoHideTimerTime, max(egcMinAutoHideTimerTime, cpInfo->dwAutoOpenTimerTime));
    float fVal = static_cast<float>(dwcVal) / static_cast<float>(egcAutoHideTimerMagnifer);
    DDX_Text(pDX, IDC_DELAY_OPEN_EDIT, fVal);
    SendDlgItemMessage(IDC_DELAY_OPEN_SPIN, UDM_SETRANGE, 0, MAKELONG(egcMaxAutoHideTimerTime / egcAutoHideTimerMagnifer, egcMinAutoHideTimerTime / egcAutoHideTimerMagnifer));
    CheckDlgButton(IDC_DELAY_OPEN_CHECK, (0 != cpInfo->dwAutoOpenTimerTime) ? BST_CHECKED : BST_UNCHECKED);
    SyncOpenCtrls();
  };
  //wnd pattern's
  if(FALSE != pDX->m_bSaveAndValidate)
  {
    //save
    RegSaveData();
  }
  else
  {
    //load - from InitDlg only (list ctrl not inited yet)
    //RegLoadData();
  };
}

void CAutoShrinkPatterns::SyncOpenCtrls()
{
  const BOOL bcChecked = (BST_CHECKED == IsDlgButtonChecked(IDC_DELAY_OPEN_CHECK));
  using slw::EnableCtrl;
  EnableCtrl(m_hWnd, IDC_DELAY_OPEN_EDIT, bcChecked);
  EnableCtrl(m_hWnd, IDC_DELAY_OPEN_SPIN, bcChecked);
};


BEGIN_MESSAGE_MAP(CAutoShrinkPatterns, CChildDlg)
	//{{AFX_MSG_MAP(CAutoShrinkPatterns)
	ON_BN_CLICKED(IDC_DEL_BTN, OnDelBtn)
	ON_BN_CLICKED(IDC_INS_BTN, OnInsBtn)
	ON_NOTIFY(LVN_DELETEITEM, IDC_WND_PAT_LIST, OnDeleteitemWndPatList)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_WND_PAT_LIST, OnGetdispinfoWndPatList)
	ON_NOTIFY(NM_DBLCLK, IDC_WND_PAT_LIST, OnDblclkWndPatList)
	ON_BN_CLICKED(IDC_EDIT_BTN, OnEditBtn)
	ON_BN_CLICKED(IDC_ENABLED_CHECK, OnEnabledCheck)
	ON_NOTIFY(LVN_KEYDOWN, IDC_WND_PAT_LIST, OnKeydownWndPatList)
	ON_BN_CLICKED(IDC_DELAY_OPEN_CHECK, OnDelayOpenCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoShrinkPatterns message handlers

BOOL CAutoShrinkPatterns::OnInitDialog() 
{
	const BOOL bcRes = CChildDlg::OnInitDialog();

  //list ctrl
  SLCHECK(FALSE != ShrinkType.Create(IDB_PATT_TYPE_BMP, eicShirnkTypeWirth, 0, eicShirnkTypeNull));
  WndPatList.SetImageList(&ShrinkType, LVSIL_SMALL);
  WndPatList.Init(cpcInitListInfo);

  //try load pattern's from registry
  RegLoadData();

  //sync edit btn's state
  SyncEditBtns(true);

  return bcRes;
}

void CAutoShrinkPatterns::OnDelBtn() 
{
	//get sel
  //if valid - remove

  //sel
  const int icSel = WndPatList.GetNextItem(-1, LVNI_FOCUSED);
  if(0 <= icSel)
  {
    WndPatList.DeleteItem(icSel);
    //select item after deleted
    const int icItemNumb = WndPatList.GetItemCount();
    if(0 < icItemNumb)
    {
      WndPatList.SetItemState(min(icItemNumb - 1, icSel), LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
    };
    bSomePatternsChnged = true;
    SyncEditBtns(true);
  };	
}

void CAutoShrinkPatterns::OnInsBtn() 
{
	//new data
  //call "edit" dlg
  //if ok - insert

  __CWndPatternInfo Info;
  CSelWndPatternDlg Dlg(this, &Info);
  if(IDOK == Dlg.DoModal())
  {
    const bool bcFirstItem = 0 == WndPatList.GetItemCount();
    InsertPattern(Info.dwPattType, Info.AppName, Info.ClassName, Info.TitleName);
    bSomePatternsChnged = true;
    SyncEditBtns(true);
  };	
}

void CAutoShrinkPatterns::InsertPattern(const DWORD dwcType, const CString& rcApp, const CString& rcClass, const CString& rcTitle, const int icInd, const bool bcIsChng)
{
  const bool bcNew = (0 > icInd);

  LVITEM Item;
  Item.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
  Item.iItem = WndPatList.GetItemCount();
  __CWndPatternInfo* const cpItem = (false != bcNew)
    ? ListInfoMgr.New()
    : reinterpret_cast<__CWndPatternInfo*>(WndPatList.GetItemData(icInd));
  cpItem->AppName = rcApp;
  cpItem->ClassName = rcClass;
  cpItem->TitleName = rcTitle;
  cpItem->dwPattType = dwcType;
  Item.lParam = reinterpret_cast<LPARAM>(cpItem);

  //app
  Item.iImage = I_IMAGECALLBACK;
  Item.iSubItem = eccAppCol;
  Item.pszText = LPSTR_TEXTCALLBACK;
  if(false != bcNew)
  {
    Item.iItem = WndPatList.InsertItem(&Item);
  }
  else
  {
    WndPatList.SetItem(&Item);
  };
  //class 
  Item.mask = LVIF_TEXT;
  Item.iSubItem = eccClassCol;
  Item.pszText = LPSTR_TEXTCALLBACK;
  WndPatList.SetItem(&Item);
  //Title 
  Item.mask = LVIF_TEXT;
  Item.iSubItem = eccTitleCol;
  Item.pszText = LPSTR_TEXTCALLBACK;
  WndPatList.SetItem(&Item);
  //chgn and sel item
  if(false != bcIsChng)
  {
    bSomePatternsChnged = true;
    SyncEditBtns(true);
  };
};

void CAutoShrinkPatterns::ReadCfgEntryAndInsert(const DWORD dwcInd)
{
  using sl::CSLCfgReg;
  CSLCfgReg Reg;
  if(false != Reg.Open(cpcCfgKeyName, true))
  {
    //entry name
    TCHAR cpBuff[20];
    wsprintf(cpBuff, cpcCfgWPatternsEntryMask, dwcInd);
    //try open entry val
    DWORD dwEntryLen = 0;
    if(false != Reg.GetValSize(cpBuff, dwEntryLen))
    {
      //alloc buffer
      using sl::CSLAutoPtr;
      CSLAutoPtr<CHAR> pData = new CHAR[dwEntryLen + 1];
      //try get data
      if(false != Reg.ReadStr(cpBuff, pData.GetPtr(), dwEntryLen))
      {
        //insert to list
        CString App;
        CString Class;
        CString Title;
        CString PattType;
        AfxExtractSubString(App, pData, 0, '\n');
        AfxExtractSubString(Class, pData, 1, '\n');
        AfxExtractSubString(Title, pData, 2, '\n');
        AfxExtractSubString(PattType, pData, 3, '\n');
        InsertPattern(::atoi(PattType), App, Class, Title);
      };
    };
  };
};

void CAutoShrinkPatterns::RegLoadData()
{
  //free already
  //try load from registry and insert to list

  //open cfg reg key
  using sl::CSLCfgReg;
  CSLCfgReg Reg;
  if(false != Reg.Open(cpcCfgKeyName, true))
  {
    //number of pattern's
    DWORD dwNumb = 0;
    Reg.GetVal(cpcCfgWPatternsMainName, dwNumb, 0);
    if(0 != dwNumb)
    {
      //iterate
      for(DWORD dwCount = 0; dwNumb > dwCount; dwCount++)
      {
        ReadCfgEntryAndInsert(dwCount);
      };
    };
  };
};

void CAutoShrinkPatterns::RegSaveData()
{
  //mem "old" reg entry count (for later "delete")
  //save "base" entry
  //save each entry
  //delete "old" - base on "old count" and "new count"
  //notify host wnd

  if(false != bSomePatternsChnged || false != bEnableChnged)
  {
    //save WndPat's only if chnged
    if(false != bSomePatternsChnged)
    {
      using sl::CSLCfgReg;
      CSLCfgReg Reg;
      bool bRes = Reg.Open(cpcCfgKeyName, true);
      //mem old count
      DWORD dwOldCount = 0;
      Reg.GetVal(cpcCfgWPatternsMainName, dwOldCount, 0);
      //base entry
      const DWORD dwNumb = WndPatList.GetItemCount();
      if(false != bRes)
      {
        bRes = Reg.PutVal(cpcCfgWPatternsMainName, dwNumb);
      };

      //"each"
      if(false != bRes)
      {
        //each entry
        for(DWORD dwCount = 0; dwNumb > dwCount; dwCount++)
        {
          //entry name
          TCHAR cpBuff[20];
          wsprintf(cpBuff, cpcCfgWPatternsEntryMask, dwCount);
          //entry val
          const __CWndPatternInfo* const cpcSaveInfo = reinterpret_cast<__CWndPatternInfo*>(WndPatList.GetItemData(dwCount));
          CString Str;
          Str.Format(_T("%s\n%s\n%s\n%d"), cpcSaveInfo->AppName, cpcSaveInfo->ClassName, cpcSaveInfo->TitleName, cpcSaveInfo->dwPattType);
          //save
          Reg.PutVal(cpBuff, Str, _tcslen(Str));
        };
      };

      //delete old
      for(DWORD dwDelCount = dwNumb; dwOldCount > dwDelCount; dwDelCount++)
      {
        TCHAR cpBuff[20];
        wsprintf(cpBuff, cpcCfgWPatternsEntryMask, dwDelCount);
        Reg.DelVal(cpBuff);
      };
    };

    //info to host
    CMISharedInfo& rInfo = *(theApp.pCfgMem);
    ::PostMessage(rInfo.hMainHostWnd, emcReReadWndPattList, 0, false != bSomePatternsChnged);
  };
  bSomePatternsChnged = false;
  bEnableChnged = true;
};

void CAutoShrinkPatterns::OnDeleteitemWndPatList(NMHDR* pNMHDR, LRESULT* pResult) 
{
  NMLISTVIEW* const cpInfo = reinterpret_cast<NMLISTVIEW*>(pNMHDR);

  ListInfoMgr.Delete(reinterpret_cast<__CWndPatternInfo*>(cpInfo->lParam));
	
	*pResult = 0;
}

void CAutoShrinkPatterns::OnGetdispinfoWndPatList(NMHDR* pNMHDR, LRESULT* pResult) 
{
  NMLVDISPINFO* const cpInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

  LVITEM& rItem = cpInfo->item;
  const __CWndPatternInfo* const cpcItemInfo = reinterpret_cast<__CWndPatternInfo*>(rItem.lParam);

  if(0 != (LVIF_TEXT & rItem.mask))
  {
    switch(rItem.iSubItem)
    {
    case eccAppCol:
      _tcsncpy(rItem.pszText, cpcItemInfo->AppName, rItem.cchTextMax);
      break;

    case eccClassCol:
      _tcsncpy(rItem.pszText, cpcItemInfo->ClassName, rItem.cchTextMax);
      break;

    case eccTitleCol:
      _tcsncpy(rItem.pszText, cpcItemInfo->TitleName, rItem.cchTextMax);
      break;

    default:
      //unknown column
      _ASSERT(FALSE);
      break;
    };
  };
  if(0 != (LVIF_IMAGE & rItem.mask))
  {
    switch(cpcItemInfo->dwPattType)
    {
    case estShrink:
    default:
      rItem.iImage = eiiSShrink;
      break;

    case estActivateWnd:
      rItem.iImage = eiiSActivateOnly;
      break;
    };
  };

  //if empty string providede - load mask from res's
  if(0 == ::_tcslen(rItem.pszText))
  {
    SLCHECK(0 != ::LoadString(AfxGetResourceHandle(), IDS_STR_MASK_ALL, rItem.pszText, rItem.cchTextMax));
  };
	
	*pResult = 0;
}

void CAutoShrinkPatterns::OnDblclkWndPatList(NMHDR* pNMHDR, LRESULT* pResult) 
{
  OnEditBtn();
  *pResult = 0;
}

void CAutoShrinkPatterns::OnEditBtn() 
{
	const int icSel = WndPatList.GetNextItem(-1, LVNI_FOCUSED);
  if(0 <= icSel)
  {
    __CWndPatternInfo Info(*reinterpret_cast<__CWndPatternInfo*>(WndPatList.GetItemData(icSel)));
    CSelWndPatternDlg Dlg(this, &Info);
    if(IDOK == Dlg.DoModal())
    {
      InsertPattern(Info.dwPattType, Info.AppName, Info.ClassName, Info.TitleName, icSel);
      bSomePatternsChnged = true;
      //refresh in list view ctrl
      WndPatList.Update(icSel);
    };	
  };
}

void CAutoShrinkPatterns::SyncEditBtns(const bool bcSelIfNone)
{
  int iInd = WndPatList.GetNextItem(-1, LVNI_FOCUSED);
  //if not current selection find - select first
  if(0 < WndPatList.GetItemCount() && false != bcSelIfNone && 0 > iInd)
  {
    WndPatList.SetItemState(0, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
    iInd = 0;
  };
  const BOOL bcSetFocus = 0 <= iInd;
  using slw::EnableCtrl;
  EnableCtrl(m_hWnd, IDC_DEL_BTN, bcSetFocus);
  EnableCtrl(m_hWnd, IDC_EDIT_BTN, bcSetFocus);
};

void CAutoShrinkPatterns::OnEnabledCheck() 
{
  bEnableChnged	= true;
}

void CAutoShrinkPatterns::OnKeydownWndPatList(NMHDR* pNMHDR, LRESULT* pResult) 
{
  const NMLVKEYDOWN* const cpcInfo = reinterpret_cast<const NMLVKEYDOWN*>(pNMHDR);

  switch(cpcInfo->wVKey)
  {
  case VK_DELETE:
    OnDelBtn();
    break;

  default:
    break;
  };
	
	*pResult = 0;
}

void CAutoShrinkPatterns::OnDelayOpenCheck() 
{
	SyncOpenCtrls();	
}
