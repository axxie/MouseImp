/**********************************************************************
thunk's (for func's call) - based on ATL thunk's (Intel only)
**********************************************************************/

#ifndef __SLTHUNK_H
#define __SLTHUNK_H

#include "SLUtil.h"

#include "SLDbg.h"

_SL_BEG



//////////////////////////////////////////////////////////////////////
//this code hardly based on ATL's Thunk's and AtdAux lib



//////////////////////////////////////////////////////////////////////
//CSLWndProcThunk

#pragma pack(push,1)

template <class T>
class CSLWndProcThunk
{

#ifdef _M_IX86
    DWORD   m_mov;      // mov dword ptr [esp+0x4], pThis (esp+0x4 is hWnd)
    DWORD   m_this;     //
    BYTE    m_jmp;      // jmp func
    DWORD   m_relproc;  // relative jmp
#endif // _M_IX86

#ifdef _M_X64
    USHORT  RcxMov;         // mov rcx, pThis
    ULONG64 RcxImm;         // 
    USHORT  RaxMov;         // mov rax, target
    ULONG64 RaxImm;         //
    USHORT  RaxJmp;         // jmp target
#endif // _M_X64
    
    typedef LRESULT (T              ::*PWndProcMethod  )(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    PWndProcMethod m_TargetWndProc;
    T*             m_storedThis;

    static LRESULT InternalWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        CSLWndProcThunk* pThis = (CSLWndProcThunk*)hwnd;
        return ((pThis->m_storedThis)->*pThis->m_TargetWndProc)(pThis->m_storedThis->hcWnd, uMsg, wParam, lParam);
    }

public:


    void InitThunk(PWndProcMethod method, T* pThis)
    {
        m_TargetWndProc = method;
        m_storedThis = pThis;

#ifdef _M_IX86
        m_mov = 0x042444C7;  //C7 44 24 0C
        m_this = (DWORD)this;
        m_jmp = 0xE9;
        m_relproc = (DWORD)&InternalWndProc - (DWORD)(&m_relproc + 1);
#endif

#ifdef _M_X64
        RcxMov = 0xb948;          // mov rcx, pThis
        RcxImm = (ULONG64)this;   // 
        RaxMov = 0xb848;          // mov rax, target
        RaxImm = (ULONG64)&InternalWndProc;   //
        RaxJmp = 0xe0ff;          // jmp rax
#endif // _M_X64

        DWORD dwOldProtect;
        VirtualProtect(this, sizeof(*this), PAGE_EXECUTE_READWRITE, &dwOldProtect);

        FlushInstructionCache(GetCurrentProcess(), this, sizeof(*this));
    }
    FARPROC GetThunk() const 
    { 
        return (FARPROC)this;
    }
    

};

_SL_END

#endif//__SLTHUNK_H