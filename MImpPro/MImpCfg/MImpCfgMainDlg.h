/**********************************************************************
Main cfg dialog
**********************************************************************/

#if !defined(AFX_MIMPCFGMAINDLG_H__EB83B9D3_8939_11D2_8813_204C4F4F5020__INCLUDED_)
#define AFX_MIMPCFGMAINDLG_H__EB83B9D3_8939_11D2_8813_204C4F4F5020__INCLUDED_

#pragma once

#include "resource.h"

#include "BmpStatic.h"

#include "RegShowWnd.h"

#include "..\\SlibExc\\SLCont.h"


class CChildDlg;
using sl::CSLStack;

/////////////////////////////////////////////////////////////////////////////
// CMImpCfgMainDlg dialog

class CMImpCfgMainDlg : public CDialog
{
protected:

  enum ClassConstEnum
  {
    //size's of logo bmp
    eccLogoBmpWidth = 120,
    eccLogoBmpHeight = 260,
    //prm's for "LogoChng" timer
    eccLogoChngTimerTime = 1000,
    eccLogoChngTimerId = 1
  };

  //scale factor for page
  float fScale;

  //bmp ctrl's
  CBmpStatic LogoBmp;
  CBmpStatic TitleBmp;
  //current bmp in "logo chng" section
  DWORD dwCurrChngLogo;

  ////stack for hide/unhide wnd's
  //used for hide/unhide all wnd's (in pop find)
  typedef CSLStack<HWND> HideWndStackType;
  HideWndStackType HideWndStack;

public:
  
public:
  
  CMImpCfgMainDlg();

  //page navigation for tab ctrl - next/prev
  //called from App::PretranslateMsg
  void PageNext(const bool bcPrev);

  //hide/unhide all wnd's
  void WndAllHide();
  inline void WndAllShow();

protected:

  //helper for hide wnd's
  BOOL WndAllHideHelper(HWND hcWnd, LPARAM lParam);

  //insert dlg to tab ctrl
  void InsertDlg(CChildDlg* const cpDlg, const DWORD dwcHelpId);
  //selectewd dlg
  bool GetSelDlg(CChildDlg*& rpDlg);

  //process update for chng logo (called from timer and from init)
  void ChngLogoProcess();

  //read reg info
  void ReadRegInfo();

protected:

  virtual void OnOK();
  virtual BOOL PreTranslateMessage(MSG* pMsg);

// Dialog Data
	//{{AFX_DATA(CMImpCfgMainDlg)
	enum { IDD = IDD_MAIN_DIALOG };
	CTabCtrl	ChildTab;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMImpCfgMainDlg)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CMImpCfgMainDlg)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSelchangingChildTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeChildTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnApply();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	afx_msg void OnRegInfo();
  afx_msg LRESULT OnGetHelptopicId(WPARAM, LPARAM);
  afx_msg LRESULT OnActivateApp(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};

/**********************************************************************
implementation
**********************************************************************/

//////////////////////////////////////////////////////////////////////
//CMImpCfgMainDlg

inline void CMImpCfgMainDlg::WndAllShow()
{
  while(false == HideWndStack.IsEmpty())
  {
    const HWND hcWnd = *(HideWndStack.Top());
    HideWndStack.Pop();
    ::ShowWindow(hcWnd, SW_SHOW);
  };
  HideWndStack.FreeAll();
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIMPCFGMAINDLG_H__EB83B9D3_8939_11D2_8813_204C4F4F5020__INCLUDED_)
