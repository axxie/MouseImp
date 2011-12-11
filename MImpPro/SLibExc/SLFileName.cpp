/**********************************************************************
file name class
**********************************************************************/


#define STRICT
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#pragma hdrstop


//incluse tchar bofore header for avoid _T macro redefenition by XLib
#include <tchar.h>


#include "SLFileName.h"


#include <list>


_SL_BEG

static const TCHAR cDriveSep = ':';
static LPCTSTR const cpcNameSep = "/\\";
static const TCHAR cExtSep = '.';
static LPCTSTR const cpcCompStart = "\\\\";

static LPCTSTR const cpscCompStr = "\\\\";
static LPCTSTR const cpscDriveStr = ":";
static LPCTSTR const cpscPathStr = "\\";
static const TCHAR cPathSep = '\\';
static LPCTSTR const cpscExtStr = ".";
static const TCHAR cCurrDir = '.';
static LPCTSTR const cpcPrevDir = "..";


//////////////////////////////////////////////////////////////////////
//str chunk's - helper for path list

struct StrChunk
{
  LPCTSTR const cpcStr;
  const DWORD dwcStrLen;
  inline StrChunk(LPCTSTR const cpcInitStr, const DWORD dwcInitStrLen);
  inline bool operator!=(const StrChunk& rcCmpChunk) const;
  inline bool IsPrev() const;
  inline bool IsCurr() const;
  inline bool IsRoot() const;
  inline bool IsNorm() const;
};

inline bool StrChunk::IsNorm() const
{
  bool bRes = true;
  if(false != IsRoot() || false != IsPrev() || false != IsCurr())
  {
    bRes = false;
  };
  return bRes;
};

inline StrChunk::StrChunk(LPCTSTR const cpcInitStr, const DWORD dwcInitStrLen)
  :cpcStr(cpcInitStr),
  dwcStrLen(dwcInitStrLen)
{
};

inline bool StrChunk::IsRoot() const
{
  return 0 == dwcStrLen;
};

inline bool StrChunk::IsPrev() const
{
  bool bRes = false;
  if(dwcStrLen == strlen(cpcPrevDir) && 0 == strncmp(cpcPrevDir, cpcStr, strlen(cpcPrevDir)))
  {
    bRes = true;
  };
  return bRes;
};

inline bool StrChunk::IsCurr() const
{
  bool bRes = false;
  if(1 == dwcStrLen && cCurrDir == *cpcStr)
  {
    bRes = true;
  };
  return bRes;
};

inline bool StrChunk::operator!=(const StrChunk& rcCmpChunk) const
{
  return dwcStrLen != rcCmpChunk.dwcStrLen || 0 != _strnicmp(cpcStr, rcCmpChunk.cpcStr, dwcStrLen);
};


//////////////////////////////////////////////////////////////////////
//some find function (SBCS only - possible need conversion to MBCS)



//find first TCHAR in passed str
static inline LPCTSTR FindFirstChar(LPCTSTR const cpcStr, const DWORD dwcStrLen, const TCHAR cChar)
{
  LPCTSTR pRes = cpcStr;
  for(DWORD dwCount = 0; dwcStrLen > dwCount; dwCount++)
  {
    if(cChar == *pRes)
    {
      return pRes;
    };
    pRes++;
  };
  return 0;
};

//find fist TCHAR from charset in passed str
static inline LPCTSTR FindFirstCharSet(LPCTSTR const cpcStr, const DWORD dwcStrLen, LPCTSTR const cpcCharSet)
{
  LPCSTR pRes = cpcStr;
  for(DWORD dwCount = 0; dwcStrLen > dwCount; dwCount++)
  {
    if(0 != _tcschr(cpcCharSet, *pRes))
    {
      return pRes;
    };
    pRes++;
  };
  return 0;
};

//find last TCHAR in str
static inline LPCTSTR FindLastChar(LPCTSTR const cpcStr, const DWORD dwcStrLen, const TCHAR cChar)
{
  LPCSTR pRes = cpcStr + dwcStrLen - 1;
  for(DWORD dwCount = 0; dwcStrLen > dwCount; dwCount++)
  {
    if(cChar == *pRes)
    {
      return pRes;
    };
    pRes--;
  };
  return 0;
};

