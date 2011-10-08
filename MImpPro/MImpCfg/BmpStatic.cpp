/**********************************************************************
bitmap draw ctrl
**********************************************************************/

#include "stdafx.h"


#include "BmpStatic.h"


#include "..\\SlibExc\\SLResMem.h"

#include "..\\PngLib\\PngPub.h"

#include "..\\SlibExc\\SLAutoPtr.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBmpStatic

CBmpStatic::CBmpStatic(const bool bcInitLikeUrl)
  :CClickStatic(bcInitLikeUrl)
{
}

CBmpStatic::~CBmpStatic()
{
}


BEGIN_MESSAGE_MAP(CBmpStatic, CClickStatic)
	//{{AFX_MSG_MAP(CBmpStatic)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CBmpStatic::LoadBmpFromRes(const UINT uicResId)
{
  //free corrent object's
  //try load bmp from res's (by SlibExc)
  //create object's
  //set draw

  //free
  Bmp.DeleteObject();
  Pal.DeleteObject();

  //try load
  using sl::CSLResMem;
  using sl::CSLBmpMem;
  //res mem 
  CSLResMem BmpResMem;
  SLCHECK(false != BmpResMem.Load(AfxGetResourceHandle(), uicResId, _T("PNG")));

  using sl::CSLAutoPtr;
  CSLAutoPtr<BITMAPINFO> pBmpInfo;
  CSLAutoPtr<BYTE> pBmpData;
  SLCHECK(false != PngCreateBmpFromRes(BmpResMem.GetPtr(), BmpResMem.GetSize(), &pBmpInfo, reinterpret_cast<void**>(&pBmpData)));

  //create 
  CSLBmpMem BmpMem;
  BmpMem.FromMem(pBmpInfo, pBmpData);  
  HPALETTE hPal = 0;
  CClientDC DC(this);
  HBITMAP hBmp = BmpMem.CreateBmp(DC, &hPal);
  _ASSERT(0 != hBmp);
  //attach object's
  Bmp.Attach(hBmp);
  Pal.Attach(hPal);
  //draw
  Invalidate();
};

void CBmpStatic::OnPaint() 
{
  //draw bmp in dc

  //easy
	CPaintDC DC(this);

  //draw
  CDC MemDC;
  MemDC.CreateCompatibleDC(&DC);
  MemDC.SelectObject(&Bmp);
  //ctrl rect
  CRect Rect;
  GetClientRect(&Rect);
  //palette
  CPalette* const cpOldPal = DC.SelectPalette(&Pal, FALSE);
  DC.RealizePalette();
  //draw
  DC.BitBlt
    (
    Rect.left,
    Rect.top,
    Rect.Width(),
    Rect.Height(),
    &MemDC, 
    Rect.left,
    Rect.top,
    SRCCOPY
    );

  //restore 
  DC.SelectPalette(cpOldPal, TRUE);
}
