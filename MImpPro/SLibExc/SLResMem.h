/**********************************************************************
resource manipulation - loading
**********************************************************************/

#ifndef __SLRESMEM_H
#define __SLRESMEM_H
#pragma once

#include "SLUtil.h"

#include "SLDbg.h"

_SL_BEG

//////////////////////////////////////////////////////////////////////
//CSLModule - dynamicaly loaded module

class CSLModule
{
protected:

  //module
  HINSTANCE hInst;
  
public:
  
public:

  //init
  inline CSLModule();
  //init and load
  inline CSLModule(LPCTSTR const cpcModuleFileName);
  //quit
  inline ~CSLModule();

  //is module ok
  inline bool IsOk() const;  
  //free dll
  inline void Free();
  //load module
  inline bool Load(LPCTSTR const cpcModuleFileName);

  //for easy work
  inline operator HINSTANCE() const;

protected:
};




//////////////////////////////////////////////////////////////////////
//CSLResMem
//
//implement Win32 Resource loading and automatikaly freeing (like smart pointer)
//
//Example :
//
//void main()
//{
//  CSLResMem Res;
//  if(false != Res.Load(AfxGetResourceHandle(), IDR_SOME_JPG, "jpg"))
//  {
//    //some doing
//  };
//};
//

class CSLResMem
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
  inline CSLResMem();
  //cnstruct and load resource (like Open function)
  inline CSLResMem(HMODULE hMod, LPCTSTR const cpcResName, LPCTSTR const cpcResType);
  //cnstruct and load resource (like Open function)
  inline CSLResMem(HMODULE hMod, const UINT uicResId, LPCTSTR const cpcResType);
  //destruct and free loaded buffer's
  inline ~CSLResMem();

  //free loaded resource buffer's
  inline void Free();
  //load Win32 resource from hMod, cpcResName resource id (MAKEINTRESOURCE for UINT ResId)
  //and cpcResType type (look RT_Xxxx in FindResource func for predefined values)
  inline bool Load(HMODULE hMod, LPCTSTR const cpcResName, LPCTSTR const cpcResType);
  //like previouse
  inline bool Load(HMODULE hMod, const UINT uicResId, LPCTSTR const cpcResType);
  //is load process ok
  inline bool IsOk() const;

  //get size of loaded resource buffer
  inline DWORD GetSize() const;
  //get address of loaded resource buffer
  inline BYTE* GetPtr() const;

public:

protected:
};

//////////////////////////////////////////////////////////////////////
//CSLBmpMem

class CSLBmpMem
{
protected:

  enum ClassConstEnum
  {
    ////bmp data aligment
    eccDataAlign = sizeof(DWORD)
  };

  //pointer to memory
  BITMAPINFO* pBmpInfo;
  //bmp data memory
  VOID* pBmpData;   

public:

public:

  //empty
  inline CSLBmpMem();
  //for create from resource
  inline CSLBmpMem(BITMAPINFO* pBmpInfo);
  //from separeted mem
  inline CSLBmpMem(BITMAPINFO* const cpInfo, VOID* const cpMem);

  //is memory attached
  inline bool IsOk() const;

  //get bmp memory (actival image)
  inline BYTE* GetImgMem() const;
  //get start of "bmp palette" (RGBQUAD)
  inline RGBQUAD* GetPalMem() const;
  //get number of color's
  inline DWORD GetColorNumb() const;

  //get bmp hdr's
  inline BITMAPINFO* GetInfo() const;
  //get bmp line for passed vert coord
  inline BYTE* GetLine(const DWORD dwcVertCoord) const;
  //get vertical size of bmp
  inline DWORD GetVertSize() const;

  ////bmp geometry
  inline DWORD GetWidth() const;
  inline DWORD GetHeight() const;

  //create from resource (BITMAPINFO first)
  inline void FromResMem(void* const cpMem);
  //from res handler
  inline void FromResMem(const CSLResMem& rcMem);
  //create from file image
  inline bool FromFileMem(void* const cpMem);
  //from splited mem (png for example)
  inline void FromMem(BITMAPINFO* const cpInfo, VOID* const cpMem);

