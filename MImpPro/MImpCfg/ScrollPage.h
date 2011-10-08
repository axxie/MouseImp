/**********************************************************************
scroll prm's
**********************************************************************/
#if !defined(AFX_SCROLLPAGE_H__6AB45867_8533_11D2_B0F7_00A0C907DA21__INCLUDED_)
#define AFX_SCROLLPAGE_H__6AB45867_8533_11D2_B0F7_00A0C907DA21__INCLUDED_

#pragma once

#include "resource.h"

#include "ChildDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CScrollPage dialog

class CScrollPage : public CChildDlg
{
protected:

  enum ClassConstEnum
  {
  };

public:
  
public:
  
  CScrollPage(CWnd* const cpParent);
  ~CScrollPage();

protected:

protected:

// Dialog Data
  //{{AFX_DATA(CScrollPage)
	enum { IDD = IDD_SCROLL_PAGE };
	CComboBox	ControlTypeCombo;
	CComboBox	DisableKeyCombo;
  CComboBox SmartKeyCombo;
  CComboBox CtrlKeyCombo;
	//}}AFX_DATA


// Overrides
  // ClassWizard generate virtual function overrides
  //{{AFX_VIRTUAL(CScrollPage)
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

  // Generated message map functions
  //{{AFX_MSG(CScrollPage)
  virtual BOOL OnInitDialog();
	afx_msg void OnBdScrollCheck();
	//}}AFX_MSG
  DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCROLLPAGE_H__6AB45867_8533_11D2_B0F7_00A0C907DA21__INCLUDED_)
