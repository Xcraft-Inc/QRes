// QResPropertyPage.cpp : implementation file
//
// Revision History:                                                 
//-------------------------------------------------------------------
//YYMMDD| Author         | Reason                        | Version   
//------+----------------+-------------------------------+-----------
//970315| B.Engelbrecht  | Corrrect writing /R flag      | 1.0.6.0
//      |                |                               |
//------+----------------+-------------------------------+-----------

#include "stdafx.h"
#include "resource.h"
#include "QResCfg.h"
#include "QResPropertyPage.h"
#include "Shortcut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// IMPLEMENT_DYNCREATE(CQResPropertyPage4, CPropertyPage)



/////////////////////////////////////////////////////////////////////////////
// CQResPropertyPage4 property page

CQResPropertyPage4::CQResPropertyPage4() : CPropertyPage(CQResPropertyPage4::IDD)
{
	//{{AFX_DATA_INIT(CQResPropertyPage4)
	//}}AFX_DATA_INIT
}

CQResPropertyPage4::~CQResPropertyPage4()
{
}

void CQResPropertyPage4::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQResPropertyPage4)
	DDX_Control(pDX, IDC_SUMMARY, m_CSummary);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQResPropertyPage4, CPropertyPage)
	//{{AFX_MSG_MAP(CQResPropertyPage4)
	ON_BN_CLICKED(IDC_TEST, OnTest)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


///////////////////////////////////////////////////////////
// Wizard interface functions

///////////////////////////////////////////////////////////
// Page 4

BOOL CQResPropertyPage4::OnSetActive() 
{
    // set special display features
    theApp.m_pSheet->HideButton(ID_WIZNEXT, TRUE);
    theApp.m_pSheet->HideButton(ID_WIZFINISH, FALSE);
	
    // read properties
    m_iAction   = theApp.m_pSheet->m_iAction;    // page 1
    m_iLinkType = theApp.m_pSheet->m_iLinkType;  // page 1
    strcpy(m_szLink, theApp.m_pSheet->m_szLink); // page 2
	m_iColors   = theApp.m_Params.mNew.wBitsPixel; // page 3
	m_iXRes     = theApp.m_Params.mNew.dwXRes;     // page 3
    m_fRestore  = theApp.m_Params.dwFlags & QF_RESTORE; // page 3
    m_fDirect   = theApp.m_Params.dwFlags & QF_DIRECT;  // page 3
    ShowSummary();

    EnableButton(IDC_TEST, (m_iAction == IDC_ADDQRES));
    EnableButton(IDC_TEST_TXT, (m_iAction == IDC_ADDQRES));
    theApp.m_pSheet->EnableButton(ID_WIZFINISH, TRUE); // for W98 OSR1

    if (m_iLinkType == IDC_NEWLINK)
    {
      SetBtnText(IDC_FINISH_TXT, IDS_FINISH2);
      SetBtnText(IDC_TEST_TXT, IDS_TEST2);
    }
    else
    {
      SetBtnText(IDC_FINISH_TXT, IDS_FINISH1);
      SetBtnText(IDC_TEST_TXT, IDS_TEST1);
    }

    if (m_iAction == IDC_REMQRES)
    {
      // move the focus to the finish button
      PostMessage(WM_NEXTDLGCTL);
      PostMessage(WM_NEXTDLGCTL);
    }
    return CPropertyPage::OnSetActive();
}


BOOL CQResPropertyPage4::OnKillActive() 
{
    // reset special display features
    theApp.m_pSheet->HideButton(ID_WIZFINISH, TRUE);
    theApp.m_pSheet->HideButton(ID_WIZNEXT, FALSE);
	
	return CPropertyPage::OnKillActive();
}


BOOL CQResPropertyPage4::OnWizardFinish() 
{
    if (!UpdateData())  //note: parameter is TRUE by default
        return FALSE;
	return CPropertyPage::OnWizardFinish();
}

