/**********************************************************************
splash screen
**********************************************************************/

#define STRICT
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#pragma hdrstop


#include "SLWSplash.h"


#include <crtdbg.h>

#include <tchar.h>


_SLW_BEG


//////////////////////////////////////////////////////////////////////
//class for work with res's

class __CSLResMem
{
protected:

  //find resource handle
  HRSRC hResource;
  //module handle for resource
  HMODULE hResMod;

  //loaded resource handle's
  HGLOBAL hTemplate;
  BYTE* pResMem;

public:

  //construct empty
  inline __CSLResMem();
  //destruct and free loaded buffer's
  inline ~__CSLResMem();

  //free loaded resource buffer's
  inline void Free();
  //load Win32 resource from hMod, cpcResName resource id (MAKEINTRESOURCE for UINT ResId)
  //and cpcResType type (look RT_Xxxx in FindResource func for predefined values)
  inline bool Load(HMODULE hMod, LPCTSTR const cpcResName, LPCTSTR const cpcResType);
  //is load process ok
  inline bool IsOk() const;

  //get size of loaded resource buffer
  inline DWORD GetSize() const;
  //get address of loaded resource buffer
  inline BYTE* GetPtr() const;

public:

protected:
};


inline __CSLResMem::__CSLResMem()
  :hResource(0),
  hResMod(0),
  hTemplate(0),
  pResMem(0)
{
};

inline __CSLResMem::~__CSLResMem()
{
  Free();
};

inline void __CSLResMem::Free()
{
  if(0 != pResMem)
  {
    UnlockResource(pResMem);
    pResMem = 0;
  };
  if(0 != hTemplate)
  {
    FreeResource(hTemplate);
    hTemplate = 0;
  };
  hResource = 0;
  hResMod = 0;
};

inline bool __CSLResMem::Load(HMODULE hMod, LPCTSTR const cpcResName, LPCTSTR const cpcResType)
{
  bool bRes = false;
  Free();

  hResMod = hMod;
  hResource = ::FindResource(hResMod, cpcResName, cpcResType);
  if(0 != hResource)
  {
    hTemplate = ::LoadResource(hResMod, hResource);
    if(0 != hTemplate)
    {
      pResMem = reinterpret_cast<BYTE*>(::LockResource(hTemplate));
      if(0 != pResMem)
      {
        bRes = true;
      };
    };
  };

  return bRes;
};

inline bool __CSLResMem::IsOk() const
{
  return (0 != pResMem);
};

inline DWORD __CSLResMem::GetSize() const
{
  return ::SizeofResource(hResMod, hResource);
};

inline BYTE* __CSLResMem::GetPtr() const
{
  return pResMem;
};


//////////////////////////////////////////////////////////////////////
//global's

static LPCTSTR const cpcClassName = _T("SLWSplashWndClass");
static LPCTSTR const cpcRgnResType = _T("RGN");


//get color ref for pixel 
static inline RGBTRIPLE GetPixelColor(const BYTE* const cpcLineBaseAddr, const DWORD dwcHorPos, const RGBQUAD* const cpcColorTableBase)
{
  RGBTRIPLE Res;
  const BYTE cPixel = cpcLineBaseAddr[dwcHorPos];
  //color value for pixel
  Res.rgbtBlue = cpcColorTableBase[cPixel].rgbBlue;
  Res.rgbtGreen = cpcColorTableBase[cPixel].rgbGreen;
  Res.rgbtRed = cpcColorTableBase[cPixel].rgbRed;
  return Res;
};

//compare two passec color's
static inline bool CompareColors(const RGBTRIPLE& rcFirst, const RGBTRIPLE& rcSecond)
{
  return rcFirst.rgbtBlue == rcSecond.rgbtBlue 
    && rcFirst.rgbtGreen == rcSecond.rgbtGreen
    && rcFirst.rgbtRed == rcSecond.rgbtRed;
};




//////////////////////////////////////////////////////////////////////
//CSWSplash

CSWSplash* CSWSplash::pThis = 0;

CSWSplash::CSWSplash()
  :hBmp(0),
  hPal(0),
  hWnd(0),
  bSplashPresent(false)
{
  _ASSERT(0 == pThis);
  pThis = this;  
};

CSWSplash::~CSWSplash()
{
  //unlock "this"
  pThis = 0;
};


