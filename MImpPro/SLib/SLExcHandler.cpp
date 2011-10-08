/**********************************************************************
Exception info hanlder
**********************************************************************/
#define STRICT
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#pragma hdrstop


#include "SLExcHandler.h"


#include <imagehlp.h>



_SL_BEG

//////////////////////////////////////////////////////////////////////
//global's

//name for report file
static TCHAR cpRepFileName[MAX_PATH];
//prev filter
static PTOP_LEVEL_EXCEPTION_FILTER pPrevFilter = 0;
//"installed" flag
static BOOL bIsInstalled = FALSE;

//print to passed file like "wsprintf"
static int SLPrintf(HANDLE hFile, LPCTSTR const cpcMask, ...)
{
  //buff
  TCHAR cpBuff[1024];
  //print
  va_list Arg;
  va_start(Arg, cpcMask);
  const int icRes = wvsprintf(cpBuff, cpcMask, Arg);
  va_end(Arg);
  //to file
  DWORD dwWLen = 0;
  ::WriteFile(hFile, cpBuff, icRes, &dwWLen, 0);

  return icRes;
};

//get logical address
static BOOL SLGetLogicalAddress(PVOID const addr, PTSTR szModule, DWORD len, DWORD& section, DWORD& offset )
{
  MEMORY_BASIC_INFORMATION mbi;
  if (!VirtualQuery( addr, &mbi, sizeof(mbi) ) )        return FALSE;
  DWORD hMod = (DWORD)mbi.AllocationBase;
  if ( !GetModuleFileName( (HMODULE)hMod, szModule, len ) )
    return FALSE;
  // Point to the DOS header in memory
  PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER)hMod;
  // From the DOS header, find the NT (PE) header
  PIMAGE_NT_HEADERS pNtHdr = (PIMAGE_NT_HEADERS)(hMod + pDosHdr->e_lfanew);
  PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION( pNtHdr );
  DWORD rva = (DWORD)addr - hMod;
  // RVA is offset from module load address
  // Iterate through the section table, looking for the one that encompasses
  // the linear address.
  for (   unsigned i = 0;
  i < pNtHdr->FileHeader.NumberOfSections;
  i++, pSection++ )
  {
    DWORD sectionStart = pSection->VirtualAddress;
    DWORD sectionEnd = sectionStart + max(pSection->SizeOfRawData, pSection->Misc.VirtualSize);
    // Is the address in this section???
    if ( (rva >= sectionStart) && (rva <= sectionEnd) )
    {
      // Yes, address is in the section.  Calculate section and offset,
      // and store in the "section" & "offset" params, which were
      // passed by reference.
      section = i+1;
      offset = rva - sectionStart;
      return TRUE;
    }
  }
  // Should never get here!
  return FALSE;
}


//////////////////////////////////////////////////////////////////////
//stack walk section

// Make typedefs for some IMAGEHLP.DLL functions so that we can use them
// with GetProcAddress
typedef BOOL (__stdcall * SYMINITIALIZEPROC)( HANDLE, LPSTR, BOOL );
static SYMINITIALIZEPROC _SymInitialize = 0;

typedef BOOL (__stdcall *SYMCLEANUPPROC)( HANDLE );
static SYMCLEANUPPROC _SymCleanup = 0;

typedef BOOL (__stdcall * STACKWALKPROC)( DWORD, HANDLE, HANDLE, LPSTACKFRAME, LPVOID, PREAD_PROCESS_MEMORY_ROUTINE,PFUNCTION_TABLE_ACCESS_ROUTINE, PGET_MODULE_BASE_ROUTINE, PTRANSLATE_ADDRESS_ROUTINE );
static STACKWALKPROC _StackWalk = 0;

typedef LPVOID (__stdcall *SYMFUNCTIONTABLEACCESSPROC)( HANDLE, DWORD );
static SYMFUNCTIONTABLEACCESSPROC _SymFunctionTableAccess = 0;

typedef DWORD (__stdcall *SYMGETMODULEBASEPROC)( HANDLE, DWORD );
static SYMGETMODULEBASEPROC _SymGetModuleBase = 0;

