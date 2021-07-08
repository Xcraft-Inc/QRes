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
/*      |                |                               |           */
/*                                                                   */
/* Copyright 1997 Berend Engelbrecht. All rights reserved.           */
/*********************************************************************/

#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>
#include <stdio.h>
#include <string.h>
#include <qreslib.h>



// Windows main. Note that we do not have a message loop.
// All error messages etc. are shown in message boxes and the
// program does not require a GUI. This keeps it nice and small
// and guarantees good performance on all systems.
//
int PASCAL	WinMain(  
				HANDLE hInstance,     // Instance handle 
				HANDLE hPrevInstance, // Previous instance handle 
				LPSTR lpszCmdLine,    // Command line string 
				int cmdShow )         // ShowWindow flag 
{
  QUICKRES   tQRes;     // QuickRes application data
  QRES_PARS  tCmdPars;  // User-specified parameters
  int        iResult;
  char       szMsg[768];
  MSG        tMsg;

  // Get rid of the process feedback cursor (this is a bit dirty).
  // Windows 95 and NT normally display the feedback cursor until 
  // a program starts its message loop. Since qres doesn't have one 
  // this would mean it would be displayed for the maximum period of
  // 7 seconds when qres is started, confusing.
  if (PeekMessage(&tMsg, (HWND)0, 0, 0, PM_NOREMOVE))
    GetMessage(&tMsg, (HWND)0, 0, 0);

  // Get command line parameters
  iResult = GetQResPars(lpszCmdLine, &tCmdPars);
  if (iResult <= 0)
  {
    strcpy(szMsg, tCmdPars.szErr);
    strcat(szMsg,
             "syntax: qres [options] program name\n\n"
             "where [options] is one or more of the following:\n"
             " X=nnn - set horizontal pixels\n"
             " Y=nnn - set vertical pixels\n"
             " C=nnn - set bits/pixel or number of colors\n"
             " /R    - restore mode after program finishes\n"
           "\n'program name' is optional file name of program to\n"
           "execute after screen mode switch.\n\n"
           "© 1997 Berend Engelbrecht - einhard@pi.net");
    MessageBox(NULL, szMsg, "qres: QuickRes screen mode switch", 0);
    return 0;
  }

  // Complete missing parameters
  CompleteQResPars(&tCmdPars);

  // Get QuickRes app data
  if (!FindQuickResApp(&tQRes))
  {
    if (MessageBox(NULL, "QuickRes not installed. Resolution switch is disabled.", 
               "qres", MB_ICONEXCLAMATION | MB_OKCANCEL) != IDOK)
      return 0;
  }
  else if (!(tCmdPars.dwFlags & QF_NOSWITCH))
  {
    if (!SwitchQResMode(&tCmdPars.mNew, &tQRes))
    {
      sprintf(szMsg, "Screen mode not found: x=%ld y=%ld c=%d",
              tCmdPars.mNew.dwXRes, tCmdPars.mNew.dwYRes, 
              tCmdPars.mNew.wBitsPixel);
      MessageBox(NULL, szMsg, "qres", 0); 
      return 0;
    }
  }

  // Finally run the app if one was specified
  if (tCmdPars.szApp[0])
  {
    STARTUPINFO         tStartup;
    PROCESS_INFORMATION tProcess;
    DWORD               dwExitCode;

    memset(&tStartup, 0, sizeof(STARTUPINFO));
    tStartup.cb = sizeof(STARTUPINFO);
    tStartup.dwFlags = STARTF_USESHOWWINDOW;
    tStartup.wShowWindow = cmdShow;

    if (CreateProcess(NULL, 
          tCmdPars.szApp, // pointer to command line string
		  NULL,           // pointer to process security attributes 
		  NULL,           // pointer to thread security attributes 
		  FALSE,          // handle inheritance flag 
		  0L,             // creation flags 
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
    }
    else
    {
      sprintf(szMsg, "Cannot run %s", tCmdPars.szApp);
      MessageBox(NULL, szMsg, "qres", 0);
    }
    // Switch back to old mode
    if (tCmdPars.dwFlags & QF_RESTORE)
      SwitchQResMode(&tCmdPars.mOld, &tQRes);
  }
  return 0;
}
