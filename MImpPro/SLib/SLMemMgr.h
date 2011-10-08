/**********************************************************************
mem mgr's
**********************************************************************/

#ifndef __SLMEMMGR_H
#define __SLMEMMGR_H

#include "SLUtil.h"

#include "SLDbg.h"

#include <new.h>

_SL_BEG


//////////////////////////////////////////////////////////////////////
//CSLMemChunk
//
//chunk memory allocator. alloc piece of memory (dwcChunkSize size)
//and alloc dwcChunkCount piece's on one

class CSLMemChunk
{
protected:

  //SList node dummy struct
  struct CNode
  {
    CNode* pNext;
  };

  //list of free chunk's
  CNode* pFreeChunksList;
  //list of alloced block's
  CNode* pBlockList;

  //const's
  const DWORD dwcChunkSize;
  const DWORD dwcChunkCount;

public:

public:

  inline CSLMemChunk(const DWORD dwcInitChunkSize, const DWORD dwcInitChunkCount = 20);
  inline ~CSLMemChunk();

  //free all alloced memory
  inline void FreeAll();
  //free passed chunk
  inline void FreeChunk(void* const cpNode);
  //some as FreeChunk
  inline void Delete(void* const cpNode);
  //get new chunk
  inline void* NewChunk();
  //some as NewChunk
  inline void* New();
  //check if mem in this alloc (for DBG - !!!SLOW!!!)
  bool CheckChunk(const void* const cpcMem) const;

protected:
  //calc alloc chunk size
  inline DWORD MaxChunkSize() const;
};


//////////////////////////////////////////////////////////////////////
//CSLTmplChunk - tamplate version of CSLMemChunk

template<class Type, const DWORD dwcChunkCount = 20>
class CSLTmplChunk
{
protected:

  //SList node dummy struct
  struct CNode
  {
    CNode* pNext;
  };

  //list of free chunk's
  CNode* pFreeChunksList;
  //list of alloced block's
  CNode* pBlockList;

public:

public:

  inline CSLTmplChunk();
  inline ~CSLTmplChunk();

  //free all alloced memory (destructor's not called)
  inline void FreeAll();
  //free passed chunk
  inline void FreeChunk(Type* const cpNode);
  //some as FreeChunk
  inline void Delete(Type* const cpNode);
  //get new chunk
  inline Type* NewChunk();
  //some as NewChunk
  inline Type* New();
  //check if mem in this alloc (for DBG - !!!SLOW!!!)
  bool CheckChunk(const Type* const cpcMem) const;

protected:
  //calc alloc chunk size
  inline DWORD MaxChunkSize() const;
};


/**********************************************************************
implementation
**********************************************************************/

//////////////////////////////////////////////////////////////////////
//CSLMemChunk

inline CSLMemChunk::CSLMemChunk(const DWORD dwcInitChunkSize, const DWORD dwcInitChunkCount)
  :pFreeChunksList(0),
  pBlockList(0),
  dwcChunkSize(dwcInitChunkSize),
  dwcChunkCount(dwcInitChunkCount)
{
};

inline CSLMemChunk::~CSLMemChunk()
{
  FreeAll();
};

inline void CSLMemChunk::FreeAll()
{
  while(0 != pBlockList)
  {
    CNode* const cpCurr = pBlockList;
    pBlockList = pBlockList->pNext;
    delete reinterpret_cast<BYTE*>(cpCurr);
  };
  pFreeChunksList = 0;
};

inline void CSLMemChunk::Delete(void* const cpNode)
{
  FreeChunk(cpNode);
};

inline void CSLMemChunk::FreeChunk(void* const cpNode)
{
  CNode* const cpDel = reinterpret_cast<CNode*>(cpNode);
#ifdef _DEBUG
  ZeroMemory(cpDel, dwcChunkSize);
#endif//_DEBUG
  cpDel->pNext = pFreeChunksList;
  pFreeChunksList = cpDel;
};

inline void* CSLMemChunk::New()
{
  return NewChunk();
};

inline void* CSLMemChunk::NewChunk()
{
  //alloc new block (if need)
  if(0 == pFreeChunksList)
  {
    //alloc chunk size
    //block mem size - chunk count + extra mem for block head
    const DWORD dwcBlockSize = dwcChunkCount * MaxChunkSize() + sizeof(CNode);
    //alloc mem + mem to block list
    CNode* const cpBlockBase = reinterpret_cast<CNode*>(new BYTE[dwcBlockSize]);
    cpBlockBase->pNext = pBlockList;
    pBlockList = cpBlockBase;
    //break block to chunk's
    BYTE* pChunkPtr = reinterpret_cast<BYTE*>(cpBlockBase + 1);
    for(DWORD dwCount = 0; dwcChunkCount > dwCount; dwCount++)
    {
      //put to FreeChunkList
      CNode* const cpThisChunk = reinterpret_cast<CNode*>(pChunkPtr);
      cpThisChunk->pNext = pFreeChunksList;
      pFreeChunksList = cpThisChunk;
      //move to next
      pChunkPtr += MaxChunkSize();
    };
  };

  //get from FreeChunkList
  CNode* pRes = pFreeChunksList;
  pFreeChunksList = pRes->pNext;

  //return
  return reinterpret_cast<void*>(pRes);
};

