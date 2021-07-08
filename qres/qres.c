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
/*970114| B.Engelbrecht  | Split off qreslib.c           | 0.3.0.0   */
/*970116| B.Engelbrecht  | Fix Win95 OSR2 menu bug       | 0.3.0.1   */
/*970122| B.Engelbrecht  | Save/restore desktop icon pos | 0.4.0.0   */
/*970315| B.Engelbrecht  | Support display freq for NT4  | 1.0.6.0   */
/*970405| B.Engelbrecht  | Extra switches                | 1.0.7.0   */
/*980125| B.Engelbrecht  | Win98/IE4 fix                 | 1.0.8.2   */
/*980228| B.Engelbrecht  | Screen saver support          | 1.0.8.2   */
/*050109| B.Engelbrecht  | Modifications for Borland C++ | 1.0.9.7   */
/*      |                |                               |           */
/*                                                                   */
/* Copyright 1998-2005 Berend Engelbrecht. All rights reserved.      */
/*********************************************************************/
#if defined(__BORLANDC__)
#pragma warn -8057 // suppress unused parameter warnings in BCPP 5.0
#endif

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <string.h>
#include <qreslib.h>

static  char szMsg[800];

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
  QUICKRES   tQRes;     // QuickRes application data
  QRES_PARS  tCmdPars, tKeepPars;  // User-specified parameters
  int        iResult;
  MSG        tMsg;
  BOOL       fHook,                 // set to TRUE if hook is installed
             fDesktopSaved = FALSE; // set to TRUE if qres32.dll call succeeds

  // Get rid of the process feedback cursor (this is a bit dirty).
  // Windows 95 and NT normally display the feedback cursor until 
  // a program starts its message loop. Since qres doesn't have one 
  // this would mean it would be displayed for the maximum period of
  // 7 seconds when qres is started, confusing.
  if (PeekMessage(&tMsg, (HWND)0, 0, 0, PM_NOREMOVE))
    GetMessage(&tMsg, (HWND)0, 0, 0);

  memset(&tQRes, 0, sizeof(tQRes));
  memset(&tCmdPars, 0, sizeof(tCmdPars));

  // Get command line parameters
  iResult = GetQResPars(lpszCmdLine, &tCmdPars);
  if (iResult <= 0)
  {
  	MSGBOXPARAMS tParams;

    strcpy(szMsg, tCmdPars.szErr);
    strcat(szMsg, "QRes version 1.0.9.4\r\n"
           "© 1998-2000 Berend Engelbrecht - einhard@wxs.nl\r\n"
           "            http://www.berend.com/qres.html\r\n\r\n"
		   "Press [Help] for a list of command line parameters.");
  	memset(&tParams, 0, sizeof(tParams));
	tParams.cbSize = sizeof(tParams);
	tParams.lpszText = szMsg; 
    tParams.lpszCaption = "QRes: QuickRes screen mode switch"; 
    tParams.dwStyle = MB_HELP; 
    tParams.dwContextHelpId = 10002; // CommandLine topic
	tParams.lpfnMsgBoxCallback = QResMessageBoxCallback;
	MessageBoxIndirect(&tParams);
    return 0;
  }

  if (tCmdPars.dwFlags & QF_CHECK)
  {
    // Just check if QuickRes is installed
	return !CheckQuickRes(TRUE);
  }
  tKeepPars = tCmdPars;

  do // while (tCmdPars.dwFlags & QF_SCRSAVER)
  {
  tCmdPars = tKeepPars;

  // if app is a screen saver, wait until it is active
  if (tCmdPars.dwFlags & QF_SCRSAVER)
  {
	while(!FindWindowEx(NULL, NULL, "WindowsScreenSaverClass", NULL))
      Sleep(500);
  }

  // Complete missing parameters
  CompleteQResPars(&tCmdPars);
  
  // Activate the Explorer hook DLL
  fHook = InstallQResFilter(TRUE);

    // Save icons if we are running an app and /R is specified
  if ((tCmdPars.dwFlags & QF_RESTORE) && tCmdPars.szApp[0])
  {
    if (fHook)  // if hook DLL is active
    {
      PostMessage(QResMsgWnd(), QRES_MESSAGE, QRES_SAVE_DESKTOP, 0);
      fDesktopSaved = TRUE;
    }
  }

  // get rid of the desktop mouse cursor
  if (fHook && !(tCmdPars.dwFlags & QF_NOSWITCH) && (GetWinVer() != WVER_ME))
    PostMessage(QResMsgWnd(), QRES_MESSAGE, QRES_HIDE_MOUSE, 0);

  // 970405: New delay before looking for QuickRes
  if (tCmdPars.wQRDelay)
    Sleep (100 * tCmdPars.wQRDelay);
  else
    Sleep(100); // minimum delay

  // Only get QuickRes app data if we use QuickRes for switching
  tQRes.fDirect = (tCmdPars.dwFlags & QF_DIRECT);
  if (!tQRes.fDirect && !FindQuickResApp(&tQRes))
  {
	CheckQuickRes(TRUE);

 //   if (MessageBox(NULL, "QuickRes not installed. Resolution switch is disabled.", 
 //              "qres", MB_ICONEXCLAMATION | MB_OKCANCEL) != IDOK)
 //   {
 //     InstallQResFilter(FALSE);
 //     return 0;
 //   }
  }
  else if (!(tCmdPars.dwFlags & QF_NOSWITCH))
  {
    if (!SwitchQResMode(&tCmdPars.mNew, &tQRes))
    {
      if (fHook)
        PostMessage(QResMsgWnd(), QRES_MESSAGE, QRES_SHOW_MOUSE, 0);
      sprintf(szMsg, "Screen mode not found: x=%ld y=%ld c=%d",
              tCmdPars.mNew.dwXRes, tCmdPars.mNew.dwYRes, 
              tCmdPars.mNew.wBitsPixel);
      MessageBox(NULL, szMsg, "qres", 0); 
      InstallQResFilter(FALSE);
      return 0;
    }
  }

  // Restore the mouse cursor
  if (fHook && (GetWinVer() != WVER_ME))
    PostMessage(QResMsgWnd(), QRES_MESSAGE, QRES_SHOW_MOUSE, 0);

  // Finally run the app if one was specified
  if (tCmdPars.szApp[0])
  {
    STARTUPINFO         tStartup;
    PROCESS_INFORMATION tProcess;
    DWORD               dwExitCode;

    memset(&tStartup, 0, sizeof(STARTUPINFO));
    tStartup.cb = sizeof(STARTUPINFO);
    tStartup.dwFlags = STARTF_USESHOWWINDOW;
    tStartup.wShowWindow = (WORD)cmdShow;

    if (tCmdPars.wAppDelay)
      Sleep (100 * tCmdPars.wAppDelay);

    if (CreateProcess(NULL, 
          tCmdPars.szApp, // pointer to command line string
		  NULL,           // pointer to process security attributes 
		  NULL,           // pointer to thread security attributes 
		  FALSE,          // handle inheritance flag 
		  CREATE_NEW_PROCESS_GROUP, // creation flags 
          NULL,           // pointer to new environment block 
          NULL,           // pointer to current directory name 
          &tStartup,      // pointer to STARTUPINFO 
          &tProcess       // pointer to PROCESS_INFORMATION  
        ))
    {
      if (tCmdPars.dwFlags & QF_RESTORE)
      {
        // Wait for app to quit
        while (GetExitCodeProcess(tProcess.hProcess, &dwExitCode))
        {
          if (dwExitCode != STILL_ACTIVE)
            break;
          Sleep(1000);
        }
        
      }

      // Detach child process
      CloseHandle(tProcess.hThread);
      CloseHandle(tProcess.hProcess);

	  // Specific fix for MSIE4. Loader process quits while
	  // browser is still running on Windows 98
	  strupr(tCmdPars.szApp);
	  if (strstr(tCmdPars.szApp, "IEXPLORE") || strstr(tCmdPars.szApp, "RUNDLL32"))
	  {
		  while (FindWindowEx(NULL, NULL, "IEFrame", NULL))
            Sleep(1000);
	  }

	  // specific code for screensaver
	  if (tCmdPars.dwFlags & QF_SCRSAVER)
	  {
	    while(FindWindowEx(NULL, NULL, "WindowsScreenSaverClass", NULL))
          Sleep(1000);
	  }
    }
    else
    {
      sprintf(szMsg, "Cannot run %s", tCmdPars.szApp);
      MessageBox(NULL, szMsg, "qres", 0);
    }
    // Switch back to old mode
    if (tCmdPars.dwFlags & QF_RESTORE)
    {
      if (fHook && (GetWinVer() != WVER_ME))
      {
        PostMessage(QResMsgWnd(), QRES_MESSAGE, QRES_HIDE_MOUSE, 0);
        Sleep(100);
      }
      SwitchQResMode(&tCmdPars.mOld, &tQRes);
      if (fHook && (GetWinVer() != WVER_ME))
        PostMessage(QResMsgWnd(), QRES_MESSAGE, QRES_SHOW_MOUSE, 0);

      // If we managed to save desktop icon positions, restore them now
      if (fDesktopSaved)
      {
        PostMessage(QResMsgWnd(), QRES_MESSAGE, QRES_RESTORE_DESKTOP, 0);
        Sleep(5000);
      }
    }
  }
  
  InstallQResFilter(FALSE);

  }
  while (tCmdPars.dwFlags & QF_SCRSAVER);
  if (tQRes.hwnd && (tCmdPars.dwFlags & QF_HIDEQR)) {
    SetRegValue(HKEY_CURRENT_USER,"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
		  "Taskbar Display Controls", NULL);
    PostMessage(tQRes.hwnd, WM_CLOSE, 0, 0);
  }
  return 0;
}
