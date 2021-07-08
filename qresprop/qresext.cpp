// MODULE:   qresext.cpp
//
// Purpose:  Implements the class factory code as well as CQResExt::QI,
//           CQResExt::AddRef and CQResExt::Release code.
//
// Copyright 1998-2005 Berend Engelbrecht. All rights reserved.
#if defined(__BORLANDC__)
#pragma warn -8057 // suppress unused parameter warnings in BCPP 5.0
#endif

#include "priv.h"

//
// Initialize GUIDs (should be done only and at-least once per DLL/EXE)
//
#pragma data_seg(".text")
#define INITGUID
#include <initguid.h>
#include <shlguid.h>
#include "qresext.h"
#pragma data_seg()

//
// Global variables
//
UINT      g_cRefThisDll = 0;    // Reference count of this DLL.
HINSTANCE g_hmodThisDll = NULL;	// Handle to this DLL itself.

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        ODS("In DLLMain, DLL_PROCESS_ATTACH\r\n");

        // Extension DLL one-time initialization

        g_hmodThisDll = hInstance;
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        ODS("In DLLMain, DLL_PROCESS_DETACH\r\n");
    }

    return 1;   // ok
}

//---------------------------------------------------------------------------
// DllCanUnloadNow
//---------------------------------------------------------------------------

STDAPI DllCanUnloadNow(void)
{
    ODS("In DLLCanUnloadNow\r\n");

    return (g_cRefThisDll == 0 ? S_OK : S_FALSE);
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppvOut)
{
    ODS("In DllGetClassObject\r\n");

    *ppvOut = NULL;

    if (IsEqualIID(rclsid, CLSID_QResExtension))
    {
        CQResExtClassFactory *pcf = new CQResExtClassFactory;

        return pcf->QueryInterface(riid, ppvOut);
    }

    return CLASS_E_CLASSNOTAVAILABLE;
}

CQResExtClassFactory::CQResExtClassFactory()
{
    ODS("CQResExtClassFactory::CQResExtClassFactory()\r\n");

    m_cRef = 0L;

//    if (!g_cRefThisDll)
    CoInitialize(NULL);
    g_cRefThisDll++;	
}
																
CQResExtClassFactory::~CQResExtClassFactory()				
{
    g_cRefThisDll--;
//    if (!g_cRefThisDll)
    CoUninitialize();
}

STDMETHODIMP CQResExtClassFactory::QueryInterface(REFIID riid,
                                                   LPVOID FAR *ppv)
{
    ODS("CQResExtClassFactory::QueryInterface()\r\n");

    *ppv = NULL;

    // Any interface on this object is the object pointer

    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
        *ppv = (LPCLASSFACTORY)this;

        AddRef();

        return NOERROR;
    }

    return E_NOINTERFACE;
}	

STDMETHODIMP_(ULONG) CQResExtClassFactory::AddRef()
{
    return ++m_cRef;
}

STDMETHODIMP_(ULONG) CQResExtClassFactory::Release()
{
    if (--m_cRef)
        return m_cRef;

    delete this;

    return 0L;
}

STDMETHODIMP CQResExtClassFactory::CreateInstance(LPUNKNOWN pUnkOuter,
                                                      REFIID riid,
                                                      LPVOID *ppvObj)
{
    ODS("CQResExtClassFactory::CreateInstance()\r\n");

    *ppvObj = NULL;

    // Shell extensions typically don't support aggregation (inheritance)

    if (pUnkOuter)
    	return CLASS_E_NOAGGREGATION;

    // Create the main shell extension object.  The shell will then call
    // QueryInterface with IID_IShellExtInit--this is how shell extensions are
    // initialized.

    LPCQRESEXT pQResExt = new CQResExt();  //Create the CQResExt object

    if (NULL == pQResExt)
    	return E_OUTOFMEMORY;

    return pQResExt->QueryInterface(riid, ppvObj);
}


STDMETHODIMP CQResExtClassFactory::LockServer(BOOL fLock)
{
    return NOERROR;
}

// *********************** CQResExt *************************
CQResExt::CQResExt()
{
    ODS("CQResExt::CQResExt()\r\n");

    m_cRef = 0L;
    m_pDataObj = NULL;

    g_cRefThisDll++;
}

CQResExt::~CQResExt()
{
    if (m_pDataObj)
        m_pDataObj->Release();

    g_cRefThisDll--;
}

STDMETHODIMP CQResExt::QueryInterface(REFIID riid, LPVOID FAR *ppv)
{
    *ppv = NULL;

    if (IsEqualIID(riid, IID_IShellExtInit) || IsEqualIID(riid, IID_IUnknown))
    {
        ODS("CQResExt::QueryInterface()==>IID_IShellExtInit\r\n");

    	*ppv = (LPSHELLEXTINIT)this;
    }
    else if (IsEqualIID(riid, IID_IContextMenu))
    {
        ODS("CQResExt::QueryInterface()==>IID_IContextMenu\r\n");

        *ppv = (LPCONTEXTMENU)this;
    }
    else if (IsEqualIID(riid, IID_IExtractIcon))
    {
        ODS("CQResExt::QueryInterface()==>IID_IExtractIcon\r\n");

        *ppv = (LPEXTRACTICON)this;
    }
    else if (IsEqualIID(riid, IID_IPersistFile))
    {
        ODS("CQResExt::QueryInterface()==>IPersistFile\r\n");

        *ppv = (LPPERSISTFILE)this;
    }
    else if (IsEqualIID(riid, IID_IShellPropSheetExt))
    {
        ODS("CQResExt::QueryInterface()==>IShellPropSheetExt\r\n");

        *ppv = (LPSHELLPROPSHEETEXT)this;
    }
    else if (IsEqualIID(riid, IID_IShellCopyHook))
    {
        ODS("CQResExt::QueryInterface()==>ICopyHook\r\n");

        *ppv = (LPCOPYHOOK)this;
    }

    if (*ppv)
    {
        AddRef();

        return NOERROR;
    }

    ODS("CQResExt::QueryInterface()==>Unknown Interface!\r\n");

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CQResExt::AddRef()
{
    ODS("CQResExt::AddRef()\r\n");

    return ++m_cRef;
}

STDMETHODIMP_(ULONG) CQResExt::Release()
{
    ODS("CQResExt::Release()\r\n");

    if (--m_cRef)
        return m_cRef;

    delete this;

    return 0L;
}
