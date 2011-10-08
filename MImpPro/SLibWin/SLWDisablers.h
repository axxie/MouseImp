/**********************************************************************
disablers
**********************************************************************/

#ifndef __SWLDISABLERS_H
#define __SWLDISABLERS_H

#pragma once

#include "SLWUtil.h"

_SLW_BEG


//disable one ctrl (special handling)
void EnableCtrl(const HWND hcDlg, const UINT uicCtrlId, const BOOL bcEnable);
//disable wnd 
void EnableWnd(const HWND hcWnd, const BOOL bcEnable);
//disable whole wnd (by look all ctrl's)
void EnableDlgWnd(const HWND hcWnd, const BOOL bcEnable);
//enable group of ctrl's (zero delimited)
void EnableCtrlGrp(const HWND hcWnd, const UINT* const cpcCtrl, const BOOL bcEnable);
//scan passed ctrl id's mass (end on -1 or 0) and look on 0 < GetWindwTextLen() (check for fill "needed ctrl's in dlg")
//result false if some ctrl find and focus set to "this" ctrl
bool CheckEmptyCtrlGrp(const HWND hcWnd, const UINT* const cpcCtrl);

////init combo box control
////name's for flash prm's
struct __CSLWListBoxInitData
{
  DWORD dwResId;
  DWORD dwDataId;
};
////init combo box control
void SLWComboControlInit(HINSTANCE hInst, HWND hDlg, const UINT uicCtrl, const DWORD dwcInitData, const __CSLWListBoxInitData* const cpcInitData, const DWORD dwcDayaSize);
////get data from curr selection in combo box
bool SLWComboControlGetSel(HWND hDlg, const UINT uicCtrl, DWORD& rData);

//////////////////////////////////////////////////////////////////////
//CSLWWaitCursor

class CSLWWaitCursor
{
protected:

  ////cursor for restore after kill this class
  HCURSOR hOldCursor;
  
public:
  
public:

  inline CSLWWaitCursor();
  inline ~CSLWWaitCursor();
  
protected:
};

/**********************************************************************
implementation
**********************************************************************/

//////////////////////////////////////////////////////////////////////
//CSLWWaitCursor

inline CSLWWaitCursor::CSLWWaitCursor()
  :hOldCursor(::SetCursor(::LoadCursor(0, IDC_WAIT)))
{

};

inline CSLWWaitCursor::~CSLWWaitCursor()
{
  ////restore prev cursor back
  ::SetCursor(hOldCursor);
};



_SLW_END

#endif//__SWLDISABLERS_H