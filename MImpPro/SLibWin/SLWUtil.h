/**********************************************************************
base utilites
**********************************************************************/

#ifndef __SLWUTIL_H
#define __SLWUTIL_H

#pragma once


//////////////////////////////////////////////////////////////////////
//NameSpace's

#define _SLW_BEG namespace slw{
#define _SLW_END };
#define _SLW_USE using namespace slw;


#ifndef COUNTOF
#define COUNTOF(pMass) (sizeof(pMass) / sizeof(pMass[0]))
#endif//COUNTOF



#endif//__SLWUTIL_H