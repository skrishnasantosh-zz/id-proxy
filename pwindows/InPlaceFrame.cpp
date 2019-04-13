#include "BrowserFrame.h"

AdInPlaceFrame::AdInPlaceFrame(AdBrowserFrame* frame) : m_mainFrame(nullptr)
{	
	m_mainFrame = frame;
}

HRESULT STDMETHODCALLTYPE AdInPlaceFrame::GetWindow(HWND* lphwnd)
{
	if (lphwnd == nullptr)
		return E_POINTER;

	if (m_mainFrame == nullptr)
		return E_FAIL;

	*lphwnd = m_mainFrame->GetHwnd();

	
	return S_OK;
}

HRESULT STDMETHODCALLTYPE AdInPlaceFrame::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
	*ppvObj = nullptr;

	return E_NOTIMPL;
}

ULONG STDMETHODCALLTYPE AdInPlaceFrame::AddRef()
{
	return ALWAYS_ON_OBJECT;
}

ULONG STDMETHODCALLTYPE AdInPlaceFrame::Release()
{
	return ALWAYS_ON_OBJECT;
}

HRESULT STDMETHODCALLTYPE AdInPlaceFrame::ContextSensitiveHelp(BOOL fEnterMode)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE AdInPlaceFrame::GetBorder(LPRECT lprectBorder)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE AdInPlaceFrame::RequestBorderSpace(LPCBORDERWIDTHS pborderwidths)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE AdInPlaceFrame::SetBorderSpace(LPCBORDERWIDTHS pborderwidths)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE AdInPlaceFrame::SetActiveObject(IOleInPlaceActiveObject* pActiveObject, LPCOLESTR pszObjName)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE AdInPlaceFrame::InsertMenus(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE AdInPlaceFrame::SetMenu(HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE AdInPlaceFrame::RemoveMenus(HMENU hmenuShared)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE AdInPlaceFrame::SetStatusText(LPCOLESTR pszStatusText)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE AdInPlaceFrame::EnableModeless(BOOL fEnable)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE AdInPlaceFrame::TranslateAccelerator(LPMSG lpmsg, WORD wID)
{
	return E_NOTIMPL;
}
