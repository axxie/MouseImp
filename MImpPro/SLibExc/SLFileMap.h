/**********************************************************************
fileMap representation
**********************************************************************/

#ifndef __SLFILEMAP_H
#define __SLFILEMAP_H
#pragma once

#include <crtdbg.h>

#include "SLUtil.h"

#include "SLDbg.h"

_SL_BEG

//////////////////////////////////////////////////////////////////////
//CSLFileMap
//
//implement MemoryMappedFile (Win32)
//
//Example :
//
//void main()
//{
//  CSLFileMap Mem;
//  if(false != Mem.MapFile(_T("SomeFile")))
//  {
//  };
//};
//

class CSLFileMap
{
protected:

  //file handle
  HANDLE hFile;
  //file mapping object
  HANDLE hMapping;
  //mapping area
  BYTE* pMap;
  //MapSize
  DWORD dwMapSize;

public:

  //open modes for mapping
  enum OpenMode
  {
    eomRead = 1,
    eomWrite = 2,
    eomReadWrite = eomRead | eomWrite
  };

public:

  //construct empty
  inline CSLFileMap();
  //construct and open like MapFile func
  inline CSLFileMap(LPCTSTR const cpcFileName, const enum OpenMode ecOpenMode = eomReadWrite, const DWORD dwcWantSize = static_cast<DWORD>(-1));
  //destruct and free data
  inline ~CSLFileMap();

  //some sa create empty
  inline void Init();

  //is all ok 
  inline bool IsOk() const;
  //get associated memory
  inline BYTE* GetPtr() const;
  //get size of associated memory
  inline DWORD GetSize() const;

  //detach old and try attach new cpcFileName file as MemoryMappedFile in ecOpenMode mode 
  //and dwcWantSize size (if -1 and file already present - used true file size, else file created or shrinking to dwcWantSize size)
  //if 0 == cpcFileName - it's map from system page file
  bool MapFile(LPCTSTR const cpcFileName, const enum OpenMode ecOpenMode = eomReadWrite, const DWORD dwcWantSize = static_cast<DWORD>(-1), LPCTSTR const cpcObjName = 0);
  //some as MapFile
  inline bool Open(LPCTSTR const cpcFileName, const enum OpenMode ecOpenMode = eomReadWrite);
  //open existing mapiing (by name) and passed map size
  bool OpenExist(LPCTSTR const cpcMapName, const DWORD dwcMapSize, const enum OpenMode ecOpenMode = eomReadWrite);
  //free all data (if present)
  void UnMapFile();
  ////free all data
  //similar to UnMapFile
  inline void Close();


protected:

};

//////////////////////////////////////////////////////////////////////
//CSLFileOper

class CSLFileOper
{
protected:

  ////file handle
  HANDLE hFile;
  
public:
  
  //open modes for mapping
  enum OpenMode
  {
    eomRead = 1,
    eomWrite = 2,
    eomReadWrite = eomRead | eomWrite,
    eomMakeIfApsent = 4,
  };

public:

  inline CSLFileOper();
  inline ~CSLFileOper();

  inline bool Open(LPCTSTR const cpcFileName, const DWORD dwcOpenMode = eomReadWrite | eomMakeIfApsent);
  inline void Close();

  ///get file size
  inline DWORD GetSize() const;
  //trip file to current "file" pos
  inline void SetFileEnd() const;
  ////write/read file
  //return false if errro
  inline bool Read(BYTE* const cpBuff, const DWORD dwcSize) const;
  inline bool Read(void* const cpBuff, const DWORD dwcSize) const;
  inline bool Write(const BYTE* const cpcBuff, const DWORD dwcSize) const;
  inline bool Write(const void* const cpcBuff, const DWORD dwcSize) const;
  ////seek file to pos
  inline void SeekTo(const DWORD dwcFromStart) const;
  inline void SeekOff(const DWORD dwcOffset) const;
  inline void SeekEnd() const;

  ////is file ok
  inline bool IsOk() const;

  ////get file creation time
  inline void GetFileCreateTime(FILETIME& rOutFileTime) const;
  inline void GetFileModifyTime(FILETIME& rOutFileTime) const;
  inline void SetFileAllTime(const FILETIME& rcSetTime) const;

  ////get/set attr's
  static inline DWORD GetAttr(LPCTSTR const cpcFileName);
  static inline bool SetAttr(LPCTSTR const cpcFileName, const DWORD dwcAddAttr, const DWORD dwcDelAttr);

protected:
};



//////////////////////////////////////////////////////////////////////
//CSLSharedMem

template<class PtrType>
class CSLSharedMem
{
protected:

  ////shared mapping hadnle's
  HANDLE hMap;
  PtrType* pMem;

public:
  
public:

