//
//  MODULE:   qresprop.cpp
//
//  PURPOSE:   Implements the IShellPropSheetExt member functions necessary to 
//             support the property sheet page of this shell extension.  This
//             page is displayed when the user selects "Properties..." after
//             right clicking on a .lnk file
//
// Revision History:                                                 
//-------------------------------------------------------------------
//YYMMDD| Author         | Reason                        | Version   
//------+----------------+-------------------------------+-----------
//970315| B.Engelbrecht  | Corrrect writing /R flag      | 1.0.6.0
//050109| B.Engelbrecht  | Modifications for Borland C++ | 1.0.9.7
//      |                |                               |
//------+----------------+-------------------------------+-----------
//
// Copyright 1998-2005 Berend Engelbrecht. All rights reserved.
#if defined(__BORLANDC__)
#pragma warn -8057 // suppress unused parameter warnings in BCPP 5.0
#pragma warn -8084 // suppress warning for MAKELONG in BCPP 5.0
#endif

#include "priv.h"
#include "qresext.h"
#include "resource.h"
#include <windowsx.h>


extern UINT g_cRefThisDll;      // Reference count of this DLL.
extern HINSTANCE g_hmodThisDll; // Handle to this DLL itself.

#define SET_CHANGED  if (m_fValid) PropSheet_Changed(m_hwndSheet = GetParent(m_hDlg), m_hDlg)

//
//  FUNCTION: QResPageDlgProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Callback dialog procedure for the property page
//
//  PARAMETERS:
//    hDlg      - Dialog box window handle
//    uMessage  - current message
//    wParam    - depends on message
//    lParam    - depends on message
//
//  RETURN VALUE:
//
//    Depends on message.  In general, return TRUE if we process it.
//
//  COMMENTS:
//

BOOL CALLBACK QResPageDlgProc(HWND hDlg, 
                              UINT uMessage, 
                              WPARAM wParam, 
                              LPARAM lParam)
{
    LPPROPSHEETPAGE psp=(LPPROPSHEETPAGE)GetWindowLong(hDlg, DWL_USER);
    LPCQRESEXT lpcs;

    switch (uMessage)
    {
        //
        // When the shell creates a dialog box for a property sheet page,
        // it passes the pointer to the PROPSHEETPAGE data structure as
        // lParam. The dialog procedures of extensions typically store it
        // in the DWL_USER of the dialog box window.
        //
        case WM_INITDIALOG:
            SetWindowLong(hDlg, DWL_USER, lParam);
            psp=(LPPROPSHEETPAGE)lParam;
            lpcs = (LPCQRESEXT)psp->lParam;
            lpcs->InitDlg(hDlg);
            break;

        case WM_DESTROY:
           break;

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
              case IDC_COLOR:
                lpcs = (LPCQRESEXT)psp->lParam;
                if (HIWORD(wParam) == CBN_SELCHANGE)
                  lpcs->OnSelchangeColor();
                break;

              case IDC_NOCOLOR:
                lpcs = (LPCQRESEXT)psp->lParam;
                lpcs->OnNocolor();
                break;

              case IDC_NOAREA:
                lpcs = (LPCQRESEXT)psp->lParam;
                lpcs->OnNoarea();
                break;

              case IDC_QRES_ON:
              case IDC_QRES_OFF:
                lpcs = (LPCQRESEXT)psp->lParam;
                lpcs->EnableQRes(SendDlgItemMessage(hDlg, IDC_QRES_ON, 
                                                    BM_GETCHECK, 0, 0) == BST_CHECKED);
                break;

              default:
                break;
            }
            break;

        case WM_HSCROLL:
            lpcs = (LPCQRESEXT)psp->lParam;
            lpcs->OnHScroll((UINT)LOWORD(wParam), (UINT)HIWORD(wParam));
            break;

        case WM_NOTIFY:
            switch (((NMHDR FAR *)lParam)->code)
            {
                case PSN_SETACTIVE:
                   break;

                case PSN_APPLY:
                    //User has clicked the OK or Apply button
                    lpcs = (LPCQRESEXT)psp->lParam;
                    lpcs->ApplyQRes();
                    break;
            
                default:
                    break;
            }
            break;

        case WM_HELP:
          LPHELPINFO lphi; 

          lphi = (LPHELPINFO) lParam; 
          ::WinHelp(hDlg, "qres.hlp", HELP_CONTEXTPOPUP, (DWORD)lphi->iCtrlId);

          //char       szMsg[80];

          //sprintf(szMsg, "ctrlid %d ctxtid %ld", lphi->iCtrlId, lphi->dwContextId);
          //MessageBox(NULL, szMsg, "qres", 0);
          break;

        default:
            return FALSE;
    }

    return TRUE;
}

