// The shortcut module encapsulates all shortcut link stuff for the
// QResCfg application.
// A pre-requisite is that the main app calls CoInitialize at init and
// CoUninitialize just before terminating. This is a requirement of
// Microsoft's COM library.

#include "stdafx.h"
#include "Shortcut.h"
#include "..\qreslib\qreslib.h"


CShortcut::CShortcut()
{
  HRESULT hr;

  m_psl = NULL;
  m_ppf = NULL;
  m_fNew = FALSE;

  hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                        IID_IShellLink, (void **)&m_psl);
  if (SUCCEEDED(hr))
  {
    hr = m_psl->QueryInterface(IID_IPersistFile, (void **)&m_ppf);
    if (!SUCCEEDED(hr))
      m_ppf = NULL;
  }
  else
    m_psl = NULL;
  *m_wszLink = 0;
}

CShortcut::~CShortcut()
{
  if (m_ppf)
  {
    m_ppf->Release();
    m_ppf = NULL;
  }
  if (m_psl)
  {
    m_psl->Release();
    m_psl = NULL;
  }
  *m_wszLink = 0;
}


BOOL CShortcut::Load(LPSTR szLink)
{
  HRESULT hr;

  m_fNew = FALSE;
  if (m_ppf)
  {
    MultiByteToWideChar(CP_ACP, 0, szLink, -1, m_wszLink, MAX_PATH);
    hr = m_ppf->Load(m_wszLink, STGM_READWRITE);
    return SUCCEEDED(hr);
  }
  return FALSE;
}


BOOL CShortcut::CreateOnDesktop(LPSTR szLink)
{
  BOOL         fResult = FALSE, fTargetIsLink = FALSE;
  char         szPath[MAX_PATH], szName[MAX_PATH], *ptr;

  if (!m_psl || !m_ppf)
    return FALSE;
  if (GetSpecialFolderLocation(CSIDL_DESKTOPDIRECTORY, szPath))
  {
    ptr = strrchr(szLink, '\\');
    ptr = ptr ? (ptr+1) : szLink;
    strcpy(szName, ptr);
    ptr = strrchr(szName, '.');
    if (ptr) 
    {
      // find out if target is a shortcut
      if (!stricmp(ptr, ".lnk"))
        fTargetIsLink = TRUE;
      *ptr = 0;
    }

    strcat(szName, ".lnk");
    strcat(szPath, "Shortcut to ");
    strcat(szPath, szName);

    fResult =  fTargetIsLink ? Load(szLink)
                             : SUCCEEDED(m_psl->SetPath(szLink));
  }

  MultiByteToWideChar(CP_ACP, 0, szPath, -1, m_wszLink, MAX_PATH);
  m_fNew = TRUE;
  return fResult;
}


BOOL CShortcut::Resolve(HWND hwnd)
{
  HRESULT hr;

  if (m_psl)
  {
    hr = m_psl->Resolve(hwnd, SLR_ANY_MATCH);
    return SUCCEEDED(hr);
  }
  return FALSE;
}


BOOL CShortcut::GetTarget(LPSTR szRet, int cbMaxRet)
{
  HRESULT         hr;
  WIN32_FIND_DATA wfd;

  if (m_psl)
  {
    hr = m_psl->GetPath(szRet, cbMaxRet, &wfd, SLGP_UNCPRIORITY);
    return SUCCEEDED(hr);
  }
  return FALSE;
}


/////////////////////////////////////////////////
// Specific QRes functionality


BOOL CShortcut::IsQResLink(void)
{
  char szTarget[MAX_PATH];

  if(GetTarget(szTarget, sizeof(szTarget)))
  {
    strlwr(szTarget);
    return (strstr(szTarget, "\\qres.exe") != NULL);
  }
  return FALSE;
}


int CShortcut::GetQResPars(QRES_PARS *ptParsRet)
{
  HRESULT hr;
  char    szArgs[MAX_PATH+512];

  memset(ptParsRet, 0, sizeof(QRES_PARS));
  if (!m_psl)
  {
    strcpy(ptParsRet->szErr, "IShellLink init failure\n\n");
    return -1;
  }

  if (!IsQResLink())
    return 0;

  hr = m_psl->GetArguments(szArgs, sizeof(szArgs));
  if (!SUCCEEDED(hr))
  {
    strcpy(ptParsRet->szErr, "Could not get link arguments\n\n");
    return -1;
  }
  return ::GetQResPars(szArgs, ptParsRet);
}


