/**********************************************************************
file name class 
**********************************************************************/

#ifndef __SLFILENAME_H
#define __SLFILENAME_H

#include "SLUtil.h"

#include "SLDbg.h"

#include "SLString.h"


_SL_BEG



//////////////////////////////////////////////////////////////////////
//CSLFileName - stl <string> requeired

class CSLFileName
{
protected:

  typedef CSLString FileNameStrType;

  //comp name (if UNC)
  FileNameStrType CompStr;
  //drive/ShareName (if UNC)
  FileNameStrType DriveShareStr;
  //Path
  FileNameStrType PathStr;
  //name
  FileNameStrType NameStr;
  //extention
  FileNameStrType ExtStr;

public:

  enum BFilePartsEnum
  {
    enpComp = 1,
    enpShare = 2,
    enpDrive = 2,
    enpPath = 4,
    enpName = 8,
    enpExt = 16,
    enpAll = enpExt | enpName | enpPath | enpDrive | enpShare | enpComp,
    enpFullPath = enpComp | enpDrive | enpPath | enpShare,
    enpFullName = enpExt | enpName,
    enpPathName = enpPath | enpExt | enpName
  };

  enum BFileNamesEnum
  {
    //current application idrectory
    esnCurrDir = 0,
    //temp file (tmp name and tmp directory)
    esnTmpFile,
    //tmp directory
    esnTmpDir,
    //system roo tirectory
    esnSysRoot
  };


public:

  //empty
  inline CSLFileName();
  //from module file name (ex. from AfxGetInstanceHandle())
  inline CSLFileName(HMODULE hModule);
  //copy
  inline CSLFileName(const CSLFileName& rcInitName);
  //parse part's
  explicit inline CSLFileName(LPCTSTR const cpcInitName, const DWORD dwcHasParts);
  //special part's
  inline CSLFileName(const BFileNamesEnum eName);

  inline FileNameStrType GetFullName() const;
  inline FileNameStrType GetName() const;
  inline FileNameStrType GetExt() const;
  inline FileNameStrType GetPath() const;
  inline FileNameStrType GetFullPath() const;
  inline FileNameStrType GetPathName() const;
  inline FileNameStrType GetAll() const;
  void GetSpecial(const BFileNamesEnum eName);

  //dataAccess
  inline bool IsUnc() const;
  FileNameStrType PartsGet(const DWORD dwcParts) const;
  FileNameStrType PartGet(const BFilePartsEnum cPart) const;
  void PartsSet(const CSLFileName& rcSetName, const DWORD dwcParts);
  inline void PartsSet(LPCTSTR const cpcInitName, const DWORD dwcParts);
  inline void PartsSetIfEmpty(LPCTSTR const cpcInitName, const DWORD dwcParts);
  void PartsCls(const DWORD dwcParts);
  bool PartsEmpty(const DWORD dwcParts);

  //convert this name from absolute to relative as based on rcBaseName as current directory
  CSLFileName ToRel(const CSLFileName& rcBaseName) const;
  //convert this name from relative to absolute based on rcBaseName as on "Current directory"
  CSLFileName ToAbs(const CSLFileName& rcBaseName) const;

  //FileOperation's
  //name validity
  inline bool IsValid() const;
  static inline bool IsValid(LPCTSTR const cpcFileName);
  //file existance
  inline bool IsExist() const;
  static inline bool IsExist(LPCTSTR const cpcFileName);
  //remove file
  inline bool FileDelete() const;
  static inline bool FileDelete(LPCTSTR const cpcFileName);
  //file copy 
  inline bool FileCopy(const CSLFileName& rcToName, const bool bcFailIfExist) const;
  static inline bool FileCopy(LPCTSTR const cpcFromName, LPCTSTR const cpcToName, const bool bcFailIfExist);
  //file move 
  inline bool FileMove(const CSLFileName& rcToName);
  static inline bool FileMove(LPCTSTR const cpcFromName, LPCTSTR const cpcToName);
  //change to dir
  bool DirChange() const;
  //create dir
  inline bool DirCreate() const;
  static inline bool DirCreate(LPCTSTR const cpcDirName);
  //remove dir
  static inline bool DirDelete(LPCTSTR const cpcDirName);
  inline bool DirDelete() const;

  //get file information

  //file time (ptr's can be null's if not requierd) - result in UTC
  static inline bool GetFileTime(LPCSTR const cpFileName, FILETIME* const cpCreateTime, FILETIME* const cpLastAccessTime, FILETIME* const cpLastWriteTime);
  static inline bool GetFileTime(HANDLE hFile, FILETIME* const cpCreateTime, FILETIME* const cpLastAccessTime, FILETIME* const cpLastWriteTime);
  inline bool GetFileTime(FILETIME* const cpCreateTime, FILETIME* const cpLastAccessTime, FILETIME* const cpLastWriteTime);


protected:

  //set specified Name part's
  void Parse(LPCTSTR const cpcParseStr, const DWORD dwcParts);
};

//////////////////////////////////////////////////////////////////////
//CSLFileFindIter

