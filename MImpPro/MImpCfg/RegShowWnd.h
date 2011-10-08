/**********************************************************************
wnd for show registration info
**********************************************************************/

#if !defined(AFX_REGSHOWWND_H__6B0154C4_C8D9_11D3_B401_00400535408E__INCLUDED_)
#define AFX_REGSHOWWND_H__6B0154C4_C8D9_11D3_B401_00400535408E__INCLUDED_

#include "resource.h"

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CRegShowWnd window

class CRegShowWnd : public CStatic
{
protected:

  enum ClassConstEnum
  {
    eccTimerId = 100,
    eccTimerTime = 200,
  };

  LONG lTextOutShift;
  bool bShiftDiretion;
  POINT CtrlSize;
  CString Text;
  SIZE TextSize;
  CFont DrawFont;
  bool bTimeStarted;
  
public:
  
public:
  
	CRegShowWnd();
	virtual ~CRegShowWnd();

  inline void SetText(LPCTSTR const cpcText);

protected:

  inline void DrawCtrl(CDC& rDC);

protected:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegShowWnd)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CRegShowWnd)
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/**********************************************************************
implementation
**********************************************************************/


//////////////////////////////////////////////////////////////////////
//CRegShowWnd

inline void CRegShowWnd::SetText(LPCTSTR const cpcText)
{
  if(0 == DrawFont.m_hObject)
  {
    DrawFont.CreateStockObject(DEFAULT_GUI_FONT);
  };

  //text
  CClientDC DC(this);
  Text = cpcText;
  lTextOutShift = 0;
  bShiftDiretion = false;
  DC.SelectObject(&DrawFont);
  TextSize = DC.GetTextExtent(Text);

  //ctrl size
  RECT CliRect;
  CWnd::GetClientRect(&CliRect);
  CtrlSize.x = CliRect.right - CliRect.left;
  CtrlSize.y = CliRect.bottom - CliRect.top;

  if
    (
    CtrlSize.x < TextSize.cx
    && false == bTimeStarted
    )
  {
    bTimeStarted = 
      eccTimerId == CWnd::SetTimer(eccTimerId, eccTimerTime, 0);
  };

  Invalidate();
};

inline void CRegShowWnd::DrawCtrl(CDC& rDC)
{
  const POINT cTextPnt = 
  {
    (CtrlSize.x - TextSize.cx) / 2 + lTextOutShift,
    (CtrlSize.y - TextSize.cy) / 2
  };
  rDC.SetBkColor(::GetSysColor(COLOR_3DFACE));
  rDC.SelectObject(&DrawFont);
  rDC.SetBkMode(OPAQUE);
  rDC.SetTextColor(COLOR_BTNTEXT);
  const RECT cCliRect = {0, 0, CtrlSize.x, CtrlSize.y};
  rDC.ExtTextOut(cTextPnt.x, cTextPnt.y, ETO_CLIPPED | ETO_OPAQUE, &cCliRect, Text, 0);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGSHOWWND_H__6B0154C4_C8D9_11D3_B401_00400535408E__INCLUDED_)
