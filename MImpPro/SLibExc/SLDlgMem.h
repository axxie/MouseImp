/**********************************************************************
Dlg template memory
**********************************************************************/

#ifndef __SLDLGMEM_H
#define __SLDLGMEM_H

#pragma once

#include "SLUtil.h"

#include "SLDbg.h"

_SL_BEG


//bug:eccDataAlign is set to sizeof(WORD), but from SDK should be sizeof(DWORD)


class CSLDlgTmplIter;
class CSLDlgTmpl;


//////////////////////////////////////////////////////////////////////
//DlgTmplEx struct's

/*
#pragma pack(push, 1)

typedef struct
{
        WORD dlgVer;
        WORD signature;
        DWORD helpID;
        DWORD exStyle;
        DWORD style;
        WORD cDlgItems;
        short x;
        short y;
        short cx;
        short cy;
} DLGTEMPLATEEX;

typedef struct
{
        DWORD helpID;
        DWORD exStyle;
        DWORD style;
        short x;
        short y;
        short cx;
        short cy;
        DWORD id;
} DLGITEMTEMPLATEEX;

#pragma pack(pop)
*/

//////////////////////////////////////////////////////////////////////
//CSLDlgTmpl

class CSLDlgTmpl
{
protected:

  friend CSLDlgTmplIter;

  enum ClassConstEnum
  {
    //extended (DLGTEMPLATEEX) template signature
    eccExtTmplSign = 0xffff,
    eccTmplMenuResSign = 0xffff,
    eccTmplClassOrdVal = 0xffff,
    eccTmplClassSysVal = 0x0000
  };

  //memory
  BYTE* pPtr;
  
public:
  
public:

  inline CSLDlgTmpl();
  inline CSLDlgTmpl(BYTE* const cpInitMem);

  //attach tmpl to class
  inline void Attach(BYTE* const cpInitMem);


  //is DlgTmpl extended
  inline bool IsExt() const;

  //////////////////////////////////////////////////////////////////////
  //for normal (DLGTEMPLATE) tamplates

  inline DLGTEMPLATE* GetTmpl() const;
  //get menu res string (if *GetMenu() == 0 - no menu present). if false != rResStr - *GetMenu() is a resource string
  inline LPWSTR GetMenu(bool& rResStr) const;
  //get class name of dlg. if (0 == *GetClass()) - system predefined class. if (false != rOrdVal) - *GetClass() - ordinal value of a predefined system window class (???)
  inline LPWSTR GetClass(bool& rOrdVal, bool& rSysClass) const;
  //if (0 == *GetTitle()) - title not present
  inline LPWSTR GetTitle() const;
  //return !0 if font setted (DS_SETFONT & GetTmpl()->style)
  inline LPWSTR GetFont(DWORD& rcFontPntSize) const;
  //is font present in tmpl (DS_SETFONT)
  inline bool HasFont() const;
  //get TmplItem base
  inline DLGITEMTEMPLATE* GetTmplItems() const;
  
protected:

  //////////////////////////////////////////////////////////////////////
  //internals for normal (DLGTEMPLATE) tamplates
  inline BYTE* GetTmplEnd() const;
  inline BYTE* GetMenuEnd() const;
  inline BYTE* GetClassEnd() const;
  inline BYTE* GetTitleEnd() const;
  inline BYTE* GetFontEnd() const;

  //skip string until not null
  inline static LPWSTR StrEnd(LPWSTR const cpStr);
};

//////////////////////////////////////////////////////////////////////
//CSLDlgTmplIter
//
//iterate through tmpl items

class CSLDlgTmplIter
{
protected:

  enum ClassContEnum
  {
    eccClassSign = 0xffff,
    eccClassAlign = sizeof(WORD),
    eccTitleSign = 0xffff,
    eccTitleAllign = sizeof(WORD),
    eccDataAlign = sizeof(WORD)/*sizeof(DWORD)*/,
    eccItemAllign = sizeof(DWORD)
  };
  
  //curr item
  DLGITEMTEMPLATE* pCurr;
  //item count (before end in pCurr)
  DWORD dwItemCount;

public:

  enum CtrlTypeEnum
  {
    ectButton = 0x80,
    ectEdit = 0x81,
    ectStatic = 0x82,
    ectListBox = 0x83,
    ectScrollBar = 0x84,
    ectComboBox = 0x85
  };
  
public:

  inline CSLDlgTmplIter();
  inline CSLDlgTmplIter(CSLDlgTmpl& rcInitTmpl);
  inline CSLDlgTmplIter(CSLDlgTmpl* const cpInitTmpl);

  //is curr member ok
  inline bool IsOk() const;
  //goto next item
  inline void Next();
  //goto next item (some as Next())
  inline void operator++();
  //goto next item (some as Next())
  inline void operator++(int);
  //get curr item info DLGITEMTEMPLATE
  inline DLGITEMTEMPLATE* GetItem() const;
  //some as GetItem()
  inline DLGITEMTEMPLATE* operator->() const;

  //////////////////////////////////////////////////////////////////////
  //DataAccess section

