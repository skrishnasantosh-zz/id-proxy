#include <windows.h>
#include "BrowserFrame.h"

AdBrowserEventSink::AdBrowserEventSink() : m_refCount(0) { }

STDMETHODIMP AdBrowserEventSink::QueryInterface(REFIID riid, void** ppvObject)
{	
	if (IsBadWritePtr(ppvObject, sizeof(void*))) 
		return E_POINTER;
	
	(*ppvObject) = NULL;

	if (!IsEqualIID(riid, IID_IUnknown) && 
		!IsEqualIID(riid, IID_IDispatch) && 
		!IsEqualIID(riid, DIID_DWebBrowserEvents2)) 		
		return E_NOINTERFACE;
		
	(*ppvObject) = (void*)this;
	return S_OK;
}

STDMETHODIMP_(ULONG) AdBrowserEventSink::AddRef()
{
	return ::InterlockedIncrement(&m_refCount);
}

STDMETHODIMP_(ULONG) AdBrowserEventSink::Release()
{
	return ::InterlockedDecrement(&m_refCount);
}

STDMETHODIMP AdBrowserEventSink::GetTypeInfoCount(UINT* pctinfo)
{
	UNREFERENCED_PARAMETER(pctinfo);

	return E_NOTIMPL;
}

STDMETHODIMP AdBrowserEventSink::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo)
{
	UNREFERENCED_PARAMETER(iTInfo);
	UNREFERENCED_PARAMETER(lcid);
	UNREFERENCED_PARAMETER(ppTInfo);

	return E_NOTIMPL;
}

STDMETHODIMP AdBrowserEventSink::GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId)
{
	UNREFERENCED_PARAMETER(riid);
	UNREFERENCED_PARAMETER(rgszNames);
	UNREFERENCED_PARAMETER(cNames);
	UNREFERENCED_PARAMETER(lcid);
	UNREFERENCED_PARAMETER(rgDispId);

	return E_NOTIMPL;
}

STDMETHODIMP AdBrowserEventSink::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
	UNREFERENCED_PARAMETER(lcid);
	UNREFERENCED_PARAMETER(wFlags);
	UNREFERENCED_PARAMETER(pVarResult);
	UNREFERENCED_PARAMETER(pExcepInfo);
	UNREFERENCED_PARAMETER(puArgErr);
	VARIANT v[5]; 
	int n;
	bool b;
	PVOID pv;
	LONG lbound, ubound, sz;

	if (!IsEqualIID(riid, IID_NULL)) 
		return DISP_E_UNKNOWNINTERFACE; 

	// Initialize the variants
	for (n = 0; n < 5; n++) 
		VariantInit(&v[n]);

	switch (dispIdMember)
	{
	case DISPID_BEFORENAVIGATE2:	
		VariantChangeType(&v[0], &pDispParams->rgvarg[5], 0, VT_BSTR); // url
		VariantChangeType(&v[1], &pDispParams->rgvarg[4], 0, VT_I4); // Flags
		VariantChangeType(&v[2], &pDispParams->rgvarg[3], 0, VT_BSTR); // TargetFrameName
		VariantChangeType(&v[3], &pDispParams->rgvarg[2], 0, VT_UI1 | VT_ARRAY); // PostData
		VariantChangeType(&v[4], &pDispParams->rgvarg[1], 0, VT_BSTR); // Headers
		if (v[3].vt != VT_EMPTY) {
			SafeArrayGetLBound(v[3].parray, 0, &lbound);
			SafeArrayGetUBound(v[3].parray, 0, &ubound);
			sz = ubound - lbound + 1;
			SafeArrayAccessData(v[3].parray, &pv);
		}
		else {
			sz = 0;
			pv = NULL;
		}
		b = Event_BeforeNavigate2((LPOLESTR)v[0].bstrVal, v[1].lVal, (LPOLESTR)v[2].bstrVal, (PUCHAR)pv, sz, (LPOLESTR)v[4].bstrVal, ((*(pDispParams->rgvarg[0].pboolVal)) != VARIANT_FALSE));
		if (v[3].vt != VT_EMPTY) SafeArrayUnaccessData(v[3].parray);
		if (b) * (pDispParams->rgvarg[0].pboolVal) = VARIANT_TRUE;
		else *(pDispParams->rgvarg[0].pboolVal) = VARIANT_FALSE;
		break;	
	}
	
	for (n = 0; n < 5; n++) 
		VariantClear(&v[n]);

	return S_OK;
}

bool AdBrowserEventSink::Event_BeforeNavigate2(LPOLESTR url, LONG Flags, LPOLESTR TargetFrameName, PUCHAR PostData, LONG PostDataSize, LPOLESTR Headers, bool Cancel)
{
	TCHAR msg[1024];
	wsprintf(msg, TEXT("url=%ls\nFlags=0x%08X\nTargetFrameName=%ls\nPostData=%hs\nPostDataSize=%d\nHeaders=%ls\nCancel=%s"), url, Flags, TargetFrameName, (char*)PostData, PostDataSize, Headers, ((Cancel) ? (TEXT("true")) : (TEXT("false"))));
	MessageBox(NULL, msg, TEXT("BeforeNavigate2 event fired!"), MB_OK | MB_ICONINFORMATION);
	return Cancel; //TRUE to prevent IE from opening the page
}
