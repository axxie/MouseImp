/**********************************************************************
png lib public interface
**********************************************************************/

#define STRICT
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#pragma hdrstop


#include "PngPub.h"


#include <limits.h>


extern "C"
{
#include "png.h"
};

enum ModuleConstEnum
{
  //allign boundary for bmp data
  emcBmpLineAllign = sizeof(DWORD)
};

//////////////////////////////////////////////////////////////////////
//helper's

struct MemRdStr
{
  const BYTE* pcMem;
  DWORD dwMemSize;
};

struct MemWrStr
{
  BYTE* pMem;
  DWORD dwMemSize;
};

//size of one pixel
static inline DWORD CalcPixSize(const BITMAPINFOHEADER* const cpcInitHdr)
{
  DWORD dwRes = 0;
  switch(cpcInitHdr->biBitCount)
  {
  case 1:
  case 4:
  case 8:
    //it's palettezed bitmap
    dwRes = cpcInitHdr->biBitCount;
    break;

  case 16:
  case 24:
    //true color bitmap
    dwRes = cpcInitHdr->biBitCount;
    break;

  case 32:
    //true color bitmap
    dwRes = sizeof(DWORD) * CHAR_BIT;
    break;

  default:
    break;
  };
  return dwRes;
};

//calc size of BITMAPINFO with known color count
static inline DWORD CalcBmpInfoSize(const DWORD dwcColorNumb)
{
  return sizeof(BITMAPINFO) + sizeof(RGBQUAD) * (dwcColorNumb - 1);
};


//calc size of hor line
inline DWORD CalcHorLineSize(const BITMAPINFOHEADER* const cpcInitHdr)
{
  const DWORD dwcPixelSize = CalcPixSize(cpcInitHdr);
  const LONG lcHorSize = (cpcInitHdr->biWidth * dwcPixelSize / CHAR_BIT + emcBmpLineAllign - 1) & (~(emcBmpLineAllign - 1));
  return lcHorSize;
};

//calc mem of bmp data
inline DWORD CalcArraySize(const BITMAPINFOHEADER* const cpcInitHdr)
{
  return max(cpcInitHdr->biHeight, cpcInitHdr->biHeight)  * CalcHorLineSize(cpcInitHdr);
};

//shift from start of data for passed line
static inline DWORD GetLineShift(const BITMAPINFOHEADER* const cpcHdr, const DWORD dwcWantLine)
{
  const DWORD dwcLineNumb = (0 < cpcHdr->biHeight)
    //bottom up 
    ? (cpcHdr->biHeight - dwcWantLine - 1)
    //bottom down
    : dwcWantLine;
  return dwcLineNumb * CalcHorLineSize(cpcHdr);
};


//////////////////////////////////////////////////////////////////////
//Pnglib helper func's

static void int_png_error(png_struct *png_ptr, const char *message)
{                                                
  throw(message);
}                                                

static void int_png_warn(png_struct *png_ptr, const char *message)
{
};

static void int_png_write(png_structp png_ptr, png_bytep data, png_size_t length)
{
  struct MemWrStr* const cpMemInfo = reinterpret_cast<struct MemWrStr*>(png_ptr->io_ptr);
  const DWORD dwcNeedMem = length * sizeof(*data);
  const DWORD dwcCopySize = min(dwcNeedMem, cpMemInfo->dwMemSize);
  CopyMemory(cpMemInfo->pMem, data, dwcCopySize);
  cpMemInfo->dwMemSize -= dwcCopySize;
  cpMemInfo->pMem += dwcCopySize;
  if(dwcNeedMem != dwcCopySize)
  {
    png_error(png_ptr, "End of memory");
  };
};

static void int_png_read(png_structp png_ptr, png_bytep data, png_size_t length)
{
  struct MemWrStr* const cpMemInfo = reinterpret_cast<struct MemWrStr*>(png_ptr->io_ptr);
  const DWORD dwcNeedMem = length * sizeof(*data);
  const DWORD dwcCopySize = min(dwcNeedMem, cpMemInfo->dwMemSize);
  CopyMemory(data, cpMemInfo->pMem, dwcCopySize);
  cpMemInfo->dwMemSize -= dwcCopySize;
  cpMemInfo->pMem += dwcCopySize;
  if(dwcNeedMem != dwcCopySize)
  {
    png_error(png_ptr, "End of memory");
  };
};

static void int_png_flush(png_structp png_ptr)
{
};


//////////////////////////////////////////////////////////////////////
//create bmp from res mem

