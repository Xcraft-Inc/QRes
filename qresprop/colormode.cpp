// Copyright 1998-2005 Berend Engelbrecht. All rights reserved.

#include "priv.h"
#include "colormode.h"

CColorModeList::CColorModeList()
{
  m_wCount = 0;
  memset(m_aMode, 0, sizeof(m_aMode));
}

CColorModeList::~CColorModeList()
{
  RemoveAll();
}

BOOL CColorModeList::AddTail(CColorMode *pColorMode)
{
  if (m_wCount >= MAX_CMODES)
    return FALSE;

  m_aMode[m_wCount] = new CColorMode;
  if (!m_aMode[m_wCount])
    return FALSE;
  *m_aMode[m_wCount] = *pColorMode;
  m_wCount++;
  return TRUE;
}

void CColorModeList::RemoveAll()
{
  UINT w, wCount = m_wCount;

  m_wCount = 0;
  for (w = 0; w < wCount; w++)
    if (m_aMode[w])
    {
      delete m_aMode[w];
      m_aMode[w] = NULL;
    }
}



void CColorModeList::CreateColorList()
{
  DEVMODE     tMode;
  DWORD       dwMode = 0;
  CColorMode *pColorMode, *pColorMode2;
  UINT        wPos, wPos2;
  int         iColor, iArea;

  DestroyColorList(); // In case Create is called more than once
  pColorMode = new CColorMode;
  while (EnumDisplaySettings(NULL, dwMode, &tMode))
  {
    if (tMode.dmBitsPerPel >= 8) // QuickRes does not support < 256 colors
    {
      pColorMode->tMode = tMode;
      pColorMode->iColor = -1;
      pColorMode->iArea = -1;
      AddTail(pColorMode);
    }
    dwMode++;
  }
  delete pColorMode;

  // Now search the list of screen modes, find distinct
  // color depth and resolution settings
  iColor = 0;
  iArea = 0;
  for( wPos = 0; wPos < m_wCount; wPos++ )
  {
    pColorMode = m_aMode[wPos];
    for( wPos2 = 0; 
         (wPos2 < wPos) &&
         ((pColorMode->iColor < 0) || (pColorMode->iArea < 0)); wPos2++)
    {
      pColorMode2 = m_aMode[wPos2];

      // Compare color depth
      if (pColorMode2->tMode.dmBitsPerPel == pColorMode->tMode.dmBitsPerPel)
        pColorMode->iColor = pColorMode2->iColor;

      // Compare screen area
      if ((pColorMode2->tMode.dmPelsWidth == pColorMode->tMode.dmPelsWidth) &&
          (pColorMode2->tMode.dmPelsHeight == pColorMode->tMode.dmPelsHeight))
        pColorMode->iArea = pColorMode2->iArea;
    }

    // If color mode or area doesn't appear in the list yet, increment the index
    if (pColorMode->iColor < 0)
      pColorMode->iColor = iColor++;
    if (pColorMode->iArea < 0)
      pColorMode->iArea = iArea++;
  }
}


void CColorModeList::DestroyColorList()
{
  RemoveAll();
}