// SetQResPars sets QRes parameters for the current link
BOOL CShortcut::SetQResPars(QRES_PARS *ptPars, HWND hwnd, BOOL fSave)
{
  QRES_PARS tOldPars;
  char      szApp[512], szIcon[MAX_PATH], szArgs[MAX_PATH+512];
  char      szWorkDir[MAX_PATH];
  int       iIcon;

  if (!m_psl || !m_ppf || !*m_wszLink)
    return FALSE;

  // Preserve icon location and working directory
  *szIcon = 0; *szWorkDir = 0;
  m_psl->GetIconLocation(szIcon, sizeof(szIcon), &iIcon);
  m_psl->GetWorkingDirectory(szWorkDir, sizeof(szWorkDir));

  // Get current arguments
  if (IsQResLink() && (GetQResPars(&tOldPars) > 0))
  {
    strcpy(szApp, tOldPars.szApp);
  }
  else
  {
    // get original application name
    strcpy(szApp, ptPars->szApp);
    if (!GetTarget(szApp, sizeof(szApp)) ||
        !SUCCEEDED(m_psl->GetArguments(szArgs, sizeof(szArgs))) )
      return FALSE;

    // If the icon location was not specified, use the first one from 
    // the program
    if (!*szIcon)
    {
      strcpy(szIcon, szApp);
      iIcon = 0;
    }

    // enclose app name in quotes if it contains spaces
    if ((*szApp != '\"') && strchr(szApp, ' '))
    {
      memmove(szApp+1, szApp, strlen(szApp)+1);
      *szApp = '\"';
      strcat(szApp, "\"");
    }

    // concatenate program arguments, if any
    if (*szArgs)
    {
      strcat(szApp, " ");
      strcat(szApp, szArgs);
    }
  }

  // szArgs is a concatenation of
  // [qres options] [appname] [app arguments]
  *szArgs = 0;
  if (ptPars->mNew.wBitsPixel)
    sprintf(szArgs+strlen(szArgs),"C=%u ", ptPars->mNew.wBitsPixel);
  if (ptPars->mNew.dwXRes)
    sprintf(szArgs+strlen(szArgs),"X=%lu ", ptPars->mNew.dwXRes);
  if (ptPars->mNew.wFreq) // BE 970315
    sprintf(szArgs+strlen(szArgs),"F=%u ", ptPars->mNew.wFreq);
  if (ptPars->dwFlags & QF_RESTORE)
    strcat(szArgs, "/R ");
  if (ptPars->dwFlags & QF_DIRECT) // BE 970913
    strcat(szArgs, "/D ");
  strcat(szArgs, szApp);

  if (!SUCCEEDED(m_psl->SetPath("qres")) ||
      !SUCCEEDED(m_psl->SetArguments(szArgs)) ||
      !SUCCEEDED(m_psl->Resolve(hwnd, SLR_ANY_MATCH)) )
    return FALSE;

  //Restore the original icon location and working dir
  m_psl->SetIconLocation(szIcon, iIcon);
  if (*szWorkDir)
    m_psl->SetWorkingDirectory(szWorkDir);

  if (fSave)
  {
    if (!SUCCEEDED(m_ppf->Save(m_fNew ? m_wszLink : NULL, TRUE)))
      return FALSE;
    if (!m_fNew)
      m_ppf->SaveCompleted(m_wszLink);
  }

  return TRUE;
}


