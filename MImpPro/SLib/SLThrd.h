/**********************************************************************
threading support
**********************************************************************/

#ifndef __SLTHRD_H
#define __SLTHRD_H

#pragma once

#include "SLUtil.h"

#include "SLDbg.h"

#include <process.h>

_SL_BEG


//////////////////////////////////////////////////////////////////////
//CSLCritSect - critical section

class CSLCritSect : public CRITICAL_SECTION
{
protected:
  
public:
  
public:

  //create/destroy
  inline CSLCritSect();
  inline ~CSLCritSect();
  //lock/unlock
  inline void Lock();
  inline void UnLock();
  
protected:
};


//////////////////////////////////////////////////////////////////////
//CSLLock - locker class

template<class ObjType>
class CSLLock
{
protected:

  //ref to locked object
  ObjType& rObj;
  
public:
  
public:

  //auto lock/unlock
  inline CSLLock(ObjType& rInitObj);
  inline ~CSLLock();
  
protected:
};



//////////////////////////////////////////////////////////////////////
//CSLThread - implement thread ctrl
//
//inherit from this class and implement ThreadCircleFunc 
//(bool ThreadCircleFunc() - if not more need working - return false)
//(or ThreadFunc for non trivial functionaly)
//
//Example:
//class CMyThrd : public CSLThread<CMyThrd>
//{
//protected:
//  
//public:
//  
//public:
//  
//protected:
//
//  bool ThreadCircleFunc()
//  {
//    return true;
//  };
//
//};


template<class Child>
class CSLThread
{
protected:

  //handle of thread
  HANDLE hThrd;
  //close event
  HANDLE hCloseEvent;
  
public:
  
public:

  //empty
  inline CSLThread();
  //create thread (dwcPrior - THREAD_PRIORITY_XXXXX)
  inline CSLThread(const DWORD dwcPrior);
  //destroy
  inline ~CSLThread();

  //create thread with passes priority
  inline bool Create(const DWORD dwcPrior = THREAD_PRIORITY_NORMAL);

  //is thread created
  inline bool IsOk() const;  

  //thread handle
  inline HANDLE Get() const;
  inline operator HANDLE() const;

  //wait for end thread in passed time
  inline bool WaitForEnd(const DWORD dwcWaitTime = INFINITE);
  //set "end thread" flag
  //for actual thread end need call WaitForEnd
  inline void SetEnd();
  //terminate thread immediate
  inline bool Terminate(); 

protected:

  //free all resources
  inline void Free();

  //start thread func
  inline static unsigned __stdcall StartThrd(void* pParam);
  //main thread proc (turned on this class)
  inline void ThreadFunc();
  //test for "need end" function
  inline bool EndTestFunc();
  //get "EndEvent" handle
  inline HANDLE GetEndEvent() const;
};


/**********************************************************************
implementation
**********************************************************************/

//////////////////////////////////////////////////////////////////////
//CSLThread

template<class Type>
inline CSLThread<Type>::CSLThread()
  :hThrd(0),
  hCloseEvent(0)
{
};

template<class Type>
inline CSLThread<Type>::CSLThread(const DWORD dwcPrior)
  :hThrd(0),
  hCloseEvent(0)
{
  SLCHECK(false != Create(dwcPrior));
};

template<class Type>
inline CSLThread<Type>::~CSLThread()
{
  //are thread correctly terminated???
  _ASSERT(false == IsOk());
  Terminate();
  WaitForEnd();
};

template<class Type>
inline void CSLThread<Type>::Free()
{
  if(0 != hThrd)
  {
    CloseHandle(hThrd);
    hThrd = 0;
  };
  if(0 != hCloseEvent)
  {
    CloseHandle(hCloseEvent);
    hCloseEvent = 0;
  };
};

template<class Type>
inline bool CSLThread<Type>::Create(const DWORD dwcPrior)
{
  //not shuld be already created
  _ASSERT(false == IsOk());
  _ASSERT(0 == hCloseEvent);

  //create end event
  hCloseEvent = CreateEvent(0, FALSE, FALSE, 0);

  unsigned uiThrdId = 0;
  //cast from this to derived class
  Type* const cpDer = static_cast<Type*>(this);
  const unsigned long ulcRes = _beginthreadex(0, 0, StartThrd, reinterpret_cast<void*>(cpDer), dwcPrior, &uiThrdId);
  hThrd = reinterpret_cast<HANDLE>(ulcRes);

  return IsOk();
};

template<class Type>
inline bool CSLThread<Type>::IsOk() const
{
  return 0 != hThrd;
};

template<class Type>
inline unsigned __stdcall CSLThread<Type>::StartThrd(void* pParam)
{
  //unwind passed arg and call thrd func
  Type* const cpThis = reinterpret_cast<Type*>(pParam);
  cpThis->ThreadFunc();
  return 0;
};

template<class Type>
inline void CSLThread<Type>::ThreadFunc()
{
  while(false != EndTestFunc())
  {
    if(false == static_cast<Type*>(this)->ThreadCircleFunc())
    {
      //end cycle
      break;
    };
  };
};

template<class Type>
inline HANDLE CSLThread<Type>::Get() const
{
  return hThrd;
};

template<class Type>
inline CSLThread<Type>::operator HANDLE() const
{
  return Get();
};

template<class Type>
inline bool CSLThread<Type>::WaitForEnd(const DWORD dwcWaitTime)
{
  bool bRes = false;

  if(false != IsOk())
  {
    bRes = WAIT_OBJECT_0 == WaitForSingleObject(Get(), dwcWaitTime);
    if(false != bRes)
    {
      Free();
    };
  };

  return bRes;
};

template<class Type>
inline void CSLThread<Type>::SetEnd()
{
  _ASSERT(0 != hCloseEvent);
  SetEvent(hCloseEvent);
};

template<class Type>
inline bool CSLThread<Type>::Terminate()
{
  bool bRes = false;
  if(false != IsOk())
  {
    bRes = FALSE != TerminateThread(Get(), 0);
    if(false != bRes)
    {
      Free();
    };
  };
  return bRes;
};

template<class Type>
inline bool CSLThread<Type>::EndTestFunc()
{
  return WAIT_OBJECT_0 == WaitForSingleObject(GetEndEvent());
};

template<class Type>
inline HANDLE CSLThread<Type>::GetEndEvent() const
{
  return hCloseEvent;
};

//////////////////////////////////////////////////////////////////////
//CSLCritSect

inline CSLCritSect::CSLCritSect()
{
  ::InitializeCriticalSection(this);
};

inline CSLCritSect::~CSLCritSect()
{
  ::DeleteCriticalSection(this);
};

inline void CSLCritSect::Lock()
{
  ::EnterCriticalSection(this);
};

inline void CSLCritSect::UnLock()
{
  ::LeaveCriticalSection(this);
};

//////////////////////////////////////////////////////////////////////
//CSLLock

template<class ObjType>
inline CSLLock<ObjType>::CSLLock(ObjType& rInitObj)
  :rObj(rInitObj)
{
  rObj.Lock();
};

template<class ObjType>
inline CSLLock<ObjType>::~CSLLock()
{
  rObj.UnLock();
};



_SL_END

#endif//__SLTHRD_H
