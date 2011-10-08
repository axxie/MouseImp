/**********************************************************************
resource clases
**********************************************************************/

#ifndef __SLWRES_H
#define __SLWRES_H

#pragma once


#include "SLWUtil.h"

#include "crtdbg.h"


_SLW_BEG


//////////////////////////////////////////////////////////////////////
//MFC AfxMessageBox like function


UINT SLWMessageBox(const HWND hWnd, const UINT uicResStr, const UINT uicResCap = 0, const UINT uicType = MB_OK, HINSTANCE hInst = GetModuleHandle(0));


//////////////////////////////////////////////////////////////////////
//CSLWIcon

class CSLWIcon
{
protected:

  //resource handle
  HICON hIcon;
  
public:

  enum IconSizeEnum
  {
    //load first finded
    esiDefault,
    eisSmall,
    eisNormal,
    eisOneAndHalf,
    eisDouble
  };

public:
  
  //empty
  inline CSLWIcon();
  //load on create (as load)
  inline CSLWIcon(LPCTSTR const cpcResName, const IconSizeEnum ecSize = esiDefault, HINSTANCE hInst = GetModuleHandle(0));
  //load on create (as load)
  inline CSLWIcon(const UINT uicResName, const IconSizeEnum ecSize = esiDefault, HINSTANCE hInst = GetModuleHandle(0));
  //destroy
  inline ~CSLWIcon();

  //load by name and passed resource module (0 == hInst - load std icon's - cpcResName as IDI_XXXX const's (LoadIcon))
  bool Load(LPCTSTR const cpcResName, const IconSizeEnum ecSize = esiDefault, HINSTANCE hInst = GetModuleHandle(0));
  inline bool Load(const UINT uicResName, const IconSizeEnum ecSize = esiDefault, HINSTANCE hInst = GetModuleHandle(0));
  //free
  inline void Free();
  //drop resource
  inline HICON Detach();

  //get data
  inline operator HICON() const;
  inline HICON Get() const;

  //is curr ok 
  inline bool IsOk() const;
  
protected:

};

//////////////////////////////////////////////////////////////////////
//CSLWMenu

class CSLWMenu
{
protected:

  //menu handle
  HMENU hMenu;
  //true if menu crated and need Destroy on free
  bool bCreated;
  
public:
  
public:

  //empty
  inline CSLWMenu();
  //from existing
  inline CSLWMenu(const HMENU hcInitMenu);
  //some as load
  inline CSLWMenu(LPCTSTR const cpcRes, HINSTANCE hInst = GetModuleHandle(0));
  //some as load
  inline CSLWMenu(const UINT uicResId, HINSTANCE hInst = GetModuleHandle(0));
  //free
  inline ~CSLWMenu();

  //load
  inline bool Load(LPCTSTR const cpcRes, HINSTANCE hInst = GetModuleHandle(0));
  inline bool Load(const UINT uicResId, HINSTANCE hInst = GetModuleHandle(0));

  //attach to menu
  inline void Attach(const HMENU hInitMenu);

  //get sub menu by cmd/pos
  inline HMENU GetSubMenu(const UINT uicPos);

  //mark passed mwnu item (by cmd) as "default" (bold)
  inline void MarkDefault(const UINT uicCmd);

  //free
  inline void Free();
  //drop
  inline HMENU Detach();

  //is menu present
  inline bool IsOk() const;

  //get
  inline operator HMENU() const;
  inline HMENU Get() const;

  //track as popup menu in cursor pos - as result is cmd of selected item
  inline UINT TrackPopUp(const HWND hcAnyWnd) const;
  //track as popup menu in passed pos and send selected cmd to wnd 
  inline BOOL TrackPopUpEx(const HWND hResivedWnd, const POINT& rcPnt) const;
  //track as popup menu in cursor pos - as result is cmd of selected item
  inline UINT TrackTray(const HWND hcMainWnd) const;
  //track as popup menu in passed pos and send selected cmd to wnd 
  inline BOOL TracktrayEx(const HWND hResivedWnd, const POINT& rcPnt) const;
  
protected:
};


/*********************************************************************
implementation
**********************************************************************/

//////////////////////////////////////////////////////////////////////
//CSLWIcon

inline CSLWIcon::CSLWIcon()
  :hIcon(0)
{
};

inline CSLWIcon::CSLWIcon(LPCTSTR const cpcResName, const IconSizeEnum ecSize, HINSTANCE hInst)
  :hIcon(0)
{
  const bool bcRes = Load(cpcResName, ecSize, hInst);
  _ASSERT(false != bcRes);
};

inline CSLWIcon::CSLWIcon(const UINT uicResName, const IconSizeEnum ecSize, HINSTANCE hInst)
  :hIcon(0)
{
  const bool bcRes = Load(uicResName, ecSize, hInst);
  _ASSERT(false != bcRes);
};

inline CSLWIcon::~CSLWIcon()
{
  Free();
};

inline bool CSLWIcon::Load(const UINT uicResName, const IconSizeEnum ecSize, HINSTANCE hInst)
{
  return Load(MAKEINTRESOURCE(uicResName), ecSize, hInst);
};

