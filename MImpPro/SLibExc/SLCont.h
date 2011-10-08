/**********************************************************************
template contsinter's New
**********************************************************************/

#ifndef __SLCONT_H
#define __SLCONT_H
#pragma once

#include "SLUtil.h"

#include "SLDbg.h"

#include "SLMemMgr.h"


#include <limits.h>


_SL_BEG

//////////////////////////////////////////////////////////////////////
//CSLStack

template<class Type, class StoreType = Type, const DWORD dwcFrameSize = 20>
class CSLStack
{
protected:
  
  //node for store information
  struct CNode
  {
    //data storage
    StoreType Data;
    //helper's
    inline CNode(Type InitData) : Data(InitData){0;};
    inline ~CNode(){0;};
  };

  //stack frame header
  struct CFrameHdr
  {
    //next frame in frame list
    CFrameHdr* pNextFrame;
  };

  //current pos in current stack frame
  DWORD dwPagePos;
  //current stack frame
  CFrameHdr* pCurrFrame;

  //list of "free frame's"
  CFrameHdr* pFreeFrames;


public:
  
public:

  inline CSLStack();
  inline ~CSLStack();

  //empty test
  inline bool IsEmpty() const;
  //if not empty
  inline bool IsOk() const;
  //get not on top of stack
  inline StoreType* Top() const;
  //push data 
  inline StoreType* Push(Type InitData);
  //pop data
  inline void Pop();
  //free all from stack 
  inline void FreeAll(const bool bcFreeMem = false);
  
protected:

  //get start "alloc'ed" memory from frame
  static inline CNode* GetStartOfFrame(CFrameHdr* const cpFrame);
  //alloc new stack frame
  inline CFrameHdr* NewFrame();
  //delete stack frame (to free list)
  inline void DeleteFrame(CFrameHdr* const cpFrame);
  //new store node
  inline CNode* NewNode(Type InitData, const DWORD dwcInFramePos, CFrameHdr* const cpAllocFrame);
  //delete store node
  inline void DeleteNode(const DWORD dwcInFramePos, CFrameHdr* const cpAllocFrame);
  //calc size of stack frame
  static inline DWORD CalcFrameSize();
};



//////////////////////////////////////////////////////////////////////
//CSLSList

template<class Type, class StoreType = Type>
class CSLSList
{
protected:
  
  struct CNode
  {
    //data storage
    StoreType Data;
    //nex node in Hash
    CNode* pNext;
    //helper's
    inline CNode(Type InitData, CNode* const cpInitNext) : Data(InitData), pNext(cpInitNext){};
    inline ~CNode(){0;};
  };

  //memory mgr for hash node's
  CSLMemChunk NodeMemMgr;
  //head of list
  CNode* pHead;

public:

  //iterator in list
  typedef void* iterator;
  
public:

  inline CSLSList(const DWORD dwcEntryCount = 20);
  inline ~CSLSList();

  //first
  inline iterator IterBegin();
  //next
  inline iterator IterNext(iterator Curr) const;
  //"ok" text
  inline bool IterIsOk(iterator Curr) const;
  //get by iterator
  inline StoreType* IterGet(iterator Curr) const;
  //remove item from list, pointer by iterator
  inline void IterRemove(iterator Curr);

  //is not empty
  inline bool IsOk() const;
  //return "true" is list empty
  inline bool IsEmpty() const;
  //insert
  inline iterator Push(Type InitData);
  //remove
  inline void Pop();
  //get top
  inline StoreType* Get();
  //delete all from list
  void FreeAll(const bool bcDelMem = false);
  
protected:

  //alloc node
  inline CNode* NewNode(Type InitData, CNode* const cpInitNext);
  //delete node
  inline void DelNode(CNode* const cpNode);
  //iter to pos
  inline CNode** IterToPos(iterator Iter) const;
  //pos to iter
  inline iterator PosToIter(CNode** const cppNode) const;
};

//////////////////////////////////////////////////////////////////////
//CSLHashSet
//
//simple HashSet (for lightweit operation's)
//
//Type = type inited stored data (user operation's)
//StoreType = actually stored data type
//dwcSizePow = power of size (1 << dwcSizePow) of hash tbl)
//
//for normal ussage should present
//operator==(StoreType, Type);
//StoreType::StoreType(Type);
//__SLHashFunc(Type); - implemented for most common types