LRESULT CALLBACK CSWSplash::SlpashWndThisProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  bool bCatched = true;
  LRESULT lRes = 0;

  switch(message)
  {
    //mosue click
  case WM_LBUTTONDOWN:
  case WM_RBUTTONDOWN:
  case WM_MBUTTONDOWN:
    Destroy();
    break;

  case WM_TIMER:
    switch(wParam)
    {
      //delete wnd on timer
    case eccTimerId:
      Destroy();
      break;

    default:
      bCatched = false;
      break;
    };

    break;

    //draw
  case WM_PAINT:
    {
      PAINTSTRUCT Info;
      HDC hDC = ::BeginPaint(hWnd, &Info);

      //draw bmp
      BITMAP BmpInfo;
      ::GetObject(hBmp, sizeof(BmpInfo), &BmpInfo);
      HDC hMemDC = ::CreateCompatibleDC(hDC);
      ::SelectObject(hMemDC, hBmp);
      ::SelectPalette(hDC, hPal, FALSE);
      ::RealizePalette(hDC);
      ::BitBlt(hDC, 0, 0, BmpInfo.bmWidth, BmpInfo.bmHeight, hMemDC, 0, 0, SRCCOPY);

      ::DeleteDC(hMemDC);

      ::EndPaint(hWnd, &Info);
    };
    break;

  default:
    bCatched = false;
    break;
  };


  return (false != bCatched)
    ? lRes
    : ::DefWindowProc(hWnd, message, wParam, lParam);
};

LRESULT CALLBACK CSWSplash::SlpashWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  return pThis->SlpashWndThisProc(hWnd, message, wParam, lParam);
};

BOOL CSWSplash::Create(const UINT uicResBmpId, const UINT uicRgnId, HINSTANCE hInst)
{
  BOOL bRes = TRUE;

  HRGN hRgn = 0;

  //load rgn from res's
  if(FALSE != bRes && 0 != uicRgnId)
  {
    __CSLResMem Res;
    bRes = Res.Load(hInst, MAKEINTRESOURCE(uicRgnId), cpcRgnResType);
    if(FALSE != bRes)
    {
      hRgn = ::ExtCreateRegion(0, Res.GetSize(), reinterpret_cast<RGNDATA*>(Res.GetPtr()));
      bRes = 0 != hRgn;
    };
  };

  //try load bmp
  if(FALSE != bRes)
  {
    __CSLResMem Res;
    bRes = Res.Load(hInst, MAKEINTRESOURCE(uicResBmpId), RT_BITMAP);
    if(FALSE != bRes)
    {
      //create pal from bmp file
      bRes = CreateBmp(Res.GetPtr(), false);
    };
  };

  //create wnd
  if(FALSE != bRes)
  {
    bRes = CreateWnd(hRgn, hInst);
  };

  return bRes;
};

BOOL CSWSplash::CreateWnd(HRGN hRgn, HINSTANCE hInstance)
{
  BOOL bRes = FALSE;
  
  //register wnd class
  WNDCLASS Class;
  ZeroMemory(&Class, sizeof(Class));
  Class.style = CS_NOCLOSE;
  Class.hInstance = hInstance;
  Class.lpfnWndProc = reinterpret_cast<WNDPROC>(SlpashWndProc);
  Class.lpszClassName = cpcClassName;
  Class.hCursor = ::LoadCursor(0, IDC_ARROW);
  if(::RegisterClass(&Class))
  {
    //size of bmp
    BITMAP BmpInfo;
    ::GetObject(hBmp, sizeof(BmpInfo), &BmpInfo);
    const int icScrH = ::GetSystemMetrics(SM_CXFULLSCREEN);
    const int icScrV = ::GetSystemMetrics(SM_CYFULLSCREEN);
    const int icLeft = icScrH / 2 - BmpInfo.bmWidth / 2;
    const int icTop = icScrV / 2 - BmpInfo.bmHeight / 2;
    hWnd = ::CreateWindow
      (
      cpcClassName,
      0,
      WS_POPUP | WS_VISIBLE,
      icLeft,
      icTop,
      BmpInfo.bmWidth,
      BmpInfo.bmHeight,
      0,
      0,
      hInstance,
      0
      );
    if(0 != hWnd)
    {
      //attach rgn
      if(0 != hRgn)
      {
        ::SetWindowRgn(hWnd, hRgn, TRUE);
      };
      //start timer
      ::SetTimer(hWnd, eccTimerId, eccTimerTime, 0);
      //refresh
      ::UpdateWindow(hWnd);
      bRes = TRUE;
    };
  };
  //if creae ok - set "splash present" flag
  bSplashPresent = FALSE != bRes;

  return bRes;
};

BOOL CSWSplash::Create(const UINT uiResId, HINSTANCE hInstance, const COLORREF cNullColor)
{
  //try load bmp from res's and create it's
  //try create rgn from bmp
  //create wnd and attach rgn

  BOOL bRes = TRUE;

  //easy
  HRGN hRgn = 0;

  //load/create
  if(FALSE != bRes)
  {
    __CSLResMem Res;
    bRes = Res.Load(hInstance, MAKEINTRESOURCE(uiResId), RT_BITMAP);
    if(FALSE != bRes)
    {
      const BYTE* cpMem = Res.GetPtr();
      //process bmp
      bRes = CreateDataFromBmp(cpMem, hRgn, cNullColor);
    };
  };

  //create wnd
  if(FALSE != bRes)
  {
    bRes = CreateWnd(hRgn, hInstance);
  };

  return bRes;
};

