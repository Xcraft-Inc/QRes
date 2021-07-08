// QResPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "QResCfg.h"
#include "QResPropertyPage.h"
#include "AdvDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// IMPLEMENT_DYNCREATE(CQResPropertyPage3, CPropertyPage)



/////////////////////////////////////////////////////////////////////////////
// CQResPropertyPage3 property page

CQResPropertyPage3::CQResPropertyPage3() : CPropertyPage(CQResPropertyPage3::IDD)
{
	//{{AFX_DATA_INIT(CQResPropertyPage3)
	m_fNoArea = FALSE;
	m_fNoColor = FALSE;
	//}}AFX_DATA_INIT
}

CQResPropertyPage3::~CQResPropertyPage3()
{
  m_ColorModeList.DestroyColorList();
}

void CQResPropertyPage3::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQResPropertyPage3)
	DDX_Control(pDX, IDC_NOAREA, m_CNoArea);
	DDX_Control(pDX, IDC_NOCOLOR, m_CNoColor);
	DDX_Control(pDX, IDC_COLOR, m_CColor);
	DDX_Control(pDX, IDC_AREA_TXT, m_CAreaTxt);
	DDX_Control(pDX, IDC_AREA, m_CArea);
	DDX_Check(pDX, IDC_NOAREA, m_fNoArea);
	DDX_Check(pDX, IDC_NOCOLOR, m_fNoColor);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQResPropertyPage3, CPropertyPage)
	//{{AFX_MSG_MAP(CQResPropertyPage3)
	ON_CBN_SELCHANGE(IDC_COLOR, OnSelchangeColor)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_NOAREA, OnNoarea)
	ON_BN_CLICKED(IDC_NOCOLOR, OnNocolor)
	ON_BN_CLICKED(IDC_ADVANCED, OnAdvanced)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


///////////////////////////////////////////////////////////
// Page 3

BOOL CQResPropertyPage3::OnSetActive() 
{
  static BOOL fInit = FALSE;

  if (!fInit)
  {
    fInit = TRUE;

    // Do first-time intialisations
    InitColorDlg();
  }

  // Get property values
  m_fNoColor = (theApp.m_Params.mNew.wBitsPixel == 0);
  m_fNoArea  = (theApp.m_Params.mNew.dwXRes == 0);
  SetColorMode(m_fNoColor ? 8 : theApp.m_Params.mNew.wBitsPixel);
  SetScreenArea(m_fNoArea ? 640 : theApp.m_Params.mNew.dwXRes);

  m_CNoArea.SetCheck(m_fNoArea);
  m_CNoColor.SetCheck(m_fNoColor);

  return CPropertyPage::OnSetActive();
}

BOOL CQResPropertyPage3::OnKillActive() 
{
    BOOL fResult = CPropertyPage::OnKillActive();

    // Write back property values
    theApp.m_Params.mNew.wBitsPixel = m_fNoColor ? 0 : m_iColors;
    theApp.m_Params.mNew.dwXRes     = m_fNoArea  ? 0 : m_iXRes;

	return fResult;
}


// InitColorDlg initialses color drop down list and list of allowed resolutions	
void CQResPropertyPage3::InitColorDlg(void)
{
  UINT        wPos;
  int         iColor, iArea, iItem;
  char        szColor[40], *ptr;
  CColorMode *pColorMode;

  m_ColorModeList.CreateColorList();

  iColor = -1; iArea = -1;
  for( wPos = 0; wPos < m_ColorModeList.m_wCount; wPos++ )
  {
    pColorMode = m_ColorModeList.m_aMode[wPos];

    if (pColorMode->iColor > iColor)
    {
      iColor = pColorMode->iColor;
      switch(pColorMode->tMode.dmBitsPerPel)
      {
        case 8:
          strcpy(szColor, "256 colors");
          break;
        case 15:
          strcpy(szColor, "32768 colors (15 bit)");
          break;
        case 16:
          strcpy(szColor, "65536 colors (16 bit)");
          break;
        default:
          if (pColorMode->tMode.dmBitsPerPel < 15)
            ptr = "Low";
          else if (pColorMode->tMode.dmBitsPerPel < 24)
            ptr = "High";
          else
            ptr = "True";
          sprintf(szColor, "%s color (%ld bit)", ptr, pColorMode->tMode.dmBitsPerPel);
          break;
      }
      iItem = m_CColor.AddString(szColor);
      if (iItem >= 0)
        m_CColor.SetItemData(iItem, pColorMode->tMode.dmBitsPerPel);
    }
    if (pColorMode->iArea > iArea)
    {
      iArea = pColorMode->iArea;
    }
  }
  m_CArea.SetRange(0, iArea);
}


// SetColorMode sets the color mode
void CQResPropertyPage3::SetColorMode(int iColorMode)
{
  int i, iCount = m_CColor.GetCount();

  for (i = 0; i < iCount; i++)
    if (iColorMode == (int)m_CColor.GetItemData(i))
    {
      m_CColor.SetCurSel(i);
      m_iColors = iColorMode;
    }

  EnableButton(IDC_COLOR, !m_fNoColor);
}