template<class Type, class StoreType = Type, const DWORD dwcSizePow = 6>
class CSLHashSet
{
protected:

  enum ClassConstEnum
  {
    eccTblSize = 1 << dwcSizePow,
    eccTblSizeMask = eccTblSize - 1
  };

  struct CNode
  {
    //data storage
    StoreType Data;
    //nex node in Hash
    CNode* pNext;
    //helper's
    inline CNode(Type InitData, CNode* const cpInitNext) : Data(InitData), pNext(cpInitNext){};
    inline ~CNode(){0;};
  };

  //memory mgr for hash node's
  CSLMemChunk NodeMemMgr;

  //hash mass
  CNode* pHashMass[eccTblSize];
  
public:

  struct iterator
  {
    inline iterator(const iterator& rcInitIter) : dwHashPos(rcInitIter.dwHashPos), ppNode(rcInitIter.ppNode){};
  protected:
    //only hash set can use
    friend CSLHashSet;
    //hash pos
    DWORD dwHashPos;
    //node ptr's
    CNode* const * ppNode;
    inline iterator(const DWORD dwcInitHashPos, CNode** const cppInitNode) : dwHashPos(dwcInitHashPos), ppNode(cppInitNode){};
  };

public:


  //construct 
  inline CSLHashSet(const DWORD dwcEntryCount = 20);
  //destruct (clear all data)
  inline ~CSLHashSet();

  //find in set (0 if not find)
  inline StoreType* Find(Type Key) const;
  //add entry to set
  inline StoreType* Add(Type Key);
  //remove from set (false if not remover (apsent))
  inline bool Remove(Type Key);

  //free all (false != bcFreeAllMem - free all internal's memory)
  inline void FreeAll(const bool bcFreeAllMem = false);

  //if container empty
  inline bool IsEmpty() const;
  //calc count of items in container
  inline DWORD GetCount() const;

  //iter's
  //begin
  inline iterator IterBegin();
  //next
  inline iterator IterNext(iterator Curr) const;
  //"ok" text
  inline bool IterIsOk(const iterator& Curr) const;
  //get by iterator
  inline StoreType* IterGet(const iterator& Curr) const;
  //remove item from list, pointer by iterator
  inline void IterRemove(const iterator& Curr);

  
protected:

  //make new node
  inline CNode* NewNode(Type InitData, CNode* const cpInitNext);
  //delete node
  inline void DelNode(CNode* const cpNode);
  //recalc Type to HashPos
  inline DWORD TypeToHashPos(Type InitData) const;

};

//////////////////////////////////////////////////////////////////////
//HashFunc's

template<class Type>
inline DWORD __SLHashFunc(Type Val)
{
  return ((DWORD)Val);
};

template<>
inline DWORD __SLHashFunc<LPCTSTR>(LPCTSTR pStr)
{
  DWORD dwRes = 0;
  for(LPCSTR pcScan = pStr; 0 != *pcScan; pcScan++)
  {
    dwRes = (dwRes << (sizeof(dwRes) * CHAR_BIT / 2)) + dwRes + *pcScan;
  };
  return dwRes;
};

/**********************************************************************
impelemntation
**********************************************************************/

//////////////////////////////////////////////////////////////////////
//CSLHashSet


template<class Type, class StoreType, const DWORD dwcSizePow>
inline CSLHashSet<Type, StoreType, dwcSizePow>::CSLHashSet(const DWORD dwcEntryCount)
  :NodeMemMgr(sizeof(CNode), dwcEntryCount)
{
  ZeroMemory(pHashMass, sizeof(*pHashMass) * eccTblSize);
};

template<class Type, class StoreType, const DWORD dwcSizePow>
inline CSLHashSet<Type, StoreType, dwcSizePow>::~CSLHashSet()
{
  FreeAll();
};

template<class Type, class StoreType, const DWORD dwcSizePow>
inline void CSLHashSet<Type, StoreType, dwcSizePow>::FreeAll(const bool bcFreeAllMem)
{
  for(DWORD dwCount = 0; eccTblSize > dwCount; dwCount++)
  {
    CNode*& rcBasePtr = pHashMass[dwCount];
    while(0 != rcBasePtr)
    {
      CNode* const cpDelNode = rcBasePtr;
      rcBasePtr = rcBasePtr->pNext;
      DelNode(cpDelNode);
    };
  };
  if(false != bcFreeAllMem)
  {
    NodeMemMgr.FreeAll();
  };
};

