/**********************************************************************
hyper link ctrl - for subclass static ctrl
**********************************************************************/

#if !defined(AFX_HYPERLINK_H_04ET323B01_023500_0204251998_ENG_INCLUDED_)
#define AFX_HYPERLINK_H_04ET323B01_023500_0204251998_ENG_INCLUDED_

#pragma once


#include "ClickStatic.h"

/////////////////////////////////////////////////////////////////////////////
// CHyperLink window

class CHyperLink : public CClickStatic
{
protected:

  enum ClassConstEnum
  {
    //color for link
    eccLinkColor = RGB(0, 0, 255)
  };

  // Underlined font
	CFont Font;					

public:
  
public:
  
	CHyperLink();
	virtual ~CHyperLink();

  //chng url 
  inline void SetUrl(LPCTSTR const cpcNewUrl);

  //subclass ctrl and set url
  inline BOOL SubclassDlgItem(UINT nID, CWnd* pParent);


protected:


protected:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHyperLink)
	protected:
	virtual void PreSubclassWindow();	
	//}}AFX_VIRTUAL

	void AdjustWindow();	

	//{{AFX_MSG(CHyperLink)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/**********************************************************************
implementation
**********************************************************************/

//////////////////////////////////////////////////////////////////////
//CHyperLink

inline void CHyperLink::SetUrl(LPCTSTR const cpcNewUrl)
{
  //call parent
  //ajust wnd size
  //invaludate

  //parent
  CClickStatic::SetUrl(cpcNewUrl);
  //ajust wnd size
  AdjustWindow();
  //invaludate
  Invalidate();
};


inline BOOL CHyperLink::SubclassDlgItem(UINT nID, CWnd* pParent)
{
  return CClickStatic::SubclassDlgItem(nID, pParent);
}


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line

#endif // !defined(AFX_HYPERLINK_H_04ET323B01_023500_0204251998_ENG_INCLUDED_)
