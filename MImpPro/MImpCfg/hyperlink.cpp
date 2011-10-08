/**********************************************************************
hyper link ctrl - for subclass static ctrl
**********************************************************************/

#include "stdafx.h"


#include "HyperLink.h"


#include "resource.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHyperLink

#define SETBITS(dw, bits)	(dw |= bits)
#define CLEARBITS(dw, bits)	(dw &= ~(bits))
#define BITSET(dw, bit)		(((dw) & (bit)) != 0L)


CHyperLink::CHyperLink()
  :CClickStatic(true)
{
}

CHyperLink::~CHyperLink()
{
  Font.DeleteObject();
}

BEGIN_MESSAGE_MAP(CHyperLink, CClickStatic)
    //{{AFX_MSG_MAP(CHyperLink)
    ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHyperLink message handlers

void CHyperLink::PreSubclassWindow() 
{		    
  // Get the current window font	
  CFont* pFont = GetFont();	
  
  if (pFont != NULL) {
    LOGFONT lf;
    pFont->GetLogFont(&lf);
    lf.lfUnderline = TRUE;
    if (Font.CreateFontIndirect(&lf))
      CClickStatic::SetFont(&Font);    
    // Adjust window size to fit URL if necessary
    AdjustWindow();
  };
  
  CClickStatic::PreSubclassWindow();
}

// Handler for WM_CTLCOLOR reflected message (see message map)
HBRUSH CHyperLink::CtlColor(CDC* pDC, UINT nCtlColor) 
{		
	ASSERT(nCtlColor == CTLCOLOR_STATIC);

  pDC->SetTextColor(eccLinkColor);
	// Set transparent drawing mode
	pDC->SetBkMode(TRANSPARENT);
	return (HBRUSH)GetStockObject(NULL_BRUSH);
}

// Move and resize the window so that its client area has the same size
// as the hyperlink text. This prevents the hyperlink cursor being active
// when it is not over the text.
void CHyperLink::AdjustWindow()
{	
  ASSERT(::IsWindow(GetSafeHwnd()));
  
  if(0 != Font.m_hObject)
  {
    // Get the current window rect
    CRect rcWnd;
    GetWindowRect(rcWnd);
    
    // For a child CWnd object, window rect is relative to the 
    // upper-left corner of the parent window’s client area. 
    CWnd* pParent = GetParent();
    if (pParent)
      pParent->ScreenToClient(rcWnd);
    
    // Get the current client rect
    CRect rcClient;
    GetClientRect(rcClient);
    
    // Calc border size based on window and client rects
    int borderWidth = rcWnd.Width() - rcClient.Width();
    int borderHeight = rcWnd.Height() - rcClient.Height();
    
    // Get the extent of window text 
    CString strWndText;
    GetWindowText(strWndText);
    
    CDC* pDC = GetDC();	
    CFont* pOldFont = pDC->SelectObject(&Font);
    CSize Extent = pDC->GetTextExtent(strWndText);
    pDC->SelectObject(pOldFont);
    ReleaseDC(pDC);
    
    // Get the text justification style
    DWORD dwStyle = GetStyle();
    
    // Recalc window size and position based on text justification
    if (BITSET(dwStyle, SS_CENTERIMAGE))
      rcWnd.DeflateRect(0, (rcWnd.Height() - Extent.cy) / 2);
    else
      rcWnd.bottom = rcWnd.top + Extent.cy;
    
    if (BITSET(dwStyle, SS_CENTER))
      rcWnd.DeflateRect((rcWnd.Width() - Extent.cx) / 2, 0);
    else if (BITSET(dwStyle,SS_RIGHT))
      rcWnd.left  = rcWnd.right - Extent.cx;
    else // SS_LEFT
      rcWnd.right = rcWnd.left + Extent.cx;
    
    // Move and resize the window
    MoveWindow(rcWnd.left, rcWnd.top, rcWnd.Width() + borderWidth,
      rcWnd.Height() + borderHeight);
  };
}

