/**********************************************************************
reg dlg
**********************************************************************/

#if !defined(AFX_REGDLG_H__12CD2DF6_D6CB_11D2_B1ED_00A0C907DA21__INCLUDED_)
#define AFX_REGDLG_H__12CD2DF6_D6CB_11D2_B1ED_00A0C907DA21__INCLUDED_

#pragma once


#include "resource.h"


/////////////////////////////////////////////////////////////////////////////
// CRegDlg dialog

class CRegDlg : public CDialog
{
protected:

public:
  
public:

  CRegDlg(CWnd* pParent);

  //get reg string
  inline const CString& GetKey() const;
  
protected:

protected:
	

	CString	CEdit;
// Dialog Data
	//{{AFX_DATA(CRegDlg)
	enum { IDD = IDD_REG_INFO_DLG };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegDlg)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CRegDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/**********************************************************************
implementation
**********************************************************************/


//////////////////////////////////////////////////////////////////////
//CRegDlg

inline const CString& CRegDlg::GetKey() const
{
  return CEdit;
};



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGDLG_H__12CD2DF6_D6CB_11D2_B1ED_00A0C907DA21__INCLUDED_)
