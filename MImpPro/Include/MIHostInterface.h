/*++

Module name:

    MIHostInterface.h

Abstract:

    Common type and macro declaration for 32 bit and 64 bit components.
    
    Interface of Mouse Imp Host application and Hook Library is composed 
    from these declarations.
    
--*/

//internal version
const int eccInternalVer = 10;

const size_t egcWndClassTextLen = 100;
//magnifer for AutoHide timer (from val to sec's)
const int egcAutoHideTimerMagnifer = 1000;
//AutoHide timer range's
const int egcMinAutoHideTimerTime = 250;
const int egcMaxAutoHideTimerTime = 10000;


//default ScrollReduction
const int egcMagnScrollReduction = 0x10000;
const int egcDefScrollReduction = egcMagnScrollReduction * 4;
//sub reduction values
const int egcAddReductMax = egcMagnScrollReduction * 32;
const int egcAddReductMin = egcMagnScrollReduction / 32;
//number of precent's
const int egcNumberPerCentValue = 100;
//scroll reduction margin's
const int egcMarginScrollReductionPsMin = egcAddReductMin * egcNumberPerCentValue / egcMagnScrollReduction;
const int egcMarginScrollReductionPsMax = egcAddReductMax * egcNumberPerCentValue / egcMagnScrollReduction;

////margin's for dwIESCrollConvMove (ie move sens) in PS
const int egcIEScrollConvMoveMin = 5;
const int egcIEScrollConvMoveMax = 20000;

//magnifer for convert dwIESCrollConvMove
const int egcIEScrollConvMoveMagn = 1024;

//shrink working types
enum ShrinkTypeEnum
{
    //shrik wnd
    estShrink = 0,
    //only activate wnd
    estActivateWnd,
    //numb
    estLast
};

//try icon click mode
enum TrayIconClickEnum
{
    etiTrayIconClickSwitchDirectScroll,
    etiTrayIconClickSwitchAutoHide,
    etiTrayIconClickLast,
};

////scroll control types
enum ScrollControlEnum
{
    //standard
    escStandard,
    //toggle mode with eat click's
    escToggleWithEat,
    //mix mode (escStandard + escToggleWithEat)
    escScrollModeMix,
};

//msg's for host app

enum MsgConstEnum
{
    //message, resiverd from tray icon ty main host wnd
    emcToHostTrayIconNotifyMsg = WM_APP + 100,
    //msg, notifyed main host wnd by chng mode (LPARAM is tray icon enum)
    emcChngModeSeondToMainHost,
    //wnd resive mouse activity (auto hide if present in "ProcessWndList")
    //LPARAM - passed wnd
    emcAHWndActivity,
    //wnd destroy - remove from ProcessWndList
    //LPARAM - passed wnd
    emcAHWndDestroy,
    //wnd resive activation (by key or other) - "lock" wnd auto hide
    //LPARAM - passed wnd
    //wparam - active/inactive (BOOL)
    emcAHWndActive,
    //reread wnd pattern list (send from cfg app)
    //lParam != FALSE - read cfg from reg
    emcReReadWndPattList,
    //wnd hided - lParam - HWND
    emcAHWndHide,
    //wnd showed - lParam - HWND, wParam - TRUE if wnd is active
    emcAHWndShow,
    //get info about pased process id
    //LPARAM is process id,
    emcGetAppNameById,
    //app exited - need delete app info from AppInfoList (in host)
    //LPARAM is process id
    emcAppExited,
    //need save cfg by host application (chnged by CfgApp)
    emcNeedSaveCfg,
    //new app started - sended from hook to host dll
    //LAPRAM is handle of file mapping (in page file) routed to host process (if FALSE != pCfgMem->bNotSendModuleFileName)
    //WPARAM is id of process, what started
    emcNewAppStarted,
    //registration info entered in cfg application
    emcRegInfoEnter,
};


