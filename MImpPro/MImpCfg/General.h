/**********************************************************************
AboutDlg
**********************************************************************/

#ifndef __GENERAL_H
#define __GENERAL_H

#pragma once

#include "resource.h"

#include "ChildDlg.h"


/////////////////////////////////////////////////////////////////////////////
// CGeneralPage dialog

class CGeneralPage : public CChildDlg
{
protected:

public:
  
public:
  
	CGeneralPage(CWnd* const cpParent);
	~CGeneralPage();

protected:

protected:

	//{{AFX_DATA(CGeneralPage)
	enum { IDD = IDD_GENERAL_DLG };
	//}}AFX_DATA


	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGeneralPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CGeneralPage)
	afx_msg void OnSplashShowCheck();
	afx_msg void OnTrayIconCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif//__GENERAL_H
