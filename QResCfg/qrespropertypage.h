// QResPropertyPage.h : header file
//

#ifndef __QRESPROPERTYPAGE_H__
#define __QRESPROPERTYPAGE_H__


#include <shlobj.h>   // for shell stuff
//#include <afxtempl.h> // template support
// #include <afxcoll.h>  // collection support
#include "shortcut.h" // CShortcut data type
#include "colormode.h" // CColorMode, CColorModeList


// STRUCTURES
typedef struct tagLVID
{
   LPSHELLFOLDER lpsfParent;
   LPITEMIDLIST  lpi;
   ULONG         ulAttribs;
} LVITEMDATA, *LPLVITEMDATA;

typedef struct tagID
{
   LPSHELLFOLDER lpsfParent;
   LPITEMIDLIST  lpi;
   LPITEMIDLIST  lpifq;
} TVITEMDATA, *LPTVITEMDATA;

/////////////////////////////////////////////////////////////////////////////
// CQResPropertyPage1 dialog

class CQResPropertyPage1 : public CPropertyPage
{
// DECLARE_DYNCREATE(CQResPropertyPage1) :removed to reduce size

// Construction
public:
	void EnableButton(UINT wId, BOOL fEnable);
	CQResPropertyPage1();
	~CQResPropertyPage1();

// Properties
	int  m_iAction;
	int  m_iLinkType;       // Link type

// Dialog Data
	//{{AFX_DATA(CQResPropertyPage1)
	enum { IDD = IDD_PROPPAGE1 };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CQResPropertyPage1)
	public:
	virtual LRESULT OnWizardNext();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CQResPropertyPage1)
	afx_msg void OnDesktop();
	afx_msg void OnNewlink();
	afx_msg void OnStartmenu();
	afx_msg void OnAdd();
	afx_msg void OnRem();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void SetBtnText(int iCtrl, int iString);
};


/////////////////////////////////////////////////////////////////////////////
// CQResPropertyPage2 dialog

class CQResPropertyPage2 : public CPropertyPage
{
// DECLARE_DYNCREATE(CQResPropertyPage2)

// Construction
public:
	void AddToFileList(LPSHELLFOLDER lpsf, LPITEMIDLIST lpi);
	void GetSel(void);
	int m_iRootFolder;
	CQResPropertyPage2();
	~CQResPropertyPage2();

// Properties
	int  m_iAction;         // Action type (from page 1)
	int  m_iLinkType;       // Link type (from page 1)
    char m_szLink[MAX_PATH];// fully qualified path to link or program

// Dialog Data
	//{{AFX_DATA(CQResPropertyPage2)
	enum { IDD = IDD_PROPPAGE2 };
	CListBox	m_CList2;
	CListBox	m_CList;
	CTreeCtrl	m_CDirTree;
	//}}AFX_DATA
    
// Implementation
private:
    CShortcut m_Shortcut;

// Operations
public:
 	static int CALLBACK TreeViewCompareProc(LPARAM, LPARAM, LPARAM);

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CQResPropertyPage2)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CQResPropertyPage2)
	afx_msg void OnItemexpandingDirtree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteitemDirtree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedDirtree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelcancelList1();
	afx_msg void OnSelchangeList1();
	afx_msg void OnDblclkList1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    void OnFill(int iFolder);
    void FillTreeView(LPSHELLFOLDER lpsf, LPITEMIDLIST  lpifq, HTREEITEM hParent);
    int GetIcon(LPITEMIDLIST lpi, UINT uFlags);
    void GetNormalAndSelectedIcons(LPITEMIDLIST lpifq, LPTV_ITEM lptvitem);
    LRESULT NotifyHandler(UINT message, WPARAM wParam, LPARAM lParam); 
    BOOL PopulateListView(LPTVITEMDATA lptvid, LPSHELLFOLDER lpsf);
    BOOL InitListViewItems(LPTVITEMDATA lptvid, LPSHELLFOLDER lpsf);

    // Functions that deal with PIDLs
    LPITEMIDLIST ConcatPidls(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2);
    LPITEMIDLIST GetFullyQualPidl(LPSHELLFOLDER lpsf, LPITEMIDLIST lpi);
    LPITEMIDLIST CopyITEMID(LPMALLOC lpMalloc, LPITEMIDLIST lpi);
    BOOL GetName(LPSHELLFOLDER lpsf, LPITEMIDLIST  lpi, DWORD dwFlags, 
                 LPSTR lpFriendlyName, UINT cbMaxLen);
    LPITEMIDLIST PIDLCreate(UINT cbSize);
    UINT GetSize(LPCITEMIDLIST pidl);
    LPITEMIDLIST Next(LPCITEMIDLIST pidl);

