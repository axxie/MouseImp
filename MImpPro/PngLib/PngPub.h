/**********************************************************************
png lib public interface
**********************************************************************/

#ifndef __PNGPUB_H
#define __PNGPUB_H

#pragma once


//create bmp from res mem
//ppBmpHdr and ppBmpMem dynamicaly allocated and freed by caller
bool PngCreateBmpFromRes(const BYTE* const cpcMem, const DWORD dwcSize, BITMAPINFO** ppBmpHdr, VOID** ppBmpMem);



#endif//__PNGPUB_H