  inline CSLSharedMem();
  inline ~CSLSharedMem();

  ////open/close object's
  //return true if open ok
  inline bool Open(LPCTSTR const cpcMemName, bool& rOutOpenExist, const bool bcForWrite);
  inline void Close();

  ////get object/status
  inline PtrType* GetPtr() const;
  inline bool IsOk() const;
  
protected:
};



/**********************************************************************
implementation
**********************************************************************/

//////////////////////////////////////////////////////////////////////
//CSLFileMap

inline CSLFileMap::CSLFileMap()
{
  Init();
};

inline CSLFileMap::CSLFileMap(LPCTSTR const cpcFileName, const enum OpenMode ecOpenMode, const DWORD dwcWantSize)
{
  Init();
  MapFile(cpcFileName, ecOpenMode, dwcWantSize);
};

inline CSLFileMap::~CSLFileMap()
{
  UnMapFile();
};

inline void CSLFileMap::Init()
{
  hFile = INVALID_HANDLE_VALUE;
  hMapping = 0;
  pMap = 0;
  dwMapSize = 0;
};

inline BYTE* CSLFileMap::GetPtr() const
{
  return pMap;
};

inline DWORD CSLFileMap::GetSize() const
{
  return dwMapSize;
};

inline bool CSLFileMap::IsOk() const
{
  return 0 != pMap;
};

inline bool CSLFileMap::Open(LPCTSTR const cpcFileName, const enum OpenMode ecOpenMode)
{
  return MapFile(cpcFileName, ecOpenMode);
};

inline void CSLFileMap::Close()
{
  UnMapFile();
};

//////////////////////////////////////////////////////////////////////
//CSLSharedMem

template<class PtrType>
inline CSLSharedMem<PtrType>::CSLSharedMem()
  :hMap(0),
  pMem(0)
{
};

template<class PtrType>
inline CSLSharedMem<PtrType>::~CSLSharedMem()
{
  Close();
};

template<class PtrType>
inline bool CSLSharedMem<PtrType>::Open(LPCTSTR const cpcMemName, bool& rOutOpenExist, const bool bcForWrite)
{
  ////already open 
  _ASSERT(false == IsOk());

  bool bRes = true;
  rOutOpenExist = false;

  ////try open exist/create new mapping
  if(false != bRes)
  {
    rOutOpenExist = true;
    hMap = ::OpenFileMapping
      (
      (false != bcForWrite) ? (FILE_MAP_READ | FILE_MAP_WRITE) : FILE_MAP_READ,
      FALSE,
      cpcMemName
      );
    if(0 == hMap)
    {
      rOutOpenExist = false;
      hMap = ::CreateFileMapping
        (
        reinterpret_cast<HANDLE>(0xFFFFFFFF),
        0,
        (false != bcForWrite) ? PAGE_READWRITE : PAGE_READONLY,
        0,
        sizeof(PtrType),
        cpcMemName
        );
    };
    bRes = 0 != hMap;
  };

  ////map object now
  if(false != bRes)
  {
    LPVOID const cpTmpMem = ::MapViewOfFile
      (
      hMap,
      (false != bcForWrite) ? FILE_MAP_WRITE : FILE_MAP_READ,
      0, 
      0,
      sizeof(PtrType)
      );
    pMem = reinterpret_cast<PtrType*>(cpTmpMem);
    bRes = 0 != pMem;
  };

  return bRes;
};

template<class PtrType>
inline void CSLSharedMem<PtrType>::Close()
{
  if(0 != pMem)
  {
    ::UnmapViewOfFile(pMem);
    pMem = 0;
  };
  if(0 != hMap)
  {
    ::CloseHandle(hMap);
    hMap = 0;
  };
};

template<class PtrType>
inline PtrType* CSLSharedMem<PtrType>::GetPtr() const
{
  _ASSERT(false != IsOk());
  return pMem;
};

template<class PtrType>
inline bool CSLSharedMem<PtrType>::IsOk() const
{
  return 0 != pMem;
};

//////////////////////////////////////////////////////////////////////
//CSLFileOper


inline CSLFileOper::CSLFileOper()
  :hFile(INVALID_HANDLE_VALUE)
{
};

inline CSLFileOper::~CSLFileOper()
{
  Close();
};

inline bool CSLFileOper::Open(LPCTSTR const cpcFileName, const DWORD dwcOpenMode)
{
  ////already open
  _ASSERT(false == IsOk());

  DWORD dwCreateMode = 0;
  DWORD dwAccess = 0;
  DWORD dwShareMode = 0;

  if(0 != (eomRead & dwcOpenMode))
  {
    dwAccess |= GENERIC_READ;
    dwShareMode = FILE_SHARE_WRITE | FILE_SHARE_READ;
  };
  if(0 != (eomWrite & dwcOpenMode))
  {
    dwAccess |= GENERIC_WRITE;
    dwShareMode = FILE_SHARE_READ;
  };
  dwCreateMode = (0 != (eomMakeIfApsent & dwcOpenMode))
    ? OPEN_ALWAYS
    : OPEN_EXISTING;

  hFile= ::CreateFile
    (
    cpcFileName,
    dwAccess, 
    dwShareMode,
    0,
    dwCreateMode,
    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
    0
    );

  return IsOk();
};