private:
	void SetBtnText(int iCtrl, int iString);
	BOOL HideFolder(LPITEMIDLIST lpi, ULONG ulAttrs);
	void InitTreeView();
};


/////////////////////////////////////////////////////////////////////////////
// CQResPropertyPage3 dialog

class CQResPropertyPage3 : public CPropertyPage
{
// DECLARE_DYNCREATE(CQResPropertyPage3)

// Construction
public:
	void SetScreenArea(int iXRes);
	void SetColorMode(int iColorMode);
	void EnableButton(UINT wId, BOOL fEnable);
	CQResPropertyPage3();
	~CQResPropertyPage3();

// Properties
    int  m_iXRes;           // Horizontal resolution 
    int  m_iColors;         // Color depth (bits per pixel)
// Dialog Data
	//{{AFX_DATA(CQResPropertyPage3)
	enum { IDD = IDD_PROPPAGE3 };
	CButton	m_CNoArea;
	CButton	m_CNoColor;
	CComboBox	m_CColor;
	CStatic	m_CAreaTxt;
	CSliderCtrl	m_CArea;
	BOOL	m_fNoArea;
	BOOL	m_fNoColor;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CQResPropertyPage3)
	public:
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
    CColorModeList m_ColorModeList; // list of color modes

	// Generated message map functions
	//{{AFX_MSG(CQResPropertyPage3)
	afx_msg void OnSelchangeColor();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnNoarea();
	afx_msg void OnNocolor();
	afx_msg void OnAdvanced();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void InitColorDlg(void);
	void CreateColorList(void);
    void DestroyColorList(void);
};


/////////////////////////////////////////////////////////////////////////////
// CQResPropertyPage4 dialog

class CQResPropertyPage4 : public CPropertyPage
{
// DECLARE_DYNCREATE(CQResPropertyPage4)

// Construction
public:
	void EnableButton(UINT wId, BOOL fEnable);
	CQResPropertyPage4();
	~CQResPropertyPage4();

 // Properties
	int  m_iAction;         // Action type                             (page 1)
	int  m_iLinkType;       // Link type                               (page 1)
    char m_szLink[MAX_PATH];// fully qualified path to link or program (page 2)
    QRES_PARS m_QResPars;   // QRes parameters
    int  m_iXRes;           // Horizontal resolution                   (page 3)
    int  m_iColors;         // Color depth (bits per pixel)            (page 3)
    BOOL m_fRestore;        // Restore mode flag                       (page 3)
    BOOL m_fDirect;         // Direct switch flag                      (page 3)

// Dialog Data
	//{{AFX_DATA(CQResPropertyPage4)
	enum { IDD = IDD_PROPPAGE4 };
	CEdit	m_CSummary;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CQResPropertyPage4)
	public:
	virtual BOOL OnWizardFinish();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	virtual LRESULT OnWizardBack();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CQResPropertyPage4)
	afx_msg void OnTest();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void SetBtnText(int iCtrl, int iString);
	void ShowSummary();
};



#endif // __QRESPROPERTYPAGE_H__
