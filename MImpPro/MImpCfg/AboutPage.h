/**********************************************************************
AboutDlg
**********************************************************************/

#ifndef __ABOUTPAGE_H
#define __ABOUTPAGE_H

#pragma once

#include "resource.h"

#include "ChildDlg.h"

#include "hyperlink.h"

/////////////////////////////////////////////////////////////////////////////
// CAboutPage dialog

class CAboutPage : public CChildDlg
{
protected:

public:
  
public:
  
	CAboutPage(CWnd* const cpParent);
	~CAboutPage();

protected:

  //update url's in control's
  void UpdateUrls();

  ////try download url to passed mem ptr (new BYTE[] allocated)and return true if all download ok
  bool TryDownloadFile(LPCTSTR const cpcUrl, BYTE** const cppWantMem, DWORD* const cpMemSize);

protected:

// Dialog Data
	//{{AFX_DATA(CAboutPage)
	enum { IDD = IDD_ABOUT_PAGE };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAboutPage)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CAboutPage)
	afx_msg void OnCheckNewBtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

  virtual BOOL OnInitDialog();
};

#endif __ABOUTPAGE_H