inline void CSLWIcon::Free()
{
  //nothing
  hIcon = 0;
};

inline HICON CSLWIcon::Detach()
{
  HICON hRes = hIcon;

  hIcon = 0;

  return hRes;
};

inline CSLWIcon::operator HICON() const
{
  return Get();
};

inline HICON CSLWIcon::Get() const
{
  return hIcon;
};

inline bool CSLWIcon::IsOk() const
{
  return 0 != hIcon;
};

//////////////////////////////////////////////////////////////////////
//CSLWMenu

inline CSLWMenu::CSLWMenu()
  :hMenu(0),
  bCreated(false)
{
};

inline CSLWMenu::CSLWMenu(const HMENU hcInitMenu)
  :hMenu(0),
  bCreated(false)
{
  Attach(hcInitMenu);
};

inline CSLWMenu::CSLWMenu(LPCTSTR const cpcRes, HINSTANCE hInst)
  :hMenu(0),
  bCreated(false)
{
  const bool bcRes = Load(cpcRes, hInst);
  _ASSERT(false != bcRes);
};

inline CSLWMenu::CSLWMenu(const UINT uicResId, HINSTANCE hInst)
  :hMenu(0),
  bCreated(false)
{
  const bool bcRes = Load(uicResId, hInst);
  _ASSERT(false != bcRes);
};

inline CSLWMenu::~CSLWMenu()
{
  Free();
};

inline bool CSLWMenu::Load(LPCTSTR const cpcRes, HINSTANCE hInst)
{
  bool bRes = false;

  Free();

  hMenu = ::LoadMenu(hInst, cpcRes);
  if(0 != hMenu)
  {
    bRes = true;
    bCreated = true;
  };

  return bRes;
};

inline void CSLWMenu::Attach(const HMENU hInitMenu)
{
  Free();
  hMenu = hInitMenu;
  bCreated = false;
};

inline HMENU CSLWMenu::GetSubMenu(const UINT uicPos)
{
  _ASSERT(false != IsOk());
  return ::GetSubMenu(Get(), uicPos);
};

inline void CSLWMenu::MarkDefault(const UINT uicCmd)
{
  _ASSERT(false != IsOk());
  const BOOL bcRes = ::SetMenuDefaultItem(Get(), uicCmd, FALSE);
  _ASSERT(FALSE != bcRes);
};

inline bool CSLWMenu::Load(const UINT uicResId, HINSTANCE hInst)
{
  return Load(MAKEINTRESOURCE(uicResId), hInst);
};

inline void CSLWMenu::Free()
{
  if(false != IsOk())
  {
    if(false != bCreated)
    {
      DestroyMenu(hMenu);
    };
    hMenu = 0;
    bCreated = false;
  };
};

inline HMENU CSLWMenu::Detach()
{
  HMENU hRes = hMenu;

  hMenu = 0;
  bCreated = false;

  return hRes;
};

inline bool CSLWMenu::IsOk() const
{
  return 0 != hMenu;
};

inline CSLWMenu::operator HMENU() const
{
  return Get();
};

inline HMENU CSLWMenu::Get() const
{
  _ASSERT(false != IsOk());
  return hMenu;
};

inline UINT CSLWMenu::TrackPopUp(const HWND hcAnyWnd) const
{
  _ASSERT(false != IsOk());
  POINT CurrPnt;
  GetCursorPos(&CurrPnt);
  const UINT uicRes = ::TrackPopupMenu
    (
    Get(),
    TPM_LEFTALIGN | TPM_NONOTIFY | TPM_RETURNCMD | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
    CurrPnt.x,
    CurrPnt.y,
    0,
    hcAnyWnd,
    0
    );
  return uicRes;
};

inline BOOL CSLWMenu::TrackPopUpEx(const HWND hResivedWnd, const POINT& rcPnt) const
{
  _ASSERT(false != IsOk());
  const UINT uicRes = ::TrackPopupMenu
    (
    Get(),
    TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
    rcPnt.x,
    rcPnt.y,
    0,
    hResivedWnd,
    0
    );
  return 0 != uicRes;
};

inline UINT CSLWMenu::TrackTray(const HWND hcMainWnd) const
{
  _ASSERT(false != IsOk());
  POINT Pnt;
  GetCursorPos(&Pnt);
  //activate Wnd
  ::SetForegroundWindow(hcMainWnd);
  const UINT uiTrackRes = ::TrackPopupMenu
    (
    Get(),
    TPM_RIGHTALIGN | TPM_NONOTIFY | TPM_RETURNCMD | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
    Pnt.x,
    Pnt.y,
    0,
    hcMainWnd,
    0
    );
  return uiTrackRes;
};

inline BOOL CSLWMenu::TracktrayEx(const HWND hResivedWnd, const POINT& rcPnt) const
{
  _ASSERT(false != IsOk());
  ::SetForegroundWindow(hResivedWnd);
  const UINT uicRes = ::TrackPopupMenu
    (
    Get(),
    TPM_RIGHTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
    rcPnt.x,
    rcPnt.y,
    0,
    hResivedWnd,
    0
    );
  return 0 != uicRes;
};



_SLW_END

#endif//__SLWRES_H