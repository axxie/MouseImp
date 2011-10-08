/**********************************************************************
splash screen
**********************************************************************/

#ifndef __SLWSPLASH_H
#define __SLWSPLASH_H

#pragma once

#include "SLWUtil.h"

_SLW_BEG

//////////////////////////////////////////////////////////////////////
//CSWSplash

class CSWSplash
{
protected:

  enum ClassConstEnum
  {
    //"hide timer"
    eccTimerId = 1,
    eccTimerTime = 2500
  };

  //visualisation 
  HBITMAP hBmp;
  HPALETTE hPal;

  //single instance only
  static CSWSplash* pThis;
  //wnd handle
  HWND hWnd;
  //"is splash present" flag
  bool bSplashPresent;
  
public:
  
public:

  CSWSplash();
  ~CSWSplash();

  //create splash wnd
  BOOL Create(const UINT uiResId, HINSTANCE hInstance, const COLORREF cNullColor);
  //create from BMP and rgn data
  BOOL Create(const UINT uicResBmpId, const UINT uicRgnId, HINSTANCE hInst);
  //destroy splash
  void Destroy();

  //wait for wnd of splash wnd
  void WaitForEnd();
  
protected:

  //create wnd and attach passed rgn
  BOOL CreateWnd(HRGN hRgn, HINSTANCE hInstance);
  //create bmp from passed mem
  BOOL CreateBmp(const BYTE* const cpcMem, const bool bcCheckColorNumb);
  //create bmp, pal and rgn from passed bmp mem
  BOOL CreateDataFromBmp(const BYTE* const cpcBmpData, HRGN& rRgn, const COLORREF cRgnNullColor);
  //wnd msg function 
  static LRESULT CALLBACK SlpashWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  LRESULT CALLBACK SlpashWndThisProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

_SLW_END

#endif//__SLWSPLASH_H