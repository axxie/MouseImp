/**********************************************************************
automatic (smart) pointer
**********************************************************************/

#ifndef __SLAUTOPTR_H
#define __SLAUTOPTR_H

#pragma once

#include "SLUtil.h"

#include "SLDbg.h"

_SL_BEG


//////////////////////////////////////////////////////////////////////
//CSLAutoPtr

//implement pointer (for not user defined (UDF) types (Like BYTE and etc.))
//near to STL::auto_ptr, but some enhanced 
//store dynamic allocated memory (new) on assigning/construction 
//and automaticaly deallocate (delete) on destruction
//
//Example:
//
//void main()
//{
//  const DWORD dwcSize = 100;
//  //create pointer and attach memory to it's 
//  //(on "main" function exit memory will be freeed automaticaly)
//  sl::CSLAutoPtr<BYTE> pMem(new BYTE[dwcSize]);
//  //some usage
//  for(DWORD dwCount = 0; dwcSize > dwCount; dwCount++)
//  {
//    pMem[dwCount] = static_cast<BYTE>(dwCount);
//  };
//};


template<class Type>
class CSLAutoPtr
{
protected:

  //address attached of memory
  Type* pPtr;

public:

public:

  //construction
  inline CSLAutoPtr();
  inline CSLAutoPtr(Type* const cpcInitMem);
  inline CSLAutoPtr(CSLAutoPtr<Type>& rInitMem);
  //destrucion (if memory attached - free it's)
  inline ~CSLAutoPtr();

  //get mem addr, attached to this pointer)
  inline Type* GetPtr() const;
  inline Type* Get() const;
  inline operator Type*() const;
  inline Type& operator*() const;

  //get addr of internal ptr (for pass as addr to ptr)
  inline Type** operator&();
  inline Type** GetAddr();

  //as pointer not null
  inline bool IsOk() const;

  //assigning memory to pointer (old memory freed (if present))
  inline void SetPtr(Type* const cpInitMem);
  inline void Set(Type* const cpInitMem);
  inline CSLAutoPtr& operator=(Type* const cpInitMem);
  inline CSLAutoPtr& operator=(CSLAutoPtr<Type>& rInitMem);

  //free (delete) memory, attached to pointer
  inline void Free();
  //detach mem from this pointer (memory _not_ deleted)
  inline Type* Detach();
  //some sa  Detach
  inline Type* Drop();

protected:
};


//////////////////////////////////////////////////////////////////////
//CSLAutoClassPtr
//
//like CSLAutoPtr, but implement operator-> - for UserDefined types (class's, struct's)
//
//Example:
//
//struct CSome
//{
//  unsigned uiData;
//};
//
//sl::CSLAutoClassPtr<CSome> pPtr(new CSome[100]);
//

template<class Type>
class CSLAutoClassPtr : public CSLAutoPtr<Type>
{
protected:

public:

public:

  //construct and assign memory to pointer
  inline CSLAutoClassPtr();
  inline CSLAutoClassPtr(Type* const cpcInitMem);
  inline CSLAutoClassPtr(CSLAutoPtr<Type>& rInitMem);

  //UserDefined type access
  inline Type* operator->() const;

  //assign (overloading from parent)
  inline CSLAutoClassPtr& operator=(Type* const cpInitMem);
  inline CSLAutoClassPtr& operator=(CSLAutoPtr<Type>& rInitMem);

protected:
};

//////////////////////////////////////////////////////////////////////
//CSLAutoArray
//
//smart pointer for dynamic allocated array's 
//(like CSLAutoPtr, but on Free used delete[] pMem)
//
//Example:
//void main()
//{
//  sl::CSLAutoArray pMass(new CSome[100]);
//};
//

template<class Type>
class CSLAutoArray
{
protected:

  //memory address
  Type* pPtr;
  
public:
  
public:
  
  //construction
  inline CSLAutoArray();
  inline CSLAutoArray(Type* const cpcInitMem);
  inline CSLAutoArray(CSLAutoArray<Type>& rInitMem);
  //destrucion (if memory attached - free it's)
  inline ~CSLAutoArray();

  //get mem addr, attached to this pointer)
  inline Type* GetPtr() const;
  inline Type* Get() const;
  inline Type& operator[](const DWORD dwcInd) const;

  ////is memory present
  inline bool IsOk() const;

