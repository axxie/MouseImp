/**********************************************************************
Static ctrl with click possibility (only) - for WWW link's
**********************************************************************/

#if !defined(AFX_CLICKSTATIC_H__F8B7AF97_9FAF_11D2_B139_00A0C907DA21__INCLUDED_)
#define AFX_CLICKSTATIC_H__F8B7AF97_9FAF_11D2_B139_00A0C907DA21__INCLUDED_

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CClickStatic window

class CClickStatic : public CStatic
{
protected:

  enum ClassConstEnum
  {
    //tool tip id
    eccToolTipToolId = 1
  };

  //URL 
  CString URL;
  //tool tip ctrl
  CToolTipCtrl ToolTip;

  //enable/disable URL behavior
  const bool bcLikeUrl;
  
public:
  
public:
  
	CClickStatic(const bool bcInitLikeUrl);
  virtual ~CClickStatic();

  //change url string
  inline void SetUrl(LPCTSTR const cpcNewUrl);

  //goto url's attached to ctrl
  static bool GoToUrl(LPCSTR const cpcStr, const DWORD dwcCmd);

protected:

  //goto current url
  inline void GoToUrl();

protected:

  //process context menu info
  void ProcessContextMenu(const UINT uicMenuItem);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClickStatic)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CClickStatic)
	afx_msg void OnDestroy();
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/**********************************************************************
implementation
**********************************************************************/
 
//////////////////////////////////////////////////////////////////////
//CClickStatic


inline void CClickStatic::SetUrl(LPCTSTR const cpcNewUrl)
{
  //mem url
  //chnge tool tip string

  //chng wnd rect
  RECT WndRect;
  GetClientRect(&WndRect);
  ToolTip.SetToolRect(this, eccToolTipToolId, &WndRect);

  //mem
  URL = cpcNewUrl;
  //tooltip
  ToolTip.UpdateTipText(URL, this, eccToolTipToolId);
};

inline void CClickStatic::GoToUrl()
{
  if(false == GoToUrl(URL, SW_SHOW))
  {
    //eror in follow
    ::MessageBeep(MB_ICONEXCLAMATION);
  };
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLICKSTATIC_H__F8B7AF97_9FAF_11D2_B139_00A0C907DA21__INCLUDED_)
