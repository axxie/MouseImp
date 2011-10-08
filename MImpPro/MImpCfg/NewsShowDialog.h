/**********************************************************************
Newsdialog
**********************************************************************/

#ifndef __NEWSSHOWDLG_H
#define __NEWSSHOWDLG_H

#pragma once

#include "resource.h"


//////////////////////////////////////////////////////////////////////
//CNewsShowDlg

class CNewsShowDlg : public CDialog
{
protected:
  
public:

  CString NewsText;
  CString NewsUrl;
  
public:

  inline CNewsShowDlg(CWnd* const cpInitParent);
  
protected:

protected:

  virtual void DoDataExchange(CDataExchange* pDX);
  afx_msg void GoBtnClicked();
  DECLARE_MESSAGE_MAP()  
};


/**********************************************************************
implementation
**********************************************************************/


//////////////////////////////////////////////////////////////////////
//CNewsShowDlg

inline CNewsShowDlg::CNewsShowDlg(CWnd* const cpInitParent)
  :CDialog(IDD_SHOW_NEWS_DLG, cpInitParent)
{
};



#endif//__NEWSSHOWDLG_H