/*********************************************************************/
/* qres - Windows 95 quickres requestor.                             */
/*        This program provides an automated interface to Microsofts */
/*        QuickRes screen mode switch applet.                        */
/*                                                                   */
/* Author: Berend Engelbrecht                                        */
/*                                                                   */
/* Revision History:                                                 */
/*-------------------------------------------------------------------*/
/*YYMMDD| Author         | Reason                        | Version   */
/*------+----------------+-------------------------------+-----------*/
/*970102| B.Engelbrecht  | Created                       | 0.1.0.0   */
/*970110| B.Engelbrecht  | Added /R parameter            | 0.2.0.0   */
/*970308| B.Engelbrecht  | First NT beta                 | 1.0.5.0   */
/*970315| B.Engelbrecht  | Support display frequency     | 1.0.6.0   */
/*970405| B.Engelbrecht  | Extra switches                | 1.0.7.0   */
/*971212| B.Engelbrecht  | Win98 stuff                   | 1.0.8.2   */
/*050109| B.Engelbrecht  | Modifications for Borland C++ | 1.0.9.7   */
/*      |                |                               |           */
/*                                                                   */
/* Copyright 1997-2005 Berend Engelbrecht. All rights reserved.      */
/*********************************************************************/
#if defined(__BORLANDC__)
#pragma warn -8080 // suppress warnings for _str functions in BCPP 6.0
#endif

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "qreslib.h"

#ifndef DM_POSITION
#define DM_POSITION         0x00000020L
#endif


