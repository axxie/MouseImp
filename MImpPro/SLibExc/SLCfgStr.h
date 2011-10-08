/**********************************************************************
ConfigStream implementation
**********************************************************************/

#ifndef __SLCFGSTR_H
#define __SLCFGSTR_H


#include <STRSTREA.H>

#include "SLUtil.h"

#include "SLDbg.h"

_SL_BEG


//////////////////////////////////////////////////////////////////////
//CSLCfgRegBase
//
//internal class - implement base registry operation's for CSLXxxCfgStr

class CSLCfgRegBase
{
protected:

  enum ClassConstEnum
  {
    //size of cpBuff buffer
    eccBuffSize = 2048
  };

  //handle of base key
  HKEY hKey;
  //BufferForCfg string
  TCHAR cpBuff[eccBuffSize];

public:

public:

  //open cpcKeyStr subkey from hBaseKey base (look HKEY_Xxxx const's) for read or write (bcWrite)
  inline CSLCfgRegBase(HKEY hBaseKey, LPCTSTR const cpcKeyStr, const bool bcWrite);
  //open as HKEY_CURRENT_USER\\software\\CompanyName\\SoftwareName
  //company/software from 
  CSLCfgRegBase(LPCTSTR const cpcSecName, const bool bcPerUser, const bool bcForWrite, HMODULE hModule = 0);
  //destruct and detach all
  inline ~CSLCfgRegBase();

protected:

  //clear all
  inline void Tidy();
  //internal open
  inline bool Open(HKEY hBaseKey, LPCTSTR const cpcKeyStr, const bool bcForWrite);
  
  //internal read func
  bool ReadSubKey(LPCTSTR const cpKeyName, LPTSTR const cpBuff, DWORD& rdwBuffSize);
  //internal write func
  inline bool WriteSubKey(LPCTSTR const cpcKeyName, LPCTSTR const cpcBuff, const DWORD dwcBuffSize);
  //get registry access const's for bcWrite access
  static inline REGSAM GetSec(const bool bcWrite);
};


//////////////////////////////////////////////////////////////////////
//CSLInCfgStr
//
//implement registry config getter like stl::ostream. 
//also implement Config versin control
//
//Example :
//
//void main()
//{
//  sl::CSLInCfgStr InStr(cpcECalcSecName, true);
//
//  InStr.SetEnt(cpcECalcWndInfo, dwcECalcPosVer);
//
//  CPoint Pnt;
//  InStr
//    >> Pnt.x
//    >> Pnt.y;
//  if(false == InStr.fail())
//  {
//    SetWindowPos(0, Pnt.x, Pnt.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
//  };
//
//  bool bAutoShowOff = false;
//  bool bTrayIcon = false;
//  bool bTopWnd = true;
//  InStr
//    >> bAutoShowOff
//    >> bTrayIcon
//    >> bTopWnd;
//  if(false != bTrayIcon)
//  {
//    TrayOn(true);
//  };
//  State[esfAutoShowOff] = bAutoShowOff;
//  SetTopWindow(bTopWnd);
//};

class CSLInCfgStr : public istrstream, CSLCfgRegBase
{
protected:

public:

public:

  //open cpcKeyName subkey from hBaseKey base key
  inline CSLInCfgStr(HKEY hBaseKey, LPCTSTR const cpcKeyName);
  //open cpcKeyName subkey from LOCAL_MACHINE (false == bcPerUser) or CurrentUser (false != bcPerUser)
  inline CSLInCfgStr(const bool bcPerUser, LPCTSTR const cpcKeyName);
  //open cpSecName config section on HOST\\software\\CompanyName\\SoftwareName
  //where HOST == (false == bcPerUser) ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER
  //CompanyName/SoftwareName getted from resource's VerInfo (sl::CSLVerInfo class) from hModule module
  inline CSLInCfgStr(LPCTSTR const cpSecName, const bool bcPerUser, HMODULE hModule = 0);

  //read specified cpcEntName section and match version number as dwcVerNumb (if version not matched - config read failed (multiple version's for config's))
  inline bool SetEnt(LPCTSTR const cpcEntName, const DWORD dwcVerNumb);

  //get pointer to interlan buffer (for read string)
  inline LPCTSTR GetBuff();

  //load window pos 
  void LoadWndPos(const HWND hcWnd);
  //load window place (all info)
  void LoadWndPlace(const HWND hcWnd);

protected:
  inline void Init();
};

//////////////////////////////////////////////////////////////////////
//CSLOutCfgStr - out config stream
//
//implement registry config storer like stl::ostream. 
//also implement Config versin control
//
//Example :
//
//void main()
//{
//  sl::CSLOutCfgStr OutStr(cpcECalcSecName, true);
//
//  OutStr.SetEnt(cpcECalcWndInfo, dwcECalcPosVer);
//  CRect Rect;
//  GetWindowRect(&Rect);
//  OutStr
//    << Rect.left
//    << Rect.top;
//
//  OutStr
//    << State[esfAutoShowOff]
//    << State[esfTrayIcon]
//    << IsTopWindow();
//};