class CSLFileFindIter : public CSLFileName
{
protected:

  ////Win32 find info
  WIN32_FIND_DATA FindData;
  HANDLE hFind;
  ////find ok
  BOOL bFindOk;
  
public:
  
public:

  inline CSLFileFindIter(const CSLFileName& rcNameInit);
  inline CSLFileFindIter();
  inline ~CSLFileFindIter();

  ////open/close find
  inline void Open();
  inline void Close();

  ////find ok
  inline bool IsOk() const;
  ////goto next
  inline void Next();
  ////get find info
  inline const WIN32_FIND_DATA* GetFileInfo() const;
  
protected:
  ////sync file name with find data
  inline void SyncWithFind();
};



/**********************************************************************
implementation
**********************************************************************/

//////////////////////////////////////////////////////////////////////
//CSLFileName

inline CSLFileName::CSLFileName()
  :CompStr(),
  DriveShareStr(),
  PathStr(),
  NameStr(),
  ExtStr()
{
};

inline CSLFileName::CSLFileName(HMODULE hModule)
{
  TCHAR cpBuff[MAX_PATH];
  ::GetModuleFileName(hModule, cpBuff, COUNTOF(cpBuff));
  Parse(cpBuff, enpAll);
};

inline CSLFileName::CSLFileName(const CSLFileName& rcInitName)
  :CompStr(rcInitName.CompStr),
  DriveShareStr(rcInitName.DriveShareStr),
  PathStr(rcInitName.PathStr),
  NameStr(rcInitName.NameStr),
  ExtStr(rcInitName.ExtStr)
{
};

inline CSLFileName::CSLFileName(const BFileNamesEnum eName)
{
  GetSpecial(eName);
};

inline CSLFileName::CSLFileName(LPCTSTR const cpcInitName, const DWORD dwcHasParts)
{
  Parse(cpcInitName, dwcHasParts);
};

inline bool CSLFileName::IsUnc() const
{
  return false == CompStr.IsEmpty();
};

inline CSLFileName::FileNameStrType CSLFileName::GetName() const
{
  return PartsGet(enpName);
};

inline CSLFileName::FileNameStrType CSLFileName::GetExt() const
{
  return PartsGet(enpExt);
};

inline CSLFileName::FileNameStrType CSLFileName::GetPath() const
{
  return PartsGet(enpPath);
};

inline CSLFileName::FileNameStrType CSLFileName::GetFullName() const
{
  return PartsGet(enpFullName);
};

inline CSLFileName::FileNameStrType CSLFileName::GetFullPath() const
{
  return PartsGet(enpFullPath);
};

inline CSLFileName::FileNameStrType CSLFileName::GetPathName() const
{
  return PartsGet(enpPathName);
};

inline CSLFileName::FileNameStrType CSLFileName::GetAll() const
{
  return PartsGet(enpAll);
};

inline bool CSLFileName::IsValid() const
{
  const FileNameStrType cName(GetAll());
  return IsValid(cName);
};

inline bool CSLFileName::IsValid(LPCTSTR const cpcFileName) 
{
  bool bRes = false;

  OFSTRUCT ofs;           
  ofs.cBytes = sizeof ofs;
  if(HFILE_ERROR != ::OpenFile(cpcFileName, &ofs, OF_PARSE))
  {
    bRes = true;
  };

  return bRes;
};

inline bool CSLFileName::IsExist() const
{
  const FileNameStrType cName(GetAll());
  return IsExist(cName);
};

inline bool CSLFileName::IsExist(LPCTSTR const cpcFileName) 
{
  bool bRes = false;

  OFSTRUCT ofs;           
  ofs.cBytes = sizeof ofs;  
  if(HFILE_ERROR != ::OpenFile(cpcFileName, &ofs, OF_EXIST))
  {
    bRes = true;
  };

  return bRes;
};

inline void CSLFileName::PartsSetIfEmpty(LPCTSTR const cpcInitName, const DWORD dwcParts)
{
  CSLFileName Add(cpcInitName, dwcParts);
  if(0 != (enpComp & dwcParts) && false != CompStr.IsEmpty())
  {
    CompStr = Add.CompStr;
  };
  if(0 != ((enpShare | enpDrive) & dwcParts) && false != DriveShareStr.IsEmpty())
  {
    DriveShareStr = Add.DriveShareStr;
  };
  if(0 != (enpPath & dwcParts) && false != PathStr.IsEmpty())
  {
    PathStr = Add.PathStr;
  };
  if(0 != (enpName & dwcParts) && false != NameStr.IsEmpty())
  {
    NameStr = Add.NameStr;
  };
  if(0 != (enpExt & dwcParts) && false != ExtStr.IsEmpty())
  {
    ExtStr = Add.ExtStr;
  };
};

inline void CSLFileName::PartsSet(LPCTSTR const cpcInitName, const DWORD dwcParts)
{
  PartsCls(dwcParts);
  Parse(cpcInitName, dwcParts);
};