  //get class name (if false != bPredClass - *GetClass() treat as predefined classes - CtrlTypeEnum)
  inline LPWSTR GetClass(bool& bPredClass) const;
  //get title of item (if false != bResItem - treat *GetTitle() as resource identifer (icon and other)
  inline LPWSTR GetTitle(bool& bResItem) const;
  //return data mass for control and size of DataMass (rdwMassSize)
  inline BYTE* GetData(DWORD& rdwMassSize) const;
  
protected:

  //////////////////////////////////////////////////////////////////////
  //internal DataAccess section

  inline BYTE* GetItemEnd() const;
  inline BYTE* GetClassEnd() const;
  inline BYTE* GetTitleEnd() const;
  inline BYTE* GetDataEnd() const;

  //attach template to iterator
  inline void AttachTmpl(CSLDlgTmpl& rcInitTmpl);

  //allign mem to passed BYTE size
  inline static BYTE* Allign(BYTE* const cpMem, const DWORD dwcAllignSize);
};


/**********************************************************************
implementation
**********************************************************************/

//////////////////////////////////////////////////////////////////////
//CSLDlgTmpl

inline CSLDlgTmpl::CSLDlgTmpl()
  :pPtr(0)
{
};

inline CSLDlgTmpl::CSLDlgTmpl(BYTE* const cpInitMem)
  :pPtr(cpInitMem)
{
};

inline void CSLDlgTmpl::Attach(BYTE* const cpInitMem)
{
  pPtr = cpInitMem;
};

inline bool CSLDlgTmpl::IsExt() const
{
  return eccExtTmplSign == *reinterpret_cast<const WORD*>(pPtr);
};

inline DLGTEMPLATE* CSLDlgTmpl::GetTmpl() const
{
  return reinterpret_cast<DLGTEMPLATE*>(pPtr);
};

inline LPWSTR CSLDlgTmpl::StrEnd(LPWSTR const cpStr)
{
  LPWSTR pRes = cpStr;
  WCHAR Char;
  do
  {
    Char = *pRes;
    pRes++;
  }
  while(0 != Char);
	return pRes;
};

inline LPWSTR CSLDlgTmpl::GetMenu(bool& rResStr) const
{
  //menu string follow after DlgTmpl
  LPWSTR pStr = reinterpret_cast<LPWSTR>(GetTmplEnd());
  rResStr = (eccTmplMenuResSign == *pStr);
  if(false != rResStr)
  {
    pStr++;
  };
  return pStr;
};

inline LPWSTR CSLDlgTmpl::GetClass(bool& rOrdVal, bool& rSysClass) const
{
  //class string after menu string
  LPWSTR pRes = reinterpret_cast<LPWSTR>(GetMenuEnd());
  rSysClass = (eccTmplClassSysVal == *pRes);
  rOrdVal = (eccTmplClassOrdVal == *pRes);
  if(false != rOrdVal)
  {
    pRes++;
  };
  return pRes;
};

inline LPWSTR CSLDlgTmpl::GetTitle() const
{
  //title string after title string
  LPWSTR pRes = reinterpret_cast<LPWSTR>(GetClassEnd());
  return pRes;
};

inline LPWSTR CSLDlgTmpl::GetFont(DWORD& rcFontPntSize) const
{
  LPWSTR pRes = 0;
  if(false != HasFont())
  {
    //font info after title
    WORD* const cpSize = reinterpret_cast<WORD*>(GetTitleEnd());
    rcFontPntSize = *cpSize;
    pRes = reinterpret_cast<LPWSTR>(cpSize + 1);
  };
  return pRes;
};

inline bool CSLDlgTmpl::HasFont() const
{
  return 0 != (DS_SETFONT & GetTmpl()->style);
};


inline BYTE* CSLDlgTmpl::GetTmplEnd() const
{
  return reinterpret_cast<BYTE*>(GetTmpl() + 1);
};

inline BYTE* CSLDlgTmpl::GetMenuEnd() const
{
  bool bInRes;
  LPWSTR const cpMenu = GetMenu(bInRes);
  LPWSTR pRes = (false != bInRes)
    ? cpMenu + 1
    : StrEnd(cpMenu);
  return reinterpret_cast<BYTE*>(pRes);
};

inline BYTE* CSLDlgTmpl::GetClassEnd() const
{
  bool bOrdVal;
  bool bSysVal;
  LPWSTR const cpClass = GetClass(bOrdVal, bSysVal);
  LPWSTR pRes = (false != bOrdVal || false != bSysVal)
    ? cpClass + 1
    : StrEnd(cpClass);
  return reinterpret_cast<BYTE*>(pRes);
};

inline BYTE* CSLDlgTmpl::GetTitleEnd() const
{
  LPWSTR const cpTitle = GetTitle();
  return reinterpret_cast<BYTE*>(StrEnd(cpTitle));
};

