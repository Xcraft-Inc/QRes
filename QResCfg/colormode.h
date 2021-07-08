#ifndef COLORMODE_H
#define COLORMODE_H


// Class wrapping around DEVMODE struct
class CColorMode
{
// Attributes
public:
    DEVMODE  tMode;
    int      iColor; /* index in color resolutions list (-1 undefined) */ 
    int      iArea;  /* index in desktop area sizes list (-1 undefined) */
};

#define MAX_CMODES 1024

// List of color modes
// I removed the MFC list stuff because this saved 50 kB on exe size!
class CColorModeList
{
public:
  UINT        m_wCount;
  CColorMode *m_aMode[MAX_CMODES];
  CColorModeList();
  ~CColorModeList();
  BOOL AddTail(CColorMode *pColorMode);
  void RemoveAll();
  void CreateColorList();
  void DestroyColorList();
};



#endif // COLORMODE_H