inline void CSLFileOper::Close()
{
  if(INVALID_HANDLE_VALUE != hFile)
  {
    SLCHECK(FALSE != ::CloseHandle(hFile));
    hFile = INVALID_HANDLE_VALUE;
  };
};

inline DWORD CSLFileOper::GetSize() const
{
  _ASSERT(false != IsOk());
  return ::GetFileSize(hFile, 0);
};

inline bool CSLFileOper::Read(void* const cpBuff, const DWORD dwcSize) const
{
  return Read(reinterpret_cast<BYTE*>(cpBuff), dwcSize);
};

inline bool CSLFileOper::Read(BYTE* const cpBuff, const DWORD dwcSize) const
{
  _ASSERT(false != IsOk());

  DWORD dwReadSize = 0;
  const BOOL bcRes = ::ReadFile
    (
    hFile, 
    cpBuff, 
    dwcSize, 
    &dwReadSize, 
    0
    );
  return FALSE != bcRes && dwcSize == dwReadSize;
};

inline bool CSLFileOper::Write(const void* const cpcBuff, const DWORD dwcSize) const
{
  return Write(reinterpret_cast<const BYTE*>(cpcBuff), dwcSize);
};

inline bool CSLFileOper::Write(const BYTE* const cpcBuff, const DWORD dwcSize) const
{
  _ASSERT(false != IsOk());

  DWORD dwWriteSize = 0;
  const BOOL bcRes = ::WriteFile
    (
    hFile,
    cpcBuff, 
    dwcSize,
    &dwWriteSize, 
    0
    );
  return FALSE != bcRes && dwcSize == dwWriteSize;
};

inline void CSLFileOper::SeekTo(const DWORD dwcFromStart) const
{
  _ASSERT(false != IsOk());
  SLCHECK(0xFFFFFFFF != ::SetFilePointer(hFile, dwcFromStart, 0, FILE_BEGIN));
};

inline void CSLFileOper::SeekEnd() const
{
  _ASSERT(false != IsOk());
  SLCHECK(0xFFFFFFFF != ::SetFilePointer(hFile, 0, 0, FILE_END));
};

inline void CSLFileOper::SeekOff(const DWORD dwcOffset) const
{
  _ASSERT(false != IsOk());
  SLCHECK(0xFFFFFFFF != ::SetFilePointer(hFile, dwcOffset, 0, FILE_CURRENT));
};

inline bool CSLFileOper::IsOk() const
{
  return INVALID_HANDLE_VALUE != hFile;
};

inline void CSLFileOper::GetFileModifyTime(FILETIME& rOutFileTime) const
{
  _ASSERT(false != IsOk());
  SLCHECK(FALSE != ::GetFileTime(hFile, 0, 0, &rOutFileTime));
};

inline void CSLFileOper::GetFileCreateTime(FILETIME& rOutFileTime) const
{
  _ASSERT(false != IsOk());
  SLCHECK(FALSE != ::GetFileTime(hFile, &rOutFileTime, 0, 0));
};

inline void CSLFileOper::SetFileAllTime(const FILETIME& rcSetTime) const
{
  _ASSERT(false != IsOk());
  const BOOL bcRes = ::SetFileTime(hFile, &rcSetTime, &rcSetTime, &rcSetTime);
  SLCHECK(FALSE != bcRes);
};

inline DWORD CSLFileOper::GetAttr(LPCTSTR const cpcFileName)
{
  return ::GetFileAttributes(cpcFileName);
};

inline bool CSLFileOper::SetAttr(LPCTSTR const cpcFileName, const DWORD dwcAddAttr, const DWORD dwcDelAttr)
{
  bool bRes = false;

  const DWORD dwcCurr = ::GetFileAttributes(cpcFileName);
  if(0xFFFFFFFF != dwcCurr)
  {
    DWORD dwAttr = dwcCurr;
    dwAttr &= ~dwcDelAttr;
    dwAttr |= dwcAddAttr;
    bRes = FALSE != ::SetFileAttributes(cpcFileName, dwAttr);
  };

  return bRes;
};

inline void CSLFileOper::SetFileEnd() const
{
  SLCHECK(FALSE != ::SetEndOfFile(hFile));
};

_SL_END

#endif//__SLFILEMAP_H