// dialog init method
BOOL CQResExt::InitDlg(HWND hDlg)
{
  QRES_PARS tPars;

  m_fValid = FALSE;
  m_hDlg   = hDlg;

  if (!*m_szPropSheetFileUserClickedOn)
    return FALSE;

  if (m_Shortcut.Load(m_szPropSheetFileUserClickedOn))
  {
    char   szName [MAX_PATH];
    USHORT wIcon;
    HICON  hIcon;

    // Show name and icon like in other .lnk property sheets
    m_Shortcut.GetFriendlyName(szName, sizeof(szName));
    SetDlgItemText(hDlg, IDC_STDTXT, szName);
    wIcon = 0;
    hIcon = ExtractAssociatedIcon(g_hmodThisDll, 
                      m_szPropSheetFileUserClickedOn, &wIcon);
    if (hIcon)
      SendDlgItemMessage(hDlg, IDC_STDICON, STM_SETICON, 
                         (WPARAM)(HICON) hIcon, 0); 

    // Init color settings controls
    InitColorDlg();

    // Find out current color settings and display them
    if (m_Shortcut.IsQResLink())
    {
      m_fEnabled = TRUE;
      
      // get current qres settings
      m_Shortcut.GetQResPars(&tPars);
      m_iXRes = (int)tPars.mNew.dwXRes;
      m_iColors = (int)tPars.mNew.wBitsPixel;
      m_fRestore = (tPars.dwFlags & QF_RESTORE);
    }
    else
    {
      m_fEnabled = FALSE;
      
      // display default qres settings
      m_iXRes = 640;
      m_iColors = 0;
      m_fRestore = TRUE;
    }
    SetColorMode(m_iColors);
    SetScreenArea(m_iXRes);
    SetCheck(IDC_NOCOLOR, m_fNoColor);
    SetCheck(IDC_NOAREA, m_fNoArea);
    SetCheck(IDC_RESTORE, m_fRestore);
    SetCheck(m_fEnabled ? IDC_QRES_ON : IDC_QRES_OFF, TRUE);

    m_fValid = TRUE;
  }
  SetCheck(m_fValid && m_fEnabled ? IDC_QRES_ON : IDC_QRES_OFF, TRUE);
  EnableButton(IDC_QRES_ON, m_fValid);
  return m_fValid;
}

//
//  FUNCTION: QResPageCallback(HWND, UINT, LPPROPSHEETPAGE)
//
//  PURPOSE: Callback procedure for the property page.
//
//  PARAMETERS:
//    hDlg      - Dialog box window handle
//    uMessage  - current message
//    lppsp     - Pointer to property sheet page
//
//  RETURN VALUE:
//
//    Depends on message.  In general, return TRUE if we process it.
//
//  COMMENTS:  The only reason we need this is so we can be notified when
//             the dialog has been closed.
//

UINT CALLBACK QResPageCallback(HWND hDlg, UINT uMessage, LPPROPSHEETPAGE lppsp)
{
    LPCQRESEXT lpcs=(LPCQRESEXT)lppsp->lParam;

    switch (uMessage)
    {
        case PSPCB_RELEASE:
            SetWindowLong(hDlg, DWL_USER, 0L);

            //OK, the dialog has been dismissed, so we can now safely
            //release the object.
            lpcs->Release();
            break;

        default:
            break;
    }
    return TRUE;
}