bool PngCreateBmpFromRes(const BYTE* const cpcMem, const DWORD dwcMemSize, BITMAPINFO** ppBmpHdr, VOID** ppBmpMem)
{
  *ppBmpMem = 0;
  *ppBmpHdr = 0;

  bool bRes = false;

  try
  {
    //start unwing
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, int_png_error, int_png_warn);
    png_infop info_ptr = 0;
    if(0 != png_ptr)
    {
      info_ptr = png_create_info_struct(png_ptr);
      if(0 != info_ptr)
      {
        //attach input memory
        struct MemRdStr MemInfo;
        MemInfo.pcMem = cpcMem;
        MemInfo.dwMemSize = dwcMemSize;
        png_set_read_fn(png_ptr, reinterpret_cast<png_voidp>(&MemInfo), int_png_read);

        //read image info
        png_read_info(png_ptr, info_ptr);

        //get img info
        png_uint_32 width = 0;
        png_uint_32 height = 0;
        int bit_depth = 0; 
        int color_type = 0; 
        int interlace_type = 0;
        int compression_type = 0; 
        int filter_type = 0;
        int chanels = info_ptr->channels;
        png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, &compression_type, &filter_type);
        bool bPgnSrcChng = false;

        //set what pnglib need strip alpha
        if(color_type & PNG_COLOR_MASK_ALPHA)
        {
          png_set_strip_alpha(png_ptr); 
          bPgnSrcChng = true;
        };
        //update info on chng
        if(false != bPgnSrcChng)
        {
          png_read_update_info(png_ptr, info_ptr);
        };


        //create palette for Img
        if(PNG_COLOR_TYPE_GRAY & color_type)
        {
          //grayed image
          if(16 > bit_depth)
          {
            const DWORD dwcPalSize = 1 << bit_depth;
            *ppBmpHdr = reinterpret_cast<BITMAPINFO*>(new BYTE[CalcBmpInfoSize(dwcPalSize)]);
            RGBQUAD* const cpPalBuff = (*ppBmpHdr)->bmiColors;
            for(DWORD dwCount = 0; dwcPalSize > dwCount; dwCount++)
            {
              RGBQUAD& rCurr = cpPalBuff[dwCount];
              rCurr.rgbRed = static_cast<BYTE>(dwCount);
              rCurr.rgbBlue = static_cast<BYTE>(dwCount);
              rCurr.rgbGreen = static_cast<BYTE>(dwCount);
              rCurr.rgbReserved = 0;
            };
          };
        }
        else if((PNG_COLOR_TYPE_PALETTE & color_type) && 8 >= bit_depth)
        {
          //indexed image
          const DWORD dwcPalSize = 1 << bit_depth;
          *ppBmpHdr = reinterpret_cast<BITMAPINFO*>(new BYTE[CalcBmpInfoSize(dwcPalSize)]);
          RGBQUAD* const cpPalBuff = (*ppBmpHdr)->bmiColors;
          int iPngPalSize = 0;
          png_color* pPngPal = 0;
          png_get_PLTE(png_ptr, info_ptr, &pPngPal, &iPngPalSize);
          const DWORD dwcNotFillPos = min(static_cast<DWORD>(iPngPalSize), dwcPalSize);
          const DWORD dwcNotFillSize = dwcPalSize - dwcNotFillPos;
          for(DWORD dwCount = 0; dwcNotFillPos > dwCount; dwCount++)
          {
            RGBQUAD& rCurr = cpPalBuff[dwCount];
            //copy pal from image
            const png_color& rcFrom = pPngPal[dwCount];
            rCurr.rgbRed = rcFrom.red;
            rCurr.rgbBlue = rcFrom.blue;
            rCurr.rgbGreen = rcFrom.green;
            rCurr.rgbReserved = 0;
          };
          //zeroez extra pal, that not inited from file pal
          ZeroMemory(cpPalBuff + dwcNotFillPos, dwcNotFillSize * sizeof(RGBQUAD));
        };

        //not have palette
        if(0 == *ppBmpHdr)
        {
          *ppBmpHdr = reinterpret_cast<BITMAPINFO*>(new BYTE[CalcBmpInfoSize(4)]);
        };

        //construct BitmapinfoHeader
        BITMAPINFOHEADER& Hdr = (*ppBmpHdr)->bmiHeader;
        Hdr.biSize = sizeof(BITMAPINFOHEADER);
        Hdr.biWidth = width;
        Hdr.biHeight = height;
        Hdr.biPlanes = 1;
        Hdr.biBitCount = bit_depth * chanels;
        Hdr.biCompression = BI_RGB;
        Hdr.biSizeImage = 0;
        Hdr.biXPelsPerMeter = 0;
        Hdr.biYPelsPerMeter = 0;
        Hdr.biClrUsed = 0;
        Hdr.biClrImportant = 0;
        Hdr.biSizeImage = CalcArraySize(&Hdr);

        //mem for data
        *ppBmpMem = new BYTE[Hdr.biSizeImage];

        //set scan format's
        if(PNG_COLOR_TYPE_RGB & color_type)
        {
          png_set_bgr(png_ptr);
        };

        //buffer for Get
        const DWORD dwcPassNumb = (info_ptr->interlace_type)
          ? png_set_interlace_handling(png_ptr)
          : 1;
        for(DWORD dwPassCount = 0; dwcPassNumb > dwPassCount; dwPassCount++)
        {
         for(DWORD dwVerCount = 0; height > dwVerCount; dwVerCount++)
         {
           //read one line from file
           const DWORD dwcLineShift = GetLineShift(&Hdr, dwVerCount);
           png_bytep row_pointers = reinterpret_cast<BYTE*>(*ppBmpMem) + dwcLineShift;
           png_read_row(png_ptr, row_pointers, NULL);
         };
        };

        bRes = true;

        //read end - empty resource's
        png_read_end(png_ptr, 0);
      };
      png_destroy_read_struct(&png_ptr, &info_ptr, 0);
    };
  }
  catch(...)
  {
  };

  //if failed - free object's
  if(false == bRes)
  {
    delete *ppBmpHdr;
    *ppBmpHdr = 0;
    delete *ppBmpMem;
    *ppBmpMem = 0;
  };

  return bRes;
};


