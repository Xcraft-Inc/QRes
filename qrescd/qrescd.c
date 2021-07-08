/*********************************************************************/
/* qrescd - QRes autoplay component                                  */
/*                                                                   */
/* Author: Berend Engelbrecht                                        */
/*                                                                   */
/* Revision History:                                                 */
/*-------------------------------------------------------------------*/
/*YYMMDD| Author         | Reason                        | Version   */
/*000704| B.Engelbrecht  | created                       | 0.1.0.0   */
/*      |                |                               |           */
/*                                                                   */
/* Copyright 2000 Berend Engelbrecht. All rights reserved.           */
/*********************************************************************/
#if defined(__BORLANDC__)
#pragma warn -8057 // suppress unused parameter warnings in BCPP 5.0
#endif

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <string.h>
#include <qreslib.h>


// Modified version of CheckQuickres
BOOL check_quickres(void)
{
	BOOL  bOk = TRUE;

	// detect Windows version
	int iWinVer = GetWinVer();

	if ((iWinVer != WVER_UNSUPPORTED) && (iWinVer != WVER_NT))
	{
	  QUICKRES tApp;
	  bOk = FindQuickResApp(&tApp);
	}
	return bOk;
}


void mycopy(char *szFile, char *szSrcPath, char *szDestPath, BOOL bFailIfExist)
{
  char szSrc[MAX_PATH], szDest[MAX_PATH];

  strcpy(szSrc, szSrcPath);
  strcat(szSrc, szFile);
  strcpy(szDest, szDestPath);
  strcat(szDest, szFile);
  CopyFile(szSrc, szDest, bFailIfExist);
}

// Windows main. Note that we do not have a message loop.
// All error messages etc. are shown in message boxes and the
// program does not require a GUI. This keeps it nice and small
// and guarantees good performance on all systems.
//
int PASCAL	WinMain(  
				HINSTANCE  hInstance,     // Instance handle 
				HINSTANCE  hPrevInstance, // Previous instance handle 
				LPSTR lpszCmdLine,    // Command line string 
				int cmdShow )         // ShowWindow flag 
{
  char szFullPath[MAX_PATH+3], *ptr, szQResPath[MAX_PATH];
  char szWindowsPath[MAX_PATH], szSystemPath[MAX_PATH], szTmp[2 * MAX_PATH];
  char szQResPars[MAX_PATH], szProg[MAX_PATH];
  MSG  tMsg;

  // Get rid of the process feedback cursor (this is a bit dirty).
  // Windows 95 and NT normally display the feedback cursor until 
  // a program starts its message loop. Since qres doesn't have one 
  // this would mean it would be displayed for the maximum period of
  // 7 seconds when qres is started, confusing.
  if (PeekMessage(&tMsg, (HWND)0, 0, 0, PM_NOREMOVE))
    GetMessage(&tMsg, (HWND)0, 0, 0);

  if (GetModuleFileName(hInstance, szFullPath, sizeof(szFullPath)))
  {
    ptr = strrchr(szFullPath, '\\');
    if (ptr) *(ptr+1) = 0;
  }
  else
    strcpy(szFullPath, ".\\");
  GetWindowsDirectory(szWindowsPath, sizeof(szWindowsPath));
  strcat(szWindowsPath, "\\");
  GetSystemDirectory(szSystemPath, sizeof(szSystemPath));
  strcat(szSystemPath, "\\");

  // copy qres.exe to Windows, also quickres.exe if it doesn't
  // exist.
  strcpy(szQResPath, szFullPath);
  strcat(szQResPath, "qres\\");
  mycopy("qres.exe", szQResPath, szWindowsPath, FALSE);
  mycopy("quickres.exe", szQResPath, szWindowsPath, TRUE);
  strcpy(szTmp, szWindowsPath);

  // cehck if quickres is running, run it if it isn't
  if (!check_quickres()) {
    strcpy(szTmp, szWindowsPath);
    strcat(szTmp, "quickres.exe");
    WinExec(szTmp, SW_SHOW);
  }
  
  // copy help
  strcpy(szTmp, szWindowsPath);
  strcat(szTmp, "help\\");
  mycopy("qres.hlp", szQResPath, szTmp, FALSE);
  
  // copy DLLs
  mycopy("qres32.dll", szQResPath, szSystemPath, FALSE);
  mycopy("qresprop.dll", szQResPath, szSystemPath, FALSE);

  // register property page, set quickres parameters (?)
  sprintf(szTmp, "regedit.exe /S \"%sqresprop.reg\"", szQResPath);
  WinExec(szTmp, SW_HIDE);

  // get ini file settings
  strcpy(szTmp, szFullPath);
  strcat(szTmp, "qrescd.ini");
  GetPrivateProfileString("qrescd", "qrespars", "", szQResPars, sizeof(szQResPars), szTmp);
  GetPrivateProfileString("qrescd", "program", "", szProg, sizeof(szProg), szTmp);

  // finally run qres with qres parameters and program command line
  sprintf(szTmp, "qres %s %s%s", szQResPars, szFullPath, szProg);
  WinExec(szTmp, SW_SHOW);
  return 0;
}
