/*********************************************************************/
/* qreslib.h - Include file for qres common library                  */
/*                                                                   */
/* Author: Berend Engelbrecht                                        */
/*                                                                   */
/* Revision History:                                                 */
/*-------------------------------------------------------------------*/
/*YYMMDD| Author         | Reason                        | Version   */
/*------+----------------+-------------------------------+-----------*/
/*970114| B.Engelbrecht  | Created                       | 0.3.0.0   */
/*970122| B.Engelbrecht  | Added QRES32.DLL functionality| 0.4.0.0   */
/*      |                |                               |           */
/*                                                                   */
/* Copyright 1997 - 2005 Berend Engelbrecht. All rights reserved.    */
/*********************************************************************/

#ifndef QRESLIB_H
#define QRESLIB_H
#ifdef __cplusplus
extern "C" {
#endif 

////////////////////////////////////////////////////////////
// qreslib.lib functionality

// dwFlags constants
#define QF_NOSWITCH  0x00000001  // Set if no mode switch required
#define QF_RESTORE   0x00000002  // Set if old mode must be restored
#define QF_DIRECT    0x00000004  // Set to switch without QuickRes
#define QF_CHECK     0x00000008  // Just check QuickRes setup 
#define QF_SCRSAVER  0x00000010  // Set if app is a screen saver
#define QF_FINDEXE   0x00000020  // 981024: use FindExecutable
#define QF_SAVEUSER  0x00000040  // 000522: save new resolution for
                                 //         current user
#define QF_SAVEALL   0x00000080  // 000522: save new resolution for
                                 //         all users
#define QF_HIDEQR    0x00000100  // 020330: hide quickres after use

// QuickRes screen mode data
typedef struct {
  DWORD dwXRes, dwYRes;  // screen dimensions in pixels
  UINT  wBitsPixel;      // color depth in bits per pixel
  UINT  wFreq;           // refresh rate in hertz
  DWORD  dwFlags;    // flags, see QF_* constants
} QRMODE;

// Structure holding parsed command line parameters
typedef struct {
  QRMODE mNew;       // requested screen mode
  QRMODE mOld;       // old screen mode
  DWORD  dwFlags;    // flags, see QF_* constants
  UINT   wQRDelay;   // QuickRes delay in 1/10 sec
  UINT   wAppDelay;  // Application delay in 1/10 sec
  char   szApp[512]; // requested application
  char   szErr[80];  // set if unknown parameter encountered
} QRES_PARS;

// Structure holding QuickRes app info
typedef struct {
  HWND  hwnd;                // Main app window
  DWORD dwProcess, dwThread; // Process and main thread id
  UINT  wFirstMenuID;        // First menu option ID
  BOOL  fOSR2;               // TRUE for OSR2 built-in version
  BOOL  fDirect;             // TRUE for direct switch
} QUICKRES;

// Windows version
#define WVER_UNSUPPORTED   0
#define WVER_95            1
#define WVER_OSR2          2
#define WVER_NT            3
#define WVER_98            4
#define WVER_ME            5
#define WVER_UNKNOWN      99


// GetWinVer returns one of the WVER_... constants to
// indicate the detected Windows version
int GetWinVer(void);


// GetCurQResMode returns the current QuickRes settings for the 
// screen in *ptMode.
//
void GetCurQResMode(QRMODE *ptMode);


// GetQResPars parses the command line passed to qres.
// Return value is the number of distinct parameters found.
// The function returns -1 and sets ptParsRet->cErrPar if an 
// unknown parameter is encountered.
//
int GetQResPars(LPSTR lpszCmdLine,      // Command line string
                QRES_PARS *ptParsRet ); // Returns parameters


// CompleteQResPars fills out parameters not specified by the user
// based on the current screen mode. It also initialises ptParsRet->mOld
// and sets the QF_NOSWITCH flag if no mode switch is required.
void CompleteQResPars(QRES_PARS *ptParsRet);


// FindQuickResApp looks for the Microsoft QuickRes applet
// and returns its main window and other required data in 
// *ptQRes. If the applet is found the return value is TRUE,
// otherwise FALSE.
//
BOOL FindQuickResApp(QUICKRES *ptQRes);


// IsQResDlgOpen returns TRUE if the specified dialog window
// is open and belongs to the same process as the quickres
// application.
//
BOOL IsQResDlgOpen(QUICKRES *ptQRes, char *szDlgName);


// FindReqQResMode returns >= 0 if the requested mode is supported
// by the installed graphics driver, -1 if the mode is not found.
// The return value must be added to the menu base to get the
// quickres menu option ID for the requested mode.
//
int FindReqQResMode(QRMODE *ptReqMode, BOOL fOSR2);


// SwitchQResMode calls FindReqMode to find requested mode, 
// then requests mode switch from QuickRes
BOOL SwitchQResMode(QRMODE *ptReqMode, QUICKRES *ptQRes);

// QResMessageBoxCallback will show qres help when called
// from MessageBoxIndirect
VOID CALLBACK QResMessageBoxCallback(LPHELPINFO lpHelpInfo);

// CheckQuickRes checks and reports on QuickRes installation
// Return value is TRUE if everything is OK.
// If bShowIfOK is FALSE no messagebox is shown if everything 
// is OK.
BOOL CheckQuickRes(BOOL bShowIfOK);

void SetRegValue(HKEY hKey, LPCTSTR lpSubKey, 
				 LPCTSTR lpValueName, LPCTSTR lpValue);


////////////////////////////////////////////////////////////
// qres32.dll functionality

// Constants
#define  QRES_MESSAGE  (WM_USER + 0x5152) // msg sent to explorer
#define  QRES_SAVE_DESKTOP         0x5152 // wParam: save icon pos
#define  QRES_RESTORE_DESKTOP      0x7172 // wParam: restore icon pos
#define  QRES_HIDE_MOUSE           0x5153 // wParam: hide mouse cursor
#define  QRES_SHOW_MOUSE           0x5154 // wParam: redisplay mouse cursor


// InstallQResFilter hooks/unhooks a message filter into the 
// Windows Explorer main thread. Never forget to unhook the filter
// before closing down your app. If the filter remains active the
// Windows shut down does not function properly.
// 
// fInstall - TRUE:  Install message filter
//            FALSE: Remove message filter
//
// Return value is TRUE on success 
//  
BOOL __stdcall InstallQResFilter( BOOL fInstall );


// QResMsgWnd returns the window handle for the Explorer main window.
// Use this handle as the target to post the QRes message to.
//
// Note: do not keep the handle locally, but call QResMsgWnd just
//       before each PostMessage call.
//
HWND __stdcall QResMsgWnd(void);

#ifdef __cplusplus
}
#endif 
#endif /* #ifndef QRESLIB_H */

