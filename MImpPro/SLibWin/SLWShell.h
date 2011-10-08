/**********************************************************************
shell wrapper's
**********************************************************************/

#ifndef __SLWSHELL_H
#define __SLWSHELL_H

#pragma once

#include "SLWUtil.h"


_SLW_BEG

//select folder
bool SLWSelectFolder(const HWND hcOwnerWnd, LPTSTR const cpResBuff, LPCTSTR const cpcTextToUser);


_SLW_END

#endif//__SLWSHELL_H

