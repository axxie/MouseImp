/**********************************************************************
odometer dlg
**********************************************************************/

#if !defined(AFX_ODOMETERDLG_H__6418CC26_8A74_11D2_B101_00A0C907DA21__INCLUDED_)
#define AFX_ODOMETERDLG_H__6418CC26_8A74_11D2_B101_00A0C907DA21__INCLUDED_

#pragma once

#include "resource.h"

#include "ChildDlg.h"


/////////////////////////////////////////////////////////////////////////////
// COdometerDlg dialog

class COdometerDlg : public CChildDlg
{
protected:

  enum ClassConstEnum
  {
    //metric const's
    eccMilimetersInCantimeter = 10,
    eccCantimeterInMeter = 100,
    eccMetersInKilometer = 1000
  };
  
public:
  
public:

  COdometerDlg(CWnd* pParent);

  //refresh data on activation
  virtual BOOL OnSetActive();

  
protected:

  //build string for "distanse"
  CString BuildDistString(const LONGLONG lcDist) const;

protected:

// Dialog Data
	//{{AFX_DATA(COdometerDlg)
	enum { IDD = IDD_ODOMETER_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COdometerDlg)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(COdometerDlg)
	afx_msg void OnReset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ODOMETERDLG_H__6418CC26_8A74_11D2_B101_00A0C907DA21__INCLUDED_)
