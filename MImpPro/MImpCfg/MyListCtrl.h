/**********************************************************************
MyListCtrl (alpha)
**********************************************************************/
#if !defined(AFX_MYLISTCTRL_H__A21F7BB5_7AF7_11D2_B0E9_00A0C907DA21__INCLUDED_)
#define AFX_MYLISTCTRL_H__A21F7BB5_7AF7_11D2_B0E9_00A0C907DA21__INCLUDED_

#pragma once


//////////////////////////////////////////////////////////////////////
//CListColInfo - ended by -1 in uiResId

struct CListColInfo
{
  //collumn label (from res)
  UINT uiResId;
  //col subitem (for later navigation)
  UINT uiSubItem;
  //width of col (relative to sum of all col's)
  UINT uiWidth;
};

/////////////////////////////////////////////////////////////////////////////
// CMyListCtrl window

class CMyListCtrl : public CListCtrl
{
protected:
  
  //image list for "sort arrow" in header ctrl
  CImageList HdrImgList;

  //sort info
  int iSortCol;
  bool bSortPos;
  //pos of bmp in hdr ctrl
  int iHdrColBmp;
  
public:
  
public:

  CMyListCtrl();
  ~CMyListCtrl();

  //init list ctrl
  void Init(const CListColInfo* const cpcColInitInfo);

  //resort list
  void CallSort(const bool bcLookOld);
  
protected:

  int CellRectFromPoint(CPoint & point, RECT * cellrect, int * col) const;

protected:

  int OnToolHitTest(CPoint point, TOOLINFO * pTI) const;
  void PreSubclassWindow();
  BOOL OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult );
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyListCtrl)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMyListCtrl)
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYLISTCTRL_H__A21F7BB5_7AF7_11D2_B0E9_00A0C907DA21__INCLUDED_)
