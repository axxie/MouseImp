/**********************************************************************
Newsdialog
**********************************************************************/

#include "StdAfx.h"


#include "NewsShowDialog.h"


#include "ClickStatic.h"

#include "..\\SlibWin\\SLWDisablers.h"


//////////////////////////////////////////////////////////////////////
//CNewsShowDlg

void CNewsShowDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  if(FALSE == pDX->m_bSaveAndValidate)
  {
    //load
    SetDlgItemText(IDC_NEWS_EDIT, NewsText);
    if(FALSE != NewsText.IsEmpty())
    {
      using slw::EnableCtrl;
      EnableCtrl(m_hWnd, IDC_GO_BTN, false);
    };
  };
};

BEGIN_MESSAGE_MAP(CNewsShowDlg, CDialog)
  ON_BN_CLICKED(IDC_GO_BTN, GoBtnClicked)
END_MESSAGE_MAP()

void CNewsShowDlg::GoBtnClicked()
{
  ////go url
  CClickStatic::GoToUrl(NewsUrl, SW_SHOW);

  ////close dlg
  OnOK();
};