struct CMICommonPartWith64bit
{
    //handle of "main host wnd"
    HWND hMainHostWnd;
#ifndef _WIN64
    ULONG32 Reserved; // high-order bits of 64-bits window handle, not used
#endif

    //handle of main window of cfg application (used for start only one instance)
    HWND hCfgAppWnd;
#ifndef _WIN64
    ULONG32 Reserved2; // high-order bits of 64-bits window handle, not used
#endif

    //tray icon mode (one of TrayIconClickEnum)
    DWORD dwTrayIconClickMode;

    //enable tray icon
    BOOL bEnableTrayIcon;
    //enable tray icon animation
    BOOL bEnableTrayIconAnim;
    //need show splash at start
    BOOL bShowSplash;

    //move distance summ (by cursor)
    LONGLONG llMoveDistanceSumm;
    //move distance "save" summ
    LONGLONG llMoveDistanceSaveSumm;

    //productivity for AutoShrink
    struct
    {
        //time count for "total work time" - sec
        LONGLONG llTotalTime;
        //summ of ("wnd open time" multiply ScrSq/WndSq)
        LONGLONG llWndSqTime;
    } ASOpenProd;

    ////timer for total MouseImp runnign time 
    //incremented in host and used for show in "productivity" page
    LONGLONG llTotalRunTime;

    //auto hide enabled
    BOOL bAHideEnabled;
    //auto hide "mouse inactivity" timer timer
    DWORD dwAutoHideTimerTime;
    //delay time before open wnd (0 if delay disabled)
    DWORD dwAutoOpenTimerTime;

    //bidirection scrolling enabled
    BOOL bBDScrollingEnabled;
    //not use cursor visualisation (if FALSE)
    BOOL bCursorVisualise;
    //control type (from ScrollControlEnum)
    DWORD dwControlType;
    BOOL bScrollDisabled;
    //"inverse" scrilling
    BOOL bScrollInv;
    //reduction for scroll (convert from WM_MOUSEMOVE to WM_MOUSEWHELL)
    //stored as magnifed to egcIEScrollConvMoveMagn
    DWORD dwIESCrollConvMove;
    //scrolling normal reduction
    DWORD dwScrollNormReduction;
    //scroll shift reduction
    LONG lScrollReduction;
    //key for temporary disable DirectScroll
    UINT uiDirectScrollTmpDisableKey;
    //scroll reduction virtual key
    UINT uiScollReductionKey;
    //mouse key's available for hooking
    UINT uiValidMouseKeys;

    //buffer for get app name message call
    CHAR cpGetAppNameResultBuff[MAX_PATH];
};



//name of shared info view (for open from hook dll)
LPCSTR const cpcSharedInfoName = "MImpProSharedInfo";

//name of shared info initialized event
LPCSTR const cpcSharedInfoInitializedEventName = "MImpProSharedInfoInitializedEvent";

//shared info for handle crypted data
LPCSTR const cpcSharedCryptInfo = "MImpProSharedCInfo";

