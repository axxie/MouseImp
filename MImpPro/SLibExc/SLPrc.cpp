/**********************************************************************
process ctrl's
**********************************************************************/

#define STRICT
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#pragma hdrstop


#include "SLPrc.h"


_SL_BEG

bool CSLProcess::Create(LPCTSTR const cpcFileName, LPTSTR const cpCmdLine, const DWORD dwcIdleWaitTime)
{
  bool bRes = false;

  Free();

  //startup info
  STARTUPINFO StartInfo;
  ZeroMemory(&StartInfo, sizeof(StartInfo));
  StartInfo.cb = sizeof(StartInfo);

  //process info
  PROCESS_INFORMATION PrcInfo;

  const BOOL bcCreate = CreateProcess
    (
    cpcFileName,
    cpCmdLine,
    0,
    0,
    FALSE,
    CREATE_DEFAULT_ERROR_MODE | NORMAL_PRIORITY_CLASS,
    0,
    0,
    &StartInfo,
    &PrcInfo
    );

  if(FALSE != bcCreate)
  {
    //free unneded
    CloseHandle(PrcInfo.hThread);
    hProcess = PrcInfo.hProcess;

    //not wait for IDLE
    if(0 == dwcIdleWaitTime || 0 == WaitForInputIdle(Get(), dwcIdleWaitTime))
    {
      bRes = TRUE;
    };
  };


  return bRes;
};

_SL_END
