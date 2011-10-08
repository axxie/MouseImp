/**********************************************************************
base for property child's
**********************************************************************/

#if !defined(AFX_CHILDDLG_H__1EAC3D56_7715_11D2_A816_000000000000__INCLUDED_)
#define AFX_CHILDDLG_H__1EAC3D56_7715_11D2_A816_000000000000__INCLUDED_

#pragma once


/////////////////////////////////////////////////////////////////////////////
// CChildDlg dialog

class CChildDlg : public CDialog
{
protected:

  //help topic, associated with this dialog
  DWORD dwHelpTopic;

public:
  
public:

  CChildDlg(const UINT uicResId, CWnd* pParent);

  //create dlg
  inline BOOL Create(CWnd* const cpParent);

  //set topic id, associated with this dlg
  inline void SetHelpTopicId(const DWORD dwcInitTopid);
  inline DWORD GetHelpTopicId() const;

  //get resid, attached to dlg
  inline LPCTSTR GetResId() const;
  
  virtual BOOL OnSetActive();
  virtual BOOL OnKillActive();

protected:

protected:

  //override OnInit for not set focus to new created dialog
  virtual BOOL OnInitDialog();

// Dialog Data
	//{{AFX_DATA(CChildDlg)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildDlg)
	public:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CChildDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG

  //catch "get topic id" message
  afx_msg LRESULT OnGetHelptopicId(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};

/**********************************************************************
implementation
**********************************************************************/

//////////////////////////////////////////////////////////////////////
//CChildDlg

inline LPCTSTR CChildDlg::GetResId() const
{
  return m_lpszTemplateName;
};

inline BOOL CChildDlg::Create(CWnd* const cpParent)
{
  return CDialog::Create(GetResId(), cpParent);
};

inline DWORD CChildDlg::GetHelpTopicId() const
{
  return dwHelpTopic;
};

inline void CChildDlg::SetHelpTopicId(const DWORD dwcInitTopid)
{
  dwHelpTopic = dwcInitTopid;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDDLG_H__1EAC3D56_7715_11D2_A816_000000000000__INCLUDED_)