typedef BOOL (__stdcall *SYMGETSYMFROMADDRPROC)( HANDLE, DWORD, PDWORD, PIMAGEHLP_SYMBOL );
static SYMGETSYMFROMADDRPROC _SymGetSymFromAddr = 0;

typedef BOOL (__stdcall *SYMGETMODULEINFOPROC)(HANDLE, DWORD, PIMAGEHLP_MODULE);
static SYMGETMODULEINFOPROC _SymGetModuleInfo = 0;

typedef BOOL (__stdcall *SYMLOADMODULEPROC)(HANDLE, HANDLE, PSTR, PSTR, DWORD, DWORD);
static SYMLOADMODULEPROC _SymLoadModule = 0;

static HMODULE hImgHlpModule = 0;

//try init image hlp dll
BOOL ImgHelpInit()
{
  //only if not already
  if(0 == hImgHlpModule)
  {
    hImgHlpModule = LoadLibrary( _T("IMAGEHLP.DLL") );
    if ( !hImgHlpModule )
      return FALSE;
    _SymInitialize = (SYMINITIALIZEPROC)GetProcAddress( hImgHlpModule, "SymInitialize" );
    if ( !_SymInitialize )
      return FALSE;
    _SymCleanup = (SYMCLEANUPPROC)GetProcAddress( hImgHlpModule, "SymCleanup" );
    if ( !_SymCleanup )
      return FALSE;
    _StackWalk = (STACKWALKPROC)GetProcAddress( hImgHlpModule, "StackWalk" );
    if ( !_StackWalk )
      return FALSE;
    _SymFunctionTableAccess = (SYMFUNCTIONTABLEACCESSPROC) GetProcAddress( hImgHlpModule, "SymFunctionTableAccess" );
    if ( !_SymFunctionTableAccess )
      return FALSE;
    _SymGetModuleBase=(SYMGETMODULEBASEPROC)GetProcAddress( hImgHlpModule, "SymGetModuleBase");
    if ( !_SymGetModuleBase )
      return FALSE;
    _SymGetSymFromAddr=(SYMGETSYMFROMADDRPROC)GetProcAddress( hImgHlpModule, "SymGetSymFromAddr" );
    if ( !_SymGetSymFromAddr )
      return FALSE;
    _SymGetModuleInfo = reinterpret_cast<SYMGETMODULEINFOPROC>(::GetProcAddress(hImgHlpModule, "SymGetModuleInfo"));
    if(0 == _SymGetModuleInfo)
    {
      return FALSE;
    };
    _SymLoadModule = reinterpret_cast<SYMLOADMODULEPROC>(::GetProcAddress(hImgHlpModule, "SymLoadModule"));
    if(0 == _SymLoadModule)
    {
      return FALSE;
    };

    //init image help
    if ( !_SymInitialize( GetCurrentProcess(), 0, TRUE ) )
      return FALSE;
  };
  return TRUE;
};

//finit image help func's
void ImgHelpFinit()
{
  _SymCleanup(::GetCurrentProcess());
  hImgHlpModule = 0;
};


