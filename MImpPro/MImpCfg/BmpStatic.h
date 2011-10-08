/**********************************************************************
bitmap draw ctrl
**********************************************************************/

#if !defined(AFX_BMPSTATIC_H__95538B09_8E6F_11D2_B108_00A0C907DA21__INCLUDED_)
#define AFX_BMPSTATIC_H__95538B09_8E6F_11D2_B108_00A0C907DA21__INCLUDED_

#pragma once


#include "clickStatic.h"


/////////////////////////////////////////////////////////////////////////////
// CBmpStatic window

class CBmpStatic : public CClickStatic
{
protected:

  //attached to ctrl bitmap
  CBitmap Bmp;
  //attached to ctrl palette
  CPalette Pal;
  
public:
  
public:
  
	CBmpStatic(const bool bcInitLikeUrl);
	virtual ~CBmpStatic();

  //try load bmp from resource's
  void LoadBmpFromRes(const UINT uicResId);
  //get size of bmp
  inline CSize GetBmpSize() const;

protected:

protected:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBmpStatic)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CBmpStatic)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/**********************************************************************
implementation
**********************************************************************/

//////////////////////////////////////////////////////////////////////
//CBmpStatic

inline CSize CBmpStatic::GetBmpSize() const
{
  BITMAP Info;
  Bmp.GetObject(sizeof(Info), &Info);
  return CSize(Info.bmWidth, Info.bmHeight);;
};



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BMPSTATIC_H__95538B09_8E6F_11D2_B108_00A0C907DA21__INCLUDED_)