// SetScreenArea sets the screen area
void CQResPropertyPage3::SetScreenArea(int iXRes)
{
  UINT        wPos;
  CColorMode *pColorMode;
  int         iArea = -1;
  char        szText[50];

  for( wPos = 0; wPos < m_ColorModeList.m_wCount; wPos++ )
  {
    pColorMode = m_ColorModeList.m_aMode[wPos];
    if (pColorMode->iArea > iArea)
    {
      iArea = pColorMode->iArea;
      if (pColorMode->tMode.dmPelsWidth == (DWORD)iXRes)
      {
        sprintf(szText, "%ld by %ld pixels", 
                pColorMode->tMode.dmPelsWidth,
                pColorMode->tMode.dmPelsHeight);
        m_CAreaTxt.SetWindowText(szText);
        m_CArea.SetPos(iArea);
        m_iXRes = iXRes;
        break;
      }
    }
  }

  EnableButton(IDC_AREA, !m_fNoArea);
  EnableButton(IDC_AREA_TXT, !m_fNoArea);
  EnableButton(IDC_LESS, !m_fNoArea);
  EnableButton(IDC_MORE, !m_fNoArea);
}


void CQResPropertyPage3::OnSelchangeColor() 
{
  int         iSel = m_CColor.GetCurSel(), iColors;
  UINT        wPos;
  CColorMode *pColorMode;
  BOOL        fOk = FALSE;

  if (iSel >= 0)
  {
    iColors = (int)m_CColor.GetItemData(iSel);
    if (iColors != m_iColors)
    {
      m_iColors = iColors;

      // Find out if area setting is still OK for these colors
      for( wPos = 0; wPos < m_ColorModeList.m_wCount; wPos++ )
      {
        pColorMode = m_ColorModeList.m_aMode[wPos];
        if ((pColorMode->tMode.dmBitsPerPel == (DWORD)m_iColors) &&
            (pColorMode->tMode.dmPelsWidth == (DWORD)m_iXRes))
        {
          fOk = TRUE;
          break;
        }
      }
      if (!fOk)
      {
        // find highest screen mode that supports this color mode
        for( wPos = m_ColorModeList.m_wCount; wPos > 0; wPos--)
        {
          pColorMode = m_ColorModeList.m_aMode[wPos-1];
          if (pColorMode->tMode.dmBitsPerPel == (DWORD)m_iColors)
          {
            SetScreenArea(pColorMode->tMode.dmPelsWidth);
            break;
          }
        }
      }
    }
  }
}

void CQResPropertyPage3::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
  int         iPos = m_CArea.GetPos();
  UINT        wPos;
  CColorMode *pColorMode;
  char        szText[50];
  BOOL        fOk = FALSE;

  for( wPos = 0; wPos < m_ColorModeList.m_wCount; wPos++ )
  {
    pColorMode = m_ColorModeList.m_aMode[wPos];
    if (((int)pColorMode->tMode.dmBitsPerPel == m_iColors) &&
        (pColorMode->iArea == iPos))
    {
      sprintf(szText, "%ld by %ld pixels", 
              pColorMode->tMode.dmPelsWidth,
              pColorMode->tMode.dmPelsHeight);
      m_CAreaTxt.SetWindowText(szText);
      m_iXRes = pColorMode->tMode.dmPelsWidth;
      fOk = TRUE;
      break;
    }
  }
  if (!fOk)
  {
    // search for the highest screen mode that supports this screen resolution
    for( wPos = m_ColorModeList.m_wCount; wPos > 0; wPos--)
    {
      pColorMode = m_ColorModeList.m_aMode[wPos-1];
      if (pColorMode->iArea == iPos)
      {
        sprintf(szText, "%ld by %ld pixels", 
                pColorMode->tMode.dmPelsWidth,
                pColorMode->tMode.dmPelsHeight);
        m_CAreaTxt.SetWindowText(szText);
        m_iXRes = pColorMode->tMode.dmPelsWidth;
        SetColorMode(pColorMode->tMode.dmBitsPerPel);
        break;
      }
    }
  }
	
  CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CQResPropertyPage3::EnableButton(UINT wId, BOOL fEnable)
{
  CWnd* pBtn = GetDlgItem(wId);
  
  if (pBtn)
    pBtn->EnableWindow(fEnable);
}

void CQResPropertyPage3::OnNoarea() 
{
  m_fNoArea = !m_fNoArea;
  SetScreenArea(m_iXRes ? m_iXRes : 640);

  // Not allowed to have both 'No Color' and 'No Area' checked
  if (m_fNoColor && m_fNoArea)
  {
    m_CNoColor.PostMessage(BM_CLICK);
  }
}

void CQResPropertyPage3::OnNocolor() 
{
  m_fNoColor = !m_fNoColor;
  SetColorMode(m_iColors ? m_iColors : 8);

  // Not allowed to have both 'No Color' and 'No Area' checked
  if (m_fNoColor && m_fNoArea)
  {
    m_CNoArea.PostMessage(BM_CLICK);
  }
}

void CQResPropertyPage3::OnAdvanced() 
{
  CAdvDlg dlg;

  // Update property values before calling 'advanced' dlg
  theApp.m_Params.mNew.wBitsPixel = m_fNoColor ? 0 : m_iColors;
  theApp.m_Params.mNew.dwXRes     = m_fNoArea  ? 0 : m_iXRes;

  // Open the advanced dialog, update global parameter settings
  // if it is closed with OK.
  dlg.m_Params = theApp.m_Params;
  if (dlg.DoModal() == IDOK)
    theApp.m_Params = dlg.m_Params;
}