BOOL CSWSplash::CreateBmp(const BYTE* const cpcMem, const bool bcCheckColorNumb)
{
  BOOL bRes = FALSE;
  
  //bmp data
  const BITMAPINFO* const cpcInfo = reinterpret_cast<const BITMAPINFO*>(cpcMem);
  const BITMAPINFOHEADER* const cpcInfoHdr = reinterpret_cast<const BITMAPINFOHEADER*>(&(cpcInfo->bmiHeader));
  
  //wnd dc
  HDC hDC = ::GetWindowDC(0);
  
  if(false != bcCheckColorNumb)
  {
    //only 8 bit's per pixel
    _ASSERT(8 == cpcInfoHdr->biBitCount);
    //not compression
    _ASSERT(BI_RGB == cpcInfoHdr->biCompression);
  };
  const DWORD dwcColNumb = (0 == cpcInfoHdr->biClrUsed)
    ? 1 << cpcInfoHdr->biBitCount
    : cpcInfoHdr->biClrUsed;
  
  const DWORD dwcPalMassSize = sizeof(LOGPALETTE) + sizeof(PALETTEENTRY) * (dwcColNumb - 1);
  LOGPALETTE* const cpPalMass = reinterpret_cast<LOGPALETTE*>(new BYTE[dwcPalMassSize]);
  
  cpPalMass->palVersion = 0x300;     
  cpPalMass->palNumEntries = static_cast<WORD>(dwcColNumb);
  for(DWORD dwCount = 0; dwcColNumb > dwCount; dwCount++)
  {
    cpPalMass->palPalEntry[dwCount].peRed = cpcInfo->bmiColors[dwCount].rgbRed;    
    cpPalMass->palPalEntry[dwCount].peGreen = cpcInfo->bmiColors[dwCount].rgbGreen;
    cpPalMass->palPalEntry[dwCount].peBlue = cpcInfo->bmiColors[dwCount].rgbBlue;  
    cpPalMass->palPalEntry[dwCount].peFlags = 0;                           
  };
  //create pal
  hPal = ::CreatePalette(cpPalMass);
  HPALETTE hOldPal = ::SelectPalette(hDC, hPal, FALSE);
  ::RealizePalette(hDC);
  
  //bitmap
  const BYTE* const cpcImgMem = reinterpret_cast<const BYTE*>(cpcInfo->bmiColors + dwcColNumb);
  hBmp = ::CreateDIBitmap(hDC, cpcInfoHdr, CBM_INIT, cpcImgMem, cpcInfo, DIB_RGB_COLORS);

  bRes = 0 != hBmp;

  //free all
  //select old pal
  ::SelectPalette(hDC, hOldPal, FALSE);
  //pal mass
  delete cpPalMass;
  //dc
  ::ReleaseDC(0, hDC);

  return bRes;
};