// RemQRes removes QRes from the current link
BOOL CShortcut::RemQRes(HWND hwnd)
{
  QRES_PARS tPars;
  char      szIcon[MAX_PATH], *szApp, *szArgs;
  char      szWorkDir[MAX_PATH];
  int       iIcon, iLen;

  if (!m_psl || !m_ppf || !*m_wszLink)
    return FALSE;

  if (!IsQResLink())
    return TRUE;  // Nothing to do if it isn't a QRes link

  // Preserve icon location and working directory
  *szIcon = 0; *szWorkDir = 0;
  m_psl->GetIconLocation(szIcon, sizeof(szIcon), &iIcon);
  m_psl->GetWorkingDirectory(szWorkDir, sizeof(szWorkDir));

  // Get current arguments
  if (!IsQResLink())
    return TRUE; // already OK

  if (GetQResPars(&tPars) <= 0)
    return FALSE; // failed to get qres parameters

  // parse application name and its command line arguments
  szApp = tPars.szApp;
  iLen  = strlen(szApp);
  if (iLen < sizeof(tPars.szApp))
    memset(szApp + iLen, 0, sizeof(tPars.szApp)-iLen);
  
  while(*szApp == ' ') szApp++; // skip leading spaces
  if (*szApp == '\"')
  {
    szArgs = szApp;
    while(*szArgs == '\"') szArgs++;
    szApp = szArgs;
    szArgs = strchr(szArgs, '\"');
  }
  else
  {
    // Name might contain spaces but not be quote-delimited.
    // Assume it must contain a '.' for the extension.
    szArgs = strchr(szApp, '.');
    if (szArgs)
      szArgs = strchr(szArgs, ' ');
  }
  if (szArgs)
  {
    *szArgs = 0;
    szArgs++;
    while(*szArgs && strchr(" \"",*szArgs)) szArgs++;
  }
  if (!szArgs)
    szArgs = "";

  if (!SUCCEEDED(m_psl->SetPath(szApp)) ||
      !SUCCEEDED(m_psl->SetArguments(szArgs)) ||
      !SUCCEEDED(m_psl->Resolve(hwnd, SLR_ANY_MATCH)) )
    return FALSE;

  //Restore the original icon location and working dir
  m_psl->SetIconLocation(szIcon, iIcon);
  if (*szWorkDir)
    m_psl->SetWorkingDirectory(szWorkDir);

  if (!SUCCEEDED(m_ppf->Save(NULL, TRUE)))
    return FALSE;

  m_ppf->SaveCompleted(m_wszLink);
  return TRUE;
}


BOOL CShortcut::TestQResPars(QRES_PARS *ptPars, HWND hwnd)
{
  char                szTarget[2 * MAX_PATH], szArgs[MAX_PATH];
  char                szWorkingDir[MAX_PATH];
  STARTUPINFO         tStartup;
  PROCESS_INFORMATION tProcess;
  int                 iShow;

  if (!SetQResPars(ptPars, hwnd, FALSE) ||
      !GetTarget(szTarget, sizeof(szTarget)) ||
      !SUCCEEDED(m_psl->GetArguments(szArgs, sizeof(szArgs))))
    return FALSE;

  memset(&tStartup, 0, sizeof(STARTUPINFO));
  tStartup.cb = sizeof(STARTUPINFO);
  if(SUCCEEDED(m_psl->GetShowCmd(&iShow)))
  {
    tStartup.wShowWindow = (USHORT)iShow;
    tStartup.dwFlags = STARTF_USESHOWWINDOW;
  }
  *szWorkingDir = 0;
  m_psl->GetWorkingDirectory(szWorkingDir, sizeof(szWorkingDir));
  strcat(szTarget, " /R ");
  strcat(szTarget, szArgs);

  if (CreateProcess(NULL, 
          szTarget,       // pointer to command line string
		  NULL,           // pointer to process security attributes 
		  NULL,           // pointer to thread security attributes 
		  FALSE,          // handle inheritance flag 
		  0L,             // creation flags 
          NULL,           // pointer to new environment block 
          NULL,           // pointer to current directory name 
          &tStartup,      // pointer to STARTUPINFO 
          &tProcess       // pointer to PROCESS_INFORMATION  
        ))
  {
    // Detach child process
    CloseHandle(tProcess.hThread);
    CloseHandle(tProcess.hProcess);

    return TRUE;
  }

  return FALSE;
}


/////////////////////////////////////////////////////////
// Helper functions

// GetSpecialFolderLocation provides a more friendly wrapper for
// SHGetSpecialFolderLocation. szPathRet must point to a buffer
// of at least MAX_PATH bytes.
BOOL GetSpecialFolderLocation(int iFolder, char *szPathRet)
{
  BOOL         fResult = FALSE;
  LPMALLOC     lpMalloc=NULL;
  LPITEMIDLIST pIdl=NULL;

  if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, 
                iFolder, &pIdl)) &&
      SHGetPathFromIDList(pIdl, szPathRet))
  {
    if (szPathRet[strlen(szPathRet)-1] != '\\')
      strcat(szPathRet, "\\");
    fResult = TRUE;
  }
  if (pIdl && SUCCEEDED(SHGetMalloc(&lpMalloc)))
  {
    lpMalloc->Free(pIdl);
    lpMalloc->Release();
  }
  return fResult;
}