//
//  FUNCTION: CQResExt::AddPages(LPFNADDPROPSHEETPAGE, LPARAM)
//
//  PURPOSE: Called by the shell just before the property sheet is displayed.
//
//  PARAMETERS:
//    lpfnAddPage -  Pointer to the Shell's AddPage function
//    lParam      -  Passed as second parameter to lpfnAddPage
//
//  RETURN VALUE:
//
//    NOERROR in all cases.  If for some reason our pages don't get added,
//    the Shell still needs to bring up the Properties... sheet.
//
//  COMMENTS:
//

STDMETHODIMP CQResExt::AddPages(LPFNADDPROPSHEETPAGE lpfnAddPage, LPARAM lParam)
{
    PROPSHEETPAGE psp;

    ODS("CQResExt::AddPages()\r\n");

    FORMATETC fmte = {CF_HDROP,
        	          (DVTARGETDEVICE FAR *)NULL,
        	          DVASPECT_CONTENT,
        	          -1,
        	          TYMED_HGLOBAL 
        	         };
    STGMEDIUM medium;
	HRESULT hres = 0;

    if (m_pDataObj)  //Paranoid check, m_pDataObj should have something by now...
       hres = m_pDataObj->GetData(&fmte, &medium);

    if (SUCCEEDED(hres))
    {
        //Find out how many files the user has selected...
        UINT cbFiles = 0;
        LPCQRESEXT lpcsext = this;

        if (medium.hGlobal)
            cbFiles = DragQueryFile((HDROP)medium.hGlobal, (UINT)-1, 0, 0);

        if (cbFiles < 2)
        {
            //OK, the user has only selected a single file, so lets go ahead
            //and add the property sheet.  Note that it doesn't HAVE to be this
            //way, but for simplicity and educational reasons, I'll only add
            //the property sheet if a single .GAK file is selected.

            //Get the name of the file the user has clicked on
			if (cbFiles)
               DragQueryFile((HDROP)medium.hGlobal, 
                             0, 
                             m_szPropSheetFileUserClickedOn,
                             sizeof(m_szPropSheetFileUserClickedOn));

			//
			// Create a property sheet page object from a dialog box.
			//
			// We store a pointer to our class in the psp.lParam, so we
            // can access our class members from within the QResPageDlgProc
			//
			// If the page needs more instance data, you can append
			// arbitrary size of data at the end of this structure,
			// and pass it to the CreatePropSheetPage. In such a case,
			// the size of entire data structure (including page specific
			// data) must be stored in the dwSize field.   Note that in
            // general you should NOT need to do this, as you can simply
            // store a pointer to date in the lParam member.
			//
        
            psp.dwSize      = sizeof(psp);	// no extra data.
            psp.dwFlags     = PSP_USEREFPARENT | PSP_USETITLE | PSP_USECALLBACK;
            psp.hInstance   = g_hmodThisDll;
            psp.pszTemplate = MAKEINTRESOURCE(IDD_QRESPAGE);
            psp.hIcon       = 0;
            psp.pszTitle    = "QRes";
            psp.pfnDlgProc  = QResPageDlgProc;
            psp.pfnCallback = QResPageCallback;
            psp.pcRefParent = &g_cRefThisDll;
            psp.lParam      = (LPARAM)lpcsext;
            
            //We need to AddRef() this object, since we are passing the "this"
            //pointer in psp.lParam which will be used in the WM_INITDIALOG above.
            //We don't want this object to go away, hence the AddRef().

            AddRef();

            m_hPage = CreatePropertySheetPage(&psp);
            m_hwndSheet = NULL;
            
            if (m_hPage) 
            {
                if (!lpfnAddPage(m_hPage, lParam))
                    DestroyPropertySheetPage(m_hPage);
            }
        }
    }

    return NOERROR;
}

//
//  FUNCTION: CQResExt::ReplacePage(UINT, LPFNADDPROPSHEETPAGE, LPARAM)
//
//  PURPOSE: Called by the shell only for Control Panel property sheet 
//           extensions
//
//  PARAMETERS:
//    uPageID         -  ID of page to be replaced
//    lpfnReplaceWith -  Pointer to the Shell's Replace function
//    lParam          -  Passed as second parameter to lpfnReplaceWith
//
//  RETURN VALUE:
//
//    E_FAIL, since we don't support this function.  It should never be
//    called.