//find last TCHAR from charset in passed string
static inline LPCTSTR FindLastCharSet(LPCTSTR const cpcStr, const DWORD dwcStrLen, LPCTSTR const cpcCharSet)
{
  LPCSTR pRes = cpcStr + dwcStrLen - 1;
  while(cpcStr <= pRes && 0 != *pRes)
  {
    if(0 != _tcschr(cpcCharSet, *pRes))
    {
      return pRes;
    };
    pRes--;
  };
  return 0;
};

//////////////////////////////////////////////////////////////////////
//CSLFileName


//stack by directories of path
//if path started from root (ex. \\SomeDir) then top element of stack has null length
typedef std::list<StrChunk> PathListType;

static inline void Path2Stack(LPCTSTR const cpscPath, const DWORD dwcStrLen, PathListType& rStack)
{
  LPCTSTR pcScanPos = cpscPath;
  int iStrLen = dwcStrLen;
  while(0 < iStrLen)
  {
    LPCTSTR const cpscDelimPos = FindFirstCharSet(pcScanPos, iStrLen, cpcNameSep);
    if(0 != cpscDelimPos)
    {
      const DWORD dwcDirSize = cpscDelimPos - pcScanPos;
      rStack.push_back(StrChunk(pcScanPos, dwcDirSize));
      pcScanPos += dwcDirSize + 1;
      iStrLen -= dwcDirSize + 1;
    }
    else
    {
      //last element
      rStack.push_back(StrChunk(pcScanPos, iStrLen));
      break;
    };
  };
};

static inline CSLString Stack2Path(PathListType& rStack)
{
  CSLString Res;
  
  while(false == rStack.empty())
  {
    const StrChunk cCurrPart(rStack.back());
    rStack.pop_back();
    if(false == rStack.empty())
    {
      Res =
        cpscPathStr
        + CSLString(cCurrPart.cpcStr, cCurrPart.dwcStrLen)
        + Res;
    }
    else if(0 != cCurrPart.dwcStrLen)
    {
      Res =
        CSLString(cCurrPart.cpcStr, cCurrPart.dwcStrLen)
        + Res;
    };
  };
  
  return Res;
};

static void NormStack(PathListType& rFromStack, PathListType& rToStack)
{
  while(false == rFromStack.empty())
  {
    const StrChunk Curr(rFromStack.front());
    rFromStack.pop_front();
    
    if
      (
      false != Curr.IsCurr()
      )
    {
      //".\" like
      continue;
    };
    if(false != Curr.IsPrev() && false == rToStack.empty() && false != rToStack.back().IsNorm())
    {
      //"SomeDir\..\" like
      rToStack.pop_back();
      continue;
    };
    rToStack.push_back(Curr);
  };
};

