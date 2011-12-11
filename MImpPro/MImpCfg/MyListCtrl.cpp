/**********************************************************************
MyListCtrl (alpha)
**********************************************************************/
#include "stdafx.h"


#include "MyListCtrl.h"


#include "..\\SlibExc\\SLDbg.h"


#include "resource.h"

#include "AppConst.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
//sort section

struct CSortListInfo
{
  int iCol;
  CListCtrl* pList;
  bool bPosOrder;
};

static int CALLBACK SortFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
  const CSortListInfo* const cpcInfo = reinterpret_cast<const CSortListInfo*>(lParamSort);

  LV_FINDINFO FindInfo;

  //find first item by ItemData
  FindInfo.flags = LVFI_PARAM;
  FindInfo.lParam = lParam1;
  const int icFirst = cpcInfo->pList->FindItem(&FindInfo);
  //find second func by ItemData (start from prev)
  FindInfo.flags = LVFI_PARAM | LVFI_WRAP;
  FindInfo.lParam = lParam2;
  const int icSecond = cpcInfo->pList->FindItem(&FindInfo, icFirst);

  //get string's
  TCHAR cpFirst[100];
  TCHAR cpSecond[100];
  cpcInfo->pList->GetItemText(icFirst, cpcInfo->iCol, cpFirst, COUNTOF(cpFirst));
  cpcInfo->pList->GetItemText(icSecond, cpcInfo->iCol, cpSecond, COUNTOF(cpSecond));

  const int icVal = (false != cpcInfo->bPosOrder)
    ? lstrcmp(cpFirst, cpSecond)
    : lstrcmp(cpSecond, cpFirst);
  return icVal;
};


/////////////////////////////////////////////////////////////////////////////
// CMyListCtrl

CMyListCtrl::CMyListCtrl()
  :iSortCol(-1),
  bSortPos(false),
  iHdrColBmp(-1)
{
}

CMyListCtrl::~CMyListCtrl()
{
}


BEGIN_MESSAGE_MAP(CMyListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CMyListCtrl)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	//}}AFX_MSG_MAP
  ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyListCtrl message handlers

void CMyListCtrl::Init(const CListColInfo* const cpcColInitInfo)
{
  //do insert coluumns - calc max width and process col's as part of it's width
  //set "ful row select" style
  //attach img list to header ctrl (if present)

  //wnd shoult be attached already
  _ASSERT(FALSE != ::IsWindow(m_hWnd));

  //calc col max width
  UINT uiSummWidth = 0;
  UINT uiMaxWidth = 0;
  {
    for(const CListColInfo* pcPtr = cpcColInitInfo; -1 != pcPtr->uiResId && 0 != pcPtr->uiResId; pcPtr++)
    {
      uiSummWidth += pcPtr->uiWidth;
    };
    CRect Rect;
    GetClientRect(&Rect);
    uiMaxWidth = Rect.Width() - ::GetSystemMetrics(SM_CXVSCROLL);
  };

  //now insert col's
  {
    UINT uiColNumb = 0;
    for(const CListColInfo* pcPtr = cpcColInitInfo; -1 != pcPtr->uiResId && 0 != pcPtr->uiResId; pcPtr++)
    {
      LVCOLUMN Info;
      Info.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
      Info.fmt = LVCFMT_LEFT;
      //col name
      CString Name;
      SLCHECK(FALSE != Name.LoadString(pcPtr->uiResId));
      Info.pszText = Name.LockBuffer();
      //width 
      Info.cx = pcPtr->uiWidth * uiMaxWidth / uiSummWidth;
      //subitem
      Info.iSubItem = pcPtr->uiSubItem;

      InsertColumn(uiColNumb, &Info);

      uiColNumb++;
    };
  };

  //set list ctl styles
  ListView_SetExtendedListViewStyleEx(m_hWnd, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

  {
    //get hdr ctrl wnd
    HWND hHdrCtrl = ::GetDlgItem(m_hWnd, 0);
    if(0 != hHdrCtrl)
    {
      //create header img list
      SLCHECK(FALSE != HdrImgList.Create(IDB_LIST_ARROW_BMP, eicArrowWidth, 0, eicArrowNull));

      //attach img list to hdr ctrl
      ::SendMessage(hHdrCtrl, HDM_SETIMAGELIST, 0, reinterpret_cast<LPARAM>(HdrImgList.GetSafeHandle()));
    };
  };
};

void CMyListCtrl::CallSort(const bool bcLookOld)
{
  CWaitCursor Wait;

  //mem data for focused item
  DWORD dwMemData = 0;
  if(0 != bcLookOld)
  {
    const int icItem = GetNextItem(-1, LVNI_FOCUSED);
    if(0 <= icItem)
    {
      dwMemData = GetItemData(icItem);
    };
  };

  CSortListInfo Info;
  Info.iCol = iSortCol;
  Info.pList = this;
  Info.bPosOrder = bSortPos;
  SortItems(SortFunc, reinterpret_cast<LPARAM>(&Info));

  if(0 != dwMemData)
  {
    //find memoed item and selec it
    LV_FINDINFO Info;
    Info.flags = LVFI_PARAM;
    Info.lParam = dwMemData;
    const int icInd = FindItem(&Info);
    if(0 <= icInd)
    {
      EnsureVisible(icInd, FALSE);
      SetItemState(icInd, LVIS_FOCUSED, LVIS_FOCUSED);
    };
  };

  //now process hdr ctrl
  CHeaderCtrl* const cpHdr = static_cast<CHeaderCtrl*>(GetDlgItem(0));
  _ASSERT(FALSE != ::IsWindow(cpHdr->m_hWnd));
  HD_ITEM HItem;
  //remove old
  if(-1 != iHdrColBmp)
  {
    HItem.mask = HDI_FORMAT;
    cpHdr->GetItem(iHdrColBmp, &HItem);
    HItem.mask = HDI_FORMAT | HDI_IMAGE;
    HItem.fmt &= ~HDF_IMAGE;
    HItem.iImage = 0;
    cpHdr->SetItem(iHdrColBmp, &HItem);
  };
  //mem pos
  iHdrColBmp = iSortCol;
  //set new
  HItem.mask = HDI_FORMAT;
  cpHdr->GetItem(iHdrColBmp, &HItem);
  HItem.mask = HDI_FORMAT | HDI_IMAGE;
  HItem.fmt |= HDF_IMAGE;
  HItem.iImage = (false != bSortPos) ? eiiUp : eiiDown;
  cpHdr->SetItem(iHdrColBmp, &HItem);
  //force view redraw
  cpHdr->Invalidate();
  cpHdr->UpdateWindow();
};

void CMyListCtrl::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
  const NM_LISTVIEW* const cpcInfo = reinterpret_cast<const NM_LISTVIEW*>(pNMHDR);

  const int icCol = cpcInfo->iSubItem;
  if(icCol == iSortCol)
  {
    bSortPos = !bSortPos;
  }
  else
  {
    iSortCol = icCol;
    bSortPos = true;
  };

  CallSort(false);

  *pResult = FALSE;
}