//  COMMENTS:
//

STDMETHODIMP CQResExt::ReplacePage(UINT uPageID, 
                                    LPFNADDPROPSHEETPAGE lpfnReplaceWith, 
                                    LPARAM lParam)
{
    ODS("CQResExt::ReplacePage()\r\n");

    return E_FAIL;
}


///////////////////////////////////////////////////////////////////////
// Color dialog support ported from QResCfg


// InitColorDlg initialses color drop down list and list of allowed resolutions	
void CQResExt::InitColorDlg(void)
{
  UINT        wPos;
  int         iColor, iArea, iItem;
  char        szColor[40], *ptr;
  CColorMode *pColorMode;

  m_ColorModeList.CreateColorList();

  iColor = -1; iArea = -1;
  for( wPos = 0; wPos < m_ColorModeList.m_wCount; wPos++ )
  {
    pColorMode = m_ColorModeList.m_aMode[wPos];

    if (pColorMode->iColor > iColor)
    {
      iColor = pColorMode->iColor;
      switch(pColorMode->tMode.dmBitsPerPel)
      {
        case 8:
          strcpy(szColor, "256 colors");
          break;
        case 15:
          strcpy(szColor, "32768 colors (15 bit)");
          break;
        case 16:
          strcpy(szColor, "65536 colors (16 bit)");
          break;
        default:
          if (pColorMode->tMode.dmBitsPerPel < 15)
            ptr = "Low";
          else if (pColorMode->tMode.dmBitsPerPel < 24)
            ptr = "High";
          else
            ptr = "True";
          sprintf(szColor, "%s color (%ld bit)", ptr, pColorMode->tMode.dmBitsPerPel);
          break;
      }
      iItem = SendDlgItemMessage(m_hDlg, IDC_COLOR, CB_ADDSTRING, 0,
                                 (LPARAM)(LPCTSTR)szColor);
      if (iItem >= 0)
        SendDlgItemMessage(m_hDlg, IDC_COLOR, CB_SETITEMDATA, (WPARAM)iItem,
                           (LPARAM)pColorMode->tMode.dmBitsPerPel);
    }
    if (pColorMode->iArea > iArea)
    {
      iArea = pColorMode->iArea;
    }
  }
  SendDlgItemMessage(m_hDlg, IDC_AREA, TBM_SETRANGE, (WPARAM)0, 
                     (LPARAM)MAKELONG(0, iArea));
}

// SetColorMode sets the color mode
void CQResExt::SetColorMode(int iColorMode)
{
  int i, iCount;
  
  if (!iColorMode)
  {
    iColorMode = 8;
    m_fNoColor = TRUE;
  }
  iCount = SendDlgItemMessage(m_hDlg, IDC_COLOR, CB_GETCOUNT, 0, 0);
  for (i = 0; i < iCount; i++)
    if (iColorMode == (int)SendDlgItemMessage(m_hDlg, IDC_COLOR, 
                                              CB_GETITEMDATA, (WPARAM)i, 0))
    {
      SendDlgItemMessage(m_hDlg, IDC_COLOR, CB_SETCURSEL, (WPARAM)i, 0);
      m_iColors = iColorMode;
    }

  EnableButton(IDC_COLOR, !m_fNoColor);
}

// SetScreenArea sets the screen area
void CQResExt::SetScreenArea(int iXRes)
{
  UINT        wPos;
  CColorMode *pColorMode;
  int         iArea = -1;
  char        szText[50];

  if (!iXRes)
  {
    iXRes     = 640;
    m_fNoArea = TRUE;
  }
  for( wPos = 0; wPos < m_ColorModeList.m_wCount; wPos++ )
  {
    pColorMode = m_ColorModeList.m_aMode[wPos];
    if (pColorMode->iArea > iArea)
    {
      iArea = pColorMode->iArea;
      if (pColorMode->tMode.dmPelsWidth == (DWORD)iXRes)
      {
        sprintf(szText, "%ld by %ld pixels", 
                pColorMode->tMode.dmPelsWidth,
                pColorMode->tMode.dmPelsHeight);
        SetDlgItemText(m_hDlg, IDC_AREA_TXT, szText);
        SendDlgItemMessage(m_hDlg, IDC_AREA, TBM_SETPOS,  
                           (WPARAM)TRUE, (LPARAM)(LONG)iArea);
        m_iXRes = iXRes;
        break;
      }
    }
  }

  EnableButton(IDC_AREA, !m_fNoArea);
  EnableButton(IDC_AREA_TXT, !m_fNoArea);
  EnableButton(IDC_LESS, !m_fNoArea);
  EnableButton(IDC_MORE, !m_fNoArea);
}

