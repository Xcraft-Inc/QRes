// QResPropertySheet.h : header file
//
// This class defines custom modal property sheet 
// CQResPropertySheet.
 
#ifndef __QRESPROPERTYSHEET_H__
#define __QRESPROPERTYSHEET_H__

#include "QResPropertyPage.h"

/////////////////////////////////////////////////////////////////////////////
// CQResPropertySheet

class CQResPropertySheet : public CPropertySheet
{
//  DECLARE_DYNAMIC(CQResPropertySheet) :removed to reduce size

// Construction
public:
  void EnableButton ( UINT wId, BOOL fEnable);
  void HideButton(UINT wId, BOOL fHide);
  void ShowButton(UINT wId, BOOL fShow);
  CQResPropertySheet(CWnd* pParentWnd = NULL);

// Attributes
public:
  CQResPropertyPage1 m_Page1;
  CQResPropertyPage2 m_Page2;
  CQResPropertyPage3 m_Page3;
  CQResPropertyPage4 m_Page4;

// Properties
  int       m_iAction;         // Action type                  (page 1)
  int       m_iLinkType;       // Link type                    (page 1)
  char      m_szLink[MAX_PATH];// Path to link or program      (page 2)
  CShortcut m_Shortcut;        // Shortcut

// Operations
public:

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CQResPropertySheet)
	public:
    virtual BOOL OnInitDialog();
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
  virtual ~CQResPropertySheet();

private:
  BOOL MoveButton(UINT wId, CSize shift);
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __QRESPROPERTYSHEET_H__
