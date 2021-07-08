/*********************************************************************/
/* qres32.c - qres32.dll main source module                          */
/*                                                                   */
/* To be able to save and restore desktop icon positions qres32.dll  */
/* is hooked into the Windows Explorer main thread. This makes the   */
/* DLL run in the address space of the Windows Explorer, allowing it */
/* to access Explorer listview data.                                 */
/*                                                                   */
/* Since all listview interaction is message-based it may seem that  */
/* we could control the listview directly from qres.exe. This does   */
/* not work however: any attempt to access control data from a       */
/* different task causes a GP fault in Win32, immediately crashing   */
/* the Explorer.                                                     */
/*                                                                   */
/* Author: Berend Engelbrecht                                        */
/*                                                                   */
/* Revision History:                                                 */
/*-------------------------------------------------------------------*/
/*YYMMDD| Author         | Reason                        | Version   */
/*------+----------------+-------------------------------+-----------*/
/*970122| B.Engelbrecht  | Created                       | 0.3.0.0   */
/*970405| B.Engelbrecht  | Added InstallShield extension | 1.0.7.0   */
/*050109| B.Engelbrecht  | Modifications for Borland C++ | 1.0.9.7   */
/*      |                |                               |           */
/*                                                                   */
/* Copyright 1997-2005 Berend Engelbrecht. All rights reserved.      */
/*********************************************************************/
#if defined(__BORLANDC__)
#pragma warn -8057 // suppress unused parameter warnings in BCPP 5.0
#pragma warn -8080 // suppress warnings for _str functions in BCPP 6.0
#pragma warn -8084 // suppress warning for MAKELONG in BCPP 5.0
#endif

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <string.h>
#include <qreslib.h>

/////////////////////////////////////////////////////////////////////
// Constants

#define MAX_ICONS 256


/////////////////////////////////////////////////////////////////////
// Local typedefs

typedef struct
{
  DWORD dwChecksum;
  POINT pos;
} ICON_INFO;

typedef struct
{
  UINT      wCount;
  BOOL      fSaved;
  ICON_INFO aIcon[MAX_ICONS];
} ICON_LIST;

/////////////////////////////////////////////////////////////////////
// Global variables

static HHOOK     hHk;
static UINT      nHk = 0;
static HINSTANCE hDllInstance = (HINSTANCE)0;
static ICON_LIST tList = {0, FALSE};


// Returns handle to Explorer desktop listview control
static HWND ExplorerListview(void)
{
  HWND hwnd = QResMsgWnd();

  if (hwnd)
    hwnd = FindWindowEx(hwnd, NULL, "SHELLDLL_DefView", NULL);
  if (hwnd)
    hwnd = FindWindowEx(hwnd, NULL, "SysListView32", NULL);

  return hwnd;
}


// Returns Explorer main thread id
static DWORD ExplorerThread(void)
{
  HWND hwnd = QResMsgWnd();

  if (!hwnd)
    return 0;

  return GetWindowThreadProcessId(hwnd, NULL);
}


// GetDesktopIconInfo returns desktop icon info in an ICON_INFO structure
static BOOL GetDesktopIconInfo(HWND hwndLV, int iIcon, ICON_INFO *pIconRet)
{
  char szText[MAX_PATH];
  int  i, iLen;

  // Get icon position
  if (!ListView_GetItemPosition(hwndLV, iIcon, &pIconRet->pos))
    return FALSE;

  // Get icon name and calculate checksum
  // (ListView_GetItemText does not have an error return)
  ListView_GetItemText(hwndLV, iIcon, 0, szText, sizeof(szText));
  pIconRet->dwChecksum = 0;
  iLen = strlen(szText);
  for (i = 0; i < iLen; i++)
    pIconRet->dwChecksum += (UINT)szText[i];
  return TRUE;
}


