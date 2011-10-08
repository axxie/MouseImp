/**********************************************************************
resource manipulation - loading
**********************************************************************/

#define STRICT
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#pragma hdrstop


#include "SLResMem.h"

_SL_BEG

HBITMAP CSLBmpMem::CreateBmp(HDC hMakeForDC, HPALETTE* const cpPal)
{
  HBITMAP hRes = 0;

  HDC hConvDC = (0 != hMakeForDC)
    ? hMakeForDC
    : ::GetWindowDC(0);

  const DWORD dwcColNumb = GetColorNumb();
  BYTE* const cpImgMem = GetImgMem();

  //create palette - if need and number color valid
  HPALETTE hPal = 0;
  HPALETTE hOldPal = 0;
  if(256 <= dwcColNumb)
  {
    const DWORD dwcPalMassSize = sizeof(LOGPALETTE) + sizeof(PALETTEENTRY) * (dwcColNumb - 1);
    LOGPALETTE* const cpPalMass = reinterpret_cast<LOGPALETTE*>(new BYTE[dwcPalMassSize]);

    cpPalMass->palVersion = 0x300;     
    cpPalMass->palNumEntries = static_cast<WORD>(dwcColNumb);
    for(DWORD dwCount = 0; dwcColNumb > dwCount; dwCount++)
    {
      cpPalMass->palPalEntry[dwCount].peRed = pBmpInfo->bmiColors[dwCount].rgbRed;    
      cpPalMass->palPalEntry[dwCount].peGreen = pBmpInfo->bmiColors[dwCount].rgbGreen;
      cpPalMass->palPalEntry[dwCount].peBlue = pBmpInfo->bmiColors[dwCount].rgbBlue;  
      cpPalMass->palPalEntry[dwCount].peFlags = 0;                           
    };
    hPal = CreatePalette(cpPalMass);
    hOldPal = ::SelectPalette(hConvDC, hPal, FALSE);
    ::RealizePalette(hConvDC);

    delete[] cpPalMass;
  };

  //create bmp
  hRes = CreateDIBitmap
    (
    hConvDC, 
    &(pBmpInfo->bmiHeader),
    CBM_INIT,
    cpImgMem,
    pBmpInfo,
    DIB_RGB_COLORS
    );

  //restore pal for DC
  if(0 != hOldPal)
  {
    SelectPalette(hConvDC, hOldPal, FALSE);
  };
  if(0 == hMakeForDC)
  {
    //drop dc if for entire screen
    ::ReleaseDC(0, hConvDC);
  };
  //if need palette - pass
  if(0 != cpPal)
  {
    *cpPal = hPal;
  }
  else
  {
    if(0 != hPal)
    {
      DeleteObject(hPal);
    };
  };

  return hRes;
};


_SL_END

