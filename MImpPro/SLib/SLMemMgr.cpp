/**********************************************************************
mem mgr's
**********************************************************************/

#define STRICT
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#pragma hdrstop

#include "SLMemMgr.h"


_SL_BEG

//////////////////////////////////////////////////////////////////////
//CSLMemChunk

bool CSLMemChunk::CheckChunk(const void* const cpcMem) const
{
  bool bRes = false;
  //mem should be in BlockList memory
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
      if(pcChunk == cpcMem)
      {
        bRes = true;
        break;
      };
    };
  };
  return bRes;
};


_SL_END