// Shows summary of selection
void CQResPropertyPage4::ShowSummary()
{
  char szSummary[512], *pLinkType = "";
  char szTemp[MAX_PATH], *ptr;
  int  iSel;
  
  *szSummary = 0;

  /* page 1 summary */
  switch(m_iLinkType)
  {
    case IDC_STARTMENU:
      pLinkType = (m_iAction == IDC_ADDQRES) ? "Modify Start Menu"
                                             : "Remove QRes from Start Menu";
      GetSpecialFolderLocation(CSIDL_STARTMENU, szTemp);
      break;
    case IDC_DESKTOP:
      pLinkType = (m_iAction == IDC_ADDQRES) ? "Modify Desktop"
                                             : "Remove QRes from Desktop";
      GetSpecialFolderLocation(CSIDL_DESKTOPDIRECTORY, szTemp);
      break;
    case IDC_NEWLINK:
      pLinkType = "Create new";
      *szTemp = 0;
      break;
  }
  sprintf(szSummary, "- Action : %s shortcut\r\n- Shortcut ", pLinkType);

  /* page 2 summary */
  // Strip off common part of link name
  if (strncmp(szTemp, m_szLink, strlen(szTemp)))
    *szTemp = 0;
  strcpy(szTemp, m_szLink+strlen(szTemp));

  // Strip off extension of link
  ptr = strrchr(szTemp, '.');
  if (ptr && !stricmp(ptr, ".lnk"))
    *ptr = 0;

  strcat(szSummary, (m_iLinkType == IDC_NEWLINK) ? "to : " : ": ");
  strcat(szSummary, szTemp);

  /* page 3 summary */
  if (m_iAction != IDC_REMQRES)
  {
    strcat(szSummary, "\r\n- Screen mode : ");

    if (m_iXRes)
    {
      theApp.m_pSheet->m_Page3.m_CAreaTxt.GetWindowText(szTemp, sizeof(szTemp));
      strcat(szSummary, szTemp);
      if (m_iColors)
        strcat(szSummary, " - ");
    }
    if (m_iColors)
    {
      iSel = theApp.m_pSheet->m_Page3.m_CColor.GetCurSel();
      if (iSel >= 0)
      {
        theApp.m_pSheet->m_Page3.m_CColor.GetLBText(iSel, szTemp);
        strcat(szSummary, szTemp);
      }
    }
    if (m_fRestore)
      strcat(szSummary, "\r\n- Restore mode after program finishes.");
    if (m_fDirect)
	  strcat(szSummary, "\r\n- Use direct screen mode switch.");

    strcat(szSummary, "\r\n-------------------------------------------\r\n"
                      "qres flags:");
    if (m_iColors)
      sprintf(szSummary+strlen(szSummary), " C=%d", m_iColors);
    if (m_iXRes)
      sprintf(szSummary+strlen(szSummary), " X=%d", m_iXRes);
    if (m_fRestore)
      strcat(szSummary, " /R");
    if (m_fDirect)
      strcat(szSummary, " /D");
  }

  m_CSummary.SetWindowText(szSummary);
}


LRESULT CQResPropertyPage4::OnWizardBack() 
{
  // skip page 3 when we are removing qres from a link
  if (m_iAction == IDC_REMQRES)
    return IDD_PROPPAGE2;
	
	
  return CPropertyPage::OnWizardBack();
}


void CQResPropertyPage4::SetBtnText(int iCtrl, int iString)
{
  CString s;
  CWnd* pBtn = GetDlgItem(iCtrl);

  if (pBtn && s.LoadString(iString))
    pBtn->SetWindowText(s);
}


void CQResPropertyPage4::EnableButton(UINT wId, BOOL fEnable)
{
  CWnd* pBtn = GetDlgItem(wId);
  
  if (pBtn)
    pBtn->EnableWindow(fEnable);
}

void CQResPropertyPage4::OnTest() 
{
  QRES_PARS tPars;
  CShortcut locShortcut;
 
  if (m_iLinkType == IDC_NEWLINK)
  {
    // Create new link
    locShortcut.CreateOnDesktop(m_szLink);
  }
  else
  {
    locShortcut.Load(m_szLink);
  }
  locShortcut.GetQResPars(&tPars);

  tPars.mNew = theApp.m_Params.mNew;
  locShortcut.TestQResPars(&tPars, m_hWnd);
}
