/**********************************************************************
app class
**********************************************************************/

#include "stdafx.h"


#include "MImpCfg.h"


#include "MImpCfgMainDlg.h"

#include "..\\SlibExc\\SLPrc.h"

#include "resource.h"

#include "appConst.h"

#include "QuickTourDlg.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMImpCfgApp

BEGIN_MESSAGE_MAP(CMImpCfgApp, CWinApp)
	//{{AFX_MSG_MAP(CMImpCfgApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
  ON_COMMAND(ID_HELP, OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMImpCfgApp construction

CMImpCfgApp::CMImpCfgApp()
  :hCfgMap(0),
  pCfgMem(0),
  bPopUpInstShowed(false)
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMImpCfgApp object

CMImpCfgApp theApp;

void CMImpCfgApp::StartCfg()
{
  bool bRes = true;

  //try load lib and get it's version
  if(false != bRes)
  {
    CHAR cpFileName[MAX_PATH];
    GetFullBasedName(cpcHookFundModule, cpFileName, AfxGetInstanceHandle());
    HMODULE hHookLib = ::LoadLibrary(cpFileName);
    if(0 != hHookLib)
    {
      HookGetVerFuncType const cpFinc = reinterpret_cast<HookGetVerFuncType>(::GetProcAddress(hHookLib, cpcHookGetVerFunc));
      bRes = 
        0 != cpFinc
        && GetVersionNumber() == cpFinc();
      ::FreeLibrary(hHookLib);
    };
    //to user
    if(false == bRes)
    {
      AfxMessageBox(IDS_MSG_INV_VER, MB_OK | MB_ICONERROR | MB_SETFOREGROUND | MB_TOPMOST);
    };
  };

  //try open cfg (host should be started for ok)
  if(false != bRes)
  {
    //try open cfg (if host started - ok)
    if(false == OpenSharedCfg() || FALSE == ::IsWindow(pCfgMem->hMainHostWnd))
    {
      bRes = false;
      //build host app name
      TCHAR cpHostName[MAX_PATH];
      GetFullBasedName(cpcHostAppName, cpHostName, AfxGetInstanceHandle());
      //try open host process
      using sl::CSLProcess;
      CSLProcess HostProcess;
      if(false != HostProcess.Create(cpHostName))
      {
        bRes = OpenSharedCfg();
      };
    };
    //report error
    if(false == bRes)
    {
      AfxMessageBox(IDS_MSG_HOST_NOT_START, MB_OK | MB_ICONERROR);
    };
  };

  //try bring cfg wnd if exist
  if(false != bRes)
  {
    TryShowCfgApp(pCfgMem);
  };

  //set help full file 
  if(false != bRes)
  {
    //make help file name
    CHAR cpBuff[MAX_PATH];
    GetFullBasedName(cpcHelpFileName, cpBuff, AfxGetInstanceHandle());
    //remove curr
    free(reinterpret_cast<void*>(const_cast<LPSTR>(m_pszHelpFilePath)));
    m_pszHelpFilePath = 0;
    //new
    m_pszHelpFilePath = ::strdup(cpBuff);
  };

  //start main wnd
  if(false != bRes)
  {
	  CMImpCfgMainDlg dlg;
	  m_pMainWnd = &dlg;
	  dlg.DoModal();
  };

  //close cfg
  CloseSharedCfg();
};

void CMImpCfgApp::QuitFromImp()
{
  if(false != OpenSharedCfg())
  {
    TryExitFromHostApp(pCfgMem);
    CloseSharedCfg();
  };
};

void CMImpCfgApp::StartQuickTour()
{
  CQuickTourDlg Dlg;
  Dlg.DoModal();
};

BOOL CMImpCfgApp::InitInstance()
{
  //try open shared cfg
  //if cfg not open - try start host app
  //  if open cfg not valid - exit
  //open and start main dlg
  //close cfg

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

  if(0 == ::lstrcmpi(m_lpCmdLine, cpcCmdKeyQuickTour))
  {
    StartQuickTour();
  }
  else if(0 == ::lstrcmpi(m_lpCmdLine, cpcCmgQuitFromImp))
  {
    QuitFromImp();
  }
  else
  {
    StartCfg();
  };

	return FALSE;
}

bool CMImpCfgApp::OpenSharedCfg()
{
  //close if exist
  //open new

  //close
  CloseSharedCfg();

  bool bRes = true;

  //map
  if(false != bRes)
  {
    hCfgMap = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, cpcSharedInfoName);
    bRes = 0 != hCfgMap;
  };

  //mem
  if(false != bRes)
  {
    PVOID const cpAddr = ::MapViewOfFile
      (
      hCfgMap,
      FILE_MAP_ALL_ACCESS,
      0,
      0,
      sizeof(*pCfgMem)
      );
    pCfgMem = reinterpret_cast<CMISharedInfo*>(cpAddr);
    bRes = 0 != pCfgMem;
  };

  return bRes;
};

void CMImpCfgApp::CloseSharedCfg()
{
  //close mem
  //close map

  //mem
  if(0 != pCfgMem)
  {
    ::UnmapViewOfFile(pCfgMem);
    pCfgMem = 0;
  };


  //map
  if(0 != hCfgMap)
  {
    ::CloseHandle(hCfgMap);
    hCfgMap = 0;
  };
};

void CMImpCfgApp::OnHelp()
{
  //try find help id from wnd's (from focus wnd to top wnd)
  //if find - display some topic
  //else - display default help

  //defautl vald for not find help topic
  UINT uiCmd = HELP_FINDER;
  DWORD dwContext = 0;

  for(HWND hLookWnd = ::GetFocus(); 0 != hLookWnd; hLookWnd = ::GetParent(hLookWnd))
  {
    //if some process
    DWORD dwWndPrc = 0;
    ::GetWindowThreadProcessId(hLookWnd, &dwWndPrc);
    if(::GetCurrentProcessId() != dwWndPrc)
    {
      //not some process
      break;
    };

    //get help context
    const LRESULT lRes = ::SendMessage(hLookWnd, ecmGetHelpId, 0, 0);
    if(0 != lRes)
    {
      //find help topic
      uiCmd = HELP_CONTEXT;
      dwContext = lRes;
      break;
    };
  };

  //call help
  CWinApp::WinHelp(dwContext, uiCmd);
};

void CMImpCfgApp::PumpMessageOnce()
{
  while(FALSE != ::PeekMessage(&m_msgCur, 0, 0, 0, PM_REMOVE))
  {
    if(FALSE == PreTranslateMessage(&m_msgCur))
    {
      ::TranslateMessage(&m_msgCur);
      ::DispatchMessage(&m_msgCur);
    }
  };
};
