/**********************************************************************
WndPattern finder dlg
**********************************************************************/

#if !defined(AFX_SELWNDPATTERNDLG_H__C149EF07_86B3_11D2_A82A_000000000000__INCLUDED_)
#define AFX_SELWNDPATTERNDLG_H__C149EF07_86B3_11D2_A82A_000000000000__INCLUDED_

#pragma once

#include "resource.h"

struct __CWndPatternInfo;

/////////////////////////////////////////////////////////////////////////////
// CSelWndPatternDlg dialog

class CSelWndPatternDlg : public CDialog
{
protected:

  enum ClassConstEnum
  {
    //flash timer const's
    eccFTimerId = 1,
    eccFTimerTime = 250,
    //number of flash count's
    eccFlashNumb = 8,
    //timer for "pop up find"
    eccPopUpTimerId = 2,
    eccPopUpTimerTime = eccFTimerTime,
    eccPopUpFlashNumber = 4,
    //id of hot key for wnd catching
    eccWndCatchHotKeyId = 0
  };

  //edited info
  __CWndPatternInfo* const cpCtrlInfo;
  //is capture present (find process)
  bool bCapturePresent;
  //"current find window"
  HWND hCurrFindWnd;
  //lase looking window (not some as hCurrFindWnd - becose we find wnd by "drilling up")
  HWND hLastLookingWnd;
  //last looking process (for caching);
  DWORD dwLastLookingProcess;

  ////count of "faadback flashing's"
  DWORD dwFlashCount;
  //is in popup find
  bool bPopUpFind;

public:
  
public:
  
	CSelWndPatternDlg(CWnd* pParent, __CWndPatternInfo* const cpInitCtrlInfo);

protected:

  //stop "pop up" find
  void StopPopUpFind(const bool bcOnlyStop);

  //show/hide wnd for popup find
  void PopFindShow();
  void PopUpHide();

  //is "hide main wnd" cpecified
  inline bool IsHideMainWnd();


  //sync "finder" icon with internal state
  void SyncFinderIconWithState();
  //stop find 
  void FindStopDraw();
  //stop whis window visualisation feedback
  void StopFeedBack();
  //draw feedback in window
  void DrawFeedBack();
  //sync data for finded wnd
  void SyncDataForFindWnd();

protected:

	CComboBox	TypeCombo;
// Dialog Data
	//{{AFX_DATA(CSelWndPatternDlg)
	enum { IDD = IDD_WND_PAT_SEL_DLG };
	CHotKeyCtrl	SelHotKey;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelWndPatternDlg)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CSelWndPatternDlg)
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnAppNameAllCheck();
	afx_msg void OnWndClassAllCheck();
	afx_msg void OnWndNameAllCheck();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnFindBtn();
	//}}AFX_MSG
  afx_msg void OnCaptureStatic();
  afx_msg LPARAM OnHotKey(WPARAM wParam, LPARAM lParam);
  afx_msg LPARAM OnShowAppMsg(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/**********************************************************************
implementation
**********************************************************************/

//////////////////////////////////////////////////////////////////////
//CSelWndPatternDlg

inline bool CSelWndPatternDlg::IsHideMainWnd()
{
  return BST_CHECKED == SendDlgItemMessage(IDC_HIDE_CHECK, BM_GETCHECK);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELWNDPATTERNDLG_H__C149EF07_86B3_11D2_A82A_000000000000__INCLUDED_)