BOOL CSWSplash::CreateDataFromBmp(const BYTE* const cpcBmpData, HRGN& rRgn, const COLORREF cRgnNullColor)
{
  //create bmp/pal
  //scan bmp for build rgn

  BOOL bRes = TRUE;

  //bmp/pal
  if(FALSE != bRes)
  {
    //bmp data
    const BITMAPINFO* const cpcInfo = reinterpret_cast<const BITMAPINFO*>(cpcBmpData);
    const BITMAPINFOHEADER* const cpcInfoHdr = reinterpret_cast<const BITMAPINFOHEADER*>(&(cpcInfo->bmiHeader));

    const DWORD dwcColNumb = (0 == cpcInfoHdr->biClrUsed)
      ? 1 << cpcInfoHdr->biBitCount
      : cpcInfoHdr->biClrUsed;

    //rgn
    const bool bcMakeRGN = RGB(255, 255, 255) >= cRgnNullColor;
    if(FALSE != CreateBmp(cpcBmpData, bcMakeRGN))
    {
      if(false != bcMakeRGN)
      {
        const RGBTRIPLE cNullColor = {GetRValue(cRgnNullColor), GetGValue(cRgnNullColor), GetBValue(cRgnNullColor)};
        //size of bmp
        //alloc arraow of RECT's for rgn's
        //scan bmp by line's and build "empty" point's to RECT's memory
        //combine each line to "resulr" rgn
        
        //size
        const DWORD dwcWidth = cpcInfoHdr->biWidth;
        const DWORD dwcAllignWidth = (dwcWidth + 3) & (~3);
        const DWORD dwcHeight = max(cpcInfoHdr->biHeight, -cpcInfoHdr->biHeight);
        const bool bcBmpInv = 0 < cpcInfoHdr->biHeight;
        //start of bmp img data
        const BYTE* const cpcBmpData = 
          //start
          reinterpret_cast<const BYTE*>(cpcInfo) 
          //size of hdr
          + cpcInfo->bmiHeader.biSize 
          //size of color table
          + sizeof(cpcInfo->bmiColors[0]) * dwcColNumb;
        
        //memory 
        //(allocate memory for "bad" situation - each line contatin "null"/"not null" pair's - when number of rgn rect's is max and equal to Width / 2)
        const DWORD dwcRgnMemSize = sizeof(RGNDATA) + sizeof(RECT) * dwcWidth / 2;
        RGNDATA* const cpRgnData = reinterpret_cast<RGNDATA*>(new BYTE[dwcRgnMemSize]);
        RGNDATAHEADER& rRgnHdr = cpRgnData->rdh;
        rRgnHdr.dwSize = sizeof(rRgnHdr);
        rRgnHdr.iType = RDH_RECTANGLES;
        rRgnHdr.nRgnSize = 0;
        
        //scan lines
        for(DWORD dwVert = 0; dwcHeight > dwVert; dwVert++)
        {
          //start of line bmp pixel's for "this" vertical pos
          const BYTE* const cpcLineStartAddr = (false != bcBmpInv)
            ? ((dwcHeight - dwVert - 1) * dwcAllignWidth) + cpcBmpData
            : (dwcHeight * dwcAllignWidth) + cpcBmpData;
          //start of color data in bmp
          const RGBQUAD* const cpcColorTable = cpcInfo->bmiColors;
          //prepare rgn's RECT data for scan "this" line
          rRgnHdr.nCount = 0;
          //scan pizel's in line
          for(DWORD dwHor = 0; dwcWidth > dwHor; dwHor++)
          {
            const RGBTRIPLE PixelColor = GetPixelColor(cpcLineStartAddr, dwHor, cpcColorTable);
            //compare
            if(false == CompareColors(PixelColor, cNullColor))
            {
              //start of "null color" rect (left)
              const DWORD dwFromNull = dwHor;
              //it's null color - scan "while null's color present"
              for(dwHor++; dwcWidth > dwHor; dwHor++)
              {
                const RGBTRIPLE LookPixelColor = GetPixelColor(cpcLineStartAddr, dwHor, cpcColorTable);
                if(false != CompareColors(LookPixelColor, cNullColor))
                {
                  break;
                };
              };
              //end of "null color rect" (right)
              const DWORD dwToNull = dwHor;
              //move "null color rect" to rgn
              RECT& rNullRect = reinterpret_cast<RECT*>(cpRgnData->Buffer)[rRgnHdr.nCount];
              rNullRect.left = dwFromNull;
              rNullRect.right = dwToNull;
              rNullRect.top = dwVert;
              rNullRect.bottom = dwVert + 1;
              rRgnHdr.nCount++;
            };
          };
          //after processing - push rgn's RECT data to rgn
          if(0 != rRgnHdr.nCount)
          {
            //if "summ" rgn is empty - create rgn from data
            //else - create from data, combine "summ" and "this", del "summ", put "this" to "summ"
            
            //"this" (create)
            //sizeof rgn data
            HRGN hThisRgn = ::ExtCreateRegion(0, dwcRgnMemSize, cpRgnData);
            if(0 != hThisRgn)
            {
              if(0 == rRgn)
              {
                //from "this" to "summ"
                rRgn = hThisRgn;
              }
              else
              {
                //combine "this" and "summ"
                const int icRes = ::CombineRgn(rRgn, hThisRgn, rRgn, RGN_OR);
                _ASSERT(ERROR != icRes && NULLREGION != icRes);
                //delete "this" rgn 
                ::DeleteObject(hThisRgn);
              };
            };
          };
        };
        //rgn mass
        delete cpRgnData;
      }
      else
      {
        bRes = true;
      };
    };   
  };

  return bRes;
};

void CSWSplash::WaitForEnd()
{
  MSG Msg;
  while(false != bSplashPresent && ::GetMessage(&Msg, 0, 0, 0))
  {
    DispatchMessage(&Msg);
  };
};

void CSWSplash::Destroy()
{
  if(false != bSplashPresent)
  {
    //handle of instance of application
    HINSTANCE hInst = reinterpret_cast<HINSTANCE>(::GetWindowLongPtr(hWnd, GWLP_HINSTANCE));
    //timer
    ::KillTimer(hWnd, eccTimerId);
    //wnd
    ::DestroyWindow(hWnd);
    hWnd = 0;
    //pal
    ::DeleteObject(hPal);
    //bmp
    ::DeleteObject(hBmp);
    //unregister class
    ::UnregisterClass(cpcClassName, hInst);
    bSplashPresent = false;
  };
};



_SLW_END