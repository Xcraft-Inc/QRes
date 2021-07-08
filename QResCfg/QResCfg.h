// QResCfg.h : main header file for the QRESCFG application
//
// Copyright 1997-2005 Berend Engelbrecht. All rights reserved.

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "QResPropertySheet.h"

/////////////////////////////////////////////////////////////////////////////
// CQResCfgApp:
// See QResCfg.cpp for the implementation of this class
//

class CQResCfgApp : public CWinApp
{
public:
	CQResPropertySheet *m_pSheet;
    QRES_PARS           m_Params;
	CQResCfgApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQResCfgApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CQResCfgApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CQResCfgApp theApp;

/////////////////////////////////////////////////////////////////////////////
