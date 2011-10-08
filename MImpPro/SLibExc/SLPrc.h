/**********************************************************************
process ctrl's
**********************************************************************/

#ifndef __SLPRC_H
#define __SLPRC_H

#pragma once

#include "SLUtil.h"

#include <crtdbg.h>


_SL_BEG


//////////////////////////////////////////////////////////////////////
//CSLProcess - Process controller

class CSLProcess
{
protected:

  //process main handle
  HANDLE hProcess;
  
public:
  
public:

  //empty
  inline CSLProcess();
  //create 
  inline CSLProcess(LPCTSTR const cpcFileName, LPTSTR const cpCmdLine);

  //free all
  inline ~CSLProcess();

  //create new process
  //dwcWaitTime - wait time for process activity, if 0 == dwcWaitTime - not waiting prefomed
  bool Create(LPCTSTR const cpcFileName, LPTSTR const cpCmdLine = 0, const DWORD dwcIdleWaitTime = INFINITE);
  //some as Create
  inline bool Open(LPCTSTR const cpcFileName, LPTSTR const cpCmdLine = 0, const DWORD dwcIdleWaitTime = INFINITE);

  //clear
  inline void Free();
  //drop resource
  inline HANDLE Detach();

  //is present
  inline bool IsOk() const;
  //get res
  inline HANDLE Get() const;
  //get res
  inline operator HANDLE() const;


  //process ctrl's 
  //dwcWaitTime in milliseconds (as WaitForSingleObject)
  inline bool WaitForEnd(const DWORD dwcWaitTime = INFINITE);
  
protected:

};

/**********************************************************************
impelmentation
**********************************************************************/

//////////////////////////////////////////////////////////////////////
//CSLProcess

inline CSLProcess::CSLProcess()
  :hProcess(0)
{
};

inline CSLProcess::CSLProcess(LPCTSTR const cpcFileName, LPTSTR const cpCmdLine)
  :hProcess(0)
{
  const bool bcCreate = Create(cpcFileName, cpCmdLine);
  _ASSERT(false != bcCreate);
};


inline CSLProcess::~CSLProcess()
{
  Free();
};

inline void CSLProcess::Free()
{
  if(0 != hProcess)
  {
    CloseHandle(hProcess);
    hProcess = 0;
  };
};

inline HANDLE CSLProcess::Detach()
{
  HANDLE hRes = hProcess;

  hProcess = 0;

  return hRes;
};

inline bool CSLProcess::IsOk() const
{
  return 0 != hProcess;
};

inline HANDLE CSLProcess::Get() const
{
  _ASSERT(false != IsOk());
  return hProcess;
};

inline CSLProcess::operator HANDLE() const
{
  return Get();
};

inline bool CSLProcess::WaitForEnd(const DWORD dwcWaitTime)
{
  bool bRes = false;

  if(WAIT_OBJECT_0 == WaitForSingleObject(Get(), dwcWaitTime))
  {
    bRes = 0;
  };

  return bRes;
};

inline bool CSLProcess::Open(LPCTSTR const cpcFileName, LPTSTR const cpCmdLine, const DWORD dwcIdleWaitTime)
{
  return Create(cpcFileName, cpCmdLine, dwcIdleWaitTime);
};

_SL_END

#endif//__SLPRC_H