// fully enable or disable QRes
void CQResExt::EnableQRes(BOOL fEnable)
{
  m_fEnabled = fEnable;

  EnableButton(IDC_NOCOLOR, m_fEnabled);
  EnableButton(IDC_NOAREA, m_fEnabled);
  EnableButton(IDC_RESTORE, m_fEnabled);
  EnableButton(IDC_COLORBOX, m_fEnabled);
  EnableButton(IDC_AREABOX, m_fEnabled);
  EnableButton(IDC_AREA, m_fEnabled && !m_fNoArea);
  EnableButton(IDC_AREA_TXT, m_fEnabled && !m_fNoArea);
  EnableButton(IDC_LESS, m_fEnabled && !m_fNoArea);
  EnableButton(IDC_MORE, m_fEnabled && !m_fNoArea);
  EnableButton(IDC_COLOR, m_fEnabled && !m_fNoColor);
  SET_CHANGED;
}

// ApplyQRes handles Apply/OK
void CQResExt::ApplyQRes()
{
  HWND   hwnd, hwndNext;

  if (!m_fValid || !m_Shortcut.Load(m_szPropSheetFileUserClickedOn))
    return;

  if (m_fEnabled)
  {
    QRES_PARS tPars;

    m_Shortcut.GetQResPars(&tPars);
    tPars.mNew.wBitsPixel = (UINT)(m_fNoColor ? 0 : m_iColors);
    tPars.mNew.dwXRes     = (DWORD)(m_fNoArea ? 0 : m_iXRes);
    tPars.mNew.dwYRes     = 0;
    m_fRestore = GetCheck(IDC_RESTORE); // 970315
    if (m_fRestore)
      tPars.dwFlags |= QF_RESTORE;
    else // BE 970315
	  tPars.dwFlags &= ~QF_RESTORE;
    m_Shortcut.SetQResPars(&tPars);
  }
  else
  {
    // Remove QRes from existing link
    m_Shortcut.RemQRes();
  }

  // Trick shortcut tab into thinking it has to
  // initialize
  if (!m_hwndSheet || !PropSheet_GetCurrentPageHwnd(m_hwndSheet))
    return; // property sheet window not available

  for (hwndNext = m_hDlg; hwndNext; 
       hwnd = hwndNext, hwndNext = GetNextWindow(hwnd, GW_HWNDNEXT));
  if (!hwnd)
    return;

  hwndNext = FindWindowEx(hwnd, NULL, "Edit", NULL);
  if (!hwndNext)
    return;
  hwndNext = FindWindowEx(hwnd, hwndNext, "Edit", NULL);
  if (!hwndNext)
    return;

  char szText[MAX_PATH+256];
  m_Shortcut.GetPath(szText, sizeof(szText));

  SetWindowText(hwndNext, szText);
}

void CQResExt::EnableButton(UINT wId, BOOL fEnable)
{
  HWND hBtn = GetDlgItem(m_hDlg, wId);
  
  if (hBtn)
    EnableWindow(hBtn, fEnable);
}

void CQResExt::SetCheck(UINT wId, BOOL fCheck)
{
  SendDlgItemMessage(m_hDlg, wId, BM_SETCHECK, (WPARAM)fCheck, 0);
}

BOOL CQResExt::GetCheck(UINT wId)
{
  return (SendDlgItemMessage(m_hDlg, wId, BM_GETCHECK, 0, 0) != 0);
}

//////////////////////////////////////////////////////////////////
// Event handlers