void CSLFileName::Parse(LPCTSTR const cpcParseStr, const DWORD dwcParts)
{
  //scaning filename from end (by part's priority - extention, name & other)
  LPCTSTR pcScanPos = cpcParseStr;
  size_t iStrLen = strlen(pcScanPos);
  //rest part - used for find "lowweight" part's
  DWORD dwRestParts = dwcParts;
  
  //try find extention
  dwRestParts &= (~enpExt);
  if((enpExt & dwcParts) && 0 < iStrLen)
  {
    LPCTSTR const cpcDelimPos = FindLastChar(pcScanPos, iStrLen, cExtSep);
    if(0 != cpcDelimPos)
    {
      const DWORD dwcExtStrLen = iStrLen - (cpcDelimPos - pcScanPos);
      //find one of "lowweight" delimiter's
      //      if
      //        (
      //        0 == FindLastChar(cpcDelimPos, dwcExtStrLen, cDriveSep)
      //        && 0 == FindLastCharSet(cpcDelimPos, dwcExtStrLen, cpcNameSep)
      //        )
      {
        //find extention - place it and remove from original (move StrLen)
        ExtStr = FileNameStrType(cpcDelimPos + 1, dwcExtStrLen - 1);
        iStrLen = cpcDelimPos - pcScanPos;
      };
    }
    else if(0 == dwRestParts)
    {
      ExtStr = FileNameStrType(pcScanPos, iStrLen);
      iStrLen = 0;
    };
  };
  //try find name
  dwRestParts &= (~enpName);
  if((dwcParts & enpName) && 0 < iStrLen)
  {
    //try find name sep
    LPCTSTR pcDelimPos = FindLastCharSet(pcScanPos, iStrLen, cpcNameSep);
    //if \\ not find - try find : - else treas whole string as name
    if(0 == pcDelimPos)
    {
      pcDelimPos = FindFirstChar(pcScanPos, iStrLen, cDriveSep);
      if(0 == pcDelimPos)
      {
        //':' not find - name from start of string
        pcDelimPos = pcScanPos - 1;
      };
    };
    //some succeeded
    if(0 != pcDelimPos)
    {
      const DWORD dwcNameLen = iStrLen - (pcDelimPos - pcScanPos);
      //one of "lowweights" delimiter's
      //      if
      //        (
      //        0 == FindLastChar(cpcDelimPos, dwcNameLen, cDriveSep)
      //        )
      {
        //find name
        NameStr = FileNameStrType(pcDelimPos + 1, dwcNameLen - 1);
        iStrLen = pcDelimPos - pcScanPos;
      };
    }
    else if(0 == dwRestParts)
    {
      NameStr = FileNameStrType(pcScanPos, iStrLen);
      iStrLen = 0;
    };
  };
  //try find compname
  dwRestParts &= (~enpComp);
  if((dwcParts & enpComp) && 0 < iStrLen)
  {
    if(0 == strncmp(pcScanPos, cpcCompStart, min(iStrLen, strlen(cpcCompStart))))
    {
      //found cpcCompStart at start of string - it's a computer name
      pcScanPos += strlen(cpcCompStart);
      iStrLen -= strlen(cpcCompStart);
      LPCTSTR const cpcDelimPos = FindFirstCharSet(pcScanPos, iStrLen, cpcNameSep);
      if(0 != cpcDelimPos)
      {
        const DWORD dwcCompLen = cpcDelimPos - pcScanPos;
        CompStr = FileNameStrType(pcScanPos, dwcCompLen);
        pcScanPos += dwcCompLen;
        iStrLen -= dwcCompLen;
      }
      else
      {
        CompStr = FileNameStrType(pcScanPos, iStrLen);
        iStrLen = 0;
      };
    }
    else if(0 == dwRestParts)
    {
      CompStr = FileNameStrType(pcScanPos, iStrLen);
      iStrLen = 0;
    };
  };
  //try find drive
  dwRestParts &= (~enpDrive);
  if((dwcParts & enpDrive) && 0 < iStrLen)
  {
    if(false == CompStr.IsEmpty())
    {
      //if found CompName - next part is ShareName
      //skip first '\\'
      if(0 != FindFirstChar(cpcNameSep, strlen(cpcNameSep), *pcScanPos))
      {
        pcScanPos++;
        iStrLen--;
      };
      LPCTSTR const cpcDelimPos = FindFirstCharSet(pcScanPos, iStrLen, cpcNameSep);
      if(0 != cpcDelimPos)
      {
        const DWORD dwcShareLen = cpcDelimPos - pcScanPos;
        DriveShareStr = FileNameStrType(pcScanPos, dwcShareLen);
        pcScanPos += dwcShareLen;
        iStrLen -= dwcShareLen;
      }
      else//all other is a share name //if(0 == dwRestParts)
      {
        DriveShareStr = FileNameStrType(pcScanPos, iStrLen);
        iStrLen = 0;
      };
    }
    else
    {
      //it's not UNC name - parse drive latter
      LPCTSTR const cpcDelimPos = FindFirstChar(pcScanPos, iStrLen, cDriveSep);
      if(0 != cpcDelimPos)
      {
        const DWORD dwcDriveLen = cpcDelimPos - pcScanPos;
        DriveShareStr = FileNameStrType(pcScanPos, dwcDriveLen);
        pcScanPos += dwcDriveLen + 1;
        iStrLen -= dwcDriveLen + 1;
      }
      else if(0 == dwRestParts)
      {
        DriveShareStr = FileNameStrType(pcScanPos, iStrLen);
        iStrLen = 0;
      };
    };
  };
  
  //try find path
  dwRestParts &= (~enpPath);
  if((dwcParts & enpPath) && 0 < iStrLen)
  {
    PathStr = FileNameStrType(pcScanPos, iStrLen);
    iStrLen = 0;
    
//    //process PathStr
//    PathListType FromStack;
//    Path2Stack(pcScanPos, iStrLen, FromStack);
//    PathListType ToStack;
//    NormStack(FromStack, ToStack);
//    PathStr = Stack2Path(ToStack);
//    iStrLen = 0;
  };
};