  //create bitmap and palette (if pointer passed) - null if some error
  HBITMAP CreateBmp(HDC hForDC, HPALETTE* const cpPal);

protected:
  //get image data mem from "bmpinfo/bmpdata" config
  static inline BYTE* GetImgMemInt(BITMAPINFO* const cpBmp);
  //get number of colo'r
  static inline DWORD GetColorNumbInt(const BITMAPINFOHEADER& rInfo);
};


/**********************************************************************
implemetation
**********************************************************************/

//////////////////////////////////////////////////////////////////////
//CSLModule

inline CSLModule::CSLModule()
  :hInst(0)
{
};

inline CSLModule::CSLModule(LPCTSTR const cpcModuleFileName)
  :hInst(0)
{
  Load(cpcModuleFileName);
};

inline CSLModule::~CSLModule()
{
  Free();
};

inline bool CSLModule::IsOk() const
{
  return 0 != hInst;
};

inline void CSLModule::Free()
{
  if(0 != hInst)
  {
    ::FreeLibrary(hInst);
    hInst = 0;
  };
};

inline bool CSLModule::Load(LPCTSTR const cpcModuleFileName)
{
  Free();
  hInst = ::LoadLibrary(cpcModuleFileName);
  return 0 != hInst;
};

inline CSLModule::operator HINSTANCE() const
{
  _ASSERT(false != IsOk());
  return hInst;
};


//////////////////////////////////////////////////////////////////////
//CSLResMem

inline CSLResMem::CSLResMem()
  :hResource(0),
  hResMod(0),
  hTemplate(0),
  pResMem(0)
{
};

inline CSLResMem::CSLResMem(HMODULE hMod, const UINT uicResId, LPCTSTR const cpcResType)
  :hResource(0),
  hResMod(0),
  hTemplate(0),
  pResMem(0)
{
  SLCHECK(false != Load(hMod, uicResId, cpcResType));
};

inline CSLResMem::CSLResMem(HMODULE hMod, LPCTSTR const cpcResName, LPCTSTR const cpcResType)
  :hResource(0),
  hResMod(0),
  hTemplate(0),
  pResMem(0)
{
  SLCHECK(false != Load(hMod, cpcResName, cpcResType));
};

inline CSLResMem::~CSLResMem()
{
  Free();
};

inline void CSLResMem::Free()
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

inline bool CSLResMem::Load(HMODULE hMod, const UINT uicResId, LPCTSTR const cpcResType)
{
  return Load(hMod, MAKEINTRESOURCE(uicResId), cpcResType);
};

inline bool CSLResMem::Load(HMODULE hMod, LPCTSTR const cpcResName, LPCTSTR const cpcResType)
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

inline bool CSLResMem::IsOk() const
{
  return (0 != pResMem);
};

inline DWORD CSLResMem::GetSize() const
{
  return ::SizeofResource(hResMod, hResource);
};

inline BYTE* CSLResMem::GetPtr() const
{
  return pResMem;
};

//////////////////////////////////////////////////////////////////////
//CSLBmpMem

inline CSLBmpMem::CSLBmpMem()
  :pBmpInfo(0),
  pBmpData(0)
{
};

inline CSLBmpMem::CSLBmpMem(BITMAPINFO* pBmpInfo)
  :pBmpInfo(0),
  pBmpData(0)
{
  FromResMem(pBmpInfo);
};

inline CSLBmpMem::CSLBmpMem(BITMAPINFO* const cpInfo, VOID* const cpMem)
{
  FromMem(cpInfo, cpMem);
};

inline bool CSLBmpMem::IsOk() const
{
  return 0 != pBmpInfo;
};

inline RGBQUAD* CSLBmpMem::GetPalMem() const
{
  return reinterpret_cast<RGBQUAD*>(reinterpret_cast<BYTE*>(&(pBmpInfo->bmiHeader)) + pBmpInfo->bmiHeader.biSize);
};