//name og mousr hook function (used for enhancing)
LPCSTR const cpMouseHookFunc = "MIHookFunc";
//hook proc for system activity detection
LPCSTR const cpcCBTHookFunc = "MIHookFunc1";
//name of hook module DLL file
LPCSTR const cpcHookFundModule = "MIPro.dll";
//name of host application
LPCSTR const cpcHostAppName = "MIProHst.exe";
//name of "cfg" application
LPCSTR const cpcCfgAppFileName = "MIProCfg.exe";
//application help file
LPCSTR const cpcHelpFileName = "MIPROHLP.HLP";
//name of reg key for store cfg
LPCSTR const cpcCfgKeyName = "SOFTWARE\\TV4 Studios\\MImpPro";
//name of cfg for MouseImp
LPCSTR const cpcCfgImportKeyName = "SOFTWARE\\TV4 Studios\\MouseImp";
//name of "base" entry for WndPattern's (count of pattern's)
LPCSTR const cpcCfgWPatternsMainName = "WPat";
//name of "store" entry for WndPattern's (actual data)
//each entry stored in "new line" separated format as "AppName\nClassName\nTitleName
LPCSTR const cpcCfgWPatternsEntryMask = "WPat%d";
//name of "instalation directory" entry
LPCTSTR const cpcCfgInstallDirPath = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\MIProHst.exe";
LPCSTR const cpcCfgInstallDirEnt = "Path";
//name of global message
LPCSTR const cpcGlobalIntMessage = "MImpProGlbMsg";
//name of enty in Run for start MouseImp on WndLoad - corresponding to setup.inf file
LPCSTR const cpcRunEntryName = "MImpPro";
//registration key name
LPCSTR const cpcCfgRegKey = "Reg";
//host wnd class/name
LPCSTR const cpcWndClass = "MImpProWndClass";
LPCSTR const cpcWndTitle = "MImpProWndTitle";
//name of "get version" function, imported from cpcHookFundModule
//function return GetVersionNumber()
LPCSTR const cpcHookGetVerFunc = "MIGetVer";
extern "C" typedef DWORD (*HookGetVerFuncType)();

//api function for specifying host process
LPCSTR const cpcSetIsHostProcessFunc = "MISetIsHostProcess";
extern "C" typedef VOID (*SetIsHostProcessFuncType)();

inline bool WaitForSharedInfo()
{
    HANDLE hEventSharedInitialized = CreateEvent(NULL, TRUE, FALSE, cpcSharedInfoInitializedEventName);
    if (!hEventSharedInitialized)
    {
        return false;
    }

    // wait 15 seconds
    bool bRes = (WAIT_OBJECT_0 == WaitForSingleObject(hEventSharedInitialized, 15000));
    CloseHandle(hEventSharedInitialized);
    return bRes;
}

inline void SignalSharedInfoReady()
{
    HANDLE hEventSharedInitialized = CreateEvent(NULL, TRUE, FALSE, cpcSharedInfoInitializedEventName);
    if (!hEventSharedInitialized)
    {
        return;
    }

    SetEvent(hEventSharedInitialized);
    // the handle leak is intentional, leave event in signaled state
    //CloseHandle(hEventSharedInitialized);
}


//messages, sended to cfg app
enum EnumCfgMsg
{
    //get help if from dlg (ised internaly in CfgApp in it's help system
    //on this msg wnd can return help topic id, associated with this wnd
    ecmGetHelpId = WM_APP + 100,
    //app should show itself (used for goto from "popup find" mode)
    ecmShowApp
};


////try show cfg application (return false if app not present)
inline bool TryShowCfgApp(CMICommonPartWith64bit* pCommon64)
{
    bool bRes = false;
    if(FALSE != ::IsWindow(pCommon64->hCfgAppWnd))
    {
        DWORD processId;
        GetWindowThreadProcessId(pCommon64->hCfgAppWnd, &processId);
        AllowSetForegroundWindow(processId);

        bRes = true;
        ::PostMessage(pCommon64->hCfgAppWnd, ecmShowApp, 0, 0);
    };
    return bRes;
};

//try exit from host app (with wait)
inline void TryExitFromHostApp(const CMICommonPartWith64bit* pCommon64)
{
    if(FALSE != ::IsWindow(pCommon64->hMainHostWnd))
    {
        //try open process
        HANDLE hProc = 0;
        DWORD dwWndProc = 0;
        if(0 != ::GetWindowThreadProcessId(pCommon64->hMainHostWnd, &dwWndProc))
        {
            hProc = ::OpenProcess(SYNCHRONIZE, FALSE, dwWndProc);
        };
        //post close to wnd
        ::PostMessage(pCommon64->hMainHostWnd, WM_CLOSE, 0, 0);
        //try wait for exit from host process
        if(0 != hProc)
        {
            ::WaitForSingleObject(hProc, 10 * 1000);
            ::CloseHandle(hProc);
        };
    };
};


