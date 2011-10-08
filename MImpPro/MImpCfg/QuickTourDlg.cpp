/**********************************************************************
Quick tour dlg
**********************************************************************/


#include "stdafx.h"


#include "QuickTourDlg.h"


#include "..\\SlibExc\\SLUtil.h"

#include "..\\SLibExc\\SLDbg.h"



/////////////////////////////////////////////////////////////////////////////
// CQuickTourDlg dialog

const UINT CQuickTourDlg::cpcBmpResMass[] = 
{
  IDR_QUICK0_PNG,
  IDR_QUICK1_PNG,
  IDR_QUICK2_PNG,
  IDR_QUICK3_PNG,
  IDR_QUICK4_PNG,
  IDR_QUICK5_PNG,
  IDR_QUICK6_PNG,
};

CQuickTourDlg::CQuickTourDlg()
	:CDialog(CQuickTourDlg::IDD, 0),
  ulBmpResPos(0),
  Bmp(false)
{
	//{{AFX_DATA_INIT(CQuickTourDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CQuickTourDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQuickTourDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQuickTourDlg, CDialog)
	//{{AFX_MSG_MAP(CQuickTourDlg)
	ON_BN_CLICKED(IDC_NEXT_BTN, OnNextBtn)
	ON_BN_CLICKED(IDC_PREV_BTN, OnPrevBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQuickTourDlg message handlers

void CQuickTourDlg::SyncBtns()
{
  _ASSERT(COUNTOF(cpcBmpResMass) > ulBmpResPos);
  Bmp.LoadBmpFromRes(cpcBmpResMass[ulBmpResPos]);

  HWND hCtrl = 0;
  GetDlgItem(IDC_PREV_BTN, &hCtrl);
  ::EnableWindow(hCtrl, 0 < ulBmpResPos);
  GetDlgItem(IDC_NEXT_BTN, &hCtrl);
  ::EnableWindow(hCtrl, ulBmpResPos + 1 < COUNTOF(cpcBmpResMass));
};

void CQuickTourDlg::OnNextBtn() 
{
	ulBmpResPos++;
  SyncBtns();
}

void CQuickTourDlg::OnPrevBtn() 
{
	ulBmpResPos--;
  SyncBtns();
}

BOOL CQuickTourDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

  SLCHECK(FALSE != Bmp.SubclassDlgItem(IDC_IMG_STATIC, this));
	SyncBtns();
	
	return TRUE;
}