inline DWORD CSLMemChunk::MaxChunkSize() const
{
  const DWORD dwcWantSize = max(sizeof(CNode), dwcChunkSize);
  const DWORD dwcAlignMask = sizeof(PVOID) - 1;
  return (dwcWantSize + dwcAlignMask) & (~dwcAlignMask);
};

//////////////////////////////////////////////////////////////////////
//CSLTmplChunk

template<class Type, const DWORD dwcChunkCount>
inline CSLTmplChunk<Type, dwcChunkCount>::CSLTmplChunk()
  :pFreeChunksList(0),
  pBlockList(0)
{
};

template<class Type, const DWORD dwcChunkCount>
inline CSLTmplChunk<Type, dwcChunkCount>::~CSLTmplChunk()
{
  FreeAll();
};

template<class Type, const DWORD dwcChunkCount>
inline DWORD CSLTmplChunk<Type, dwcChunkCount>::MaxChunkSize() const
{
  const DWORD dwcWantSize = max(sizeof(CNode), sizeof(Type));
  const DWORD dwcAlignMask = sizeof(PVOID) - 1;
  return (dwcWantSize + dwcAlignMask) & (~dwcAlignMask);
};

template<class Type, const DWORD dwcChunkCount>
inline void CSLTmplChunk<Type, dwcChunkCount>::FreeAll()
{
  while(0 != pBlockList)
  {
    CNode* const cpCurr = pBlockList;
    pBlockList = pBlockList->pNext;
    delete reinterpret_cast<BYTE*>(cpCurr);
  };
  pFreeChunksList = 0;
};

template<class Type, const DWORD dwcChunkCount>
inline void CSLTmplChunk<Type, dwcChunkCount>::Delete(Type* const cpNode)
{
  FreeChunk(cpNode);
};

template<class Type, const DWORD dwcChunkCount>
inline void CSLTmplChunk<Type, dwcChunkCount>::FreeChunk(Type* const cpNode)
{
  //destruct 
  cpNode->~Type();
  CNode* const cpDel = reinterpret_cast<CNode*>(cpNode);
#ifdef _DEBUG
  ZeroMemory(cpDel, MaxChunkSize());
#endif//_DEBUG
  cpDel->pNext = pFreeChunksList;
  pFreeChunksList = cpDel;
};

template<class Type, const DWORD dwcChunkCount>
inline Type* CSLTmplChunk<Type, dwcChunkCount>::New()
{
  return NewChunk();
};

template<class Type, const DWORD dwcChunkCount>
inline Type* CSLTmplChunk<Type, dwcChunkCount>::NewChunk()
{
  //alloc new block (if need)
  if(0 == pFreeChunksList)
  {
    //alloc chunk size
    //block mem size - chunk count + extra mem for block head
    const DWORD dwcBlockSize = dwcChunkCount * MaxChunkSize() + sizeof(CNode);
    //alloc mem + mem to block list
    CNode* const cpBlockBase = reinterpret_cast<CNode*>(new BYTE[dwcBlockSize]);
    cpBlockBase->pNext = pBlockList;
    pBlockList = cpBlockBase;
    //break block to chunk's
    BYTE* pChunkPtr = reinterpret_cast<BYTE*>(cpBlockBase + 1);
    for(DWORD dwCount = 0; dwcChunkCount > dwCount; dwCount++)
    {
      //put to FreeChunkList
      CNode* const cpThisChunk = reinterpret_cast<CNode*>(pChunkPtr);
      cpThisChunk->pNext = pFreeChunksList;
      pFreeChunksList = cpThisChunk;
      //move to next
      pChunkPtr += MaxChunkSize();
    };
  };

  //get from FreeChunkList
  CNode* pRes = pFreeChunksList;
  pFreeChunksList = pRes->pNext;

  //construct and return
  Type* const cpRes = reinterpret_cast<Type*>(pRes);
  ::new(reinterpret_cast<void*>(cpRes)) Type;
  return cpRes;
};


template<class Type, const DWORD dwcChunkCount>
bool CSLTmplChunk<Type, dwcChunkCount>::CheckChunk(const Type* const cpcCheckMem) const
{
  bool bRes = false;
  //mem should be in BlockList memory
  const BYTE* const cpcMem = reinterpret_cast<const BYTE*>(cpcCheckMem);
  for(const CNode* pcBlock = pBlockList; 0 != pBlockList; pcBlock = pcBlock->pNext)
  {
    const BYTE* const cpcBlockBeg = reinterpret_cast<const BYTE*>(pcBlock + 1);
    const BYTE* const cpcBlockEnd = cpcBlockBeg + MaxChunkSize() * dwcChunkCount;
    //mem in this block
    if(cpcBlockBeg <= cpcMem && cpcBlockEnd > cpcMem)
    {
      bRes = true;
      break;
    };
  };

  //if find - not in FreeList
  if(false != bRes)
  {
    for(const CNode* pcChunk = pFreeChunksList; 0 != pcChunk; pcChunk = pcChunk->pNext)
    {
      //mem in free chunk - invalid
      if(reinterpret_cast<const BYTE*>(pcChunk) == cpcMem)
      {
        bRes = true;
        break;
      };
    };
  };
  return bRes;
};

_SL_END

#endif//__SLMEMMGR_H
