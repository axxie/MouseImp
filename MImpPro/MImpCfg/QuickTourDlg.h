/**********************************************************************
Quick tour dlg
**********************************************************************/

#if !defined(AFX_QUICKTOURDLG_H__9AD89DA8_ABC2_11D3_B3C7_00400535408E__INCLUDED_)
#define AFX_QUICKTOURDLG_H__9AD89DA8_ABC2_11D3_B3C7_00400535408E__INCLUDED_

#pragma once

#include "resource.h"

#include "BmpStatic.h"


/////////////////////////////////////////////////////////////////////////////
// CQuickTourDlg dialog

class CQuickTourDlg : public CDialog
{
protected:

  static const UINT cpcBmpResMass[];
  ULONG ulBmpResPos;
  CBmpStatic Bmp;
  
public:
  
public:
  
	CQuickTourDlg();

protected:

  void SyncBtns();

protected:

// Dialog Data
	//{{AFX_DATA(CQuickTourDlg)
	enum { IDD = IDD_QUICK_TOUR_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQuickTourDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CQuickTourDlg)
	afx_msg void OnNextBtn();
	afx_msg void OnPrevBtn();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUICKTOURDLG_H__9AD89DA8_ABC2_11D3_B3C7_00400535408E__INCLUDED_)
