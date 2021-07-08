// AdvDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QResCfg.h"
#include "AdvDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdvDlg dialog


CAdvDlg::CAdvDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAdvDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAdvDlg)
	//}}AFX_DATA_INIT
}


void CAdvDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvDlg)
	DDX_Control(pDX, IDC_RESTORE, m_CRestore);
	DDX_Control(pDX, IDC_DIRECT, m_CDirect);
	DDX_Control(pDX, IDC_WAIT_TXT, m_CWaitTxt);
	DDX_Control(pDX, IDC_WAIT_SPIN, m_CWait);
	DDX_Control(pDX, IDC_WAIT_EDT, m_CWaitEdt);
	DDX_Control(pDX, IDC_REFRESH_TXT, m_CRefreshTxt);
	DDX_Control(pDX, IDC_REFRESH, m_CRefresh);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdvDlg, CDialog)
	//{{AFX_MSG_MAP(CAdvDlg)
	ON_BN_CLICKED(IDC_DIRECT, OnDirect)
	ON_NOTIFY(UDN_DELTAPOS, IDC_WAIT_SPIN, OnDeltaposWaitSpin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdvDlg message handlers

void CAdvDlg::OnDirect() 
{
	// TODO: Add your control notification handler code here
	
}

void CAdvDlg::OnDeltaposWaitSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	//NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

BOOL CAdvDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    m_CDirect.SetCheck((m_Params.dwFlags & QF_DIRECT) ? 1 : 0);
    m_CRestore.SetCheck((m_Params.dwFlags & QF_RESTORE) ? 1 : 0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAdvDlg::OnOK() 
{
	if (m_CDirect.GetCheck())
	  m_Params.dwFlags |= QF_DIRECT;
	else
	  m_Params.dwFlags &= ~QF_DIRECT;

    if (m_CRestore.GetCheck())
      m_Params.dwFlags |= QF_RESTORE;
	else
      m_Params.dwFlags &= ~QF_RESTORE;
	  
	CDialog::OnOK();
}
