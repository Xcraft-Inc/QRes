// AdvDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAdvDlg dialog

class CAdvDlg : public CDialog
{
// Construction
public:
	CAdvDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    QRES_PARS           m_Params; // local copy of all params
	//{{AFX_DATA(CAdvDlg)
	enum { IDD = IDD_ADVANCED };
	CButton	m_CRestore;
	CButton	m_CDirect;
	CStatic	m_CWaitTxt;
	CSpinButtonCtrl	m_CWait;
	CStatic	m_CWaitEdt;
	CStatic	m_CRefreshTxt;
	CComboBox	m_CRefresh;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdvDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAdvDlg)
	afx_msg void OnDirect();
	afx_msg void OnDeltaposWaitSpin(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
