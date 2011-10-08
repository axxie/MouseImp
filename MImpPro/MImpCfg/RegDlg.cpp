/**********************************************************************
reg dlg
**********************************************************************/

#include "stdafx.h"


#include "RegDlg.h"




#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegDlg dialog


CRegDlg::CRegDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRegDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRegDlg)
	//}}AFX_DATA_INIT
}


void CRegDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegDlg)
	//}}AFX_DATA_MAP
  if(FALSE != pDX->m_bSaveAndValidate)
  {
  	DDX_Text(pDX, IDC_KEY_EDIT, CEdit);
  };
}


BEGIN_MESSAGE_MAP(CRegDlg, CDialog)
	//{{AFX_MSG_MAP(CRegDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

