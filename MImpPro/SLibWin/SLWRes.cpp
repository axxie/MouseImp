/**********************************************************************
resource clases
**********************************************************************/

#define STRICT
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#pragma hdrstop


#include "SLWRes.h"


_SLW_BEG


//////////////////////////////////////////////////////////////////////
//MFC AfxMessageBox like function

UINT SLWMessageBox(const HWND hWnd, const UINT uicResStr, const UINT uicResCap, const UINT uicType, HINSTANCE hInst)
{
  UINT uiRes = 0;

  //try load string
  TCHAR cpBuff[2048];
  const int icSize = LoadString(hInst, uicResStr, cpBuff, COUNTOF(cpBuff));
  //string not found
  _ASSERT(0 != icSize);
  //string too long
  _ASSERT(COUNTOF(cpBuff) > icSize + 1);

  //try load caption 
  LPCSTR pCaption = 0;
  TCHAR cpCapBuff[100];
  if(0 != uicResCap && static_cast<UINT>(-1) != uicResCap)
  {
    const int icCapLen = LoadString(hInst, uicResCap, cpCapBuff, COUNTOF(cpCapBuff));
    //strign apsent
    _ASSERT(0 != icCapLen);
    //strign too long
    _ASSERT(COUNTOF(cpCapBuff) > icCapLen + 1);
    pCaption = cpCapBuff;
  };

  uiRes = MessageBox(hWnd, cpBuff, pCaption, uicType);

  return uiRes;
};


//////////////////////////////////////////////////////////////////////
//CSLWIcon

bool CSLWIcon::Load(LPCTSTR const cpcResName, const IconSizeEnum ecSize, HINSTANCE hInst)
{
  Free();

  SIZE Size;
  Size.cx = 0;
  Size.cy = 0;
  UINT uiFlags = LR_LOADMAP3DCOLORS | LR_SHARED;
  switch(ecSize)
  {
  case esiDefault:
    uiFlags |= LR_DEFAULTSIZE;
    break;

  case eisSmall:
    Size.cx = 16;
    Size.cy = 16;
    break;

  case eisNormal:
    Size.cx = 32;
    Size.cy = 32;
    break;

  case eisOneAndHalf:
    Size.cx = 48;
    Size.cy = 48;
    break;

  case eisDouble:
    Size.cx = 64;
    Size.cy = 64;
    break;

  default:
    break;
  };

  hIcon = reinterpret_cast<HICON>(::LoadImage(hInst, cpcResName, IMAGE_ICON, Size.cx, Size.cy, uiFlags));

  return 0 != hIcon;
};

_SLW_END