  //assigning memory to pointer (old memory freed (if present))
  inline void SetPtr(Type* const cpInitMem);
  inline void Set(Type* const cpInitMem);
  inline CSLAutoArray& operator=(Type* const cpInitMem);
  inline CSLAutoArray& operator=(CSLAutoArray<Type>& rInitMem);

  //free (delete[]) memory, attached to pointer
  inline void Free();
  //detach mem from this pointer (memory _not_ deleted)
  inline Type* Detach();
  //some as Detach
  inline Type* Drop();

protected:
};

//////////////////////////////////////////////////////////////////////
//CSLAutoGlbPtr

template<class Type>
class CSLAutoGlbPtr
{
protected:

  //global memory handle
  HGLOBAL hGlb;
  //locked memory
  Type* const cpMem;
  
public:
  
public:

  inline CSLAutoGlbPtr(HGLOBAL hInitGlb);
  inline ~CSLAutoGlbPtr();

  //get associated memory
  inline Type* GetPtr() const;
  inline Type* Get() const;
  inline operator Type*() const;
  inline Type& operator*() const;
  inline Type* operator->() const;

  //as pointer not null
  inline bool IsOk() const;
  
protected:

};


/**********************************************************************
implementation
**********************************************************************/

//////////////////////////////////////////////////////////////////////
//CSLAutoPtr


template<class Type>
inline CSLAutoPtr<Type>::CSLAutoPtr()
  :pPtr(0)
{
};

template<class Type>
inline CSLAutoPtr<Type>::CSLAutoPtr(Type* const cpcInitMem)
  :pPtr(cpcInitMem)
{
};

template<class Type>
inline CSLAutoPtr<Type>::CSLAutoPtr(CSLAutoPtr<Type>& rInitMem)
  :pPtr(rInitMem.Detach())
{
};

template<class Type>
inline CSLAutoPtr<Type>::~CSLAutoPtr()
{
  Free();
};

template<class Type>
inline bool CSLAutoPtr<Type>::IsOk() const
{
  return 0 != pPtr;
};

template<class Type>
inline Type* CSLAutoPtr<Type>::GetPtr() const
{
  //is Ptr null???
  _ASSERT(0 != pPtr);
  return pPtr;
};

template<class Type>
inline Type* CSLAutoPtr<Type>::Get() const
{
  return GetPtr();
};

template<class Type>
inline CSLAutoPtr<Type>::operator Type*() const
{
  return GetPtr();
};

template<class Type>
inline Type& CSLAutoPtr<Type>::operator*() const
{
  return *GetPtr();
};

template<class Type>
inline Type** CSLAutoPtr<Type>::GetAddr()
{
  //delete mem
  Free();
  //return addr to ptr
  return &pPtr;
};

template<class Type>
inline Type** CSLAutoPtr<Type>::operator&()
{
  return GetAddr();
};

template<class Type>
inline void CSLAutoPtr<Type>::SetPtr(Type* const cpInitMem)
{
  Free();
  pPtr = cpInitMem;
};

template<class Type>
inline void CSLAutoPtr<Type>::Set(Type* const cpInitMem)
{
  SetPtr(cpInitMem);
};

template<class Type>
inline CSLAutoPtr<Type>& CSLAutoPtr<Type>::operator=(Type* const cpInitMem)
{
  SetPtr(cpInitMem);
  return *this;
};

template<class Type>
inline CSLAutoPtr<Type>& CSLAutoPtr<Type>::operator=(CSLAutoPtr<Type>& rInitMem)
{
  SetPtr(rInitMem.Detach());
  return *this;
};

template<class Type>
inline void CSLAutoPtr<Type>::Free()
{
  if(0 != pPtr)
  {
    delete pPtr;
    pPtr = 0;
  };
};

template<class Type>
inline Type* CSLAutoPtr<Type>::Detach()
{
  Type* const cpOld = pPtr;
  pPtr = 0;
  return cpOld;
};

template<class Type>
inline Type* CSLAutoPtr<Type>::Drop()
{
  return Detach();
};


//////////////////////////////////////////////////////////////////////
//CSLAutoClassPtr

template<class Type>
inline CSLAutoClassPtr<Type>::CSLAutoClassPtr()
  :CSLAutoPtr<Type>()
{
};

template<class Type>
inline CSLAutoClassPtr<Type>::CSLAutoClassPtr(Type* const cpcInitMem)
  :CSLAutoPtr<Type>(cpcInitMem)
{
};

template<class Type>
inline CSLAutoClassPtr<Type>::CSLAutoClassPtr(CSLAutoPtr<Type>& rInitMem)
  :CSLAutoPtr(rInitMem)
{
};

