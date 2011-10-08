/**********************************************************************
wnd for show registration info
**********************************************************************/

#include "stdafx.h"


#include "RegShowWnd.h"


/////////////////////////////////////////////////////////////////////////////
// CRegShowWnd

CRegShowWnd::CRegShowWnd()
  :bTimeStarted(false),
  lTextOutShift(0),
  bShiftDiretion(false)
{
}

CRegShowWnd::~CRegShowWnd()
{
}


BEGIN_MESSAGE_MAP(CRegShowWnd, CStatic)
	//{{AFX_MSG_MAP(CRegShowWnd)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CRegShowWnd::PreSubclassWindow() 
{
	CStatic::PreSubclassWindow();

  CString DefText;
  CWnd::GetWindowText(DefText);
  SetText(DefText);
}

void CRegShowWnd::OnDestroy() 
{
  if(false != bTimeStarted)
  {
    CWnd::KillTimer(eccTimerId);
  };

  DrawFont.DeleteObject();

	CStatic::OnDestroy();
}

void CRegShowWnd::OnTimer(UINT nIDEvent) 
{
  switch(nIDEvent)
  {
  case eccTimerId:
    if(CtrlSize.x < TextSize.cx)
    {
      const LONG lcMoveDist = (TextSize.cx - CtrlSize.x) / 2;
      if(false != bShiftDiretion)
      {
        lTextOutShift++;
        if(lcMoveDist < lTextOutShift)
        {
          bShiftDiretion = false;
        };
      }
      else
      {
        lTextOutShift--;
        if(-lcMoveDist > lTextOutShift)
        {
          bShiftDiretion = true;
        };
      };
      CClientDC DC(this);
      DrawCtrl(DC);
    };
    break;

  default:
  	CStatic::OnTimer(nIDEvent);
    break;
  };
}

void CRegShowWnd::OnPaint() 
{
	CPaintDC dc(this);
  DrawCtrl(dc);
}