template<class Type, class StoreType, const DWORD dwcSizePow>
inline typename CSLHashSet<Type, StoreType, dwcSizePow>::CNode* CSLHashSet<Type, StoreType, dwcSizePow>::NewNode(Type InitData, CNode* const cpInitNext)
{
  CNode* const cpRes = reinterpret_cast<CNode*>(NodeMemMgr.New());
  cpRes->CNode::CNode(InitData, cpInitNext);
  return cpRes;
};

template<class Type, class StoreType, const DWORD dwcSizePow>
inline void CSLHashSet<Type, StoreType, dwcSizePow>::DelNode(CNode* const cpNode)
{
  //destruct
  cpNode->~CNode();
  //del mem
  NodeMemMgr.Delete(cpNode);
};

template<class Type, class StoreType, const DWORD dwcSizePow>
inline bool CSLHashSet<Type, StoreType, dwcSizePow>::IsEmpty() const
{
  //scan for all hash tbl and if not null find - not empty
  bool bRes = true;
  for(DWORD dwCount = 0; eccTblSize > dwCount; dwCount++)
  {
    if(0 != pHashMass[dwCount])
    {
      bRes = false;
      break;
    };
  };
  return bRes;
};

template<class Type, class StoreType, const DWORD dwcSizePow>
inline DWORD CSLHashSet<Type, StoreType, dwcSizePow>::GetCount() const
{
  DWORD dwRes = 0;

  for(DWORD dwCount = 0; eccTblSize > dwCount; dwCount++)
  {
    for(CNode* pLook = pHashMass[dwCount]; 0 != pLook; pLook = pLook->pNext)
    {
      dwRes++;
    };
  };

  return dwRes;
};

template<class Type, class StoreType, const DWORD dwcSizePow>
inline StoreType* CSLHashSet<Type, StoreType, dwcSizePow>::Find(Type Key) const
{
  StoreType* pRes = 0;
  const DWORD dwcHashPos = TypeToHashPos(Key);
  for(CNode* pLook = pHashMass[dwcHashPos]; 0 != pLook; pLook = pLook->pNext)
  {
    if(pLook->Data == Key)
    {
      pRes = &(pLook->Data);
      break;
    };
  };
  return pRes;
};

template<class Type, class StoreType, const DWORD dwcSizePow>
inline StoreType* CSLHashSet<Type, StoreType, dwcSizePow>::Add(Type Key)
{
  StoreType* pRes = 0;

  const DWORD dwcHashPos = TypeToHashPos(Key);

  CNode* const cpNew = NewNode(Key, pHashMass[dwcHashPos]);
  pHashMass[dwcHashPos] = cpNew;
  pRes = &(cpNew->Data);
 
  return pRes;
};

template<class Type, class StoreType, const DWORD dwcSizePow>
inline bool CSLHashSet<Type, StoreType, dwcSizePow>::Remove(Type Key)
{
  bool bRes = false;
  const DWORD dwcPos = TypeToHashPos(Key);
  for(CNode** ppLook = &(pHashMass[dwcPos]); 0 != *ppLook; ppLook = &((*ppLook)->pNext))
  {
    if((*ppLook)->Data == Key)
    {
      //we find - remove
      CNode* const cpDel = *ppLook;
      *ppLook = cpDel->pNext;
      DelNode(cpDel);
      
      bRes = true;
      break;
    };
  };
  return bRes;
};

template<class Type, class StoreType, const DWORD dwcSizePow>
inline DWORD CSLHashSet<Type, StoreType, dwcSizePow>::TypeToHashPos(Type InitData) const
{
  return __SLHashFunc(InitData) & eccTblSizeMask;
};

template<class Type, class StoreType, const DWORD dwcSizePow>
inline typename CSLHashSet<Type, StoreType, dwcSizePow>::iterator CSLHashSet<Type, StoreType, dwcSizePow>::IterBegin()
{
  //try find first item in hash
  DWORD dwHashPos = 0;
  CNode** ppNode = &(pHashMass[0]);
  //check if not empty
  while(0 == *ppNode)
  {
    if(eccTblSize - 1 > dwHashPos)
    {
      dwHashPos++;
      ppNode = &(pHashMass[dwHashPos]);
    }
    else
    {
      dwHashPos = eccTblSize;
      break;
    };
  };
  //result
  return iterator(dwHashPos, ppNode);
};

