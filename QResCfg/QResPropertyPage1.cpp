// QResPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "QResCfg.h"
#include "QResPropertyPage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// IMPLEMENT_DYNCREATE(CQResPropertyPage1, CPropertyPage)


/////////////////////////////////////////////////////////////////////////////
// CQResPropertyPage1 property page

CQResPropertyPage1::CQResPropertyPage1() : CPropertyPage(CQResPropertyPage1::IDD)
{
	//{{AFX_DATA_INIT(CQResPropertyPage1)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CQResPropertyPage1::~CQResPropertyPage1()
{
}

void CQResPropertyPage1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQResPropertyPage1)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQResPropertyPage1, CPropertyPage)
	//{{AFX_MSG_MAP(CQResPropertyPage1)
	ON_BN_CLICKED(IDC_DESKTOP, OnDesktop)
	ON_BN_CLICKED(IDC_NEWLINK, OnNewlink)
	ON_BN_CLICKED(IDC_STARTMENU, OnStartmenu)
	ON_BN_CLICKED(IDC_ADDQRES, OnAdd)
	ON_BN_CLICKED(IDC_REMQRES, OnRem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


///////////////////////////////////////////////////////////
// Wizard interface functions

///////////////////////////////////////////////////////////
// Page 1

BOOL CQResPropertyPage1::OnSetActive() 
{
    // set special display features
    theApp.m_pSheet->EnableButton(ID_WIZBACK, FALSE);

    // read properties
    m_iAction   = theApp.m_pSheet->m_iAction;
    m_iLinkType = theApp.m_pSheet->m_iLinkType;

    // display properties
    CButton * pButton = (CButton *)GetDlgItem(m_iLinkType);
    if (pButton)
      pButton->SetCheck(TRUE);
    pButton = (CButton *)GetDlgItem(m_iAction);
    if (pButton)
      pButton->SetCheck(TRUE);

    return CPropertyPage::OnSetActive();
}

BOOL CQResPropertyPage1::OnKillActive() 
{
    // write back properties
    theApp.m_pSheet->m_iAction   = m_iAction;
    theApp.m_pSheet->m_iLinkType = m_iLinkType;

    // reset special display features
    theApp.m_pSheet->EnableButton(ID_WIZBACK, TRUE);

    return CPropertyPage::OnKillActive();
}

LRESULT CQResPropertyPage1::OnWizardNext() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnWizardNext();
}

void CQResPropertyPage1::OnDesktop() 
{
  m_iLinkType = IDC_DESKTOP;	
}

void CQResPropertyPage1::OnNewlink() 
{
  m_iLinkType = IDC_NEWLINK;	
	
}

void CQResPropertyPage1::OnStartmenu() 
{
  m_iLinkType = IDC_STARTMENU;	
}

void CQResPropertyPage1::OnAdd() 
{
  m_iAction = IDC_ADDQRES;	

  SetBtnText(IDC_STARTMENU, IDS_STARTMENU);
  SetBtnText(IDC_DESKTOP, IDS_DESKTOP);
  EnableButton(IDC_NEWLINK, TRUE);
}

void CQResPropertyPage1::OnRem() 
{
  m_iAction = IDC_REMQRES;	

  if (m_iLinkType == IDC_NEWLINK)
  {
    CButton * pButton;

    m_iLinkType = IDC_STARTMENU;	
    pButton = (CButton *)GetDlgItem(IDC_STARTMENU);
    if (pButton)
      pButton->SetCheck(TRUE);
    pButton = (CButton *)GetDlgItem(IDC_NEWLINK);
    if (pButton)
      pButton->SetCheck(FALSE);
  }

  SetBtnText(IDC_STARTMENU, IDS_REM_STARTMENU);
  SetBtnText(IDC_DESKTOP, IDS_REM_DESKTOP);
  EnableButton(IDC_NEWLINK, FALSE);
}

void CQResPropertyPage1::SetBtnText(int iCtrl, int iString)
{
  CString s;
  CWnd* pBtn = GetDlgItem(iCtrl);

  if (pBtn && s.LoadString(iString))
    pBtn->SetWindowText(s);
}

void CQResPropertyPage1::EnableButton(UINT wId, BOOL fEnable)
{
  CWnd* pBtn = GetDlgItem(wId);
  
  if (pBtn)
    pBtn->EnableWindow(fEnable);
}