int CMyListCtrl::CellRectFromPoint(CPoint & point, RECT * cellrect, int * col) const
{
  int colnum;

  // Make sure that the ListView is in LVS_REPORT
  if( (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT )
    return -1;

  // Get the top and bottom row visible
  int row = GetTopIndex();
  int bottom = row + GetCountPerPage();
  if( bottom > GetItemCount() )
    bottom = GetItemCount();
  
  // Get the number of columns
  CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
  int nColumnCount = pHeader->GetItemCount();

  // Loop through the visible rows
  for( ;row <=bottom;row++)
  {
    // Get bounding rect of item and check whether point falls in it.
    CRect rect;
    GetItemRect( row, &rect, LVIR_BOUNDS );
    if( rect.PtInRect(point) )
    {
      // Now find the column
      for( colnum = 0; colnum < nColumnCount; colnum++ )
      {
        int colwidth = GetColumnWidth(colnum);
        if( point.x >= rect.left && point.x <= (rect.left + colwidth ) )
        {
          RECT rectClient;
          GetClientRect( &rectClient );
          if( col ) *col = colnum;
          rect.right = rect.left + colwidth;

          // Make sure that the right extent does not exceed
          // the client area
          if( rect.right > rectClient.right )
            rect.right = rectClient.right;
          *cellrect = rect;
          return row;
        }
        rect.left += colwidth;
      }
    }
  }
  return -1;
};

BOOL CMyListCtrl::OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
	// need to handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	CString strTipText;
	UINT nID = pNMHDR->idFrom;

	if( nID == 0 )	  	// Notification in NT from automatically
		return FALSE;   	// created tooltip

	int row = ((nID-1) >> 10) & 0x3fffff ;
	int col = (nID-1) & 0x3ff;
	strTipText = GetItemText( row, col );

#ifndef _UNICODE
	if (pNMHDR->code == TTN_NEEDTEXTA)
		lstrcpyn(pTTTA->szText, strTipText, 80);
	else
		_mbstowcsz(pTTTW->szText, strTipText, 80);
#else
	if (pNMHDR->code == TTN_NEEDTEXTA)
		_wcstombsz(pTTTA->szText, strTipText, 80);
	else
		lstrcpyn(pTTTW->szText, strTipText, 80);
#endif
	*pResult = 0;

	return TRUE;    // message was handled
}

INT_PTR CMyListCtrl::OnToolHitTest(CPoint point, TOOLINFO * pTI) const
{
	int row, col;
	RECT cellrect;
	row = CellRectFromPoint(point, &cellrect, &col );

	if ( row == -1 ) 
		return -1;

	pTI->hwnd = m_hWnd;
	pTI->uId = (UINT)((row<<10)+(col&0x3ff)+1);
	pTI->lpszText = LPSTR_TEXTCALLBACK;

	pTI->rect = cellrect;

	return pTI->uId;
}

void CMyListCtrl::PreSubclassWindow() 
{
	CListCtrl::PreSubclassWindow();

	// Add initialization code
	EnableToolTips(TRUE);
}
