#ifndef SHORTCUT_H
#define SHORTCUT_H

#include <shlobj.h>   // for shell stuff
#include <qreslib.h>

class CShortcut
{
public:
  CShortcut();
  ~CShortcut();
  BOOL Load(LPSTR szLink);
  BOOL Resolve(HWND hwnd = NULL);
  BOOL GetTarget(LPSTR szRet, int cbMaxRet);
  BOOL GetPath(LPSTR szRet, int cbMaxRet);

  // Specific QRes stuff
  BOOL IsQResLink(void);
  int  GetQResPars(QRES_PARS *ptParsRet);
  BOOL SetQResPars(QRES_PARS *ptPars, HWND hwnd = NULL,  BOOL fSave = TRUE);
  BOOL RemQRes(HWND hwnd = NULL);
  void GetFriendlyName(char *szRet, UINT wMaxRet);

private:
  IShellLink   *m_psl;               // IShellLink interface
  IPersistFile *m_ppf;               // IPersistFile interface
  WCHAR         m_wszLink[MAX_PATH]; // Location of the .lnk file
};

// Helper functions
BOOL GetSpecialFolderLocation(int iFolder, char *szPathRet);

#endif /* #ifndef SHORTCUT_H */