template<class Type, class StoreType, const DWORD dwcSizePow>
inline typename CSLHashSet<Type, StoreType, dwcSizePow>::iterator CSLHashSet<Type, StoreType, dwcSizePow>::IterNext(iterator Curr) const
{
  _ASSERT(false != IterIsOk(Curr));
  //goto next
  Curr.ppNode = &((*(Curr.ppNode))->pNext);
  //if it's end of hash line - goto next
  while(0 == *(Curr.ppNode))
  {
    if(eccTblSize - 1 > Curr.dwHashPos)
    {
      Curr.dwHashPos++;
      Curr.ppNode = &(pHashMass[Curr.dwHashPos]);
    }
    else
    {
      //end of hash pos - bewak
      Curr.dwHashPos = eccTblSize;
      break;
    };
  };
  return Curr;
};

template<class Type, class StoreType, const DWORD dwcSizePow>
inline bool CSLHashSet<Type, StoreType, dwcSizePow>::IterIsOk(const iterator& Curr) const
{
  _ASSERT(0 == *(Curr.ppNode) || false != NodeMemMgr.CheckChunk(*(Curr.ppNode)));
  return 0 != *(Curr.ppNode) && eccTblSize > Curr.dwHashPos;
};

template<class Type, class StoreType, const DWORD dwcSizePow>
inline StoreType* CSLHashSet<Type, StoreType, dwcSizePow>::IterGet(const iterator& Curr) const
{
  _ASSERT(false != IterIsOk(Curr));
  return &((*(Curr.ppNode))->Data);
};

template<class Type, class StoreType, const DWORD dwcSizePow>
inline void CSLHashSet<Type, StoreType, dwcSizePow>::IterRemove(const iterator& Curr)
{
  _ASSERT(false != IterIsOk(Curr));
  //detach node 
  CNode* const *& rpcpNodePtr = Curr.ppNode;
  CNode* pDelNode = *rpcpNodePtr;
  rpcpNodePtr = pDelNode->pNext;
  //delete node
  DelNode(pDelNode);
};

//////////////////////////////////////////////////////////////////////
//CSLSList

template<class Type, class StoreType>
inline CSLSList<Type, StoreType>::CSLSList(const DWORD dwcEntryCount)
  :NodeMemMgr(sizeof(CNode), dwcEntryCount),
  pHead(0)
{
};

template<class Type, class StoreType>
inline CSLSList<Type, StoreType>::~CSLSList()
{
  FreeAll();
};

template<class Type, class StoreType>
inline typename CSLSList<Type, StoreType>::iterator CSLSList<Type, StoreType>::IterBegin() 
{
  return PosToIter(&pHead);
};

template<class Type, class StoreType>
inline typename CSLSList<Type, StoreType>::iterator CSLSList<Type, StoreType>::IterNext(iterator Curr) const
{
  _ASSERT(false != IterIsOk(Curr));
  CNode** const cppThis = IterToPos(Curr);
  //res next
  return PosToIter(&((*cppThis)->pNext));
};

template<class Type, class StoreType>
inline bool CSLSList<Type, StoreType>::IterIsOk(iterator Curr) const
{
  //invalid iter not from this list
  _ASSERT(0 == *IterToPos(Curr) || false != NodeMemMgr.CheckChunk(*IterToPos(Curr)));
  return 0 != *IterToPos(Curr);
};

template<class Type, class StoreType>
inline void CSLSList<Type, StoreType>::IterRemove(iterator Curr)
{
  _ASSERT(false != IterIsOk(Curr));

  //remove
  CNode** const cppCurr = IterToPos(Curr);
  CNode* const cpDelNode = *cppCurr;
  *cppCurr = cpDelNode->pNext;
  //delete
  DelNode(cpDelNode);
};

template<class Type, class StoreType>
inline StoreType* CSLSList<Type, StoreType>::IterGet(iterator Curr) const
{
  _ASSERT(false != IterIsOk(Curr));
  return &((*IterToPos(Curr))->Data);
};

template<class Type, class StoreType>
inline bool CSLSList<Type, StoreType>::IsEmpty() const
{
  return 0 == pHead;
};

template<class Type, class StoreType>
inline bool CSLSList<Type, StoreType>::IsOk() const
{
  return 0 != pHead;
};

