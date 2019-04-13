#include "BrowserFrame.h"

AdDocHostUiHandler::AdDocHostUiHandler(AdBrowserFrame* frame)
{
	if (frame != nullptr)
	{
		m_browserFrame = frame;		
	}
}

HRESULT STDMETHODCALLTYPE AdDocHostUiHandler::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
	auto clientSite = m_browserFrame->GetClientSite();

	if (clientSite != nullptr)	
		return clientSite->QueryInterface(riid, ppvObj);
	
	return E_NOTIMPL;
}

ULONG STDMETHODCALLTYPE AdDocHostUiHandler::AddRef()
{
	return ALWAYS_ON_OBJECT;
}

ULONG STDMETHODCALLTYPE AdDocHostUiHandler::Release()
{
	return ALWAYS_ON_OBJECT;
}

HRESULT STDMETHODCALLTYPE AdDocHostUiHandler::ShowContextMenu(DWORD dwID, POINT* ppt, IUnknown* pcmdtReserved, IDispatch* pdispReserved)
{
	return S_OK; //Do not display context menu in the browser. Signal that we already handled it
}

HRESULT STDMETHODCALLTYPE AdDocHostUiHandler::ShowUI(DWORD dwID, IOleInPlaceActiveObject* pActiveObject, IOleCommandTarget* pCommandTarget, IOleInPlaceFrame* pFrame, IOleInPlaceUIWindow* pDoc)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE AdDocHostUiHandler::GetHostInfo(DOCHOSTUIINFO* pInfo)
{
	if (pInfo == nullptr)
		return E_POINTER;

	pInfo->cbSize = sizeof(DOCHOSTUIINFO);
	pInfo->dwFlags = DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_NO3DOUTERBORDER | DOCHOSTUIFLAG_FLAT_SCROLLBAR;

	pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE AdDocHostUiHandler::HideUI()
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE AdDocHostUiHandler::UpdateUI()
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE AdDocHostUiHandler::EnableModeless(BOOL fEnable)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE AdDocHostUiHandler::OnDocWindowActivate(BOOL fActivate)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE AdDocHostUiHandler::OnFrameWindowActivate(BOOL fActivate)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE AdDocHostUiHandler::ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow* pUIWindow, BOOL fRameWindow)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE AdDocHostUiHandler::TranslateAccelerator(LPMSG lpMsg, const GUID* pguidCmdGroup, DWORD nCmdID)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE AdDocHostUiHandler::GetOptionKeyPath(LPOLESTR* pchKey, DWORD dw)
{
	return S_FALSE;
}

HRESULT STDMETHODCALLTYPE AdDocHostUiHandler::GetDropTarget(IDropTarget* pDropTarget, IDropTarget** ppDropTarget)
{
	return S_FALSE;
}

HRESULT STDMETHODCALLTYPE AdDocHostUiHandler::GetExternal(IDispatch** ppDispatch)
{
	if (ppDispatch != nullptr)
		*ppDispatch = nullptr;

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE AdDocHostUiHandler::TranslateUrl(DWORD dwTranslate, OLECHAR* pchURLIn, OLECHAR** ppchURLOut)
{
	return S_FALSE;
}

HRESULT STDMETHODCALLTYPE AdDocHostUiHandler::FilterDataObject(IDataObject* pDO, IDataObject** ppDORet)
{
	if (ppDORet != nullptr)
		*ppDORet = nullptr;

	return S_FALSE;
}