class CSLOutCfgStr : public ostrstream, CSLCfgRegBase
{
protected:

  //name of entry presented config buffer 
  LPCSTR pcEntName;
  //is some output processed
  bool bSomeOut;

public:

public:

  //open cpcKeyName subkey from hBaseKey base key
  inline CSLOutCfgStr(HKEY hBaseKey, LPCTSTR const cpcKeyName);
  //open cpcKeyName subkey from LOCAL_MACHINE (false == bcPerUser) or CurrentUser (false != bcPerUser)
  inline CSLOutCfgStr(const bool bcPerUser, LPCTSTR const cpcKeyName);
  //open cpSecName config section on HOST\\software\\CompanyName\\SoftwareName
  //where HOST == (false == bcPerUser) ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER
  //CompanyName/SoftwareName getted from resource's VerInfo (sl::CSLVerInfo class) from hModule module
  inline CSLOutCfgStr(LPCTSTR const cpSecName, const bool bcPerUser, HMODULE hModule = 0);
  //destruct and plush data
  inline ~CSLOutCfgStr();

  //set cpcEntName entry and cfg version as dwcVerNumb. also flush old data
  inline bool SetEnt(LPCTSTR const cpcEntName, const DWORD dwcVerNumb);

  //overloaded "<<" operator for automaticaly ' ' insertion (between output values)
  template<class Type> CSLOutCfgStr& operator<<(const Type& rcOutParam)
  {
    ostrstream& rcStr = *this;
    if(false != bSomeOut)
    {
      rcStr
        << ' ';
    };
    rcStr
      << rcOutParam;
    bSomeOut = true;
    return *this;
  };
  //verride for string's
  inline CSLOutCfgStr& operator<<(LPCSTR const cpcStr);
  inline CSLOutCfgStr& operator<<(LPCWSTR const cpcStr);

  //flush chnges
  inline void flush();

  //save wnd pos
  inline void SaveWndPos(const HWND hcWnd);
  //save wnd place
  inline void SaveWndPlace(const HWND hcWnd);


protected:
  inline void Init();
  inline bool SaveCurrEnt();
};

/**********************************************************************
implementation
**********************************************************************/

//////////////////////////////////////////////////////////////////////
//CSLCfgRegBase

inline CSLCfgRegBase::CSLCfgRegBase(HKEY hBaseKey, LPCTSTR const cpcKeyStr, const bool bcWrite)
  :hKey(0)
{
  Open(hBaseKey, cpcKeyStr, bcWrite);
};

inline CSLCfgRegBase::~CSLCfgRegBase()
{
  Tidy();
};

inline void CSLCfgRegBase::Tidy()
{
  if(0 != hKey)
  {
    RegCloseKey(hKey);
    hKey = 0;
  };
};

inline bool CSLCfgRegBase::Open(HKEY hBaseKey, LPCTSTR const cpcKeyStr, const bool bcForWrite)
{
  bool bRes = false;
  Tidy();
  if(ERROR_SUCCESS != RegOpenKeyEx(hBaseKey, cpcKeyStr, 0, GetSec(bcForWrite), &hKey) && false != bcForWrite)
  {
    DWORD dwOpenState = REG_CREATED_NEW_KEY;
    if(ERROR_SUCCESS == RegCreateKeyEx(hBaseKey, cpcKeyStr, 0, 0, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &hKey, &dwOpenState))
    {
      bRes = true;
    };
  }
  else
  {
    bRes = true;
  };
  return bRes;
};

inline REGSAM CSLCfgRegBase::GetSec(const bool bcWrite)
{
  return (false != bcWrite)
    ? KEY_WRITE 
    : KEY_READ;
};

bool CSLCfgRegBase::WriteSubKey(LPCTSTR const cpcKeyName, LPCTSTR const cpcBuff, const DWORD dwcBuffSize)
{
  bool bRes = false;

  if(0 != hKey && ERROR_SUCCESS == RegSetValueEx(hKey, cpcKeyName, 0, REG_SZ, reinterpret_cast<CONST BYTE*>(cpcBuff), dwcBuffSize * sizeof(*cpcBuff)))
  {
    bRes = true;
  };

  return bRes;
};


//////////////////////////////////////////////////////////////////////
//CSLInCfgStr

inline CSLInCfgStr::CSLInCfgStr(HKEY hBaseKey, LPCTSTR const cpcKeyName)
  :CSLCfgRegBase(hBaseKey, cpcKeyName, false),
  istrstream(cpBuff, sizeof(cpBuff))
{
  Init();
};

inline CSLInCfgStr::CSLInCfgStr(const bool bcPerUser, LPCTSTR const cpcKeyName)
  :CSLCfgRegBase((false != bcPerUser) ? HKEY_CURRENT_USER : HKEY_LOCAL_MACHINE, cpcKeyName, false),
  istrstream(cpBuff, sizeof(cpBuff))
{
};

inline CSLInCfgStr::CSLInCfgStr(LPCTSTR const cpSecName, const bool bcPerUser, HMODULE hModule)
  :CSLCfgRegBase(cpSecName, bcPerUser, false, hModule),
  istrstream(cpBuff, sizeof(cpBuff))
{
  Init();
};

