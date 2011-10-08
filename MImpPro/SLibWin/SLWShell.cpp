/**********************************************************************
shell wrapper's
**********************************************************************/

#define STRICT
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#pragma hdrstop


#include "SLWShell.h"


#include <SHLOBJ.H>


_SLW_BEG

////select item helper
static int CALLBACK __SLWSelFolderCB(HWND hWnd, UINT uiMsg, LPARAM lParam, LPARAM lDataParam)
{
  int iRes = 0;
  switch(uiMsg)
  {
    //selection process
  case BFFM_INITIALIZED:
    if(0 != lDataParam)
    {
      ::SendMessage(hWnd, BFFM_SETSELECTION, TRUE, lDataParam);
    };
    break;

  default:
    break;
  };

  return iRes;
};

bool SLWSelectFolder(const HWND hcOwnerWnd, LPTSTR const cpResBuff, LPCTSTR const cpcTextToUser)
{
  bool bRes = false;

  BROWSEINFO Info;
  ZeroMemory(&Info, sizeof(Info));
  Info.ulFlags = BIF_EDITBOX;
  Info.hwndOwner = hcOwnerWnd;
  Info.lpszTitle = cpcTextToUser;
  Info.lpfn = __SLWSelFolderCB;
  Info.lParam = (0 == ::lstrlen(cpResBuff))
    ? 0
    : reinterpret_cast<LPARAM>(cpResBuff);
  //call func
  ITEMIDLIST* const cpItem = SHBrowseForFolder(&Info);
  if(0 != cpItem)
  {
    if(SUCCEEDED(::SHGetPathFromIDList(cpItem, cpResBuff)))
    {
      bRes = true;
    };
    LPMALLOC pInt = 0;
    if(SUCCEEDED(::SHGetMalloc(&pInt)))
    {
      pInt->Free(cpItem);
      pInt->Release();
    };
  };

  return bRes;
};

_SLW_END