// GetWinVer returns one of the WVER_... constants to
// indicate the detected Windows version
int GetWinVer(void)
{
  OSVERSIONINFO tVerInfo;
  char szVer[80];

  tVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  if (!GetVersionEx(&tVerInfo) ||
	  (tVerInfo.dwMajorVersion < 4))
    return WVER_UNSUPPORTED;

  if (tVerInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
    return WVER_NT;
  
  if ((tVerInfo.dwMajorVersion == 4) && 
	  (tVerInfo.dwMinorVersion == 0) &&
      (tVerInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS))
    return (tVerInfo.szCSDVersion[1] == 'B') ? WVER_OSR2 : WVER_95;
  
  if ((tVerInfo.dwMajorVersion == 4) && 
	  (tVerInfo.dwMinorVersion == 10) &&
      (tVerInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS))
	return WVER_98;

  if ((tVerInfo.dwMajorVersion == 4) && 
	  (tVerInfo.dwMinorVersion == 90) &&
      (tVerInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS))
	return WVER_ME;

  sprintf(szVer, "%ld.%ld %ld", tVerInfo.dwMajorVersion,
	     tVerInfo.dwMinorVersion, tVerInfo.dwPlatformId);
  MessageBox(NULL, szVer, "qres", 0);
  return WVER_UNKNOWN;
}

void SetRegValue(HKEY hKey, LPCTSTR lpSubKey, 
				 LPCTSTR lpValueName, LPCTSTR lpValue)
{
  if(RegOpenKeyEx(hKey, lpSubKey, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS)
  {
    if (lpValue) {
	  RegSetValueEx(hKey, lpValueName, 0, REG_SZ, (const BYTE *)lpValue, strlen(lpValue)+1);
    } else {
      RegDeleteValue(hKey, lpValueName);
    }
	RegCloseKey(hKey);
  }
}

// IsQResDlgOpen returns TRUE if the specified dialog window
// is open and belongs to the same process as the quickres
// application.
//
BOOL IsQResDlgOpen(QUICKRES *ptQRes, char *szDlgName)
{
  HWND  hwndDlg;
  DWORD dwDlgProcess;

  hwndDlg = FindWindowEx(NULL, NULL, MAKEINTRESOURCE(32770U), 
                         szDlgName);
  if (!hwndDlg)
    return FALSE;

  // Retrieve and compare the dlg process id, return TRUE if
  // it is equal to QuickRes process id
  GetWindowThreadProcessId(hwndDlg, &dwDlgProcess);

  return (ptQRes->dwProcess == dwDlgProcess);
}


// GetCurQResMode returns the current QuickRes settings for the 
// screen in *ptMode.
//
void GetCurQResMode(QRMODE *ptMode)
{
  HWND hwndScreen = GetDesktopWindow();
  HDC  hdcScreen  = GetDC(hwndScreen);

  ptMode->dwXRes     = (DWORD)GetDeviceCaps(hdcScreen,HORZRES);
  ptMode->dwYRes     = (DWORD)GetDeviceCaps(hdcScreen,VERTRES);
  ptMode->wBitsPixel = (UINT)GetDeviceCaps(hdcScreen,BITSPIXEL);
  if (GetWinVer() == WVER_NT)
	  ptMode->wFreq = (UINT)GetDeviceCaps(hdcScreen, VREFRESH);

  ReleaseDC(hwndScreen, hdcScreen);
}


// CompleteQResPars fills out parameters not specified by the user
// based on the current screen mode. It also initialises ptParsRet->mOld
// and sets the QF_NOSWITCH flag if no mode switch is required.
void CompleteQResPars(QRES_PARS *ptParsRet ) 
{
  GetCurQResMode(&ptParsRet->mOld);
  if (!ptParsRet->mNew.dwXRes && !ptParsRet->mNew.dwYRes)
  {
    ptParsRet->mNew.dwXRes = ptParsRet->mOld.dwXRes;
    ptParsRet->mNew.dwYRes = ptParsRet->mOld.dwYRes;
  }
  else if (ptParsRet->mNew.dwXRes == ptParsRet->mOld.dwXRes)
  {
    if (!ptParsRet->mNew.dwYRes)
      ptParsRet->mNew.dwYRes = ptParsRet->mOld.dwYRes;
  }
  else if (ptParsRet->mNew.dwYRes == ptParsRet->mOld.dwYRes)
  {
    if (!ptParsRet->mNew.dwXRes)
      ptParsRet->mNew.dwXRes = ptParsRet->mOld.dwXRes;
  }
  if (!ptParsRet->mNew.wBitsPixel)
    ptParsRet->mNew.wBitsPixel = ptParsRet->mOld.wBitsPixel;

  // Compare the requested mode with the current one and raise 
  // QF_NOSWITCH if they are identical
  if (!memcmp(&ptParsRet->mNew, &ptParsRet->mOld, sizeof(QRMODE)))
    ptParsRet->dwFlags |= QF_NOSWITCH;
  ptParsRet->mNew.dwFlags = ptParsRet->dwFlags; // New 20000522
  ptParsRet->mOld.dwFlags = ptParsRet->dwFlags; // New 20030907
}


// FindQuickResApp looks for the Microsoft QuickRes applet
// and returns its main window and other required data in 
// *ptQRes. If the applet is found the return value is TRUE,
// otherwise FALSE.
//
BOOL FindQuickResApp(QUICKRES *ptQRes)
{
  int iWinVer = GetWinVer();
  memset(ptQRes, 0, sizeof(QUICKRES));

  if (iWinVer == WVER_UNSUPPORTED)
    return FALSE;

  // If we are running on NT 4.0 we don't need QuickRes
  if (iWinVer == WVER_NT)
    ptQRes->fDirect = TRUE;

  if (ptQRes->fDirect)
    return TRUE;

  // Attempt to start QuickRes, in case the user didn't
  // Note: RunDLL doesn't work for Win98 beta. Why?
  // [HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run]
  // "Taskbar Display Controls"="RunDLL deskcp16.dll,QUICKRES_RUNDLLENTRY"
  switch(iWinVer)
  {
    case WVER_95:
	  WinExec("quickres", SW_NORMAL);
	  break;
	case WVER_OSR2:
    case WVER_98:
    case WVER_ME:
	  SetRegValue(HKEY_CURRENT_USER,"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
		  "Taskbar Display Controls","RunDLL deskcp16.dll,QUICKRES_RUNDLLENTRY");
	  WinExec("RunDLL deskcp16.dll,QUICKRES_RUNDLLENTRY", SW_NORMAL);
	  break;
  }

  // Wait briefly to let other app run its course
  Sleep(50);

  // This finds QuickRes on Win95 OSR2:
  ptQRes->hwnd = FindWindowEx(NULL, NULL, "SysQuickRes", "QuickRes");
  if (ptQRes->hwnd)
  {
    ptQRes->fOSR2 = TRUE;
    ptQRes->wFirstMenuID = 350;
  }
  else
  {
    ptQRes->hwnd = FindWindowEx(NULL, NULL, "QuickRes", "QuickRes");
    if (!ptQRes->hwnd)
      return FALSE;
    ptQRes->wFirstMenuID = 1000;
  }
  ptQRes->dwThread = GetWindowThreadProcessId(ptQRes->hwnd, 
                                              &ptQRes->dwProcess);
  return TRUE;
}


// FindReqQResMode returns >= 0 if the requested mode is supported
// by the installed graphics driver, -1 if the mode is not found.
// The return value must be added to the menu base to get the
// quickres menu option ID for the requested mode.
//
static int FindReqQResMode(QRMODE *ptReqMode, BOOL fOSR2)
{
  DEVMODE  tMode;
  DWORD    dwMode = 0;
  int      iMenu  = 0;
  BOOL     fFound = FALSE;

  while (EnumDisplaySettings(NULL, dwMode, &tMode))
  {
    if (!fOSR2 || (tMode.dmBitsPerPel >= 8))
    {
      if (tMode.dmBitsPerPel == (DWORD)ptReqMode->wBitsPixel)
      {
        if (   (!ptReqMode->dwXRes || 
                (ptReqMode->dwXRes == tMode.dmPelsWidth))
            && (!ptReqMode->dwYRes || 
                (ptReqMode->dwYRes == tMode.dmPelsHeight))
           )
        {
          fFound = TRUE;
          break;
        }
      }
      iMenu++;
    }
    dwMode++;
  }
  return fFound ? iMenu : -1;
}

// SwitchQResNT switches the mode without the help of QuickRes
// on Windows NT 4.x
BOOL SwitchQResNT(QRMODE *ptReqMode)
{
  DEVMODE  tMode;
  DWORD    dwMode = 0;
  DWORD    dwFlags = 0; // 20000522

  while (EnumDisplaySettings(NULL, dwMode, &tMode))
  {
    if (tMode.dmBitsPerPel == (DWORD)ptReqMode->wBitsPixel)
    {
      if (   (!ptReqMode->dwXRes || 
              (ptReqMode->dwXRes == tMode.dmPelsWidth))
          && (!ptReqMode->dwYRes || 
              (ptReqMode->dwYRes == tMode.dmPelsHeight))
          && (!ptReqMode->wFreq ||
              ((DWORD)ptReqMode->wFreq == tMode.dmDisplayFrequency))
         )
      {
        // found matching mode
        tMode.dmSize = sizeof(DEVMODE);
        tMode.dmFields = DM_BITSPERPEL	| DM_PELSWIDTH 
                         | DM_PELSHEIGHT | DM_POSITION;
        if (ptReqMode->wFreq)
          tMode.dmFields |= DM_DISPLAYFREQUENCY;

        // 20000522 - new support for permanent mode switch on NT
        if (ptReqMode->dwFlags & (QF_SAVEUSER | QF_SAVEALL)) {
          dwFlags |= CDS_UPDATEREGISTRY;
          if (ptReqMode->dwFlags & QF_SAVEALL) {
            dwFlags |= CDS_GLOBAL;
          }
        }
        //MessageBox(NULL, "NT2", "qres", 0);
        dwFlags |= CDS_SET_PRIMARY;
        return (ChangeDisplaySettings(&tMode, dwFlags)
                == DISP_CHANGE_SUCCESSFUL);
      }
    }
    dwMode++;
  }
  return FALSE;
}

// SwitchQResMode calls FindReqQResMode to find requested mode, 
// then requests mode switch from QuickRes
BOOL SwitchQResMode(QRMODE *ptReqMode, QUICKRES *ptQRes)
{
  int  iMode;

  if (ptQRes->fDirect)
    return SwitchQResNT(ptReqMode);

  if (!ptQRes->hwnd)
    return FALSE;

  // Find QuickRes menu index for requested screen mode
  iMode = FindReqQResMode(ptReqMode, ptQRes->fOSR2);
  if (iMode < 0)
    return FALSE;
  iMode += ptQRes->wFirstMenuID;

  // Do the mode switch and briefly wait for the result
  PostMessage(ptQRes->hwnd, WM_COMMAND, (WPARAM)iMode, 0L);

  // Briefly wait for the result of the resolution switch.
  // If the 'compatibility warning' box (OSR2) is open, wait for
  // it to close.
  do {
    Sleep(900);
  } while (IsQResDlgOpen(ptQRes, ptQRes->fOSR2 ? "Compatibility Warning" : "QuickRes"));
  Sleep(100);
  return TRUE;
}



