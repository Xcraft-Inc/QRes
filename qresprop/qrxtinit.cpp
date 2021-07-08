// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993-1995  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE:   shexinit.cpp
//
//  PURPOSE:   Implements the IShellExtInit member function necessary to support
//             the context menu and property sheet portioins of this shell extension.  
//
#if defined(__BORLANDC__)
#pragma warn -8057 // suppress unused parameter warnings in BCPP 5.0
#endif

#include "priv.h"
#include "qresext.h"


//
//  FUNCTION: CQResExt::Initialize(LPCITEMIDLIST, LPDATAOBJECT, HKEY)
//
//  PURPOSE: Called by the shell when initializing a context menu or property
//           sheet extension.
//
//  PARAMETERS:
//    pIDFolder - Specifies the parent folder
//    pDataObj  - Spefifies the set of items selected in that folder.
//    hRegKey   - Specifies the type of the focused item in the selection.
//
//  RETURN VALUE:
//
//    NOERROR in all cases.
//
//  COMMENTS:   Note that at the time this function is called, we don't know 
//              (or care) what type of shell extension is being initialized.  
//              It could be a context menu or a property sheet.
//

STDMETHODIMP CQResExt::Initialize(LPCITEMIDLIST pIDFolder,
                                   LPDATAOBJECT pDataObj,
                                   HKEY hRegKey)
{
    ODS("CQResExt::Initialize()\r\n");

    // Initialize can be called more than once

    if (m_pDataObj)
    	m_pDataObj->Release();

    // duplicate the object pointer and registry handle

    if (pDataObj)
    {
    	m_pDataObj = pDataObj;
    	pDataObj->AddRef();
    }

    return NOERROR;
}