inline void CSLInCfgStr::Init()
{
  setf(skipws);
};

inline LPCTSTR CSLInCfgStr::GetBuff() 
{
  return (false != fail())
    ? ""
    : cpBuff + tellg();
};

inline bool CSLInCfgStr::SetEnt(LPCTSTR const cpcEntName, const DWORD dwcVerNumb)
{
  //clear state
  clear();
  bool bRes = false;
  //set fail bit's for stream
  DWORD dwVerId = 0;
  DWORD dwBuffSize = sizeof(cpBuff);
  //to start
  seekg(0);
  if(false != ReadSubKey(cpcEntName, cpBuff, dwBuffSize))
  {
    //only if version control enabled
    if(static_cast<DWORD>(-1) != dwcVerNumb)
    {
      *this
        >> dwVerId;
      //eat space after version number
      ignore();
      if(false != good() && dwcVerNumb == dwVerId)
      {
        bRes = true;
      };
    }
    else
    {
      bRes = true;
    };
  };
  if(false == bRes)
  {
    clear(failbit);
  };
  return bRes;
};

//////////////////////////////////////////////////////////////////////
//CSLOutCfgStr

inline CSLOutCfgStr::CSLOutCfgStr(HKEY hBaseKey, LPCTSTR const cpcKeyName)
  :CSLCfgRegBase(hBaseKey, cpcKeyName, true),
  ostrstream(cpBuff, sizeof(cpBuff)),
  pcEntName(0),
  bSomeOut(false)
{
  Init();
};

inline CSLOutCfgStr::CSLOutCfgStr(const bool bcPerUser, LPCTSTR const cpcKeyName)
  :CSLCfgRegBase((false != bcPerUser) ? HKEY_CURRENT_USER : HKEY_LOCAL_MACHINE, cpcKeyName, true),
  ostrstream(cpBuff, sizeof(cpBuff)),
  pcEntName(0),
  bSomeOut(false)
{
};

inline CSLOutCfgStr::CSLOutCfgStr(LPCTSTR const cpSecName, const bool bcPerUser, HMODULE hModule)
  :CSLCfgRegBase(cpSecName, bcPerUser, true, hModule),
  ostrstream(cpBuff, sizeof(cpBuff)),
  pcEntName(0),
  bSomeOut(false)
{
  Init();
};

inline CSLOutCfgStr::~CSLOutCfgStr()
{
  SaveCurrEnt();
};

inline void CSLOutCfgStr::Init()
{
  setf(skipws);
};

inline bool CSLOutCfgStr::SetEnt(LPCTSTR const cpcEntName, const DWORD dwcVerNumb)
{
  bool bRes = false;

  SaveCurrEnt();
  pcEntName = cpcEntName;
  seekp(0);
  clear();
  //only if version control enabled
  if(static_cast<DWORD>(-1) != dwcVerNumb)
  {
    (*static_cast<ostrstream*>(this))
      << dwcVerNumb
      << ' ';
  };
  //reset "some out" flag
  bSomeOut = false;

  bRes = true;

  return bRes;
};

inline bool CSLOutCfgStr::SaveCurrEnt()
{
  bool bRes = true;
  if(0 != pcEntName)
  {
    //end of line
    const DWORD dwcStrSize = tellp();
    cpBuff[dwcStrSize] = '\0';
    //write string
    bRes = false;
    if(false != WriteSubKey(pcEntName, cpBuff, dwcStrSize))
    {
      bRes = true;
    };
    pcEntName = 0;
  };
  return bRes;
};

inline void CSLOutCfgStr::flush()
{
  SaveCurrEnt();
};

inline CSLOutCfgStr& CSLOutCfgStr::operator<<(LPCSTR const cpcStr)
{
  ostrstream& rStr = *this;
  rStr.write(cpcStr, strlen(cpcStr));
  bSomeOut = true;
  return *this;
};

inline CSLOutCfgStr& CSLOutCfgStr::operator<<(LPCWSTR const cpcStr)
{
  ostrstream& rStr = *this;
  rStr.write(reinterpret_cast<LPCSTR>(cpcStr), wcslen(cpcStr) * sizeof(*cpcStr));
  bSomeOut = true;
  return *this;
};

inline void CSLOutCfgStr::SaveWndPos(const HWND hcWnd)
{
  RECT Rect;
  SLCHECK(FALSE != ::GetWindowRect(hcWnd, &Rect));
  (*this)
    << Rect.left
    << Rect.top;
};

inline void CSLOutCfgStr::SaveWndPlace(const HWND hcWnd)
{
  WINDOWPLACEMENT Place;
  Place.length = sizeof(Place);
  SLCHECK(FALSE != ::GetWindowPlacement(hcWnd, &Place));
  (*this)
    << Place.showCmd
    << Place.rcNormalPosition.left
    << Place.rcNormalPosition.top
    << Place.rcNormalPosition.right
    << Place.rcNormalPosition.bottom;
};



_SL_END

#endif//__SLCFGSTR_H