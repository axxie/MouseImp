/**********************************************************************
const's
**********************************************************************/

#ifndef __APPCONST_H
#define __APPCONST_H

#pragma once


#include "..\\MIGlobal.h"

//////////////////////////////////////////////////////////////////////
//const's

enum InstallConstEnum
{
  //msg for sent to progress wnd
  //lparam - number of operation's
  //wparam - queue operation (look on Param1 in SPFILENOTIFY_STARTSUBQUEUE)
  eicQueueStart = WM_USER + 0x100,
  //step in queue - increment progress count
  //lparam - path to file (pointer to FILEPATHS)
  //wparam - doing operation (look on Param1 in SPFILENOTIFY_STARTSUBQUEUE)
  eicQueueInc
};

//img const's
enum ImgConstEnum
{
  //list ctrl arrow's
  eicArrowWidth = 9,
  eicArrowNull = RGB(255, 0, 0),
  //check bmp
  eicCheckWidth = 15,
  eicCheckNull = RGB(255, 0, 0),
  //shrink type
  eicShirnkTypeWirth = 15,
  eicShirnkTypeNull = RGB(255, 0, 0),
  //setup progress
  eicSetupPrgsWidth = 12,
  eicSetupPrgsNull = RGB(255, 0, 255)
};

//key img indexe's
enum ImgIndEnum
{
  //arrow lmg's
  eiiUp = 0,
  eiiDown = 1,
  //check 
  eiiCheckOn = 0,
  eiiCheckOff = 1,
  //shrink
  eiiSShrink = 0,
  eiiSActivateOnly = 1,
  //setup progress
  eiiSPCurrnet = 0,
  eiiSPOk = 1,
  eiiSPFailed = 2,
  eiiSPEmpty = 3
};


//////////////////////////////////////////////////////////////////////
//string's

//"txt" res type
extern LPCTSTR const cpcResTextTypeName;
//name of install inf file
extern LPCTSTR const cpcInstallInfFileName;
//cmd key's
extern LPCTSTR const cpcCmdKeyQuickTour;
extern LPCTSTR const cpcCmgQuitFromImp;
//name of app for check ver
extern LPCTSTR const cpcCheckAppName;

//url's
extern LPCTSTR const cpcURLHomePage;
extern LPCTSTR const cpcURLMailInfo;
extern LPCTSTR const cpcURLMailTesh;
extern LPCTSTR const cpcURLMailTeshReg;
extern LPCTSTR const cpcURLHomePageName;
extern LPCTSTR const cpcURLMailInfoName;
extern LPCTSTR const cpcURLMailTeshName;
extern LPCTSTR const cpcURLReg1;
//path for check file
extern LPCTSTR const cpcCheckFileUrl;
extern LPCTSTR const cpcCfgNewsSotredVersion;

#endif//__APPCONST_H