//do stack walikng by image help
static void ImgHelpStackWalk(HANDLE hFile, PCONTEXT pContext)
{
  SLPrintf(hFile, _T("\nCall stack:\n") );
  SLPrintf(hFile, _T("Address\tFrame\n") );
  // Could use SymSetOptions here to add the SYMOPT_DEFERRED_LOADS flag
  STACKFRAME sf;
  memset( &sf, 0, sizeof(sf) );
  // Initialize the STACKFRAME structure for the first call.  This is only
  // necessary for Intel CPUs, and isn't mentioned in the documentation.
  sf.AddrPC.Offset       = pContext->Eip;
  sf.AddrPC.Mode         = AddrModeFlat;
  sf.AddrStack.Offset    = pContext->Esp;
  sf.AddrStack.Mode      = AddrModeFlat;
  sf.AddrFrame.Offset    = pContext->Ebp;
  sf.AddrFrame.Mode      = AddrModeFlat;
  HANDLE hProcess = ::GetCurrentProcess();
  HANDLE hThrd = ::GetCurrentThread();
  while ( 1 )
  {
    if ( ! _StackWalk(  IMAGE_FILE_MACHINE_I386,
      hProcess,
      hThrd,
      &sf,
      pContext,                            
      0,
      _SymFunctionTableAccess,
      _SymGetModuleBase,
      0 ) )
      break;
    if ( 0 == sf.AddrFrame.Offset ) // Basic sanity check to make sure
      break;                      // the frame is OK.  Bail if not.
    SLPrintf(hFile, _T("%08X\t%08X  "), sf.AddrPC.Offset, sf.AddrFrame.Offset );
    // IMAGEHLP is wacky, and requires you to pass in a pointer to an
    // IMAGEHLP_SYMBOL structure.  The problem is that this structure is
    // variable length.  That is, you determine how big the structure is
    // at runtime.  This means that you can't use sizeof(struct).
    // So...make a buffer that's big enough, and make a pointer
    // to the buffer.  We also need to initialize not one, but TWO
    // members of the structure before it can be used.
    BYTE symbolBuffer[ sizeof(IMAGEHLP_SYMBOL) + 512 ];
    PIMAGEHLP_SYMBOL pSymbol = (PIMAGEHLP_SYMBOL)symbolBuffer;
    pSymbol->SizeOfStruct = sizeof(symbolBuffer);
    pSymbol->MaxNameLength = 512;
    DWORD symDisplacement = 0;  // Displacement of the input address,
    // relative to the start of the symbol
    if ( _SymGetSymFromAddr(hProcess, sf.AddrPC.Offset,
      &symDisplacement, pSymbol) )
    {
      SLPrintf(hFile, _T("%hs+%X"), pSymbol->Name, symDisplacement );
    }
    else    // No symbol found.  Print out the logical address instead.
    {
      TCHAR szModule[MAX_PATH] = _T("");
      DWORD section = 0, offset = 0;
      SLGetLogicalAddress(  (PVOID)sf.AddrPC.Offset, szModule, sizeof(szModule), section, offset );
      SLPrintf(hFile, _T("%04X:%08X %s"), section, offset, szModule );
    };

    //try get module info, what contain looked address
    const DWORD dwcModuleBase = _SymGetModuleBase(hProcess, sf.AddrPC.Offset);
    if(0 != dwcModuleBase)
    {
      IMAGEHLP_MODULE MInfo;
      ZeroMemory(&MInfo, sizeof(MInfo));
      MInfo.SizeOfStruct = sizeof(MInfo);
      if(FALSE != _SymGetModuleInfo(hProcess, dwcModuleBase, &MInfo))
      {
        SLPrintf(hFile, _T(" \"%s\""), MInfo.LoadedImageName);
      };
    };
    SLPrintf(hFile, _T("\n"));
  }
};

//////////////////////////////////////////////////////////////////////
//EXCEPTION_STRING filter section

//do intel stackwalking
static void IntelStackWalk(HANDLE hFile, PCONTEXT pContext)
{
  SLPrintf(hFile, _T("\nCall stack:\n"));
  SLPrintf(hFile, _T("Address\tFrame\tLogical addr\tModule\n"));
  DWORD pc = pContext->Eip;
  PDWORD pFrame, pPrevFrame;
  pFrame = (PDWORD)pContext->Ebp;
  do
  {
    TCHAR szModule[MAX_PATH] = _T("");
    DWORD section = 0;
    DWORD offset = 0;
    SLGetLogicalAddress((PVOID)pc, szModule,sizeof(szModule),section,offset );
    SLPrintf(hFile, _T("%08X\t%08X\t%04X:%08X\t\"%s\"\n"), pc, pFrame, section, offset, szModule );
      pc = pFrame[1];
    pPrevFrame = pFrame;
    pFrame = (PDWORD)pFrame[0]; // proceed to next higher frame on stack
    if ( (DWORD)pFrame & 3 )    // Frame pointer must be aligned on a
      break;                  // DWORD boundary.  Bail if not so.
    if ( pFrame <= pPrevFrame )
      break;
    // Can two DWORDs be read from the supposed frame address?
    if ( IsBadWritePtr(pFrame, sizeof(PVOID)*2) )
      break;
  }
  while ( 1 );
};