////calc version number, based on eccInternalVer and sizeof(CMISharedInfo)
inline DWORD GetVersionNumber()
{
    return MAKELONG(eccInternalVer, sizeof(CMICommonPartWith64bit));
};


//////////////////////////////////////////////////////////////////////////
// Utility

////activate wnd and bring to user
inline void ActivateWndToUser(const HWND hcWnd)
{
    ::SetForegroundWindow(hcWnd);
};

//change name in passed buffer and set it's to passed name
inline void ChangeNameInFullName(LPCSTR const cpcNewName, LPTSTR const cpResMass, const DWORD dwcLen)
{
    //find end '\\' or '/'
    DWORD dwFindPos = 0;
    for(DWORD dwCount = 0; dwcLen > dwCount; dwCount++)
    {
        const DWORD dwcLookPos = dwcLen - 1 - dwCount;
        const TCHAR cLook = cpResMass[dwcLookPos];
        if('\\' == cLook || '/' == cLook)
        {
            //find char
            dwFindPos = dwcLookPos + 1;
            break;
        };
    };
    //copy passed name to buffer from finded pos
    ::lstrcpy(cpResMass + dwFindPos, cpcNewName);
};

//build in passed mass cpResMass (MAX_PATH len) full name of cpcShortName application, based on path to hBaseModule
inline void GetFullBasedName(LPCTSTR const cpcShortName, LPTSTR const cpResMass, const HMODULE hBaseModule)
{
    //get based module name
    const DWORD dwcLen = ::GetModuleFileName(hBaseModule, cpResMass, MAX_PATH);
    //set to path
    ChangeNameInFullName(cpcShortName, cpResMass, dwcLen);
};


//////////////////////////////////////////////////////////////////////////
// Auto Hide

//compare str helper
inline bool __StrCmpHelper(LPCSTR const cpcStartStr, const DWORD dwcStartLen, LPCSTR const cpcStr)
{
    bool bRes = true;
    for(DWORD dwCount = 0; dwcStartLen > dwCount && '\0' != cpcStr[dwCount]; dwCount++)
    {
        if(cpcStartStr[dwCount] != cpcStr[dwCount])
        {
            bRes = false;
            break;
        };
    };
    return bRes;
};
////compare Wnd class for AShrink


//scan until passed char not skiped
inline void __SkipUntil(LPCSTR& rcStr, const CHAR ccLook)
{
    while(0 != *rcStr)
    {
        const CHAR ccMem = *rcStr;
        rcStr++;
        if(ccMem == ccLook)
        {
            //all find - break
            break;
        };
    };
};

//compare string for digit and (next) colon or zero end
inline bool __CompareHelperDigit(LPCSTR const cpcStr, const CHAR ccChar)
{
    return
        ccChar == cpcStr[0]
    && (':' == cpcStr[1] || '\0' == cpcStr[1]);
};

//string compare helper (for digit group, separated by ':')
inline bool __StrCmpHelperDig(LPCSTR& rpcWnd, LPCSTR& rpcPatt, const bool bcExtraLook)
{
    bool bRes = true;

    //mem start
    LPCSTR const cpcStartWnd = rpcWnd;
    LPCSTR const cpcStartPatt = rpcPatt;

    //compare loop (until ':' not found
    while('\0' != *rpcWnd && '\0' != *rpcPatt)
    {
        if(*rpcWnd != *rpcPatt)
        {
            //compare failed
            bRes = false;
            break;
        };
        const CHAR ccMem = *rpcWnd;
        //next
        rpcWnd++;
        rpcPatt++;
        //try find colon
        if(':' == ccMem)
        {
            //break on
            break;
        };
    };

    //if string's not equal - and pattern not '0' - string's equal
    if
        (
        //need look extra
        false != bcExtraLook
        //prev compare failed
        && false == bRes
        //pattern and wnd not have '0'
        && false == __CompareHelperDigit(cpcStartPatt, '0')
        && false == __CompareHelperDigit(cpcStartWnd, '0')
        )
    {
        bRes = true;
        //try find "colon'ed" end in pattern and wnd info
        __SkipUntil(rpcPatt, ':');
        __SkipUntil(rpcWnd, ':');
    };

    return bRes;
};

