// QResPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "QResCfg.h"
#include "QResPropertySheet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQResPropertySheet

// IMPLEMENT_DYNAMIC(CQResPropertySheet, CPropertySheet)

CQResPropertySheet::CQResPropertySheet(CWnd* pWndParent)
	 : CPropertySheet(IDS_PROPSHT_CAPTION, pWndParent)
{
    // Initialise property values
    m_iAction   = IDC_ADDQRES;   // Add QRes
    m_iLinkType = IDC_STARTMENU; // Browse start menu
    *m_szLink   = 0;             // No default program
    memset(&theApp.m_Params, 0, sizeof(theApp.m_Params));
    theApp.m_Params.mNew.dwXRes = 640;  // Standard VGA resolution
    theApp.m_Params.dwFlags |= QF_RESTORE; // Restore mode after program finishes

	// Add all of the property pages here.  Note that
	// the order that they appear in here will be
	// the order they appear in on screen.  By default,
	// the first page of the set is the active one.
	// One way to make a different property page the 
	// active one is to call SetActivePage().

	AddPage(&m_Page1);
	AddPage(&m_Page2);
	AddPage(&m_Page3);
	AddPage(&m_Page4);

	SetWizardMode();
}

CQResPropertySheet::~CQResPropertySheet()
{
}


/////////////////////////////////////////////////////////////////////////////
// CQResPropertySheet message handlers


BOOL CQResPropertySheet::OnInitDialog() 
{
    BOOL bResult = CPropertySheet::OnInitDialog();
 
#ifdef NEVER
    // Customize window
    CRect rectC, rectH, tmp;
    CSize shift;
    CWnd* pCancel;
    CWnd* pHelp;
 
    // Get the position of the Cancel button
    pCancel = GetDlgItem(IDCANCEL);
    if (pCancel==NULL)
        return bResult;
    pCancel->GetWindowRect(rectC);
 
    // Get the position of the Help button
    pHelp = GetDlgItem(IDHELP);
    if (pHelp==NULL)
        return bResult;
    pHelp->GetWindowRect(rectH);

    // Hide the Help button
    pHelp->ShowWindow(SW_HIDE);

    // Calculate the distance to shift the Back, Next, Finish 
    // and Cancel buttons
    shift.cx = rectH.left-rectC.right+rectC.Width();
    shift.cy = 0;
  
    // move the buttons
    MoveButton(IDCANCEL, shift);
    MoveButton(ID_WIZFINISH, shift);
    MoveButton(ID_WIZNEXT, shift);
    MoveButton(ID_WIZBACK, shift);
#endif

    return bResult;
}

#ifdef NEVER
/////////////////////////////////////////////////////////////////////////////
// Private helper functions

BOOL CQResPropertySheet::MoveButton(UINT wId, CSize shift)
{
    CRect tmp;
    CWnd* pTemp;

    // Get the position of the button
    pTemp = GetDlgItem(wId);
    if (pTemp==NULL)
        return FALSE;
    pTemp->GetWindowRect(tmp);
 
    // move the button
    tmp += shift;
    ScreenToClient(tmp);
    pTemp->MoveWindow(tmp);
    return TRUE;
}
#endif

void CQResPropertySheet::HideButton(UINT wId, BOOL fHide)
{
  CWnd* pBtn = GetDlgItem(wId);
  
  if (pBtn)
    pBtn->ShowWindow(fHide ? SW_HIDE : SW_SHOW);
}

void CQResPropertySheet::EnableButton (UINT wId, BOOL fEnable)
{
  CWnd* pBtn = GetDlgItem(wId);
  
  if (pBtn)
    pBtn->EnableWindow(fEnable);
}

BOOL CQResPropertySheet::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    if (LOWORD(wParam) == IDHELP)
      OnHelpInfo(NULL);
	
	return CPropertySheet::OnCommand(wParam, lParam);
}
