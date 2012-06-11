/**********************************************************************
base for property child's
**********************************************************************/

#include "stdafx.h"

#include "ChildDlg.h"

#include "Appconst.h"

#include "..\Include\MIHostInterface.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildDlg dialog


CChildDlg::CChildDlg(const UINT uicResId, CWnd* pParent)
	:CDialog(uicResId, pParent),
  dwHelpTopic(0)
{
	//{{AFX_DATA_INIT(CChildDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CChildDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChildDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BOOL CChildDlg::OnSetActive()
{
  return TRUE;
};

BOOL CChildDlg::OnKillActive()
{
  return TRUE;
};


BEGIN_MESSAGE_MAP(CChildDlg, CDialog)
	//{{AFX_MSG_MAP(CChildDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
  ON_MESSAGE(ecmGetHelpId, OnGetHelptopicId)
END_MESSAGE_MAP()

BOOL CChildDlg::OnInitDialog()
{
  CDialog::OnInitDialog();
  //retrn FALSE for new set focus to ctrl's of new created dlg
  return FALSE;
};

LRESULT CChildDlg::OnGetHelptopicId(WPARAM, LPARAM)
{
  return dwHelpTopic;
};
