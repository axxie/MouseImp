/**********************************************************************
AboutDlg
**********************************************************************/

#include "stdafx.h"


#include "Aboutpage.h"


#include "..\\MIGlobal.h"

#include "..\\SlibExc\\SLVerInfo.h"

#include "..\\SlibExc\\SLAutoPtr.h"

#include "AppConst.h"

#include <WININET.H>

#include "..\\SLibExc\\SLCfgReg.h"

#include "NewsshowDialog.h"

#include "MImpCfg.h"


//////////////////////////////////////////////////////////////////////
//global's

//name of WinInet dll
static LPCTSTR const cpcAgentName = _T("");
static LPCTSTR const cpcWNDllName = _T("WININET.DLL");
//func's
static LPCTSTR const cpcWNInternetOpen = _T("InternetOpenA");
typedef HINTERNET (WINAPI *WNInternetOpenType)(LPCSTR pcAgent, DWORD dwAccessType, LPCSTR pcProxy, LPCSTR pLocals, DWORD dwFlags);
static LPCTSTR const cpcWNInternetCloseHandle = _T("InternetCloseHandle");
typedef BOOL (WINAPI *WNInternetCloseHandleType)(HINTERNET hInet);
static LPCTSTR const cpcWNInternetOpenUrl = _T("InternetOpenUrlA");
typedef HINTERNET (WINAPI *WNInternetOpenUrlType)(HINTERNET hSess, LPCSTR pcUrl, LPCSTR pcHdr, DWORD dwHdrLen, DWORD dwFlag, DWORD dwContext);
static LPCTSTR const cpcWNInternetReadFile = _T("InternetReadFile");
typedef BOOL (WINAPI *WNInternetReadFileType)(HINTERNET hFile, LPVOID pBuff, DWORD dwReadSize, LPDWORD pReaded);


//////////////////////////////////////////////////////////////////////
// CAboutPage property page

CAboutPage::CAboutPage(CWnd* const cpParent) 
  :CChildDlg(CAboutPage::IDD, cpParent)
{
	//{{AFX_DATA_INIT(CAboutPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CAboutPage::~CAboutPage()
{
}

void CAboutPage::DoDataExchange(CDataExchange* pDX)
{
	CChildDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CAboutPage, CChildDlg)
	//{{AFX_MSG_MAP(CAboutPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAboutPage::OnInitDialog()
{
  const BOOL bcRes = CChildDlg::OnInitDialog();

  //version
  CString Text;
  SLCHECK(FALSE != Text.LoadString(IDS_STR_VER_MASK));
  //get verstion
  using sl::CSLVerInfo;
  CSLVerInfo Ver(AfxGetResourceHandle());
  CString Res;
  Res.Format
    (
    Text, 
    Ver.GetProdVersionHiHi(), 
    Ver.GetProdVersionHiLow(),
    Ver.GetProdVersionLowHi(),
    Ver.GetProdVersionLowLow()
    );
  //set back
  SetDlgItemText(IDC_VER_EDIT, Res);

  //update info from stored cfg
  UpdateUrls(); 

  return bcRes;
};

static inline void SetUrlInfo(CHyperLink& rLink, LPCTSTR const cpcDefUrl, LPCTSTR const cpcDefName)
{
  //set to hyper link
  rLink.SetWindowText(cpcDefName);
  rLink.SetUrl(cpcDefUrl);
};

void CAboutPage::UpdateUrls()
{
};

bool CAboutPage::TryDownloadFile(LPCTSTR const cpcUrl, BYTE** const cppWantMem, DWORD* const cpMemSize)
{
  return false;
};

void CAboutPage::OnCheckNewBtn() 
{
}
