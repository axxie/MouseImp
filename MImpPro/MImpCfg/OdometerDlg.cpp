/**********************************************************************
odometer dlg
**********************************************************************/

#include "stdafx.h"


#include "OdometerDlg.h"


#include "MImpCfg.h"

#include <Math.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
//global

enum ModuleConstEnum
{
  emcTimeMilSecInSec = 1000,
  emcTimeSecInMin = 60,
  emcTimeMinInHor = 60,
  emcTimeHorInDay = 24,
};

//transfer from "mouse pnt's" to "metric" (milimeters) distance (used conversion's from CMISharedInfo)
static inline void FromMousePntsToMetric(const LONGLONG& rcDist, LONGLONG& rRes)
{
  //dummy coefficient for MouseInput odometer
  rRes = rcDist * 4 / 150;
};

////meke "XXX day, YYY hors, ZZZZ min" for passed LONGLONG elapsed time counter
static inline void MakeElapsedTimeString(const LONGLONG& rcElapsedTime, CString& rOutResString)
{
  const LONGLONG llSecTotalTime = rcElapsedTime / static_cast<LONGLONG>(emcTimeMilSecInSec);
  const LONGLONG llcMinTotalTime = llSecTotalTime / static_cast<LONGLONG>(emcTimeSecInMin);
  const DWORD dwcSecTime = static_cast<DWORD>(llSecTotalTime - llcMinTotalTime * emcTimeSecInMin);
  const LONGLONG llcHourTotalTime = llcMinTotalTime / static_cast<LONGLONG>(emcTimeMinInHor);
  const DWORD dwcMinTime = static_cast<DWORD>(llcMinTotalTime - llcHourTotalTime * emcTimeMinInHor);
  const DWORD dwcDayTime = static_cast<DWORD>(llcHourTotalTime / static_cast<LONGLONG>(emcTimeHorInDay));
  const DWORD dwcHourTime = static_cast<DWORD>(llcHourTotalTime - static_cast<LONGLONG>(dwcDayTime * emcTimeHorInDay));

  if(0 == dwcDayTime)
  {
    if(0 == dwcHourTime)
    {
      rOutResString.Format(IDS_STR_MIN_SEC, dwcMinTime, dwcSecTime);
    }
    else
    {
      rOutResString.Format(IDS_STR_HOR_MIN_SEC, dwcHourTime, dwcMinTime, dwcSecTime);
    };
  }
  else
  {
    rOutResString.Format(IDS_STR_DAY_HOR_MIN_SEC, dwcDayTime, dwcHourTime, dwcMinTime, dwcSecTime);
  };
};

/////////////////////////////////////////////////////////////////////////////
// COdometerDlg dialog


COdometerDlg::COdometerDlg(CWnd* pParent)
	:CChildDlg(COdometerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COdometerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void COdometerDlg::DoDataExchange(CDataExchange* pDX)
{
	CChildDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COdometerDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

  //transfer "read only" data
  if(FALSE == pDX->m_bSaveAndValidate)
  {
    CMICommonPartWith64bit& common64 = *theApp.pCommon64;

    LONGLONG llSave;
    FromMousePntsToMetric(common64.llMoveDistanceSumm, llSave);
    SetDlgItemText(IDC_MOUSE_DIST_EDIT, BuildDistString(llSave));
    LONGLONG llTotal;
    FromMousePntsToMetric(common64.llMoveDistanceSaveSumm, llTotal);
    SetDlgItemText(IDC_MOUSE_SAVED_EDIT, BuildDistString(llTotal));
    //AShrink 
    const LONGLONG llcASPerSent = (0 == common64.ASOpenProd.llTotalTime)
      ? 0
      : common64.ASOpenProd.llWndSqTime * 100 / common64.ASOpenProd.llTotalTime;
    CString Str;
    Str.Format(IDS_STR_SH_ODM_MASK, static_cast<DWORD>(llcASPerSent));
    SetDlgItemText(IDC_ASHRINK_SAVED_EDIT, Str);

    ////time elapsed since last reset
    Str.Empty();
    MakeElapsedTimeString(common64.llTotalRunTime, Str);
    SetDlgItemText(IDC_ELAPSED_TIME_EDIT, Str);
  };
}


BEGIN_MESSAGE_MAP(COdometerDlg, CChildDlg)
	//{{AFX_MSG_MAP(COdometerDlg)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL COdometerDlg::OnSetActive()
{
  const BOOL bcRes = CChildDlg::OnSetActive();

  if(FALSE != bcRes)
  {
    UpdateData(FALSE);
  };

  return bcRes;
};

void COdometerDlg::OnReset() 
{
  CMICommonPartWith64bit& common64 = *theApp.pCommon64;
  common64.llMoveDistanceSaveSumm = 0;
  common64.llMoveDistanceSumm = 0;
  common64.ASOpenProd.llTotalTime = 0;
  common64.ASOpenProd.llWndSqTime = 0;
  common64.llTotalRunTime = 0;
  UpdateData(FALSE);
}

CString COdometerDlg::BuildDistString(const LONGLONG lcDist) const
{
  //break to part's
  //format string

  const LONG lcMM = static_cast<LONG>(lcDist % static_cast<LONGLONG>(eccMilimetersInCantimeter));
  const LONG lcCM = static_cast<LONG>(lcDist / static_cast<LONGLONG>(eccMilimetersInCantimeter) % static_cast<LONGLONG>(eccCantimeterInMeter)); 
  const LONG lcMT = static_cast<LONG>((lcDist / static_cast<LONGLONG>(eccMilimetersInCantimeter * eccCantimeterInMeter)) % static_cast<LONGLONG>(eccMetersInKilometer));
  const LONG lcKM = static_cast<LONG>(lcDist / static_cast<LONGLONG>(eccMetersInKilometer * eccMilimetersInCantimeter * eccCantimeterInMeter));

  CString Res;
  if(0 != lcKM)
  {
    Res.Format(IDS_STR_KM_MT_CM_MM, lcKM, lcMT, lcCM, lcMM);
  }
  else
  {
    if(0 != lcMT)
    {
      Res.Format(IDS_STR_MT_CM_MM, lcMT, lcCM, lcMM);
    }
    else
    {
      Res.Format(IDS_STR_CM_MM, lcCM , lcMM);
    };
  };
  return Res;
};