template<class Type, class StoreType>
inline typename CSLSList<Type, StoreType>::iterator CSLSList<Type, StoreType>::Push(Type InitData)
{
  //new node
  CNode* const cpNewNode = NewNode(InitData, pHead);
  pHead = cpNewNode;
  //res
  return PosToIter(&pHead);
};

template<class Type, class StoreType>
inline void CSLSList<Type, StoreType>::Pop()
{
  _ASSERT(false != IsOk());
  //remnove
  CNode* const cpDelNode = pHead;
  pHead = pHead->pNext;
  //delete
  DelNode(cpDelNode);
};

template<class Type, class StoreType>
inline StoreType* CSLSList<Type, StoreType>::Get()
{
  _ASSERT(false != IsOk());
  return &(pHead->Data);
};

template<class Type, class StoreType>
inline typename CSLSList<Type, StoreType>::CNode** CSLSList<Type, StoreType>::IterToPos(iterator Iter) const
{
  return reinterpret_cast<CNode**>(Iter);
};

template<class Type, class StoreType>
inline typename CSLSList<Type, StoreType>::iterator CSLSList<Type, StoreType>::PosToIter(CNode** const cppNode) const
{
  return reinterpret_cast<iterator>(cppNode);
};

template<class Type, class StoreType>
inline typename CSLSList<Type, StoreType>::CNode* CSLSList<Type, StoreType>::NewNode(Type InitData, CNode* const cpInitNext)
{
  CNode* const cpRes = reinterpret_cast<CNode*>(NodeMemMgr.New());
  cpRes->CNode::CNode(InitData, cpInitNext);
  return cpRes;
};

template<class Type, class StoreType>
inline void CSLSList<Type, StoreType>::DelNode(CNode* const cpNode)
{
  cpNode->~CNode();
  NodeMemMgr.Delete(cpNode);
};

template<class Type, class StoreType>
void CSLSList<Type, StoreType>::FreeAll(const bool bcDelMem)
{
  while(0 != pHead)
  {
    CNode* const cpDelNode = pHead;
    pHead = pHead->pNext;
    DelNode(cpDelNode);
  };
  if(false != bcDelMem)
  {
    NodeMemMgr.FreeAll();
  };
};

//////////////////////////////////////////////////////////////////////
//CSLStack

template<class Type, class StoreType, const DWORD dwcFrameSize>
inline CSLStack<Type, StoreType, dwcFrameSize>::CSLStack()
  :dwPagePos(0),
  pCurrFrame(0),
  pFreeFrames(0)
{
};

template<class Type, class StoreType, const DWORD dwcFrameSize>
inline CSLStack<Type, StoreType, dwcFrameSize>::~CSLStack()
{
  FreeAll(true);
};

template<class Type, class StoreType, const DWORD dwcFrameSize>
inline typename CSLStack<Type, StoreType, dwcFrameSize>::CNode* CSLStack<Type, StoreType, dwcFrameSize>::GetStartOfFrame(CFrameHdr* const cpFrame)
{
  return reinterpret_cast<CNode*>(cpFrame + 1);
};

template<class Type, class StoreType, const DWORD dwcFrameSize>
inline bool CSLStack<Type, StoreType, dwcFrameSize>::IsEmpty() const
{
  return 0 == pCurrFrame;
};

template<class Type, class StoreType, const DWORD dwcFrameSize>
inline bool CSLStack<Type, StoreType, dwcFrameSize>::IsOk() const
{
  return IsEmpty();
};

template<class Type, class StoreType, const DWORD dwcFrameSize>
inline StoreType* CSLStack<Type, StoreType, dwcFrameSize>::Top() const
{
  _ASSERT(false == IsEmpty());
  CNode* const cpStartNode = GetStartOfFrame(pCurrFrame);
  return &((cpStartNode + dwPagePos)->Data);
};

template<class Type, class StoreType, const DWORD dwcFrameSize>
inline StoreType* CSLStack<Type, StoreType, dwcFrameSize>::Push(Type InitData)
{
  ////goto next pos in frame
  dwPagePos++;

  ////try alloc current frame
  if(0 == pCurrFrame || dwcFrameSize <= dwPagePos)
  {
    dwPagePos = 0;
    CFrameHdr* const cpNewFrame = NewFrame();
    cpNewFrame->pNextFrame = pCurrFrame;
    pCurrFrame = cpNewFrame;
  };

  ////insert data
  //alloc node
  CNode* const cpNewNode = NewNode(InitData, dwPagePos, pCurrFrame);
  //return 
  return &(cpNewNode->Data);
};

