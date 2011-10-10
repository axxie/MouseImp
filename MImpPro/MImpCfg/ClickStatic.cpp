/**********************************************************************
Static ctrl with click possibility (only) - for WWW link's
**********************************************************************/

#include "stdafx.h"


#include "ClickStatic.h"


#include "Resource.h"

#include "..\\Slib\\SLDbg.h"

#include "..\\SlibWin\\SLWRes.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CClickStatic

CClickStatic::CClickStatic(const bool bcInitLikeUrl)
  :bcLikeUrl(bcInitLikeUrl)
{
}

CClickStatic::~CClickStatic()
{
}


BEGIN_MESSAGE_MAP(CClickStatic, CStatic)
	//{{AFX_MSG_MAP(CClickStatic)
	ON_WM_DESTROY()
	ON_WM_NCHITTEST()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClickStatic message handlers

BOOL CClickStatic::PreTranslateMessage(MSG* pMsg) 
{
	if(false != bcLikeUrl)
  {
    ToolTip.RelayEvent(pMsg);
  };
	return CStatic::PreTranslateMessage(pMsg);
}

void CClickStatic::PreSubclassWindow() 
{
	//call parent
  //if ToolTip conter is zero - create tool tip
	
  //parent
	CStatic::PreSubclassWindow();

  if(false != bcLikeUrl)
  {
    CRect WndRect;
    GetClientRect(&WndRect);
    //tool tip
    SLCHECK(FALSE != ToolTip.Create(this));
    ToolTip.AddTool(this, URL, &WndRect, eccToolTipToolId);
  };
}

void CClickStatic::OnDestroy() 
{
  //remove tool
  //dec ref counter for tool tip - if zero - remove tool tip
  //destroy wnd

  if(false != bcLikeUrl)
  {
    //tool
    if(FALSE != ::IsWindow(ToolTip.m_hWnd))
    {
      ToolTip.DelTool(this, eccToolTipToolId);
      ToolTip.DestroyWindow();
    };
  };

  //destroy
	CStatic::OnDestroy();
}

LRESULT CClickStatic::OnNcHitTest(CPoint point) 
{
	//catch for process move message's
  return HTCLIENT;
}

void CClickStatic::OnMouseMove(UINT nFlags, CPoint point) 
{
  //set cursor for wnd

  if(false != bcLikeUrl)
  {
    //cursor
    ::SetCursor(AfxGetApp()->LoadCursor(IDC_HAND_CURSOR));
  }
  else
  {
    CStatic::OnMouseMove(nFlags, point);
  };
}

BOOL CClickStatic::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
  //set cursor for wnd

  BOOL bRes = TRUE;

  if(false != bcLikeUrl)
  {
    //cursor
    ::SetCursor(AfxGetApp()->LoadCursor(IDC_HAND_CURSOR));
  }
  else
  {
    bRes = CStatic::OnSetCursor(pWnd, nHitTest, message);
  };

  return bRes;
}

static inline LONG GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR retdata)
{
  HKEY hkey;
  LONG retval = RegOpenKeyEx(key, subkey, 0, KEY_QUERY_VALUE, &hkey);
  
  if (retval == ERROR_SUCCESS) 
  {
    long datasize = MAX_PATH;
    TCHAR data[MAX_PATH];
    RegQueryValue(hkey, NULL, data, &datasize);
    lstrcpy(retdata,data);
    RegCloseKey(hkey);
  }
  return retval;
};


bool CClickStatic::GoToUrl(LPCSTR const cpcStr, const DWORD dwcCmd)
{
  TCHAR key[MAX_PATH + MAX_PATH];	
  
  // First try ShellExecute()
  HINSTANCE result = ShellExecute(NULL, _T("open"), cpcStr, NULL,NULL, dwcCmd);
  
  // If it failed, get the .htm regkey and lookup the program
  if ((UINT)result <= HINSTANCE_ERROR) {		
    
    if (GetRegKey(HKEY_CLASSES_ROOT, _T(".htm"), key) == ERROR_SUCCESS) {
      lstrcat(key, _T("\\shell\\open\\command"));
      
      if (GetRegKey(HKEY_CLASSES_ROOT,key,key) == ERROR_SUCCESS) {
        TCHAR *pos;
        pos = _tcsstr(key, _T("\"%1\""));
        if (pos == NULL) {                     // No quotes found
          pos = strstr(key, _T("%1"));       // Check for %1, without quotes
          if (pos == NULL)                   // No parameter at all...
            pos = key+lstrlen(key)-1;
          else
            *pos = '\0';                   // Remove the parameter
        }
        else
          *pos = '\0';                       // Remove the parameter
        
        lstrcat(pos, _T(" "));
        lstrcat(pos, cpcStr);
        result = (HINSTANCE) WinExec(key,dwcCmd);
      }
    }
  }
  
  return reinterpret_cast<int>(result) > HINSTANCE_ERROR;
};

void CClickStatic::OnLButtonUp(UINT nFlags, CPoint point) 
{
	//goto selected url

  //goto
  if(false != bcLikeUrl)
  {
    GoToUrl();
  }
  else
  {
    CStatic::OnLButtonUp(nFlags, point);
  };
}

void CClickStatic::ProcessContextMenu(const UINT uicMenuItem)
{
  switch(uicMenuItem)
  {
    //open lnk
  case IDC_LNK_OPEN:
    GoToUrl();
    break;

    //copy lnk to clipboard
  case IDC_LNK_COPY:
    if(FALSE != ::OpenClipboard(m_hWnd) && FALSE != EmptyClipboard())
    {
      LPCTSTR const cpcStr = URL;
      const DWORD dwcStrLen = ::_tcslen(cpcStr) + 1;
      HANDLE hData = GlobalAlloc(GMEM_DDESHARE, dwcStrLen * sizeof(*cpcStr));
      if(0 != hData)
      {
        LPTSTR const cpcMemStr = reinterpret_cast<LPTSTR>(GlobalLock(hData));
        if(0 != cpcMemStr)
        {
          CopyMemory(cpcMemStr, cpcStr, sizeof(*cpcStr) * dwcStrLen);
          GlobalUnlock(hData);
          if(0 == SetClipboardData(CF_TEXT, hData))
          {
            //failed - free mem 
            GlobalFree(hData);
          };
        };
      };
      CloseClipboard();
    };
    break;

  default:
    break;
  };
};

void CClickStatic::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if(false != bcLikeUrl)
  {
    //bring context menu for link
    using slw::CSLWMenu;
    CSLWMenu Menu;
    SLCHECK(false != Menu.Load(IDR_CONTEXT_MENU));
    //sub menu
    CSLWMenu ContMenu;
    ContMenu.Attach(Menu.GetSubMenu(0));
    //mark default
    ContMenu.MarkDefault(IDC_LNK_OPEN);
    //track
    const UINT uicRes = ContMenu.TrackPopUp(m_hWnd);
    ProcessContextMenu(uicRes);
  }
  else
  {
    CStatic::OnContextMenu(pWnd, point);
  };	
}
