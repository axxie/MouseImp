/**********************************************************************
Dbg helper's
**********************************************************************/

#define STRICT
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#pragma hdrstop


#include "SLWDbg.h"


_SLW_BEG


void FlashWnd(const HWND hcWnd)
{
  //get wnd rect
  //get wnd dc
  //insert rect end free dc

  RECT Rect;
  ::GetWindowRect(hcWnd, &Rect);

  HDC hDC = ::GetDC(0);
  if(0 != hDC)
  {
    ::InvertRect(hDC, &Rect);

    ::ReleaseDC(0, hDC);
  };

};


_SLW_END
