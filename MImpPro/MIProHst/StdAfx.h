/**********************************************************************
precompiled hdr's
**********************************************************************/

#ifndef __STDAFX_H
#define __STDAFX_H

#ifdef _DEBUG
#define MIMP_DEBUG
#endif

#pragma once

#define WINVER 0x0600
#define STRICT
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>

#include <SHELLAPI.H>
#include "sddl.h"


//not used "command line" info
#define __SLW_NO_COMMAND_LINE_INFO
//#include "..\\SLibWin\\SLWMinCrtHdr.h"


#endif//__STDAFX_H