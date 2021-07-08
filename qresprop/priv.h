//--------------------------------------------------------------
// common user interface routines
//
//
//--------------------------------------------------------------

#ifndef STRICT
#define STRICT
#endif

#define INC_OLE2        // WIN32, get ole2 from windows.h

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>  
#include <stdio.h>
#include <shlobj.h>         // IShell interface

#define ResultFromShort(i)  ResultFromScode(MAKE_SCODE(SEVERITY_SUCCESS, 0, (USHORT)(i)))