template<class Type>
inline Type* CSLAutoClassPtr<Type>::operator->() const
{
  return GetPtr();
};

template<class Type>
inline CSLAutoClassPtr<Type>& CSLAutoClassPtr<Type>::operator=(Type* const cpInitMem)
{
  SetPtr(cpInitMem);
  return *this;
};

template<class Type>
inline CSLAutoClassPtr<Type>& CSLAutoClassPtr<Type>::operator=(CSLAutoPtr<Type>& rInitMem)
{
  SetPtr(rInitMem.Detach());
  return *this;
};


//////////////////////////////////////////////////////////////////////
//CSLAutoArray

template<class Type>
inline CSLAutoArray<Type>::CSLAutoArray()
  :pPtr(0)
{
};

template<class Type>
inline CSLAutoArray<Type>::CSLAutoArray(Type* const cpcInitMem)
  :pPtr(cpcInitMem)
{
};

template<class Type>
inline CSLAutoArray<Type>::CSLAutoArray(CSLAutoArray<Type>& rInitMem)
  :pPtr(rInitMem.Deatch())
{
};

template<class Type>
inline CSLAutoArray<Type>::~CSLAutoArray()
{
  Free();
};

template<class Type>
inline Type* CSLAutoArray<Type>::GetPtr() const
{
  return pPtr;
};

template<class Type>
inline bool CSLAutoArray<Type>::IsOk() const
{
  return 0 != pPtr;
};

template<class Type>
inline Type* CSLAutoArray<Type>::Get() const
{
  return GetPtr();
};

template<class Type>
inline Type& CSLAutoArray<Type>::operator[](const DWORD dwcInd) const
{
  return *(GetPtr() + dwcInd);
};

template<class Type>
inline void CSLAutoArray<Type>::SetPtr(Type* const cpInitMem)
{
  Free();
  pPtr = cpInitMem;
};

template<class Type>
inline void CSLAutoArray<Type>::Set(Type* const cpInitMem)
{
  SetPtr(cpInitMem);
};

template<class Type>
inline CSLAutoArray<Type>& CSLAutoArray<Type>::operator=(Type* const cpInitMem)
{
  SetPtr(cpInitMem);
  return *this;
};

template<class Type>
inline CSLAutoArray<Type>& CSLAutoArray<Type>::operator=(CSLAutoArray<Type>& rInitMem)
{
  SetPtr(rInitMem.Detach());
  return *this;
};

template<class Type>
inline void CSLAutoArray<Type>::Free()
{
  if(0 != pPtr)
  {
    delete[] pPtr;
    pPtr = 0;
  };
};

template<class Type>
inline Type* CSLAutoArray<Type>::Detach()
{
  Type* const cpRes = pPtr;
  pPtr = 0;
  return cpRes;
};

template<class Type>
inline Type* CSLAutoArray<Type>::Drop()
{
  return Detach();
};

//////////////////////////////////////////////////////////////////////
//CSLAutoGlbPtr

template<class Type>
inline CSLAutoGlbPtr<Type>::CSLAutoGlbPtr(HGLOBAL hInitGlb)
  :hGlb(hInitGlb),
  cpMem(reinterpret_cast<Type*>(::GlobalLock(hInitGlb)))
{
  //check size of GlobalMemory 
  _ASSERT(::GlobalSize(hGlb) >= sizeof(Type));
};

template<class Type>
inline CSLAutoGlbPtr<Type>::~CSLAutoGlbPtr()
{
  ::GlobalUnlock(hGlb);
};

template<class Type>
inline Type* CSLAutoGlbPtr<Type>::GetPtr() const
{
  //is mem ok???
  _ASSERT(0 != cpMem);
  return cpMem;
};

template<class Type>
inline Type* CSLAutoGlbPtr<Type>::Get() const
{
  return GetPtr();
};

template<class Type>
inline CSLAutoGlbPtr<Type>::operator Type*() const
{
  return GetPtr();
};

template<class Type>
inline Type& CSLAutoGlbPtr<Type>::operator*() const
{
  return *GetPtr();
};

template<class Type>
inline Type* CSLAutoGlbPtr<Type>::operator->() const
{
  return Get();
};

template<class Type>
inline bool CSLAutoGlbPtr<Type>::IsOk() const
{
  return 0 != cpMem;
};



_SL_END

#endif//__SLAUTOPTR_H