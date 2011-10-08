/**********************************************************************
WndPattern cfg dlg
**********************************************************************/

#if !defined(AFX_AUTOHIDEPATTERNS_H__C149EF06_86B3_11D2_A82A_000000000000__INCLUDED_)
#define AFX_AUTOHIDEPATTERNS_H__C149EF06_86B3_11D2_A82A_000000000000__INCLUDED_

#pragma once

#include "resource.h"


#include "..\\SlibExc\\SLMemMgr.h"

#include "..\\MIGlobal.h"

#include "MyListCtrl.h"

using sl::CSLTmplChunk;

#include "ChildDlg.h"

//////////////////////////////////////////////////////////////////////
//CPatternInfo

struct __CWndPatternInfo
{
  //application anme
  CString AppName;
  //class of wnd
  CString ClassName;
  //title of wnd
  CString TitleName;
  //only activate
  DWORD dwPattType;
  //constr
  inline __CWndPatternInfo();
};


/////////////////////////////////////////////////////////////////////////////
// CAutoShrinkPatterns dialog

class CAutoShrinkPatterns : public CChildDlg
{
protected:

  enum ClassConstEnum
  {
  };

  //mem mgr for list info
  CSLTmplChunk<__CWndPatternInfo> ListInfoMgr;

  //srink type img
  CImageList ShrinkType;

  //"is some data chnged" flag - used for store data to registry on end dialog
  bool bSomePatternsChnged;
  bool bEnableChnged;
  
public:
  
public:
  
	CAutoShrinkPatterns(CWnd* const cpParent);
	~CAutoShrinkPatterns();

  //insert item to list (if 0 > icInd - new item)
  //used internaly and from CAppPattDlg
  void InsertPattern(const DWORD dwcType, const CString& rcApp, const CString& rcClass, const CString& rcTitle, const int icInd = -1, const bool bcIsChng = false);

protected:

  //sync edit btn's with list state
  void SyncEditBtns(const bool bcSelIfNone);
  //load/save list from registry
  void RegLoadData();
  void RegSaveData();
  //read WPAtt from reg and insert to WPattList - used from RegloadData and from WndPatReRead
  void ReadCfgEntryAndInsert(const DWORD dwcInd);

  //sync open ctrl's
  void SyncOpenCtrls();

protected:

// Dialog Data
	//{{AFX_DATA(CAutoShrinkPatterns)
	enum { IDD = IDD_WND_PATT_DLG };
	CMyListCtrl	WndPatList;
	//}}AFX_DATA

	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAutoShrinkPatterns)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CAutoShrinkPatterns)
	virtual BOOL OnInitDialog();
	afx_msg void OnDelBtn();
	afx_msg void OnInsBtn();
	afx_msg void OnDeleteitemWndPatList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetdispinfoWndPatList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkWndPatList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditBtn();
	afx_msg void OnEnabledCheck();
	afx_msg void OnKeydownWndPatList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDelayOpenCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/**********************************************************************
implementation
**********************************************************************/

//////////////////////////////////////////////////////////////////////
//__CWndPatternInfo

inline __CWndPatternInfo::__CWndPatternInfo()
  :dwPattType(estShrink)
{
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOHIDEPATTERNS_H__C149EF06_86B3_11D2_A82A_000000000000__INCLUDED_)