//get name of EXCEPTION_STRING by passing code
static LPCTSTR SLGetExcpName(const DWORD dwcExcCode)
{
  LPCTSTR pcRes = _T("Unknown EXCEPTION_STRING");
#define EXCEPTION_STRING(x) case EXCEPTION_##x: pcRes = _T(#x); break;
  switch(dwcExcCode)
  {
  EXCEPTION_STRING(ACCESS_VIOLATION)
  EXCEPTION_STRING(DATATYPE_MISALIGNMENT)
  EXCEPTION_STRING(BREAKPOINT)
  EXCEPTION_STRING(SINGLE_STEP)
  EXCEPTION_STRING(ARRAY_BOUNDS_EXCEEDED)
  EXCEPTION_STRING(FLT_DENORMAL_OPERAND)
  EXCEPTION_STRING(FLT_DIVIDE_BY_ZERO)
  EXCEPTION_STRING(FLT_INEXACT_RESULT)
  EXCEPTION_STRING(FLT_INVALID_OPERATION)
  EXCEPTION_STRING(FLT_OVERFLOW)
  EXCEPTION_STRING(FLT_STACK_CHECK)
  EXCEPTION_STRING(FLT_UNDERFLOW)
  EXCEPTION_STRING(INT_DIVIDE_BY_ZERO)
  EXCEPTION_STRING(INT_OVERFLOW)
  EXCEPTION_STRING(PRIV_INSTRUCTION)
  EXCEPTION_STRING(IN_PAGE_ERROR)
  EXCEPTION_STRING(ILLEGAL_INSTRUCTION)
  EXCEPTION_STRING(NONCONTINUABLE_EXCEPTION)
  EXCEPTION_STRING(STACK_OVERFLOW)
  EXCEPTION_STRING(INVALID_DISPOSITION)
  EXCEPTION_STRING(GUARD_PAGE)
  EXCEPTION_STRING(INVALID_HANDLE)

  default:
    break;
  };
#undef EXCEPTION_STRING
  return pcRes;
};


//report generation function
static void SLGenReport(HANDLE hFile, _EXCEPTION_POINTERS *pExceptionInfo)
{
  //easy
  PEXCEPTION_RECORD pExcRec = pExceptionInfo->ExceptionRecord;

  //baner
  SLPrintf(hFile, _T("==============================================\n"));
  //time
  SYSTEMTIME SysTime;
  ::GetLocalTime(&SysTime);
  SLPrintf
    (
    hFile,
    _T("Time : %d/%d/%d, %d-%d-%d:%d\n"),
    static_cast<DWORD>(SysTime.wYear),
    static_cast<DWORD>(SysTime.wMonth),
    static_cast<DWORD>(SysTime.wDay),
    static_cast<DWORD>(SysTime.wHour),
    static_cast<DWORD>(SysTime.wMinute),
    static_cast<DWORD>(SysTime.wSecond),
    static_cast<DWORD>(SysTime.wMilliseconds)
    );
  //code/name
  SLPrintf(hFile, _T("Exception code : %08X, \"%s\"\n"), pExcRec->ExceptionCode, SLGetExcpName(pExcRec->ExceptionCode));

  //fault pos/info
  TCHAR cpFaultModule[MAX_PATH];
  *cpFaultModule = '\0';
  DWORD dwSection = 0;
  DWORD dwOffset = 0;
  SLGetLogicalAddress(pExcRec->ExceptionAddress, cpFaultModule, COUNTOF(cpFaultModule), dwSection, dwOffset);
  //out
  SLPrintf(hFile, _T("Fault address : %08X %02X:%08X \"%s\"\n"), pExcRec->ExceptionAddress, dwSection, dwOffset, cpFaultModule);
  ////host module
  *cpFaultModule = '\0';
  ::GetModuleFileName(0, cpFaultModule, COUNTOF(cpFaultModule));
  SLPrintf(hFile, _T("HostModule : \"%s\"\n"), cpFaultModule);

  // Show the registers
  // Intel Only!
#ifdef _M_IX86
  PCONTEXT pCtx = pExceptionInfo->ContextRecord;
  SLPrintf(hFile, _T("\nRegisters:\n") );
  SLPrintf(hFile, _T("EAX:%08X\nEBX:%08X\nECX:%08X\nEDX:%08X\nESI:%08X\nEDI:%08X\n"), pCtx->Eax, pCtx->Ebx, pCtx->Ecx, pCtx->Edx, pCtx->Esi, pCtx->Edi );
  SLPrintf(hFile, _T("CS:EIP:%04X:%08X\n"), pCtx->SegCs, pCtx->Eip );
  SLPrintf(hFile, _T("SS:ESP:%04X:%08X  EBP:%08X\n"), pCtx->SegSs, pCtx->Esp, pCtx->Ebp );
  SLPrintf(hFile, _T("DS:%04X  ES:%04X  FS:%04X  GS:%04X\n"), pCtx->SegDs, pCtx->SegEs, pCtx->SegFs, pCtx->SegGs );
  SLPrintf(hFile, _T("Flags:%08X\n"), pCtx->EFlags );
#endif//_M_IX86

  // Walk the stack using x86 specific code
  if(FALSE != ImgHelpInit())
  {
    //walk with help of img help
    ImgHelpStackWalk(hFile, pCtx);
    ImgHelpFinit();
  }
  else
  {
#ifdef _M_IX86
    IntelStackWalk(hFile, pCtx);
#endif//_M_IX86
  };
};