CSLFileName::FileNameStrType CSLFileName::PartGet(const BFilePartsEnum cPart) const
{
  FileNameStrType Res;

  if(cPart == enpComp)
  {
    Res = CompStr;
  }
  else if(cPart == enpShare || cPart == enpDrive)
  {
    Res = DriveShareStr;
  }
  else if(cPart == enpPath)
  {
    Res = PathStr;
  }
  else if(cPart == enpName)
  {
    Res = NameStr;
  }
  else if(cPart == enpExt)
  {
    Res = ExtStr;
  }
  else
  {
    ////unsopported part
    _ASSERT(FALSE);
  };

  return Res;
};

CSLFileName::FileNameStrType CSLFileName::PartsGet(const DWORD dwcParts) const
{
  FileNameStrType Res;
  
  if((enpPath & dwcParts) && false == PathStr.IsEmpty())
  {
    Res +=
      PathStr
      + cpscPathStr;
  };
  if((enpName & dwcParts) && false == NameStr.IsEmpty())
  {
    Res +=
      NameStr;
  };
  if((enpExt & dwcParts) && false == ExtStr.IsEmpty())
  {
    if(false == Res.IsEmpty())
    {
      Res += cpscExtStr + ExtStr;
    }
    else
    {
      Res = ExtStr;
    };
  };
  //is UNC name
  if(false == CompStr.IsEmpty())
  {
    if(0 != ((enpShare | enpComp) & dwcParts))
    {
      if(false == DriveShareStr.IsEmpty())
      {
        Res =
          DriveShareStr
          + cpscPathStr
          + Res;
      };
      Res =
        cpscCompStr
        + CompStr
        + cpscPathStr
        + Res;
    };
  }
  else
  {
    if(false == DriveShareStr.IsEmpty() && 0 != (dwcParts & enpDrive))
    {
      Res =
        DriveShareStr
        + cpscDriveStr
        + Res;
    };
  };
  
  return Res;
};

bool CSLFileName::PartsEmpty(const DWORD dwcParts)
{
  bool bRes = false;
  if((dwcParts & enpComp) && false == bRes)
  {
    bRes = CompStr.IsEmpty();
  };
  if(((dwcParts & enpShare) || (dwcParts & enpDrive)) && false == bRes)
  {
    bRes = DriveShareStr.IsEmpty();
  };
  if((dwcParts & enpPath) && false == bRes)
  {
    bRes = PathStr.IsEmpty();
  };
  if((dwcParts & enpName) && false == bRes)
  {
    bRes = NameStr.IsEmpty();
  };
  if((dwcParts & enpExt) && false == bRes)
  {
    bRes = ExtStr.IsEmpty();
  };
  return bRes;
};

void CSLFileName::PartsCls(const DWORD dwcParts)
{
  if(dwcParts & enpComp)
  {
    CompStr.Clear();
  };
  if((dwcParts & enpShare) || (dwcParts & enpDrive))
  {
    DriveShareStr.Clear();
  };
  if(dwcParts & enpPath)
  {
    PathStr.Clear();
  };
  if(dwcParts & enpName)
  {
    NameStr.Clear();
  };
  if(dwcParts & enpExt)
  {
    ExtStr.Clear();
  };
};

void CSLFileName::GetSpecial(const BFileNamesEnum eName)
{
  DWORD dwParts = 0;
  TCHAR cpPathBuff[MAX_PATH];
  switch(eName)
  {
  case esnCurrDir:
    {
      const DWORD dwcPathSize = ::GetCurrentDirectory(sizeof(cpPathBuff), cpPathBuff);
      if(0 < dwcPathSize)
      {
        dwParts = enpDrive | enpPath | enpShare | enpComp;
      };
    };
    break;

  case esnTmpFile:
  case esnTmpDir:
    {
      const DWORD dwcPathSize = ::GetTempPath(sizeof(cpPathBuff), cpPathBuff);
      if(0 < dwcPathSize)
      {
        dwParts = enpDrive | enpPath | enpShare | enpComp;
        if(esnTmpFile == eName)
        {
          ::GetTempFileName(cpPathBuff, "TFN", 0, cpPathBuff);
          dwParts |= enpName | enpExt;
        };
      };
    };
    break;

  case esnSysRoot:
    {
      ::GetWindowsDirectory(cpPathBuff, COUNTOF(cpPathBuff));
      dwParts = enpFullPath;
    };
    break;

  default:
    break;
  };
  if(0 != dwParts)
  {
    Parse(cpPathBuff, dwParts);
  };
};