inline BYTE* CSLBmpMem::GetImgMem() const
{
  return reinterpret_cast<BYTE*>(pBmpData);
};

inline BYTE* CSLBmpMem::GetImgMemInt(BITMAPINFO* const cpBmp)
{
  BYTE* pRes = 0;
  const BITMAPINFOHEADER& rBmpInfo = cpBmp->bmiHeader;
  if(8 < rBmpInfo.biBitCount)
  {
    pRes = reinterpret_cast<BYTE*>(reinterpret_cast<DWORD*>(cpBmp->bmiColors + rBmpInfo.biClrUsed) + ((BI_BITFIELDS == rBmpInfo.biCompression) ? 3 : 0));
  }
  else
  {
    pRes = reinterpret_cast<BYTE*>(cpBmp->bmiColors + GetColorNumbInt(cpBmp->bmiHeader));
  };
  return pRes;
};

inline void CSLBmpMem::FromResMem(const CSLResMem& rcMem)
{
  _ASSERT(false != rcMem.IsOk());
  FromResMem(rcMem.GetPtr());
};

inline void CSLBmpMem::FromResMem(void* const cpMem)
{
  pBmpInfo = reinterpret_cast<BITMAPINFO*>(cpMem);
  pBmpData = reinterpret_cast<VOID*>(GetImgMemInt(pBmpInfo));
};

inline void CSLBmpMem::FromMem(BITMAPINFO* const cpInfo, VOID* const cpMem)
{
  pBmpInfo = cpInfo;
  pBmpData = cpMem;   
};

inline bool CSLBmpMem::FromFileMem(void* const cpMem)
{
  bool bRes = false;
  BITMAPFILEHEADER* const cpFileHdr = reinterpret_cast<BITMAPFILEHEADER*>(cpMem);
  //must be bmp ('BM' at start)
  if(MAKEWORD('B', 'M') == cpFileHdr->bfType)
  {
    FromResMem(reinterpret_cast<BYTE*>(cpFileHdr + 1));
    bRes = true;
  };
  return bRes;
};

inline DWORD CSLBmpMem::GetColorNumb() const
{
  return GetColorNumbInt(pBmpInfo->bmiHeader);
};

inline DWORD CSLBmpMem::GetColorNumbInt(const BITMAPINFOHEADER& rInfo) 
{
  return (0 != rInfo.biClrUsed)
    ? rInfo.biClrUsed
    : 1 << rInfo.biBitCount;
};

inline BITMAPINFO* CSLBmpMem::GetInfo() const
{
  return pBmpInfo;
};

inline BYTE* CSLBmpMem::GetLine(const DWORD dwcVertCoord) const
{
  const BITMAPINFOHEADER& rHdr = pBmpInfo->bmiHeader;
  //invalid coord
  _ASSERT(dwcVertCoord < GetVertSize());
  const DWORD dwcLinePane = (rHdr.biBitCount / CHAR_BIT) * (rHdr.biWidth + eccDataAlign - 1) & (~(eccDataAlign - 1));
  const DWORD dwcRealCoord = (0 > rHdr.biHeight)
    ? rHdr.biHeight - 1 - dwcVertCoord
    : dwcVertCoord;
  return reinterpret_cast<BYTE*>(pBmpData) + dwcRealCoord * dwcLinePane;
};

inline DWORD CSLBmpMem::GetVertSize() const
{
  _ASSERT(false != IsOk());
  const BITMAPINFOHEADER& rHdr = pBmpInfo->bmiHeader;
  return static_cast<DWORD>(max(rHdr.biHeight, -rHdr.biHeight));
};

inline DWORD CSLBmpMem::GetWidth() const
{
  _ASSERT(false != IsOk());
  return pBmpInfo->bmiHeader.biWidth;
};

inline DWORD CSLBmpMem::GetHeight() const
{
  _ASSERT(false != IsOk());
  return GetVertSize();
};



_SL_END

#endif//__SLRESMEM_H