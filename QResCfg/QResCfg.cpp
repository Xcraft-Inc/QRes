// QResCfg.cpp : Defines the class behaviors for the application.
//
// Revision History:                                                 
//-------------------------------------------------------------------
//YYMMDD| Author         | Reason                        | Version   
//------+----------------+-------------------------------+-----------
//970315| B.Engelbrecht  | Corrrect writing /R flag      | 1.0.6.0
//      |                |                               |
//------+----------------+-------------------------------+-----------
//
// Copyright 1998-2005 Berend Engelbrecht. All rights reserved.

#include "stdafx.h"
#include "QResCfg.h"
#include "QResPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQResCfgApp

BEGIN_MESSAGE_MAP(CQResCfgApp, CWinApp)
	//{{AFX_MSG_MAP(CQResCfgApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQResCfgApp construction

CQResCfgApp::CQResCfgApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CQResCfgApp object

CQResCfgApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CQResCfgApp initialization

BOOL CQResCfgApp::InitInstance()
{
    // Init Component Object Model(COM) library, used for IShellLink
    CoInitialize(NULL);

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
//	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
//	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
    m_pSheet = new CQResPropertySheet;
    m_pMainWnd = m_pSheet;

	int nResponse = m_pSheet->DoModal();
	if (nResponse ==  ID_WIZFINISH)
	{
      if (m_pSheet->m_iAction == IDC_ADDQRES)
      {
        QRES_PARS tPars;
        if (m_pSheet->m_iLinkType == IDC_NEWLINK)
        {
          // Create new link
          m_pSheet->m_Shortcut.CreateOnDesktop(m_pSheet->m_szLink);
        }
        m_pSheet->m_Shortcut.GetQResPars(&tPars);
        tPars.mNew      = theApp.m_Params.mNew;
        tPars.dwFlags   = theApp.m_Params.dwFlags;
        tPars.wQRDelay  = theApp.m_Params.wQRDelay;
        tPars.wAppDelay = theApp.m_Params.wAppDelay;
        m_pSheet->m_Shortcut.SetQResPars(&tPars);
      }
      else
      {
        // Remove QRes from existing link
        m_pSheet->m_Shortcut.RemQRes();
      }
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.

    // Cleanup
    delete m_pSheet;
    m_pSheet = NULL;
    return FALSE;
}

int CQResCfgApp::ExitInstance() 
{
	// Uninitialize COM
	CoUninitialize();
	
	return CWinApp::ExitInstance();
}

// this saves a little memory
extern "C" void _setargv() {}
extern "C" void _setenvp() {}


void CQResCfgApp::WinHelp(DWORD dwData, UINT nCmd) 
{
    ::WinHelp(m_pSheet->m_hWnd, "qres.hlp", HELP_CONTEXT, HID_PROPPAGE1 + m_pSheet->GetActiveIndex());
}
