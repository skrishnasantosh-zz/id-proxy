#include "BrowserFrame.h"

AdClientSite::AdClientSite(AdBrowserFrame* frame) :	m_frame(nullptr)
{
	m_frame = frame;
}

HRESULT __stdcall AdClientSite::QueryInterface(REFIID riid, void** ppvObject)
{
	if (m_frame == nullptr)
		return E_FAIL;

	if (ppvObject == nullptr)
		return E_POINTER;

	*ppvObject = nullptr;

	if (!memcmp((const void*)& riid, (const void*)& IID_IUnknown, sizeof(GUID)) ||
		!memcmp((const void*)& riid, (const void*)& IID_IOleClientSite, sizeof(GUID))) 	
		*ppvObject = this;			

	else if (!memcmp((const void*)& riid, &IID_IOleInPlaceSite, sizeof(GUID))) 	
		*ppvObject = m_frame->GetInPlaceSite();			

	else if (!memcmp((const void*)& riid, &IID_IDocHostUIHandler, sizeof(GUID))) 	
		*ppvObject = m_frame->GetDocHostUiHandler();			

	else if (riid == DIID_DWebBrowserEvents2 || riid == IID_IDispatch)
		*ppvObject = m_frame->GetSink();			

	if (*ppvObject != nullptr)
		return S_OK;

	return E_NOINTERFACE;	
}

ULONG __stdcall AdClientSite::AddRef()
{
	return ALWAYS_ON_OBJECT;
}

ULONG __stdcall AdClientSite::Release()
{
	return ALWAYS_ON_OBJECT;
}

HRESULT __stdcall AdClientSite::SaveObject()
{
	return E_NOTIMPL;
}

HRESULT __stdcall AdClientSite::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker** ppmk)
{
	return E_NOTIMPL;
}

HRESULT __stdcall AdClientSite::GetContainer(LPOLECONTAINER* ppContainer)
{
	if (ppContainer != nullptr)
		*ppContainer = nullptr;

	return E_NOTIMPL;
}

HRESULT __stdcall AdClientSite::ShowObject()
{
	return S_OK;
}

HRESULT __stdcall AdClientSite::OnShowWindow(BOOL fShow)
{
	return E_NOTIMPL;
}

HRESULT __stdcall AdClientSite::RequestNewObjectLayout()
{
	return E_NOTIMPL;
}