inline BYTE* CSLDlgTmpl::GetFontEnd() const
{
  DWORD dwFontSize;
  LPWSTR const cpFont = reinterpret_cast<LPWSTR>(GetFont(dwFontSize));
  LPWSTR pRes = (0 == cpFont)
    ? cpFont + 1
    : StrEnd(cpFont);
  return reinterpret_cast<BYTE*>(pRes);
};

inline DLGITEMTEMPLATE* CSLDlgTmpl::GetTmplItems() const
{
  return reinterpret_cast<DLGITEMTEMPLATE*>(GetFontEnd());
};

//////////////////////////////////////////////////////////////////////
//CSLDlgTmplIter

inline CSLDlgTmplIter::CSLDlgTmplIter()
  :pCurr(0),
  dwItemCount(0)
{
};

inline CSLDlgTmplIter::CSLDlgTmplIter(CSLDlgTmpl& rcInitTmpl)
{
  AttachTmpl(rcInitTmpl);
};

inline CSLDlgTmplIter::CSLDlgTmplIter(CSLDlgTmpl* const cpInitTmpl)
{
  AttachTmpl(*cpInitTmpl);
};

inline bool CSLDlgTmplIter::IsOk() const
{
  return 0 != dwItemCount;
};

inline void CSLDlgTmplIter::Next()
{
  if(1 < dwItemCount)
  {
    BYTE* const cpDataEnd = GetDataEnd();
    pCurr = reinterpret_cast<DLGITEMTEMPLATE*>(Allign(cpDataEnd, eccItemAllign));
    dwItemCount--;
  }
  else
  {
    dwItemCount = 0;
  };
};

inline void CSLDlgTmplIter::operator++(int)
{
  Next();
};

inline void CSLDlgTmplIter::operator++()
{
  Next();
};

inline void CSLDlgTmplIter::AttachTmpl(CSLDlgTmpl& rcInitTmpl)
{
  //only for "normal" templates
  _ASSERT(false == rcInitTmpl.IsExt());
  BYTE* const cpTmplEnd = reinterpret_cast<BYTE*>(rcInitTmpl.GetFontEnd());
  pCurr = reinterpret_cast<DLGITEMTEMPLATE*>(Allign(cpTmplEnd, eccItemAllign));
  dwItemCount = rcInitTmpl.GetTmpl()->cdit;
};

inline DLGITEMTEMPLATE* CSLDlgTmplIter::GetItem() const
{
  //if correct call(???)
  _ASSERT(false != IsOk());
  return pCurr;
};

inline DLGITEMTEMPLATE* CSLDlgTmplIter::operator->() const
{
  return GetItem();
};

inline BYTE* CSLDlgTmplIter::GetItemEnd() const
{
  return reinterpret_cast<BYTE*>(GetItem() + 1);
};

inline BYTE* CSLDlgTmplIter::GetClassEnd() const
{
  bool bPred;
  LPWSTR const cpClass = GetClass(bPred);
  LPWSTR pRes = (false == bPred)
    ? CSLDlgTmpl::StrEnd(cpClass)
    : cpClass + 1;
  return reinterpret_cast<BYTE*>(pRes);
};

inline BYTE* CSLDlgTmplIter::GetTitleEnd() const
{
  bool bRes;
  LPWSTR const cpTitle = GetTitle(bRes);
  LPWSTR pRes = (false != bRes)
    ? cpTitle + 1
    : CSLDlgTmpl::StrEnd(cpTitle);
  return reinterpret_cast<BYTE*>(pRes);
};

inline BYTE* CSLDlgTmplIter::GetDataEnd() const
{
  DWORD dwMassSize;
  BYTE* const cpData = GetData(dwMassSize);
  return cpData + dwMassSize;
};

inline LPWSTR CSLDlgTmplIter::GetClass(bool& rbPredClass) const
{
  LPWSTR const cpClass = reinterpret_cast<LPWSTR>(Allign(GetItemEnd(), eccClassAlign));
  rbPredClass = eccClassSign == *cpClass;
  return (false != rbPredClass)
    ? cpClass + 1
    : cpClass;
};

inline LPWSTR CSLDlgTmplIter::GetTitle(bool& rbResItem) const
{
  LPWSTR const cpTitle = reinterpret_cast<LPWSTR>(Allign(GetClassEnd(), eccTitleAllign));
  rbResItem = (eccTitleSign == *cpTitle);
  return (false != rbResItem)
    ? cpTitle + 1
    : cpTitle;
};

inline BYTE* CSLDlgTmplIter::GetData(DWORD& rdwMassSize) const
{
  WORD* const cpData = reinterpret_cast<WORD*>(Allign(GetTitleEnd(), eccDataAlign));
  rdwMassSize = *cpData;
  return reinterpret_cast<BYTE*>(cpData + 1);
};

inline BYTE* CSLDlgTmplIter::Allign(BYTE* const cpMem, const DWORD dwcAllignSize)
{
  const DWORD dwcMask = ~(dwcAllignSize - 1);
  return reinterpret_cast<BYTE*>(reinterpret_cast<LPARAM>(cpMem + (dwcAllignSize - 1)) & dwcMask);
};


_SL_END

#endif//__SLDLGMEM_H