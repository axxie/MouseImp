/**********************************************************************
precompiled's
**********************************************************************/
#if !defined(AFX_STDAFX_H__E275CA3E_842B_11D2_B0F5_00A0C907DA21__INCLUDED_)
#define AFX_STDAFX_H__E275CA3E_842B_11D2_B0F5_00A0C907DA21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
//MFC

//#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#include <SDKDDKVer.h>

/*
#include <afxwin.h>
#include <afxdlgs.h>
//#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <afxcmn.h>			// MFC support for Windows Common Controls
*/

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <afxcmn.h>			// MFC support for Windows Common Controls


#include "afxpriv.h"
#include "afxole.h"
#include "..\src\mfc\oleimpl2.h"
#include "..\src\mfc\afximpl.h"


////////////////////////////////////////////////////////////////////////
////STL
//
//#pragma warning(disable:4786)
//#include <map>
//#include <set>
//#include <bitset>
//#include <list>
//#include <queue>

//////////////////////////////////////////////////////////////////////
//CRT

#include <strstream>



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__E275CA3E_842B_11D2_B0F5_00A0C907DA21__INCLUDED_)