//EXCEPTION_STRING filter
LONG WINAPI SLExceptionGlobalFilter(_EXCEPTION_POINTERS *pExceptionInfo)
{
  //try open report file
  //generate info
  //continute filter's execution

  HANDLE hFile = ::CreateFile
    (
    cpRepFileName,
    GENERIC_WRITE,
    FILE_SHARE_READ,
    0,
    OPEN_ALWAYS,
    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
    0
    );
  if(INVALID_HANDLE_VALUE != hFile)
  {
    //to end
    ::SetFilePointer(hFile, 0, 0, FILE_END);
    //generate
    SLGenReport(hFile, pExceptionInfo);
    //close
    ::CloseHandle(hFile);
    //warn user
    ::MessageBeep(MB_ICONASTERISK);
  };

  return (0 == pPrevFilter)
    ? EXCEPTION_CONTINUE_SEARCH
    : pPrevFilter(pExceptionInfo);
};

//////////////////////////////////////////////////////////////////////
//interface section


void SLExcInstall(LPCTSTR const cpcRepFileName, const bool bcInstallGlobal)
{
  //build rep file name as passed or as ".rep" from GetModuleFileName()
  //install filter

  //rep file name
  if(0 == cpcRepFileName)
  {
    //build
    const DWORD dwcLen = ::GetModuleFileName(::GetModuleHandle(0), cpRepFileName, COUNTOF(cpRepFileName));
    //scan for extention
    for(DWORD dwCount = 0; dwcLen > dwCount; dwCount++)
    {
      //try find '.' or ('\\', '/')
      const DWORD dwcPos = dwcLen - 1 - dwCount;
      TCHAR const cLook = cpRepFileName[dwcPos];
      if('.' == cLook)
      {
        ::lstrcpy(cpRepFileName + dwcPos, _T(".rep"));
        break;
      };
      if('\\' == cLook || '/' == cLook)
      {
        ::lstrcpy(cpRepFileName + dwcLen, _T(".rep"));
      };
    };
  }
  else
  {
    //passed
    ::lstrcpy(cpRepFileName, cpcRepFileName);
  };

  //filter
  if(false != bcInstallGlobal)
  {
    pPrevFilter = ::SetUnhandledExceptionFilter(SLExceptionGlobalFilter);
  };
  bIsInstalled = true;

  //clear img help function valud (for "MinCrt cfg's with not init global var's)
  hImgHlpModule = 0;
};

void SLExcUnInstall()
{
  //remove
  if(FALSE != bIsInstalled)
  {
    ::SetUnhandledExceptionFilter(pPrevFilter);
    bIsInstalled = FALSE;
  };
};

LONG WINAPI SLExceptionFilter(_EXCEPTION_POINTERS *pExceptionInfo)
{
  SLExceptionGlobalFilter(pExceptionInfo);
  return EXCEPTION_EXECUTE_HANDLER;
};

_SL_END
