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
/*970122| B.Engelbrecht  | Split off qreslib2.c          | 0.4.0.0   */
/*970315| B.Engelbrecht  | Fix err msg if no app         |           */
/*      |                | Support display freq for NT4  | 1.0.6.0   */
/*970405| B.Engelbrecht  | Extra switches                | 1.0.7.0   */
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


// GetQResPars parses the command line passed to qres.
// Return value is the number of distinct parameters found.
// The function returns -1 and sets ptParsRet->cErrPar if an 
// unknown parameter is encountered.
//
int GetQResPars(LPSTR lpszCmdLine,     // Command line string
                QRES_PARS *ptParsRet ) // Returns parameters
{
  int    iTokens, iOfs;
  DWORD  dwValue;
  char   *szToken; 
  static char szApp[512];

  memset(ptParsRet, 0, sizeof(QRES_PARS));
  strncpy(szApp, lpszCmdLine, sizeof(szApp));
  szApp[sizeof(szApp)-1] = 0;
  for (szToken = strtok(lpszCmdLine, " "), iTokens = 0;
       szToken;
       szToken = strtok(NULL, " "), iTokens++)
  {
    // /X=nnn is alternate notation for X=nnn
    if ((strlen(szToken) > 2) && (szToken[0] == '/') &&
        (szToken[2] == '='))
      szToken++;
 
    // All numeric options have format A=value, no spaces allowed
    if ((strlen(szToken) > 1) && (szToken[1] == '='))
    {
      dwValue = strtoul(szToken+2, NULL, 10);
      switch(toupper(*szToken))
      {
        case 'X':
          ptParsRet->mNew.dwXRes = dwValue;
          break;
        case 'Y':
          ptParsRet->mNew.dwYRes = dwValue;
          break;
        case 'C':
          // It is allowed to specify '256 colors' instead of
          // '8 bits color'.
          if (dwValue < 256)
            ptParsRet->mNew.wBitsPixel = (UINT)dwValue;
          else if (dwValue == 256)
            ptParsRet->mNew.wBitsPixel = 8;
          break;
        case 'F': // 970315: frequency, NT only
          ptParsRet->mNew.wFreq = (UINT)dwValue;
          break;
        case 'Q': // 970405: QRes delay in 1/10 sec
          ptParsRet->wQRDelay = (UINT)dwValue;
          break;
        case 'A': // 970405: App delay in 1/10 sec
          ptParsRet->wAppDelay = (UINT)dwValue;
          break;
        case 'S':
          switch ((UINT)dwValue) {
            case 1:
              ptParsRet->dwFlags |= QF_SAVEUSER;
              break;
            case 2:
              ptParsRet->dwFlags |= QF_SAVEALL;
              break;
            default:
              sprintf(ptParsRet->szErr, "%s - option value not allowed\n\n", szToken);
              return -1;
          }
          break;
        default:
          // unknown option
          sprintf(ptParsRet->szErr, "Unknown option: '%c'\n\n", 
                  *szToken);
          return -1;
      }
    }
    else if ((strlen(szToken) > 1) && (szToken[0] == '/'))
    {
      // Boolean flags list starts with '/'
      while(*(++szToken))
      {
        switch(toupper(*szToken))
        {
          case 'R':
            ptParsRet->dwFlags |= QF_RESTORE;
            break;
          case 'D': // 970405: Direct switch (no QuickRes)
            ptParsRet->dwFlags |= QF_DIRECT;
            break;
          case 'H': // 020330: hide quickres after use
            ptParsRet->dwFlags |= QF_HIDEQR;
            break;
		  case '$': // 970913: Check QuickRes
			ptParsRet->dwFlags |= QF_CHECK;
			break;
          default:
            // unknown flag
            sprintf(ptParsRet->szErr, "Unknown option: '%c'\n\n", 
                    *szToken);
            return -1;
        }
      }
    }
    else
    {
      // It is assumed that the file name of the application to
      // run directly follows the last option token.
      // Long file names with embedded spaces are OK, quotes not
      // necessary but allowed.
      iOfs = szToken - lpszCmdLine;
      if (iOfs > 0)
	  {
		        memmove(ptParsRet->szApp, szApp + iOfs, 
                sizeof(szApp) - iOfs); 
		strcpy(szApp, ptParsRet->szApp);
		strlwr(szApp);
		szToken = strstr(szApp, ".scr");

		// .scr must be at end of string or followed by quote or
		// space.
		if (szToken && (!*(szToken+4) || strchr("\"' ", *(szToken+4))))
		  ptParsRet->dwFlags |= QF_SCRSAVER;
		  
	  }
      break; // stop parsing when file name is found
    }
  }

  return iTokens;
}