const CHAR cpcMFCClassPrefix[] = {'A', 'f', 'x', ':'};
inline bool ASCompareClass(LPCSTR const cpcWnd, LPCSTR const cpcPatt)
{
    bool bRes = 0 == ::lstrcmpi(cpcWnd, cpcPatt);

    //if compare failed - try look on "MFC class"
    if(false == bRes)
    {
        //look on "MFC prefix before pattern and wnd class
        if(false != __StrCmpHelper(cpcMFCClassPrefix, ARRAYSIZE(cpcMFCClassPrefix), cpcWnd) && false != __StrCmpHelper(cpcMFCClassPrefix, ARRAYSIZE(cpcMFCClassPrefix), cpcPatt))
        {
            LPCSTR pcWnd = cpcWnd + ARRAYSIZE(cpcMFCClassPrefix);
            LPCSTR pcPatt = cpcPatt + ARRAYSIZE(cpcMFCClassPrefix);
            //look on "Afx:Inst:Style" format
            bRes =
                //instance
                false != __StrCmpHelperDig(pcWnd, pcPatt, false)
                //style
                && false != __StrCmpHelperDig(pcWnd, pcPatt, false);
            //look on "Afx:Inst:Stype:Cursor:Back:Icon" format
            if(false != bRes && '\0' != *pcWnd && '\0' != *pcPatt)
            {
                //compare end
                bRes =
                    //cursor
                    false != __StrCmpHelperDig(pcWnd, pcPatt, true)
                    //back
                    && false != __StrCmpHelperDig(pcWnd, pcPatt, false)
                    //icon
                    && false != __StrCmpHelperDig(pcWnd, pcPatt, true);
            };
        };
    };

    return bRes;
};


//////////////////////////////////////////////////////////////////////
//exclude info for AutoShrink

struct __CShrinkExcludeInfo
{
    //wnd class name
    LPCSTR cpWndClass;
    //test wnd style
    DWORD dwStyle;
    //test wnd style mask
    DWORD dwStyleMask;
    //wnd border size
    UINT uiBorderSizeMetric;
    //wnd caption size
    UINT uiCaptionSizeMetric;
    //place for caption (for DrawEdge function)
    //beside from caption
    UINT uiEdgePos;
};

const __CShrinkExcludeInfo cpcExcludeInfo[] =
{
    {"MsoCommandBar", WS_POPUP, WS_POPUP | WS_CHILD, SM_CYSIZEFRAME, SM_CYSMCAPTION, BF_BOTTOM}
};

//try fins exclude info for some wnd
inline const __CShrinkExcludeInfo* FindExcludeInfo(const HWND hcWnd)
{
    const __CShrinkExcludeInfo* pRes = 0;

    CHAR cpClass[egcWndClassTextLen];
    bool bClassNameValid = false;
    const DWORD dwcStyle = ::GetWindowLong(hcWnd, GWL_STYLE);
    for(DWORD dwCount = 0; ARRAYSIZE(cpcExcludeInfo) > dwCount; dwCount++)
    {
        const __CShrinkExcludeInfo* const cpcInfo = cpcExcludeInfo + dwCount;
        if(cpcInfo->dwStyle == (cpcInfo->dwStyleMask & dwcStyle))
        {
            //compare by class
            if(false == bClassNameValid)
            {
                bClassNameValid = true;
                ::GetClassName(hcWnd, cpClass, ARRAYSIZE(cpClass));
            };
            if(false != ASCompareClass(cpClass, cpcInfo->cpWndClass))
            {
                pRes = cpcInfo;
                break;
            };
        };
    };

    return pRes;
};

