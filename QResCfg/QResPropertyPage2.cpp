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

// IMPLEMENT_DYNCREATE(CQResPropertyPage2, CPropertyPage)


/////////////////////////////////////////////////////////////////////////////
// CQResPropertyPage2 property page

CQResPropertyPage2::CQResPropertyPage2() : CPropertyPage(CQResPropertyPage2::IDD)
{
	//{{AFX_DATA_INIT(CQResPropertyPage2)
	//}}AFX_DATA_INIT

}

CQResPropertyPage2::~CQResPropertyPage2()
{
}

void CQResPropertyPage2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQResPropertyPage2)
	DDX_Control(pDX, IDC_LIST2, m_CList2);
	DDX_Control(pDX, IDC_LIST1, m_CList);
	DDX_Control(pDX, IDC_DIRTREE, m_CDirTree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQResPropertyPage2, CPropertyPage)
	//{{AFX_MSG_MAP(CQResPropertyPage2)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_DIRTREE, OnItemexpandingDirtree)
	ON_NOTIFY(TVN_DELETEITEM, IDC_DIRTREE, OnDeleteitemDirtree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_DIRTREE, OnSelchangedDirtree)
	ON_LBN_SELCANCEL(IDC_LIST1, OnSelcancelList1)
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
	ON_LBN_DBLCLK(IDC_LIST1, OnDblclkList1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CQResPropertyPage2::OnFill(int iFolder)
{
    LPSHELLFOLDER lpsf=NULL;
    HRESULT       hr;
    TV_SORTCB     tvscb;
    LPITEMIDLIST  lpi=NULL;

    if (iFolder == m_iRootFolder)
      return; // no change
    m_iRootFolder = iFolder;
    hr=SHGetDesktopFolder(&lpsf);
    if (SUCCEEDED(hr))
    {
       // Get folder location and bind desktop folder to it
       // BE 20010823: only if we actually want a different folder
       if (iFolder && SUCCEEDED(SHGetSpecialFolderLocation(NULL, 
                           iFolder, &lpi)))
       {
         lpsf->BindToObject(lpi, NULL, IID_IShellFolder, (void **)&lpsf);
       }

       // Initialize the list and tree view controls to be empty.
      m_CList.ResetContent();
      m_CList2.ResetContent();
      m_CDirTree.DeleteAllItems();

       // Fill in the tree view control from the root.
       BeginWaitCursor( );
       FillTreeView(lpsf, NULL,	TVI_ROOT);
       EndWaitCursor();

       // Release the folder pointer.
       lpsf->Release();
    }

    // Sort the items in the tree view control.
    tvscb.hParent     = TVI_ROOT;
    tvscb.lpfnCompare = TreeViewCompareProc;
    tvscb.lParam      = 0;

    ::SendMessage(m_CDirTree.m_hWnd,TVM_SORTCHILDRENCB, (WPARAM)0, (LPARAM)&tvscb);
}

/****************************************************************************
*
*  FUNCTION: FillTreeView( LPSHELLFOLDER lpsf,
*                          LPITEMIDLIST  lpifq,
*                          HTREEITEM     hParent)
*
*  PURPOSE: Fills a branch of the TreeView control.  Given the
*           shell folder, enumerate the subitems of this folder,
*           and add the appropriate items to the tree.
*
*  PARAMETERS:
*    lpsf         - Pointer to shell folder that we want to enumerate items 
*    lpifq        - Fully qualified item id list to the item that we are enumerating
*                   items for.  In other words, this is the PIDL to the item
*                   identified by the lpsf parameter.
*    hParent      - Parent node
*
*  COMMENTS:
*    This function enumerates the items in the folder identifed by lpsf.
*    Note that since we are filling the left hand pane, we will only add
*    items that are folders and/or have sub-folders.  We *could* put all
*    items in here if we wanted, but that's not the intent.
*
****************************************************************************/
void CQResPropertyPage2::FillTreeView (
   LPSHELLFOLDER lpsf, LPITEMIDLIST lpifq, HTREEITEM hParent)
{
    TV_ITEM         tvi;            // tree view item
    TV_INSERTSTRUCT tvins;          // tree view insert structure
    LPENUMIDLIST    lpe=NULL;
    LPITEMIDLIST    lpi=NULL, lpifqThisItem=NULL;
    LPTVITEMDATA    lptvid;
    LPMALLOC        lpMalloc=NULL;
    ULONG           ulFetched;
    HRESULT         hr;
    char            szBuff[MAX_PATH+1];
    HWND            hwnd=::GetParent(m_CDirTree.m_hWnd);
    BOOL            fHide;

    // Allocate a shell memory object. 
    hr=::SHGetMalloc(&lpMalloc);
    if (FAILED(hr))
       return;

    if (SUCCEEDED(hr))
    {
        // Get the IEnumIDList object for the given folder.
        hr=lpsf->EnumObjects(hwnd, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &lpe);

        if (SUCCEEDED(hr))
        {
            // Enumerate throught the list of folder and nonfolder objects.
            while (S_OK==lpe->Next(1, &lpi, &ulFetched))
            {
                // If this is the root, show links from the
                // current directory in the list box
                if (hParent == TVI_ROOT)
                  AddToFileList(lpsf, lpi);

                // Create a fully qualified path to the current item.
                // The SH* functions take a fully qualified path PIDL,
                // while the interface member functions take a 
                // relative path PIDL.
                ULONG ulAttrs = SFGAO_HASSUBFOLDER | SFGAO_FOLDER;

                // Determine what type of object you have.
                lpsf->GetAttributesOf (
                   1, (const struct _ITEMIDLIST **)&lpi, &ulAttrs);
                fHide = HideFolder(lpi, ulAttrs);

                if (ulAttrs & (SFGAO_HASSUBFOLDER | SFGAO_FOLDER))
                {
                   // You need this next if statement to
                   // avoid adding objects that are not real 
                   // folders to the tree. Some objects can
                   // have subfolders, but aren't real folders.                   
                   if (!fHide && (ulAttrs & SFGAO_FOLDER))
                   {
                      tvi.mask= TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;

                      if (ulAttrs & SFGAO_HASSUBFOLDER)
                      {
                         // This item has subfolders, so put a plus sign in
                         // the tree view control. The first time the user clicks
                         // the item, you should populate the sub-folders.
                         tvi.cChildren=1;
                         tvi.mask |= TVIF_CHILDREN;
                      }
                        
                      // Get some memory for the ITEMDATA structure.
                      lptvid = (LPTVITEMDATA)lpMalloc->Alloc(sizeof(TVITEMDATA));
                      if (!lptvid)
                         goto Done;  // Error - could not allocate memory.
   
                      // Now get the friendly name to 
                      // put in the tree view control.
                      if (!GetName(lpsf, lpi, SHGDN_NORMAL, szBuff, sizeof(szBuff)))
                         goto Done; // Error - could not get friendly name

                      tvi.pszText    = szBuff;
                      tvi.cchTextMax = MAX_PATH;
    
                      lpifqThisItem=ConcatPidls(lpifq, lpi);
      
                      // Now make a copy of the ITEMIDLIST
                      lptvid->lpi=CopyITEMID(lpMalloc, lpi);
   
                      GetNormalAndSelectedIcons(lpifqThisItem, &tvi);
   
                      lptvid->lpsfParent=lpsf;    // store the parent folder's SF
                      lpsf->AddRef();

                      lptvid->lpifq=ConcatPidls(lpifq, lpi);
   
                      tvi.lParam = (LPARAM)lptvid;
   
                      // Populate the tree view insert structure.
                      // The item is the one filled above.
                      // Insert it after the last item inserted at this level.
                      // Indicate this is a root entry.
                      tvins.item         = tvi;
                      tvins.hInsertAfter = TVI_FIRST;
                      tvins.hParent      = hParent;
   
                      // Add the item to the tree.
                      m_CDirTree.InsertItem(&tvins);
                   }
                   // Free the task allocator for this item.
                   if (lpifqThisItem)
                     lpMalloc->Free(lpifqThisItem);  
                   lpifqThisItem=NULL;
                }

                lpMalloc->Free(lpi);  // free PIDL the shell gave you
                lpi=NULL;
            }
        }

    }
    else
       return;

Done:
    if (lpe)  
        lpe->Release();

    // The following two if statements will be TRUE only if you got here
    // on an error condition from the goto statement.  Otherwise, free 
    // this memory at the end of the while loop above.
    if (lpi && lpMalloc)           
        lpMalloc->Free(lpi);
    if (lpifqThisItem && lpMalloc) 
        lpMalloc->Free(lpifqThisItem);  
    if (lpMalloc) 
        lpMalloc->Release();
}

/****************************************************************************
*
*    FUNCTION: GetIcon(LPITEMIDLIST lpi, UINT uFlags)
*
*    PURPOSE:  Gets the index for the current icon.  Index is index into system
*              image list.
*
*  PARAMETERS:
*    lpi    - Fully qualified item id list for current item.
*    uFlags - Flags for SHGetFileInfo()
*
*  RETURN VALUE:
*    Icon index for current item.
****************************************************************************/
int CQResPropertyPage2::GetIcon(LPITEMIDLIST lpi, UINT uFlags)
{
   SHFILEINFO    sfi;

   SHGetFileInfo((LPCSTR)lpi, 0, &sfi, sizeof(SHFILEINFO), uFlags);

   return sfi.iIcon;
}

/****************************************************************************
*
*    FUNCTION: GetNormalAndSelectedIcons(LPITEMIDLIST lpifq, LPTV_ITEM lptvitem)
*
*    PURPOSE:  Gets the index for the normal and selected icons for the current item.
*
*    PARAMETERS:
*    lpifq    - Fully qualified item id list for current item.
*    lptvitem - Pointer to treeview item we are about to add to the tree.
*
****************************************************************************/
void CQResPropertyPage2::GetNormalAndSelectedIcons (
   LPITEMIDLIST lpifq, LPTV_ITEM lptvitem)
{
   // Don't check the return value here. 
   // If IGetIcon() fails, you're in big trouble.
   lptvitem->iImage = GetIcon (lpifq, 
      SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
   
   lptvitem->iSelectedImage = GetIcon (lpifq, 
      SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_OPENICON);
   
   return;
}


/****************************************************************************
*
*    FUNCTION: PopulateListView(LPTVITEMDATA, LPSHELLFOLDER )
*
*    PURPOSE:  Populates the list view.
*
****************************************************************************/
BOOL CQResPropertyPage2::PopulateListView(LPTVITEMDATA lptvid, LPSHELLFOLDER lpsf)
{
    m_CList.ResetContent();
    m_CList2.ResetContent();
    return InitListViewItems(lptvid, lpsf);
}


/****************************************************************************
*
*    FUNCTION: InitListViewItems(LPTVITEMDATA lptvid,
*                              LPSHELLFOLDER lpsf,
*                              LPITEMIDLIST lpi)
*
*    PURPOSE:  Add items to the list view.
*
*    PARAMETERS:
*      HWND - Handle to the listview control
*      LPTVITEMDATA - Pointer to TreeView item data
*      LPSHELLFOLDER - Pointer to parent shell folder
*
****************************************************************************/
BOOL CQResPropertyPage2::InitListViewItems(LPTVITEMDATA lptvid, LPSHELLFOLDER lpsf)
{
    HRESULT      hr;
    LPMALLOC     lpMalloc = NULL;                                             
    LPITEMIDLIST lpi=NULL;
    LPENUMIDLIST lpe=NULL;
    ULONG        ulFetched;
    HWND         hwnd=::GetParent(m_CList.m_hWnd);

    hr=SHGetMalloc(&lpMalloc);
    if (FAILED(hr))
       return FALSE;

    if (SUCCEEDED(hr))
    {
      hr=lpsf->EnumObjects(hwnd,SHCONTF_NONFOLDERS, &lpe);

      if (SUCCEEDED(hr))
      {
        while (S_OK==lpe->Next(1, &lpi, &ulFetched))
        {
          AddToFileList(lpsf, lpi);
          lpMalloc->Free(lpi);  // free PIDL the shell gave you
          lpi=NULL;
        }
      }
    }

    if (lpe)  
        lpe->Release();
    if (lpi && lpMalloc)           
        lpMalloc->Free(lpi);
    if (lpMalloc) 
        lpMalloc->Release();
 
    return TRUE;
}


/****************************************************************************
*
*    FUNCTION: TreeViewCompareProc(LPARAM, LPARAM, LPARAM)
*
*    PURPOSE:  Callback routine for sorting the tree 
*
****************************************************************************/
int CALLBACK CQResPropertyPage2::TreeViewCompareProc(LPARAM lparam1, LPARAM lparam2, LPARAM lparamSort)
{
    LPTVITEMDATA lptvid1=(LPTVITEMDATA)lparam1;
    LPTVITEMDATA lptvid2=(LPTVITEMDATA)lparam2;
    HRESULT   hr;

    hr = lptvid1->lpsfParent->CompareIDs(0,lptvid1->lpi,lptvid2->lpi);

    if (FAILED(hr))
       return 0;

    return (hr);
}

// 
// FUNCTIONS THAT DEAL WITH PIDLs
//
/****************************************************************************
*
*    FUNCTION: Next(LPCITEMIDLIST pidl)
*
*    PURPOSE:  Gets the next PIDL in the list 
*
****************************************************************************/
LPITEMIDLIST CQResPropertyPage2::Next(LPCITEMIDLIST pidl)
{
   LPSTR lpMem=(LPSTR)pidl;

   lpMem+=pidl->mkid.cb;

   return (LPITEMIDLIST)lpMem;
}

/****************************************************************************
*
*    FUNCTION: GetSize(LPCITEMIDLIST pidl)
*
*    PURPOSE:  Gets the size of the PIDL 
*
****************************************************************************/
UINT CQResPropertyPage2::GetSize(LPCITEMIDLIST pidl)
{
    UINT cbTotal = 0;
    if (pidl)
    {
        cbTotal += sizeof(pidl->mkid.cb);       // Null terminator
        while (pidl->mkid.cb)
        {
            cbTotal += pidl->mkid.cb;
            pidl = Next(pidl);
        }
    }

    return cbTotal;
}

/****************************************************************************
*
*    FUNCTION: PIDLCreate(UINT cbSize)
*
*    PURPOSE:  Allocates a PIDL 
*
****************************************************************************/
LPITEMIDLIST CQResPropertyPage2::PIDLCreate(UINT cbSize)
{
    LPMALLOC lpMalloc;
    HRESULT  hr;
    LPITEMIDLIST pidl;

    hr=SHGetMalloc(&lpMalloc);

    if (FAILED(hr))
       return 0;

    pidl=(LPITEMIDLIST)lpMalloc->Alloc(cbSize);

    if (pidl)
        memset(pidl, 0, cbSize);      // zero-init for external task   alloc

    if (lpMalloc) lpMalloc->Release();

    return pidl;
}

/****************************************************************************
*
*    FUNCTION: ConcatPidls(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2)
*
*    PURPOSE:  Concatenates two PIDLs 
*
****************************************************************************/
LPITEMIDLIST CQResPropertyPage2::ConcatPidls(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2)
{
    LPITEMIDLIST pidlNew;
    UINT cb1;
    UINT cb2;

    if (pidl1)  //May be NULL
       cb1 = GetSize(pidl1) - sizeof(pidl1->mkid.cb);
    else
       cb1 = 0;

    cb2 = GetSize(pidl2);

    pidlNew = PIDLCreate(cb1 + cb2);
    if (pidlNew)
    {
        if (pidl1)
           memcpy(pidlNew, pidl1, cb1);
        memcpy(((LPSTR)pidlNew) + cb1, pidl2, cb2);
    }
    return pidlNew;
}

/****************************************************************************
*
*    FUNCTION: CopyITEMID(LPMALLOC lpMalloc, LPITEMIDLIST lpi)
*
*    PURPOSE:  Copies the ITEMID 
*
****************************************************************************/
LPITEMIDLIST CQResPropertyPage2::CopyITEMID(LPMALLOC lpMalloc, LPITEMIDLIST lpi)
{
   LPITEMIDLIST lpiTemp;

   lpiTemp=(LPITEMIDLIST)lpMalloc->Alloc(lpi->mkid.cb+sizeof(lpi->mkid.cb));
   CopyMemory((PVOID)lpiTemp, (CONST VOID *)lpi, lpi->mkid.cb+sizeof(lpi->mkid.cb));

   return lpiTemp;
}

/****************************************************************************
*
*    FUNCTION: GetName(LPSHELLFOLDER lpsf,LPITEMIDLIST  lpi,DWORD dwFlags,
*             LPSTR         lpFriendlyName)
*
*    PURPOSE:  Gets the friendly name for the folder 
*
****************************************************************************/
BOOL CQResPropertyPage2::GetName (LPSHELLFOLDER lpsf, LPITEMIDLIST lpi, 
   DWORD dwFlags, LPSTR lpFriendlyName, UINT cbMaxLen)
{
   BOOL   bSuccess=TRUE;
   STRRET str;

   *lpFriendlyName = 0;
   if (NOERROR==lpsf->GetDisplayNameOf(lpi,dwFlags, &str))
   {
      switch (str.uType)
      {
         case STRRET_WSTR:
            WideCharToMultiByte(
               CP_ACP,                 // code page
               0,		               // dwFlags
               str.pOleStr,            // lpWideCharStr
               wcslen(str.pOleStr)+1,  // cchWideCharStr
               lpFriendlyName,         // lpMultiByteStr
               cbMaxLen,               // cchMultiByte
               NULL,                   // lpDefaultChar
               NULL);                  // lpUsedDefaultChar
             break;

         case STRRET_OFFSET:
             lstrcpy(lpFriendlyName, (LPSTR)lpi+str.uOffset);
             break;

         case STRRET_CSTR:             
             lstrcpy(lpFriendlyName, (LPSTR)str.cStr);
             break;

         default:
             bSuccess = FALSE;
             break;
      }
   }
   else
      bSuccess = FALSE;

   return bSuccess;
}

/****************************************************************************
*
*    FUNCTION: GetFullyQualPidl(LPSHELLFOLDER lpsf, LPITEMIDLIST lpi)
*
*    PURPOSE:  Gets the Fully qualified Pidls for the folder 
*
****************************************************************************/
LPITEMIDLIST CQResPropertyPage2::GetFullyQualPidl(LPSHELLFOLDER lpsf, LPITEMIDLIST lpi)
{
   char szBuff[MAX_PATH];
   OLECHAR szOleChar[MAX_PATH];
   LPSHELLFOLDER lpsfDeskTop;
   LPITEMIDLIST  lpifq;
   ULONG ulEaten, ulAttribs;
   HRESULT hr;

   if (!GetName(lpsf, lpi, SHGDN_FORPARSING, szBuff, sizeof(szBuff)))
      return NULL;

   hr=SHGetDesktopFolder(&lpsfDeskTop);

   if (FAILED(hr))
      return NULL;

   MultiByteToWideChar(CP_ACP,
					   MB_PRECOMPOSED,
					   szBuff,
					   -1,
					   (wchar_t *)szOleChar,
					   sizeof(szOleChar));

   hr=lpsfDeskTop->ParseDisplayName(NULL,
									NULL,
									szOleChar,
									&ulEaten,
									&lpifq,
									&ulAttribs);

   lpsfDeskTop->Release();

   if (FAILED(hr))
      return NULL;

   return lpifq;
}


void CQResPropertyPage2::InitTreeView()
{
  // Extra init for tree view and list view

  // Get the handle to the system image list, for our icons
  HIMAGELIST  hImageList;
  SHFILEINFO    sfi;

  hImageList = (HIMAGELIST)SHGetFileInfo(
                  (LPCSTR)"C:\\", 0, &sfi, sizeof(SHFILEINFO), 
                  SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

  // Attach ImageList to TreeView
  if (hImageList)
    ::SendMessage(m_CDirTree.m_hWnd, TVM_SETIMAGELIST, (WPARAM) TVSIL_NORMAL,
                 (LPARAM)hImageList);

  // Make sure the tree view is filled
  m_iRootFolder = -1;
}


void CQResPropertyPage2::OnItemexpandingDirtree(NMHDR* pNMHDR, LRESULT* pResult) 
{
    LPTVITEMDATA   lptvid;  //Long pointer to TreeView item data
    LPSHELLFOLDER  lpsf2=NULL;
    TV_SORTCB      tvscb;
	NM_TREEVIEW*   pnmtv = (NM_TREEVIEW*)pNMHDR;
    HRESULT        hr;
    char           szName[MAX_PATH];

	*pResult = 0;
    if ((pnmtv->itemNew.state & TVIS_EXPANDEDONCE))
      return;
		
    lptvid=(LPTVITEMDATA)pnmtv->itemNew.lParam;
    if (lptvid)
    {
      BeginWaitCursor();
      hr=lptvid->lpsfParent->BindToObject(lptvid->lpi,
                0, IID_IShellFolder,(LPVOID *)&lpsf2);
      
      if (SUCCEEDED(hr))
      {
        GetName(lptvid->lpsfParent, lptvid->lpi, SHGDN_FORPARSING,
                szName, sizeof(szName));
        FillTreeView(lpsf2,
                     lptvid->lpifq,
                     pnmtv->itemNew.hItem);

        if (strchr(szName, '\\'))
        {
          ::SendMessage(m_CDirTree.m_hWnd, TVM_SORTCHILDREN, 
                        (WPARAM)0, (LPARAM)pnmtv->itemNew.hItem);
        }
        else
        {
          tvscb.hParent     = pnmtv->itemNew.hItem;
          tvscb.lpfnCompare = TreeViewCompareProc;
          tvscb.lParam      = 0;

          ::SendMessage(m_CDirTree.m_hWnd,TVM_SORTCHILDRENCB, 
                        (WPARAM)0, (LPARAM)&tvscb);
        }
      }
      EndWaitCursor();
    }
	
	*pResult = 0;
}

BOOL CQResPropertyPage2::HideFolder(LPITEMIDLIST lpi, ULONG ulAttrs)
{
  char sz[MAX_PATH];
  if (SHGetPathFromIDList(lpi, sz))
    return (sz[strlen(sz)-1] <= ' ');
  return (m_iLinkType != IDC_DESKTOP) && !(ulAttrs & SFGAO_HASSUBFOLDER);
}

void CQResPropertyPage2::OnDeleteitemDirtree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pnmtv = (NM_TREEVIEW*)pNMHDR;
    LPTVITEMDATA lptvid;  //Long pointer to TreeView item data
    LPMALLOC     lpMalloc;
    HRESULT      hr;

    //Let's free the memory for the TreeView item data...
    hr=SHGetMalloc(&lpMalloc);
    if (FAILED(hr))
      return;
		
    lptvid=(LPTVITEMDATA)pnmtv->itemOld.lParam;
    lptvid->lpsfParent->Release();
    lpMalloc->Free(lptvid->lpi);  
    lpMalloc->Free(lptvid->lpifq);  
    lpMalloc->Free(lptvid);  
    lpMalloc->Release();
	
	*pResult = 0;
}

void CQResPropertyPage2::OnSelchangedDirtree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW*  pnmtv = (NM_TREEVIEW*)pNMHDR;
    LPTVITEMDATA  lptvid;  //Long pointer to TreeView item data
    HRESULT       hr;
    LPSHELLFOLDER lpsf2=NULL;
	
	//Do this only if we are not exiting the application...
    lptvid=(LPTVITEMDATA)pnmtv->itemNew.lParam;
    if (lptvid)
    {
      hr=lptvid->lpsfParent->BindToObject(lptvid->lpi,
		                0,IID_IShellFolder,(LPVOID *)&lpsf2);
		
      if (SUCCEEDED(hr))
      {
        PopulateListView(lptvid,lpsf2);
        lpsf2->Release();
      }
    }
	*pResult = 0;
}



///////////////////////////////////////////////////////////
// Wizard interface functions


///////////////////////////////////////////////////////////
// Page 2

BOOL CQResPropertyPage2::OnSetActive() 
{
    static BOOL fInit = FALSE;

    if (!fInit)
    {
      fInit = TRUE;
      InitTreeView();
    }

    // read properties
    if (m_iAction != theApp.m_pSheet->m_iAction)
    {
      m_iRootFolder = -1; // force reload of file list if action type changed
      m_iAction     = theApp.m_pSheet->m_iAction;
      switch (m_iAction)
      {
        case IDC_ADDQRES:
          SetBtnText(IDC_PROMPT, IDS_PROMPT2);
          break;
        case IDC_REMQRES:
          SetBtnText(IDC_PROMPT, IDS_PROMPT2_REM);
          break;
      }
    }
    m_iLinkType = theApp.m_pSheet->m_iLinkType;
    strcpy(m_szLink, theApp.m_pSheet->m_szLink);

    // display file list
    switch(m_iLinkType)
    {
      case IDC_STARTMENU:
        OnFill(CSIDL_STARTMENU);
        break;
      case IDC_DESKTOP:
        OnFill(CSIDL_DESKTOPDIRECTORY);
        break;
      case IDC_NEWLINK:
        OnFill(CSIDL_DESKTOP);
        break;
    }

    // disable next button if list changed
    GetSel();

    return CPropertyPage::OnSetActive();
}

BOOL CQResPropertyPage2::OnKillActive() 
{
    // write back properties
    // (m_iLinkType is not modified on page 2)
    strcpy(theApp.m_pSheet->m_szLink, m_szLink);

    // reset special display features
    theApp.m_pSheet->EnableButton(ID_WIZNEXT, TRUE);
	
	return CPropertyPage::OnKillActive();
}

void CQResPropertyPage2::OnSelcancelList1() 
{
  GetSel();
}

void CQResPropertyPage2::OnSelchangeList1() 
{
  GetSel();
}

void CQResPropertyPage2::OnDblclkList1() 
{
  GetSel();
  theApp.m_pSheet->SetActivePage(theApp.m_pSheet->GetActiveIndex()
                                 + ((m_iAction == IDC_REMQRES) ? 2 : 1));
}

// GetSel updates data related to listbox selection
void CQResPropertyPage2::GetSel(void)
{
  if (m_CList.GetCurSel() >= 0)
  {
    // full item name can be found in m_CList2
    int iSel2 = m_CList.GetItemData(m_CList.GetCurSel());
    m_CList2.GetText(iSel2, m_szLink);
    theApp.m_pSheet->EnableButton(ID_WIZNEXT, TRUE);
  }
  else
  {
    *m_szLink = 0;
    theApp.m_pSheet->EnableButton(ID_WIZNEXT, FALSE);
  }

}


// Helper function, adds one file item to list box
void CQResPropertyPage2::AddToFileList(LPSHELLFOLDER lpsf, LPITEMIDLIST lpi)
{
  char  szName[MAX_PATH+1];
  int   iItem1, iItem2;

  if (GetName(lpsf, lpi, SHGDN_FORPARSING, szName, sizeof(szName)))
  {
    // Only add extension .lnk if we are listing shortcuts,
    // extension .lnk, .exe, .bat or .com if we want to 
    // create a new shortcut
    char *ptr = strrchr(szName, '.');

    if (!ptr) return;

    strlwr(ptr);
    if (m_iLinkType == IDC_NEWLINK)
    {
      if (strcmp(ptr, ".lnk") &&
          strcmp(ptr, ".exe") &&
          strcmp(ptr, ".com") &&
          strcmp(ptr, ".bat"))
        return;
    }
    else if (strcmp(ptr,".lnk"))
      return;

    if (m_iAction == IDC_REMQRES)
    {
      // only show links that point to qres.exe
      if (!m_Shortcut.Load(szName) ||
          !m_Shortcut.IsQResLink())
        return;
    }
    iItem2 = m_CList2.AddString(szName);
    GetName(lpsf, lpi, SHGDN_NORMAL, szName, sizeof(szName));
    iItem1 = m_CList.AddString(szName);
    if ((iItem1 < 0) || (iItem2 < 0))
      return; // Uh-Oh, out of string memory

    // cross-reference lists both ways
    m_CList.SetItemData(iItem1, (DWORD)iItem2);
    m_CList2.SetItemData(iItem2, (DWORD)iItem1);
  }
}


void CQResPropertyPage2::SetBtnText(int iCtrl, int iString)
{
  CString s;
  CWnd* pBtn = GetDlgItem(iCtrl);

  if (pBtn && s.LoadString(iString))
    pBtn->SetWindowText(s);
}


LRESULT CQResPropertyPage2::OnWizardNext() 
{
  // Set shortcut in CQResPropertySheet if we are modifying an existing one
  if (m_iAction != IDC_NEWLINK)
  {
    CShortcut *pShortcut = &theApp.m_pSheet->m_Shortcut;

    pShortcut->Load(m_szLink);

    // If the shortcut already contains a qres link, 
    // get the current parameters.
    if (pShortcut->IsQResLink())
    {
      QRES_PARS tPars;

      if (pShortcut->GetQResPars(&tPars) > 0)
        theApp.m_Params = tPars;
    }
  }

  // skip page 3 when we are removing qres from a link
  if (m_iAction == IDC_REMQRES)
    return IDD_PROPPAGE4;
	
  return CPropertyPage::OnWizardNext();
}