void CSLFileName::PartsSet(const CSLFileName& rcSetName, const DWORD dwcParts)
{
  if(dwcParts & enpComp)
  {
    CompStr = rcSetName.CompStr;
  };
  if((dwcParts & enpShare) || (dwcParts & enpDrive))
  {
    DriveShareStr = rcSetName.DriveShareStr;
  };
  if(dwcParts & enpPath)
  {
    PathStr = rcSetName.PathStr;
  };
  if(dwcParts & enpName)
  {
    NameStr = rcSetName.NameStr;
  };
  if(dwcParts & enpExt)
  {
    ExtStr = rcSetName.ExtStr;
  };
};

CSLFileName CSLFileName::ToRel(const CSLFileName& rcBaseName) const
{
  CSLFileName Res(*this);
  if(false != CompStr.IsEmpty() || 0 == _tcscmp(CompStr, rcBaseName.CompStr))
  {
    Res.CompStr.Clear();
    if(false != DriveShareStr.IsEmpty() || 0 == _tcscmp(DriveShareStr, rcBaseName.DriveShareStr))
    {
      Res.DriveShareStr.Clear();
      //drive and comp name equal - compare path
      //present base path and it's "root" like
      if(false == rcBaseName.PathStr.IsEmpty() && cPathSep == rcBaseName.PathStr.At(0))
      {
        //this path present
        if(false == PathStr.IsEmpty())
        {
          //rootlike
          if(cPathSep == PathStr.At(0))
          {
            //root like
            PathListType BaseStack;
            Path2Stack(rcBaseName.PathStr, rcBaseName.PathStr.GetLen(), BaseStack);
            PathListType ThisStack;
            Path2Stack(PathStr, PathStr.GetLen(), ThisStack);
            while(false == BaseStack.empty() && false == ThisStack.empty())
            {
              if(BaseStack.front() != ThisStack.front())
              {
                break;
              };
              BaseStack.pop_front();
              ThisStack.pop_front();
            };
            for(DWORD dwCount = BaseStack.size(); 0 < dwCount; dwCount--)
            {
              ThisStack.push_back(StrChunk(cpcPrevDir, strlen(cpcPrevDir)));
            };
            Res.PathStr = Stack2Path(ThisStack);
          };
        };
      };
    };
  };
  return Res;
};

CSLFileName CSLFileName::ToAbs(const CSLFileName& rcBaseName) const
{
  CSLFileName Res(*this);

  if(false != CompStr.IsEmpty() || 0 == _tcscmp(rcBaseName.CompStr, CompStr))
  {
    if(false != DriveShareStr.IsEmpty() || 0 == _tcscmp(rcBaseName.DriveShareStr, DriveShareStr))
    {
      if(false == rcBaseName.PathStr.IsEmpty())
      {
        //based name path present

        if(false != PathStr.IsEmpty() || cPathSep != PathStr.At(0))
        {
          //this path not present or not "rootlike"
          //simpe add it's and convert by stack's
          Res.PathStr = rcBaseName.PathStr + cpscPathStr + PathStr;
          PathListType PathStack;
          Path2Stack(Res.PathStr, Res.PathStr.GetLen(), PathStack);
          PathListType NewStack;
          NormStack(PathStack, NewStack);
          Res.PathStr = Stack2Path(NewStack);
        }
        else
        {
          //"this" path not valid for "Absolute" rewinding - simple copy all
          if(false == PathStr.IsEmpty())
          {
            Res.PathStr = PathStr;
          };
        };
      };
      //drive/share
      Res.DriveShareStr = rcBaseName.DriveShareStr;
    };
    //copy CompName
    Res.CompStr = rcBaseName.CompStr;
  };

  return Res;
};

bool CSLFileName::DirChange() const
{
  bool bRes = false;

  const FileNameStrType cDir(PartsGet(enpFullPath));
  if(FALSE != ::SetCurrentDirectory(cDir))
  {
    bRes = true;
  };

  return bRes;
};


_SL_END