inline LONG GetWndBorderSize(const HWND hcWnd, const DWORD dwcStyle, const bool bcHor)
{
    LONG lRes = 0;

    if(0 != (WS_BORDER & dwcStyle))
    {
        if(0 != (WS_THICKFRAME & dwcStyle))
        {
            lRes = ::GetSystemMetrics((false != bcHor) ? SM_CXSIZEFRAME : SM_CYSIZEFRAME);
        }
        else
        {
            lRes = ::GetSystemMetrics((false != bcHor) ? SM_CXFIXEDFRAME : SM_CYFIXEDFRAME);
        };
    };
    return lRes;
};


//check "is window have needed style for AutoHide"
inline BOOL AHideCheckWndStyle(const HWND hcWnd, const bool bcLookOnCaptionSize = true)
{
    BOOL bRes = FALSE;
    //try find wnd in "exclusion" list
    const __CShrinkExcludeInfo* const cpcExcInfo = FindExcludeInfo(hcWnd);
    if(0 != cpcExcInfo)
    {
        bRes = TRUE;
    };

    if(FALSE == bRes)
    {
        const LONG lcStyle = ::GetWindowLong(hcWnd, GWL_STYLE);
        if(0 == ((WS_POPUP | WS_CHILD) & lcStyle))
        {
            //overlapped window - have caption as default
            bRes = TRUE;
        }
        else if(0 != (WS_POPUP & lcStyle))
        {
            //for pop up's we look for caption style or for distance between top wnd and it's client area
            bRes = WS_CAPTION == (WS_CAPTION & lcStyle);
            //caption apsent
            if(false != bcLookOnCaptionSize && FALSE == bRes)
            {
                //look distance between wnd top wnd client top
                const LONG lcHorBord = GetWndBorderSize(hcWnd, lcStyle, true) + ::GetSystemMetrics(SM_CXEDGE);
                const LONG lcVertBord = GetWndBorderSize(hcWnd, lcStyle, false) + ::GetSystemMetrics(SM_CYEDGE);
                RECT WndRect;
                RECT CliRect;
                ::GetWindowRect(hcWnd, &WndRect);
                ::GetClientRect(hcWnd, &CliRect);
                ::MapWindowPoints(hcWnd, 0, reinterpret_cast<POINT*>(&CliRect), 2);
                //client rect smaller when wnd rect (non client present)
                bRes =
                    WndRect.top + lcHorBord < CliRect.top
                    || WndRect.bottom - lcHorBord > CliRect.bottom
                    || WndRect.left + lcVertBord < CliRect.left
                    || WndRect.right - lcVertBord > CliRect.right;
            };
        }
        else
        {
            //for child's look on Caption style
            //(???) possible not good idea AutoHide for child's
            bRes = WS_CAPTION == (WS_CAPTION & lcStyle);
        };
    };

    return bRes;
};


//get parent window while drilling up
inline HWND GetParentWndForDrillUp(const HWND hcWnd)
{
    const HWND hcRes = ::GetParent(hcWnd);
    return (0 == hcRes)
        ? ::GetWindow(hcWnd, GW_OWNER)
        : hcRes;
};
//drilling for window up while find "AutoHide" window
inline bool AHideDrillForWindow(const HWND hcFromWnd, HWND& rFindWnd)
{
    //drill while wnd exist and have "valid" style's

    bool bRes = false;

    rFindWnd = 0;
    for(HWND hLook = hcFromWnd; 0 != hLook; hLook = GetParentWndForDrillUp(hLook))
    {
        if(FALSE != AHideCheckWndStyle(hLook))
        {
            bRes = true;
            rFindWnd = hLook;
            break;
        };
    };

    return bRes;
};
