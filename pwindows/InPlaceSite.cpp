#include "BrowserFrame.h"

AdInPlaceSite::AdInPlaceSite(AdBrowserFrame* frame) : m_frame(nullptr)
{
	m_frame = frame;
}

HRESULT STDMETHODCALLTYPE AdInPlaceSite::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
	if (ppvObj == nullptr)
		return E_POINTER;

	if (m_frame == nullptr)
		return E_FAIL;

	auto client = m_frame->GetClientSite();

	if (client != nullptr)
		return client->QueryInterface(riid, ppvObj);

	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE AdInPlaceSite::AddRef()
{
	return ALWAYS_ON_OBJECT;
}

ULONG STDMETHODCALLTYPE AdInPlaceSite::Release()
{
	return ALWAYS_ON_OBJECT;
}

HRESULT STDMETHODCALLTYPE AdInPlaceSite::GetWindow(HWND* lphwnd)
{
	if (m_frame == nullptr)
		return E_FAIL;

	if (lphwnd == nullptr)
		return E_POINTER;

	*lphwnd = m_frame->GetHwnd();

	return S_OK;
}

HRESULT STDMETHODCALLTYPE AdInPlaceSite::ContextSensitiveHelp(BOOL fEnterMode)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE AdInPlaceSite::CanInPlaceActivate()
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE AdInPlaceSite::OnInPlaceActivate()
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE AdInPlaceSite::OnUIActivate()
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE AdInPlaceSite::GetWindowContext(LPOLEINPLACEFRAME* lplpFrame, LPOLEINPLACEUIWINDOW* lplpDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	if (m_frame == nullptr)
		return E_FAIL;

	if (lplpFrame == nullptr || lpFrameInfo == nullptr)
		return E_POINTER;

	*lplpFrame = m_frame->GetInPlaceFrame();
	*lplpDoc = nullptr;

	lpFrameInfo->cAccelEntries = 0;
	lpFrameInfo->hwndFrame = m_frame->GetHwnd();
	lpFrameInfo->haccel = 0;
	lpFrameInfo->fMDIApp = FALSE;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE AdInPlaceSite::Scroll(SIZE scrollExtent)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE AdInPlaceSite::OnUIDeactivate(BOOL fUndoable)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE AdInPlaceSite::OnInPlaceDeactivate()
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE AdInPlaceSite::DiscardUndoState()
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE AdInPlaceSite::DeactivateAndUndo()
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE AdInPlaceSite::OnPosRectChange(LPCRECT lprcPosRect)
{
	IOleInPlaceObject* inPlaceObject;

	if (m_frame == nullptr)
		return E_FAIL;

	auto webBrowser = m_frame->GetWebBrowser();

	if (webBrowser == nullptr)
		return E_FAIL;

	if (SUCCEEDED(webBrowser->QueryInterface(IID_IOleInPlaceObject, (void**)&inPlaceObject))) {
		inPlaceObject->SetObjectRects(lprcPosRect, lprcPosRect);
		inPlaceObject->Release();
	}
		
	return S_OK;
}