void CQResExt::OnSelchangeColor() 
{
  int         iSel, iColors;
  UINT        wPos;
  CColorMode *pColorMode;
  BOOL        fOk = FALSE;

  iSel = (int) SendDlgItemMessage(m_hDlg, IDC_COLOR, CB_GETCURSEL, 0, 0);
  if (iSel >= 0)
  {
    iColors = (int)SendDlgItemMessage(m_hDlg, IDC_COLOR, 
                                      CB_GETITEMDATA, (WPARAM)iSel, 0);
    if (iColors != m_iColors)
    {
      m_iColors = iColors;

      // Find out if area setting is still OK for these colors
      for( wPos = 0; wPos < m_ColorModeList.m_wCount; wPos++ )
      {
        pColorMode = m_ColorModeList.m_aMode[wPos];
        if ((pColorMode->tMode.dmBitsPerPel == (DWORD)m_iColors) &&
            (pColorMode->tMode.dmPelsWidth == (DWORD)m_iXRes))
        {
          fOk = TRUE;
          break;
        }
      }
      if (!fOk)
      {
        // find highest screen mode that supports this color mode
        for( wPos = m_ColorModeList.m_wCount; wPos > 0; wPos--)
        {
          pColorMode = m_ColorModeList.m_aMode[wPos-1];
          if (pColorMode->tMode.dmBitsPerPel == (DWORD)m_iColors)
          {
            SetScreenArea(pColorMode->tMode.dmPelsWidth);
            break;
          }
        }
      }
      SET_CHANGED;
    }
  }
}

void CQResExt::OnHScroll(UINT nSBCode, UINT nPos) 
{
  UINT        wPos;
  CColorMode *pColorMode;
  int         iPos;
  char        szText[50];
  BOOL        fOk = FALSE;

  iPos = (int)SendDlgItemMessage(m_hDlg, IDC_AREA, TBM_GETPOS, 0, 0);
  for( wPos = 0; wPos < m_ColorModeList.m_wCount; wPos++ )
  {
    pColorMode = m_ColorModeList.m_aMode[wPos];
    if (((int)pColorMode->tMode.dmBitsPerPel == m_iColors) &&
        (pColorMode->iArea == iPos))
    {
      sprintf(szText, "%ld by %ld pixels", 
              pColorMode->tMode.dmPelsWidth,
              pColorMode->tMode.dmPelsHeight);
      SetDlgItemText(m_hDlg, IDC_AREA_TXT, szText);
      m_iXRes = pColorMode->tMode.dmPelsWidth;
      fOk = TRUE;
      break;
    }
  }
  if (!fOk)
  {
    // search for the highest screen mode that supports this screen resolution
    for( wPos = m_ColorModeList.m_wCount; wPos > 0; wPos--)
    {
      pColorMode = m_ColorModeList.m_aMode[wPos-1];
      if (pColorMode->iArea == iPos)
      {
        sprintf(szText, "%ld by %ld pixels", 
                pColorMode->tMode.dmPelsWidth,
                pColorMode->tMode.dmPelsHeight);
        SetDlgItemText(m_hDlg, IDC_AREA_TXT, szText);
        m_iXRes = pColorMode->tMode.dmPelsWidth;
        SetColorMode(pColorMode->tMode.dmBitsPerPel);
        break;
      }
    }
  }
  SET_CHANGED;
}


void CQResExt::OnNocolor() 
{
  m_fNoColor = !m_fNoColor;
  SetColorMode(m_iColors ? m_iColors : 8);
  SET_CHANGED;

  // Not allowed to have both 'No Color' and 'No Area' checked
  if (m_fNoColor && m_fNoArea)
    PostMessage(GetDlgItem(m_hDlg, IDC_NOAREA), BM_CLICK, 0, 0);
}


void CQResExt::OnNoarea() 
{
  m_fNoArea = !m_fNoArea;
  SetScreenArea(m_iXRes ? m_iXRes : 640);
  SET_CHANGED;

  // Not allowed to have both 'No Color' and 'No Area' checked
  if (m_fNoColor && m_fNoArea)
    PostMessage(GetDlgItem(m_hDlg, IDC_NOCOLOR), BM_CLICK, 0, 0);
}
