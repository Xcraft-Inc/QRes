//
// The class ID of the QRes shell extension class.
//
// class id:  6fe20680-7558-11d0-b7f2-444553540000
//
//
// NOTE!!!  If you use this shell extension as a starting point, 
//          you MUST change the GUID below.  Simply run UUIDGEN.EXE
//          to generate a new GUID.
//
                                  
#define ODS(sz) OutputDebugString(sz)

#ifndef _QRESEXT_H
#define _QRESEXT_H

#include "shortcut.h"
#include "colormode.h"


DEFINE_GUID(CLSID_QResExtension, 0x6fe20680L, 0x7558, 0x11d0, 0xb7, 0xf2, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00);

// this class factory object creates context menu handlers for Chicago shell
class CQResExtClassFactory : public IClassFactory
{
protected:
	ULONG	m_cRef;

public:
	CQResExtClassFactory();
	~CQResExtClassFactory();

	//IUnknown members
	STDMETHODIMP			QueryInterface(REFIID, LPVOID FAR *);
	STDMETHODIMP_(ULONG)	AddRef();
	STDMETHODIMP_(ULONG)	Release();

	//IClassFactory members
	STDMETHODIMP		CreateInstance(LPUNKNOWN, REFIID, LPVOID FAR *);
	STDMETHODIMP		LockServer(BOOL);

};
typedef CQResExtClassFactory *LPCQRESEXTCLASSFACTORY;


// this is the actual OLE Shell context menu handler
class CQResExt : public IShellExtInit, 
                        IShellPropSheetExt
{
public:
    char         m_szPropSheetFileUserClickedOn[MAX_PATH]; 

protected:
	ULONG        m_cRef;
	LPDATAOBJECT m_pDataObj;

public:
	CQResExt();
	~CQResExt();

	//IUnknown members
	STDMETHODIMP			QueryInterface(REFIID, LPVOID FAR *);
	STDMETHODIMP_(ULONG)	AddRef();
	STDMETHODIMP_(ULONG)	Release();

	//IShellExtInit methods
	STDMETHODIMP		    Initialize(LPCITEMIDLIST pIDFolder, 
	                                   LPDATAOBJECT pDataObj, 
	                                   HKEY hKeyID);

    //IShellPropSheetExt methods
    STDMETHODIMP AddPages(LPFNADDPROPSHEETPAGE lpfnAddPage, LPARAM lParam);
    
    STDMETHODIMP ReplacePage(UINT uPageID, 
                             LPFNADDPROPSHEETPAGE lpfnReplaceWith, 
                             LPARAM lParam);

    // QRes-specific stuff
public:
    BOOL    m_fEnabled;
    int     m_iXRes;           // Horizontal resolution 
    int     m_iColors;         // Color depth (bits per pixel)
	BOOL	m_fRestore;
	BOOL	m_fNoArea;
	BOOL	m_fNoColor;
    BOOL    m_fValid;

    BOOL    InitDlg(HWND hwnd);
    void    OnSelchangeColor();
    void    OnHScroll(UINT nSBCode, UINT nPos);
    void    OnNocolor();
    void    OnNoarea();
    void    EnableQRes(BOOL fEnable);
    void    ApplyQRes();

private:
	void    InitColorDlg(void);
	void    CreateColorList(void);
    void    DestroyColorList(void);
    void    SetColorMode(int iColorMode);
    void    SetScreenArea(int iXRes);
    void    EnableButton(UINT wId, BOOL fEnable);
    void    SetCheck(UINT wId, BOOL fCheck);
    BOOL    GetCheck(UINT wId);

    HWND           m_hDlg, m_hwndSheet;
    HPROPSHEETPAGE m_hPage;
    CShortcut      m_Shortcut;
    CColorModeList m_ColorModeList; // list of color modes
};
typedef CQResExt *LPCQRESEXT;

#endif // _QRESEXT_H