inline bool CSLFileName::FileCopy(LPCTSTR const cpcFromName, LPCTSTR const cpcToName, const bool bcFailIfExist)
{
  return FALSE != ::CopyFile(cpcFromName, cpcToName, bcFailIfExist);
};

bool CSLFileName::FileCopy(const CSLFileName& rcToName, const bool bcFailIfExist) const
{
  const FileNameStrType cName(PartsGet(enpAll));
  const FileNameStrType cToName(rcToName.PartsGet(enpAll));
  return FileCopy(cName, cToName, bcFailIfExist);
};

inline bool CSLFileName::FileMove(LPCTSTR const cpcFromName, LPCTSTR const cpcToName)
{
  return FALSE != ::MoveFile(cpcFromName, cpcToName);
};

inline bool CSLFileName::FileMove(const CSLFileName& rcToName)
{
  bool bRes = false;

  const FileNameStrType cName(PartsGet(enpAll));
  const FileNameStrType cToName(rcToName.PartsGet(enpAll));
  if(false != FileMove(cName, cToName))
  {
    PartsSet(rcToName, enpAll);
    bRes = true;
  };

  return bRes;
};

inline bool CSLFileName::GetFileTime(LPCSTR const cpFileName, FILETIME* const cpCreateTime, FILETIME* const cpLastAccessTime, FILETIME* const cpLastWriteTime)
{
  bool bRes = false;
  HANDLE hFile = ::CreateFile
    (
    cpFileName, 
    0, 
    FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
    0,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    0
    );
  if(INVALID_HANDLE_VALUE != hFile)
  {
    bRes = GetFileTime(hFile, cpCreateTime, cpLastAccessTime, cpLastWriteTime);
    ::CloseHandle(hFile);
  };
  return bRes;
};

inline bool CSLFileName::GetFileTime(HANDLE hFile, FILETIME* const cpCreateTime, FILETIME* const cpLastAccessTime, FILETIME* const cpLastWriteTime)
{
  return FALSE != ::GetFileTime(hFile, cpCreateTime, cpLastAccessTime, cpLastWriteTime);
};

inline bool CSLFileName::GetFileTime(FILETIME* const cpCreateTime, FILETIME* const cpLastAccessTime, FILETIME* const cpLastWriteTime)
{
  const FileNameStrType cName(PartsGet(enpAll));
  return GetFileTime(cName, cpCreateTime, cpLastAccessTime, cpLastWriteTime);
};

inline bool CSLFileName::FileDelete(LPCTSTR const cpcFileName) 
{
  return FALSE != ::DeleteFile(cpcFileName);
};

inline bool CSLFileName::FileDelete() const
{
  const FileNameStrType cName(PartsGet(enpAll));
  return FileDelete(cName);
};

inline bool CSLFileName::DirDelete(LPCTSTR const cpcDirName)
{
  return FALSE != ::RemoveDirectory(cpcDirName);
}

inline bool CSLFileName::DirDelete() const
{
  const FileNameStrType cDir(PartsGet(enpFullPath));
  return DirDelete(cDir);
};

inline bool CSLFileName::DirCreate(LPCTSTR const cpcDirName)
{
  bool bRes = false;

  if(FALSE != ::CreateDirectory(cpcDirName, 0))
  {
    bRes = true;
  };

  return bRes;
};

inline bool CSLFileName::DirCreate() const
{
  const FileNameStrType cDir(PartsGet(enpFullPath));
  return DirCreate(cDir);
};

//////////////////////////////////////////////////////////////////////
//CSLFileFindIter

inline CSLFileFindIter::CSLFileFindIter(const CSLFileName& rcNameInit)
  :CSLFileName(rcNameInit),
  hFind(0),
  bFindOk(FALSE)
{
};

inline CSLFileFindIter::CSLFileFindIter()
  :hFind(0),
  bFindOk(FALSE)
{
};

inline CSLFileFindIter::~CSLFileFindIter()
{
  Close();
};

inline void CSLFileFindIter::Open()
{
  Close();

  ////get string from filename
  hFind = ::FindFirstFile(GetAll(), &FindData);
  bFindOk = INVALID_HANDLE_VALUE != hFind;
  SyncWithFind();
};

inline void CSLFileFindIter::Close()
{
  if(0 != hFind)
  {
    ::FindClose(hFind);
    hFind = 0;
    bFindOk = FALSE;
  };
};

inline bool CSLFileFindIter::IsOk() const
{
  return FALSE != bFindOk;
};

inline void CSLFileFindIter::Next()
{
  _ASSERT(false != IsOk());
  bFindOk = ::FindNextFile(hFind, &FindData);
  SyncWithFind();
};

inline const WIN32_FIND_DATA* CSLFileFindIter::GetFileInfo() const
{
  _ASSERT(false != IsOk());
  return &FindData;
};

inline void CSLFileFindIter::SyncWithFind()
{
  if(false != bFindOk)
  {
    PartsSet(FindData.cFileName, enpFullName);
  };
};

_SL_END

#endif//__SLFILENAME_H