template<class Type, class StoreType, const DWORD dwcFrameSize>
inline void CSLStack<Type, StoreType, dwcFrameSize>::Pop()
{
  _ASSERT(false == IsEmpty());
  ////free data
  DeleteNode(dwPagePos, pCurrFrame);
  //try dec frame pos or goto next stack frame
  if(0 == dwPagePos)
  {
    ////prev frame
    //pos in frame
    dwPagePos = dwcFrameSize - 1;
    //chng frame
    CFrameHdr* const cpDelFrame = pCurrFrame;
    pCurrFrame = cpDelFrame->pNextFrame;
    DeleteFrame(cpDelFrame);
  }
  else
  {
    ////prev node in frame
    dwPagePos--;
  };
};

template<class Type, class StoreType, const DWORD dwcFrameSize>
inline void CSLStack<Type, StoreType, dwcFrameSize>::FreeAll(const bool bcFreeMem)
{
  ////remove info from frame
  //frame iteration
  while(0 != pCurrFrame)
  {
    ////nodes in frame
    while(0 != dwPagePos)
    {
      //next node
      dwPagePos--;
      //delete node
      DeleteNode(dwPagePos, pCurrFrame);
    };
    //next frame
    dwPagePos = dwcFrameSize - 1;
    pCurrFrame = pCurrFrame->pNextFrame;
  };
  ////free frames
  if(false != bcFreeMem)
  {
    while(0 != pFreeFrames)
    {
      CFrameHdr* const cpDelFrame = pFreeFrames;
      pFreeFrames = pFreeFrames->pNextFrame;
      delete reinterpret_cast<BYTE*>(cpDelFrame);
    };
  };
};

template<class Type, class StoreType, const DWORD dwcFrameSize>
inline typename CSLStack<Type, StoreType, dwcFrameSize>::CFrameHdr* CSLStack<Type, StoreType, dwcFrameSize>::NewFrame()
{
  CFrameHdr* pRes = 0;
  if(0 != pFreeFrames)
  {
    //try get frame from stack
    pRes = pFreeFrames;
    pFreeFrames = pFreeFrames->pNextFrame;
  }
  else
  {
    //alloc new frame from heap
    pRes = reinterpret_cast<CFrameHdr*>(new BYTE[CalcFrameSize()]);
  };
  return pRes;
};

template<class Type, class StoreType, const DWORD dwcFrameSize>
inline void CSLStack<Type, StoreType, dwcFrameSize>::DeleteFrame(CFrameHdr* const cpFrame)
{
  //clear memory
#ifdef _DEBUG
  ZeroMemory(cpFrame, CalcFrameSize());
#endif//_DEBUG
  //push frame to free stack
  cpFrame->pNextFrame = pFreeFrames;
  pFreeFrames = cpFrame;
};

template<class Type, class StoreType, const DWORD dwcFrameSize>
inline typename CSLStack<Type, StoreType, dwcFrameSize>::CNode* CSLStack<Type, StoreType, dwcFrameSize>::NewNode(Type InitData, const DWORD dwcInFramePos, CFrameHdr* const cpAllocFrame)
{
  CNode* const cpStartNode = GetStartOfFrame(cpAllocFrame);
  CNode* const cpRes = cpStartNode + dwcInFramePos;
  cpRes->CNode::CNode(InitData);
  return cpRes;
};

template<class Type, class StoreType, const DWORD dwcFrameSize>
inline void CSLStack<Type, StoreType, dwcFrameSize>::DeleteNode(const DWORD dwcInFramePos, CFrameHdr* const cpAllocFrame)
{
  CNode* const cpStartNode = GetStartOfFrame(cpAllocFrame);
  CNode* const cpDelNode = cpStartNode + dwcInFramePos;
  cpDelNode->~CNode();
#ifdef _DEBUG
  ZeroMemory(cpDelNode, sizeof(*cpDelNode));
#endif//_DEBUG
};

template<class Type, class StoreType, const DWORD dwcFrameSize>
inline DWORD CSLStack<Type, StoreType, dwcFrameSize>::CalcFrameSize()
{
  return 
    //size of hdr
    sizeof(CFrameHdr)
    //store memory
    + sizeof(CNode) * dwcFrameSize;
};



_SL_END

#endif//__SLCONT_H