VOID CALLBACK QResMessageBoxCallback(LPHELPINFO lpHelpInfo)
{
  WinHelp(NULL, "qres.hlp", HELP_CONTEXT, lpHelpInfo->dwContextId);
}

// CheckQuickRes checks and reports on QuickRes installation
// Return value is TRUE if everything is OK.
// If bShowIfOK is FALSE no messagebox is shown if everything 
// is OK.
BOOL CheckQuickRes(BOOL bShowIfOK)
{
	int iWinVer = GetWinVer();
    char *szVer = "Unknown Windows version\n"
		          "QRes may not work on this PC.";
	char *szApp = "";
	char  szMsg[400];
	int   iTopic = 10000; // QuickRes help topic
	UINT  uStyle = MB_OK;
	BOOL  bOk = TRUE;
	MSGBOXPARAMS tParams;


	// detect Windows version
	switch(iWinVer)
	{
	  case WVER_UNSUPPORTED:
		szVer = "This version of Windows is not supported.\r\n"
			    "QRes can not be used on your PC.";
		bOk = FALSE;
		break;
	  case WVER_95:
		szVer = "Detected Windows 95 (old version).";
        uStyle |= MB_HELP;
		break;
	  case WVER_OSR2:
		szVer = "Detected Windows 95 OSR2 with built-in QuickRes.";
		break;
      case WVER_98:
      case WVER_ME:
		szVer = "Detected Windows 98 or higher with built-in QuickRes.";
		break;
	  case WVER_NT:
		szVer = "Detected Windows NT 4.0 or higher\r\n\r\n"
			    "Microsoft QuickRes is not needed on your PC.";
		break;
	}

	if ((iWinVer != WVER_UNSUPPORTED) && (iWinVer != WVER_NT))
	{
		QUICKRES tApp;
		if (FindQuickResApp(&tApp))
		{
		  szApp = "QuickRes is correctly installed on your PC.";
		}
		else if ((iWinVer == WVER_OSR2) || (iWinVer == WVER_98))
		{
		  szApp = "Microsoft QuickRes is not enabled\r\n"
		          "on your PC. (see Help)";
 		  bOk = FALSE;
		  iTopic = 10003; // Memphis help topic
          uStyle |= MB_HELP;
		}
		else
		{
		  szApp = "Microsoft QuickRes is not or not correctly installed\r\n"
		          "on your PC.\r\n"
				  "Please download and install QuickRes (see Help)";
		  bOk = FALSE;
		  iTopic = 10001; // Powertoy help topic
          uStyle |= MB_HELP;
		}
	}

	if (bShowIfOK || !bOk)
	{
      if (iWinVer == WVER_UNSUPPORTED)
	    uStyle |= MB_ICONERROR;
      else
        uStyle |= (bOk ? MB_ICONINFORMATION : MB_ICONWARNING); 

	  sprintf(szMsg, "%s\r\n\r\n%s", szVer, szApp);
	  memset(&tParams, 0, sizeof(tParams));
	  tParams.cbSize = sizeof(tParams);
	  tParams.lpszText = szMsg; 
      tParams.lpszCaption = "QuickRes Installation Check"; 
      tParams.dwStyle = uStyle; 
      tParams.dwContextHelpId = iTopic; 
	  tParams.lpfnMsgBoxCallback = QResMessageBoxCallback;
	  MessageBoxIndirect(&tParams);
	}
	
	return bOk;
}




	


			    

