/**********************************************************************
app class
**********************************************************************/

#if !defined(AFX_MIMPCFG_H__E275CA3A_842B_11D2_B0F5_00A0C907DA21__INCLUDED_)
#define AFX_MIMPCFG_H__E275CA3A_842B_11D2_B0F5_00A0C907DA21__INCLUDED_

#pragma once

#include "..\Include\MIGlobal.h"

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CMImpCfgApp:

class CMImpCfgApp;
extern CMImpCfgApp theApp;

class CMImpCfgApp : public CWinApp
{
protected:

public:
  
  //shared cfg
  HANDLE hCfgMap;
  //cfg mem
  CMISharedInfo* pCfgMem;
  //instruction for popup find show to user
  bool bPopUpInstShowed;
  
public:

  CMImpCfgApp();

  //check validity for "first protection" layer
  //return false if first layer not valid
  inline bool CheckRegValueCheckBox(HWND hcDlg, const UINT uicCtrlId);
  //set check box to "unregistered" state
  //return true if control style chnged and not need chng by "DDX operation's"
  inline bool CheckRegBoolCheckBox(HWND hDlg, const UINT uicCtrlId, BOOL& rbOpt, const BOOL bcUnregDefVal) const;

  //do msg pumping one cycle
  void PumpMessageOnce();

protected:

  //try open shared cfg 
  bool OpenSharedCfg();
  //close shared cfg
  void CloseSharedCfg();

  //start as cfg apllication
  void StartCfg();
  //QuickTour
  void StartQuickTour();
  //quit from imp 
  void QuitFromImp();

protected:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMImpCfgApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMImpCfgApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
  afx_msg void OnHelp();
	DECLARE_MESSAGE_MAP()
};


/**********************************************************************
implementation
**********************************************************************/

//////////////////////////////////////////////////////////////////////
//CMImpCfgApp

inline bool CMImpCfgApp::CheckRegValueCheckBox(HWND hcDlg, const UINT uicCtrlId)
{
  bool bRes = true;

  return bRes;
};

inline bool CMImpCfgApp::CheckRegBoolCheckBox(HWND hDlg, const UINT uicCtrlId, BOOL& rbOpt, const BOOL bcUnregDefVal) const
{
  bool bRes = false;
  return bRes;
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIMPCFG_H__E275CA3A_842B_11D2_B0F5_00A0C907DA21__INCLUDED_)
