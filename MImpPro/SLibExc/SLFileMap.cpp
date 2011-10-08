/**********************************************************************
fileMap representation
**********************************************************************/

#define STRICT
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#pragma hdrstop


#include "SLFileMap.h"


#include "SLDbg.h"


_SL_BEG

//////////////////////////////////////////////////////////////////////
//CSLFileMap

bool CSLFileMap::OpenExist(LPCTSTR const cpcMapName, const DWORD dwcMapSize, const enum OpenMode ecOpenMode)
{
  bool bRes = false;

  //unmap exist
  UnMapFile();
  _ASSERT(0 != dwcMapSize && 0 != cpcMapName);
  SLPTRSTR(cpcMapName);

  //try open exist
  DWORD dwOpenMode = 0;
  switch(ecOpenMode)
  {
  case eomRead:
    dwOpenMode = FILE_MAP_READ;
    break;

  case eomWrite:
    dwOpenMode = FILE_MAP_WRITE;
    break;

  case eomReadWrite:
    dwOpenMode = FILE_MAP_ALL_ACCESS;
    break;

  default:
    break;
  };

  hMapping = OpenFileMapping(dwOpenMode, FALSE, cpcMapName);
  if(0 != hMapping)
  {
    //try map memory
    const DWORD dwcViewMode = (eomRead & ecOpenMode) ? FILE_MAP_READ : FILE_MAP_WRITE;
    pMap = reinterpret_cast<BYTE*>(MapViewOfFile
      (
      hMapping,
      dwcViewMode,
      0,
      0,
      dwcMapSize
      ));
    if(0 != pMap)
    {
      bRes = true;
    };
  };

  return bRes;
};

bool CSLFileMap::MapFile(LPCTSTR const cpcFileName, const enum OpenMode ecOpenMode, const DWORD dwcWantSize, LPCTSTR const cpcObjName)
{
  bool bRes = false;

  UnMapFile();

  //can't set size for read only file
  _ASSERT(static_cast<DWORD>(-1) == dwcWantSize || 0 != ((ecOpenMode & eomWrite) & eomReadWrite));

  //try open file (if need)
  bool bFileValid = true;
  if(0 != cpcFileName)
  {
    bFileValid = false;

    DWORD dwCreateMode = 0;
    DWORD dwShareMode = 0;
    switch(ecOpenMode)
    {
    case eomRead:
      dwCreateMode = GENERIC_READ;
      dwShareMode = FILE_SHARE_READ;
      break;

    case eomWrite:
    case eomReadWrite:
      dwCreateMode = GENERIC_READ | GENERIC_WRITE;
      break;

    default:
      _ASSERT(FALSE);
      break;
    };

    hFile = ::CreateFile
      (
      cpcFileName,
      dwCreateMode,
      dwShareMode,
      0,
      (static_cast<DWORD>(-1) == dwcWantSize) ? OPEN_EXISTING : OPEN_ALWAYS,
      FILE_FLAG_RANDOM_ACCESS,
      0
      );
    if(INVALID_HANDLE_VALUE != hFile)
    {
      dwMapSize = dwcWantSize;
      if(static_cast<DWORD>(-1) == dwcWantSize)
      {
        //use default file size
        dwMapSize = ::GetFileSize(hFile, 0);
      }
      else
      {
        //set file size
        ::SetFilePointer(hFile, dwMapSize, 0, FILE_BEGIN);
        ::SetEndOfFile(hFile);
      };
      bFileValid = true;
    };
  };

  if(false != bFileValid)
  {
    const DWORD dwcMappingMode = 
      (eomWrite & ecOpenMode) ? PAGE_READWRITE : PAGE_READONLY;
    hMapping = ::CreateFileMapping
      (
      //if filename apsent - map from page file
      (0 == cpcFileName) ? reinterpret_cast<HANDLE>(0xFFFFFFFF) : hFile,
      0,
      dwcMappingMode,
      0,
      (static_cast<DWORD>(-1) == dwcWantSize) ? 0 : dwcWantSize,
      cpcObjName
      );
    if(0 != hMapping)
    {
      const DWORD dwcViewMode = (eomWrite & ecOpenMode) ? FILE_MAP_ALL_ACCESS : FILE_MAP_READ;
      pMap = reinterpret_cast<BYTE*>(MapViewOfFile
        (
        hMapping,
        dwcViewMode,
        0,
        0,
        (static_cast<DWORD>(-1) == dwcWantSize) ? 0 : dwcWantSize
        ));
      if(0 != pMap)
      {
        bRes = true;
      };
    };
  };

  return bRes;
};

void CSLFileMap::UnMapFile()
{
  dwMapSize = 0;
  if(0 != pMap)
  {
    //flush data
    FlushViewOfFile(pMap, dwMapSize);
    UnmapViewOfFile(pMap);
    pMap = 0;
  };
  if(0 != hMapping)
  {
    CloseHandle(hMapping);
    hMapping = 0;
  };
  if(INVALID_HANDLE_VALUE != hFile)
  {
    ::CloseHandle(hFile);
    hFile = INVALID_HANDLE_VALUE;
  };
};

_SL_END