// RestoreDesktopIconPos checks current info for the specified desktop icon and
// compares to saved info. If the icon is still valid and the position has
// changed RestoreIcon attempts to change it back.
static BOOL RestoreDesktopIconPos(HWND hwndLV, int iIcon, ICON_INFO *pIconSaved)
{
  ICON_INFO tIconCurr;

  if (!GetDesktopIconInfo(hwndLV, iIcon, &tIconCurr))
    return FALSE;

  if (tIconCurr.dwChecksum != pIconSaved->dwChecksum)
    return FALSE;

  if (POINTTOPOINTS(tIconCurr.pos) != POINTTOPOINTS(pIconSaved->pos))
  {
    // restore icon position
    ListView_SetItemPosition32(hwndLV, iIcon, 
                               pIconSaved->pos.x, pIconSaved->pos.y);
  }
  return TRUE;
}


// SaveIcons saves icon positions in global struct tList
static void SaveIcons(HWND hwndLV)
{
  int i, iCount;
  
  tList.fSaved = FALSE;
  iCount = ListView_GetItemCount(hwndLV);
  if (iCount > MAX_ICONS)
    iCount = MAX_ICONS;

  tList.wCount = 0;
  for (i = 0; i < iCount; i++)
  {
    if (!GetDesktopIconInfo(hwndLV, i, &tList.aIcon[i]))
      break;
    tList.wCount++;
  }
  tList.fSaved = TRUE;
}


// RestoreIcons restores icon positions from global struct tList
static void RestoreIcons(HWND hwndLV)
{
  int i;

  for (i = 0; i < (int)tList.wCount; i++)
    RestoreDesktopIconPos(hwndLV, i, &tList.aIcon[i]);
}


// Hook procedure that is injected into the Explorer
//
LRESULT CALLBACK QResMsgProc(
    int code,       // hook code
    WPARAM wParam,  // removal flag
    LPARAM lParam   // address of structure with message
)
{
  MSG *pMsg = (MSG *)lParam;

  if (pMsg->message == QRES_MESSAGE)
  {
    switch (pMsg->wParam)
    {
      case QRES_SAVE_DESKTOP:
        SaveIcons(ExplorerListview());
        break;

      case QRES_RESTORE_DESKTOP:
        RestoreIcons(ExplorerListview());
        break;
      
      case QRES_HIDE_MOUSE:
        while(ShowCursor(FALSE) >= 0);
        break;

      case QRES_SHOW_MOUSE:
        while(ShowCursor(TRUE) < 0);
        break;
    }
  }

  return CallNextHookEx(hHk, code, wParam, lParam);	
}
 

// InstallQResFilter hooks/unhooks a message filter into the 
// Windows Explorer main thread. Never forget to unhook the filter
// before closing down your app. If the filter remains active the
// Windows shut down does not function properly.
// 
// fInstall - TRUE:  Install message filter
//            FALSE: Remove message filter
//  
BOOL  __stdcall InstallQResFilter( BOOL fInstall )
{
  if (fInstall) 
  {
    if (!nHk)
    {
      hHk = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)QResMsgProc, 
                             hDllInstance, ExplorerThread());
      if (!hHk)
        return FALSE;
    }
    nHk++;
  } 
  else if (nHk && !fInstall)
  {
    nHk--;
    if (!nHk)
    {
      if (hHk && !UnhookWindowsHookEx(hHk))
        return FALSE;
      hHk = NULL;
    }
  }
  return TRUE;
}


// QResMsgWnd returns the window handle for the Explorer main window.
// Use this handle as the target to post the QRes message to.
//
// Note: do not keep the handle locally, but call QResMsgWnd just
//       before each PostMessage call.
//
HWND __stdcall QResMsgWnd(void)
{
  return FindWindowEx(NULL, NULL, "Progman", NULL);
}


// Main DLL entry point
//
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD ul_reason_for_call, 
                       LPVOID lpReserved )
{
  switch( ul_reason_for_call ) {
    case DLL_PROCESS_ATTACH:
      hDllInstance = hModule;
  }
  return TRUE;
}


// Installshield extension function
CHAR WINAPI QRCheck(HWND hwnd, LPSTR szSrcDir, 
                    LPSTR szSupport, LPSTR szInst, 
                    LPSTR szRes)
{
  CheckQuickRes(FALSE);
  return 1; // OK to continue
}

