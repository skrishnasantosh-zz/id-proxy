#include <windows.h>
#include <math.h>
#include <exdisp.h>		
#include <mshtml.h>		
#include <mshtmhst.h>	
#include <crtdbg.h>		// for _ASSERT()
#include <shcore.h>
#include <ShellScalingApi.h> //For DPI Aware operations
#pragma comment(lib,"Shcore.lib")

#include "..\platform\platform.h"

#define IE_VERSION 0x02710 //IE 10+ (0x2328 for IE9) 


#ifndef GWL_USERDATA
#define GWL_USERDATA (-21)
#endif

DWORD g_adviseCookie = 0;

HWND g_hWnd;
volatile long g_nBrowserCount = 0; //For Auditing

static const TCHAR	ClassName[] = TEXT("IdProxyWin");

// Our IOleInPlaceFrame functions that the browser may call
HRESULT STDMETHODCALLTYPE Frame_QueryInterface(IOleInPlaceFrame FAR* This, REFIID riid, LPVOID FAR* ppvObj);
HRESULT STDMETHODCALLTYPE Frame_AddRef(IOleInPlaceFrame FAR* This);
HRESULT STDMETHODCALLTYPE Frame_Release(IOleInPlaceFrame FAR* This);
HRESULT STDMETHODCALLTYPE Frame_GetWindow(IOleInPlaceFrame FAR* This, HWND FAR* lphwnd);
HRESULT STDMETHODCALLTYPE Frame_ContextSensitiveHelp(IOleInPlaceFrame FAR* This, BOOL fEnterMode);
HRESULT STDMETHODCALLTYPE Frame_GetBorder(IOleInPlaceFrame FAR* This, LPRECT lprectBorder);
HRESULT STDMETHODCALLTYPE Frame_RequestBorderSpace(IOleInPlaceFrame FAR* This, LPCBORDERWIDTHS pborderwidths);
HRESULT STDMETHODCALLTYPE Frame_SetBorderSpace(IOleInPlaceFrame FAR* This, LPCBORDERWIDTHS pborderwidths);
HRESULT STDMETHODCALLTYPE Frame_SetActiveObject(IOleInPlaceFrame FAR* This, IOleInPlaceActiveObject* pActiveObject, LPCOLESTR pszObjName);
HRESULT STDMETHODCALLTYPE Frame_InsertMenus(IOleInPlaceFrame FAR* This, HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths);
HRESULT STDMETHODCALLTYPE Frame_SetMenu(IOleInPlaceFrame FAR* This, HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject);
HRESULT STDMETHODCALLTYPE Frame_RemoveMenus(IOleInPlaceFrame FAR* This, HMENU hmenuShared);
HRESULT STDMETHODCALLTYPE Frame_SetStatusText(IOleInPlaceFrame FAR* This, LPCOLESTR pszStatusText);
HRESULT STDMETHODCALLTYPE Frame_EnableModeless(IOleInPlaceFrame FAR* This, BOOL fEnable);
HRESULT STDMETHODCALLTYPE Frame_TranslateAccelerator(IOleInPlaceFrame FAR* This, LPMSG lpmsg, WORD wID);

//Build the vtable
IOleInPlaceFrameVtbl MyIOleInPlaceFrameTable = { Frame_QueryInterface,
Frame_AddRef,
Frame_Release,
Frame_GetWindow,
Frame_ContextSensitiveHelp,
Frame_GetBorder,
Frame_RequestBorderSpace,
Frame_SetBorderSpace,
Frame_SetActiveObject,
Frame_InsertMenus,
Frame_SetMenu,
Frame_RemoveMenus,
Frame_SetStatusText,
Frame_EnableModeless,
Frame_TranslateAccelerator };

typedef struct {
	IOleInPlaceFrame	frame;		// The IOleInPlaceFrame must be first!

	//TODO: Extras
	HWND				window;
} _IOleInPlaceFrameEx;

// IOleClientSite 
HRESULT STDMETHODCALLTYPE Site_QueryInterface(IOleClientSite FAR* This, REFIID riid, void** ppvObject);
HRESULT STDMETHODCALLTYPE Site_AddRef(IOleClientSite FAR* This);
HRESULT STDMETHODCALLTYPE Site_Release(IOleClientSite FAR* This);
HRESULT STDMETHODCALLTYPE Site_SaveObject(IOleClientSite FAR* This);
HRESULT STDMETHODCALLTYPE Site_GetMoniker(IOleClientSite FAR* This, DWORD dwAssign, DWORD dwWhichMoniker, IMoniker** ppmk);
HRESULT STDMETHODCALLTYPE Site_GetContainer(IOleClientSite FAR* This, LPOLECONTAINER FAR* ppContainer);
HRESULT STDMETHODCALLTYPE Site_ShowObject(IOleClientSite FAR* This);
HRESULT STDMETHODCALLTYPE Site_OnShowWindow(IOleClientSite FAR* This, BOOL fShow);
HRESULT STDMETHODCALLTYPE Site_RequestNewObjectLayout(IOleClientSite FAR* This);

// vtable 
IOleClientSiteVtbl MyIOleClientSiteTable = { Site_QueryInterface,
Site_AddRef,
Site_Release,
Site_SaveObject,
Site_GetMoniker,
Site_GetContainer,
Site_ShowObject,
Site_OnShowWindow,
Site_RequestNewObjectLayout };

// IDocHostUIHandler 
HRESULT STDMETHODCALLTYPE UI_QueryInterface(IDocHostUIHandler FAR* This, REFIID riid, void** ppvObject);
HRESULT STDMETHODCALLTYPE UI_AddRef(IDocHostUIHandler FAR* This);
HRESULT STDMETHODCALLTYPE UI_Release(IDocHostUIHandler FAR* This);
HRESULT STDMETHODCALLTYPE UI_ShowContextMenu(IDocHostUIHandler FAR* This, DWORD dwID, POINT __RPC_FAR* ppt, IUnknown __RPC_FAR* pcmdtReserved, IDispatch __RPC_FAR* pdispReserved);
HRESULT STDMETHODCALLTYPE UI_GetHostInfo(IDocHostUIHandler FAR* This, DOCHOSTUIINFO __RPC_FAR* pInfo);
HRESULT STDMETHODCALLTYPE UI_ShowUI(IDocHostUIHandler FAR* This, DWORD dwID, IOleInPlaceActiveObject __RPC_FAR* pActiveObject, IOleCommandTarget __RPC_FAR* pCommandTarget, IOleInPlaceFrame __RPC_FAR* pFrame, IOleInPlaceUIWindow __RPC_FAR* pDoc);
HRESULT STDMETHODCALLTYPE UI_HideUI(IDocHostUIHandler FAR* This);
HRESULT STDMETHODCALLTYPE UI_UpdateUI(IDocHostUIHandler FAR* This);
HRESULT STDMETHODCALLTYPE UI_EnableModeless(IDocHostUIHandler FAR* This, BOOL fEnable);
HRESULT STDMETHODCALLTYPE UI_OnDocWindowActivate(IDocHostUIHandler FAR* This, BOOL fActivate);
HRESULT STDMETHODCALLTYPE UI_OnFrameWindowActivate(IDocHostUIHandler FAR* This, BOOL fActivate);
HRESULT STDMETHODCALLTYPE UI_ResizeBorder(IDocHostUIHandler FAR* This, LPCRECT prcBorder, IOleInPlaceUIWindow __RPC_FAR* pUIWindow, BOOL fRameWindow);
HRESULT STDMETHODCALLTYPE UI_TranslateAccelerator(IDocHostUIHandler FAR* This, LPMSG lpMsg, const GUID __RPC_FAR* pguidCmdGroup, DWORD nCmdID);
HRESULT STDMETHODCALLTYPE UI_GetOptionKeyPath(IDocHostUIHandler FAR* This, LPOLESTR __RPC_FAR* pchKey, DWORD dw);
HRESULT STDMETHODCALLTYPE UI_GetDropTarget(IDocHostUIHandler FAR* This, IDropTarget __RPC_FAR* pDropTarget, IDropTarget __RPC_FAR* __RPC_FAR* ppDropTarget);
HRESULT STDMETHODCALLTYPE UI_GetExternal(IDocHostUIHandler FAR* This, IDispatch __RPC_FAR* __RPC_FAR* ppDispatch);
HRESULT STDMETHODCALLTYPE UI_TranslateUrl(IDocHostUIHandler FAR* This, DWORD dwTranslate, OLECHAR __RPC_FAR* pchURLIn, OLECHAR __RPC_FAR* __RPC_FAR* ppchURLOut);
HRESULT STDMETHODCALLTYPE UI_FilterDataObject(IDocHostUIHandler FAR* This, IDataObject __RPC_FAR* pDO, IDataObject __RPC_FAR* __RPC_FAR* ppDORet);

// vtable
IDocHostUIHandlerVtbl MyIDocHostUIHandlerTable = { UI_QueryInterface,
UI_AddRef,
UI_Release,
UI_ShowContextMenu,
UI_GetHostInfo,
UI_ShowUI,
UI_HideUI,
UI_UpdateUI,
UI_EnableModeless,
UI_OnDocWindowActivate,
UI_OnFrameWindowActivate,
UI_ResizeBorder,
UI_TranslateAccelerator,
UI_GetOptionKeyPath,
UI_GetDropTarget,
UI_GetExternal,
UI_TranslateUrl,
UI_FilterDataObject };

// IOleInPlaceSite 
HRESULT STDMETHODCALLTYPE InPlace_QueryInterface(IOleInPlaceSite FAR* This, REFIID riid, void** ppvObject);
HRESULT STDMETHODCALLTYPE InPlace_AddRef(IOleInPlaceSite FAR* This);
HRESULT STDMETHODCALLTYPE InPlace_Release(IOleInPlaceSite FAR* This);
HRESULT STDMETHODCALLTYPE InPlace_GetWindow(IOleInPlaceSite FAR* This, HWND FAR* lphwnd);
HRESULT STDMETHODCALLTYPE InPlace_ContextSensitiveHelp(IOleInPlaceSite FAR* This, BOOL fEnterMode);
HRESULT STDMETHODCALLTYPE InPlace_CanInPlaceActivate(IOleInPlaceSite FAR* This);
HRESULT STDMETHODCALLTYPE InPlace_OnInPlaceActivate(IOleInPlaceSite FAR* This);
HRESULT STDMETHODCALLTYPE InPlace_OnUIActivate(IOleInPlaceSite FAR* This);
HRESULT STDMETHODCALLTYPE InPlace_GetWindowContext(IOleInPlaceSite FAR* This, LPOLEINPLACEFRAME FAR* lplpFrame, LPOLEINPLACEUIWINDOW FAR* lplpDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo);
HRESULT STDMETHODCALLTYPE InPlace_Scroll(IOleInPlaceSite FAR* This, SIZE scrollExtent);
HRESULT STDMETHODCALLTYPE InPlace_OnUIDeactivate(IOleInPlaceSite FAR* This, BOOL fUndoable);
HRESULT STDMETHODCALLTYPE InPlace_OnInPlaceDeactivate(IOleInPlaceSite FAR* This);
HRESULT STDMETHODCALLTYPE InPlace_DiscardUndoState(IOleInPlaceSite FAR* This);
HRESULT STDMETHODCALLTYPE InPlace_DeactivateAndUndo(IOleInPlaceSite FAR* This);
HRESULT STDMETHODCALLTYPE InPlace_OnPosRectChange(IOleInPlaceSite FAR* This, LPCRECT lprcPosRect);

// vtable
IOleInPlaceSiteVtbl MyIOleInPlaceSiteTable = { InPlace_QueryInterface,
InPlace_AddRef,
InPlace_Release,
InPlace_GetWindow,
InPlace_ContextSensitiveHelp,
InPlace_CanInPlaceActivate,
InPlace_OnInPlaceActivate,
InPlace_OnUIActivate,
InPlace_GetWindowContext,
InPlace_Scroll,
InPlace_OnUIDeactivate,
InPlace_OnInPlaceDeactivate,
InPlace_DiscardUndoState,
InPlace_DeactivateAndUndo,
InPlace_OnPosRectChange };

typedef struct {
	IOleInPlaceSite			inplace;	// IOleInPlaceSite object
	
	//TODO: Extras
	_IOleInPlaceFrameEx		frame;		// IOleInPlaceFrame object
} _IOleInPlaceSiteEx;

typedef struct {
	IDocHostUIHandler		ui;			// IDocHostUIHandler object

	//TODO: Extras
} _IDocHostUIHandlerEx;

typedef struct {
	IOleClientSite			client;		// IOleClientSite object 
	_IOleInPlaceSiteEx		inplace;	// IOleInPlaceSite object
	_IDocHostUIHandlerEx	ui;			// IDocHostUIHandler object

	//TODO: Extras
} _IOleClientSiteEx;

#define NOTIMPLEMENTED _ASSERT(0); return(E_NOTIMPL)

// proceed if only IE is > 4.0. No IDocHostUIHandler before IE4
HRESULT STDMETHODCALLTYPE UI_QueryInterface(IDocHostUIHandler FAR* This, REFIID riid, LPVOID FAR* ppvObj)
{
	return(Site_QueryInterface((IOleClientSite*)((char*)This - sizeof(IOleClientSite) - sizeof(_IOleInPlaceSiteEx)), riid, ppvObj));
}

HRESULT STDMETHODCALLTYPE UI_AddRef(IDocHostUIHandler FAR * This)
{
	return(1);
}

HRESULT STDMETHODCALLTYPE UI_Release(IDocHostUIHandler FAR * This)
{
	return(1);
}

// Called when the browser object is about to display its context menu.
HRESULT STDMETHODCALLTYPE UI_ShowContextMenu(IDocHostUIHandler FAR * This, DWORD dwID, POINT __RPC_FAR * ppt, IUnknown __RPC_FAR * pcmdtReserved, IDispatch __RPC_FAR * pdispReserved)
{	
	return(S_OK); //No context menu
}

// set browser features 
HRESULT STDMETHODCALLTYPE UI_GetHostInfo(IDocHostUIHandler FAR * This, DOCHOSTUIINFO __RPC_FAR * pInfo)
{
	pInfo->cbSize = sizeof(DOCHOSTUIINFO);

	
	pInfo->dwFlags = DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_DISABLE_HELP_MENU | DOCHOSTUIFLAG_FLAT_SCROLLBAR | DOCHOSTUIFLAG_URL_ENCODING_ENABLE_UTF8 | 
					 DOCHOSTUIFLAG_NOTHEME | 
					 DOCHOSTUIFLAG_LOCAL_MACHINE_ACCESS_CHECK | //Do not hijack cookies
					 DOCHOSTUIFLAG_DISABLE_UNTRUSTEDPROTOCOL |
					 DOCHOSTUIFLAG_ENABLE_REDIRECT_NOTIFICATION | //capture redirects (for access token)
					 DOCHOSTUIFLAG_ENABLE_ACTIVEX_INACTIVATE_MODE | //prevent default activation of ActiveX or Applets
					 DOCHOSTUIFLAG_DPI_AWARE | //Support high resolutions
					 DOCHOSTUIFLAG_IME_ENABLE_RECONVERSION ; //support complex characters such as japanese

	pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;

	return(S_OK);
}

// Called when the browser object shows its UI
HRESULT STDMETHODCALLTYPE UI_ShowUI(IDocHostUIHandler FAR * This, DWORD dwID, IOleInPlaceActiveObject __RPC_FAR * pActiveObject, IOleCommandTarget __RPC_FAR * pCommandTarget, IOleInPlaceFrame __RPC_FAR * pFrame, IOleInPlaceUIWindow __RPC_FAR * pDoc)
{
	// tell the browser not to display its menus/toolbars. 
	return(S_OK);
}

// Called when browser object hides its UI. 
HRESULT STDMETHODCALLTYPE UI_HideUI(IDocHostUIHandler FAR * This)
{
	return(S_OK);
}

// Called when the browser object wants to notify us that the command state has changed. 
HRESULT STDMETHODCALLTYPE UI_UpdateUI(IDocHostUIHandler FAR * This)
{
	// We update our UI in our window message loop so we don't do anything here.
	return(S_OK);
}

// Called from the browser object's IOleInPlaceActiveObject object's EnableModeless() function. Also called when the browser displays a modal dialog box.
HRESULT STDMETHODCALLTYPE UI_EnableModeless(IDocHostUIHandler FAR * This, BOOL fEnable)
{	
	return(S_OK);
}

// Called from the browser object's IOleInPlaceActiveObject object's OnDocWindowActivate() function.
HRESULT STDMETHODCALLTYPE UI_OnDocWindowActivate(IDocHostUIHandler FAR * This, BOOL fActivate)
{
	return(S_OK);
}

// Called from the browser object's IOleInPlaceActiveObject object's OnFrameWindowActivate() function.
HRESULT STDMETHODCALLTYPE UI_OnFrameWindowActivate(IDocHostUIHandler FAR * This, BOOL fActivate)
{
	return(S_OK);
}

// Called from the browser object's IOleInPlaceActiveObject object's ResizeBorder() function.
HRESULT STDMETHODCALLTYPE UI_ResizeBorder(IDocHostUIHandler FAR * This, LPCRECT prcBorder, IOleInPlaceUIWindow __RPC_FAR * pUIWindow, BOOL fRameWindow)
{	
	return(S_OK);
}

// Called from the browser object's TranslateAccelerator routines to translate key strokes to commands.
HRESULT STDMETHODCALLTYPE UI_TranslateAccelerator(IDocHostUIHandler FAR * This, LPMSG lpMsg, const GUID __RPC_FAR * pguidCmdGroup, DWORD nCmdID)
{
	return(S_OK);
}

HRESULT STDMETHODCALLTYPE UI_GetOptionKeyPath(IDocHostUIHandler FAR * This, LPOLESTR __RPC_FAR * pchKey, DWORD dw)
{
	// Let the browser use its default registry settings.
	return(S_FALSE);
}

HRESULT STDMETHODCALLTYPE UI_GetDropTarget(IDocHostUIHandler FAR * This, IDropTarget __RPC_FAR * pDropTarget, IDropTarget __RPC_FAR * __RPC_FAR * ppDropTarget)
{
	return(S_FALSE);
}

// Called by the browser when it wants a pointer to our IDispatch object. 
HRESULT STDMETHODCALLTYPE UI_GetExternal(IDocHostUIHandler FAR * This, IDispatch __RPC_FAR * __RPC_FAR * ppDispatch)
{	
	* ppDispatch = NULL;
	return(S_FALSE);
}

// Called by the browser object to give us an opportunity to modify the URL to be loaded.
HRESULT STDMETHODCALLTYPE UI_TranslateUrl(IDocHostUIHandler FAR * This, DWORD dwTranslate, OLECHAR __RPC_FAR * pchURLIn, OLECHAR __RPC_FAR * __RPC_FAR * ppchURLOut)
{	
	*ppchURLOut = NULL;
	return(S_FALSE);
}

// Called by the browser when it does cut/paste to the clipboard. 
HRESULT STDMETHODCALLTYPE UI_FilterDataObject(IDocHostUIHandler FAR * This, IDataObject __RPC_FAR * pDO, IDataObject __RPC_FAR * __RPC_FAR * ppDORet)
{
	* ppDORet = 0;
	return(S_FALSE);
}


////////////////////////////////////// My IOleClientSite functions  /////////////////////////////////////

HRESULT STDMETHODCALLTYPE Site_QueryInterface(IOleClientSite FAR * This, REFIID riid, void** ppvObject)
{	
	if (!memcmp(riid, &IID_IUnknown, sizeof(GUID)) || !memcmp(riid, &IID_IOleClientSite, sizeof(GUID)))
		* ppvObject = &((_IOleClientSiteEx*)This)->client;

	else if (!memcmp(riid, &IID_IOleInPlaceSite, sizeof(GUID)))
		* ppvObject = &((_IOleClientSiteEx*)This)->inplace;

	else if (!memcmp(riid, &IID_IDocHostUIHandler, sizeof(GUID)))
		* ppvObject = &((_IOleClientSiteEx*)This)->ui;
	else
	{
		*ppvObject = 0;
		return(E_NOINTERFACE);
	}

	return(S_OK);
}

HRESULT STDMETHODCALLTYPE Site_AddRef(IOleClientSite FAR * This)
{
	return(1);
}

HRESULT STDMETHODCALLTYPE Site_Release(IOleClientSite FAR * This)
{
	return(1);
}

HRESULT STDMETHODCALLTYPE Site_SaveObject(IOleClientSite FAR * This)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Site_GetMoniker(IOleClientSite FAR * This, DWORD dwAssign, DWORD dwWhichMoniker, IMoniker * *ppmk)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Site_GetContainer(IOleClientSite FAR * This, LPOLECONTAINER FAR * ppContainer)
{
	// Tell the browser that we are a simple object and don't support a container
	*ppContainer = 0;

	return(E_NOINTERFACE);
}

HRESULT STDMETHODCALLTYPE Site_ShowObject(IOleClientSite FAR * This)
{
	return(NOERROR);
}

HRESULT STDMETHODCALLTYPE Site_OnShowWindow(IOleClientSite FAR * This, BOOL fShow)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Site_RequestNewObjectLayout(IOleClientSite FAR * This)
{
	NOTIMPLEMENTED;
}


////////////////////////////////////// IOleInPlaceSite /////////////////////////////////////

HRESULT STDMETHODCALLTYPE InPlace_QueryInterface(IOleInPlaceSite FAR * This, REFIID riid, LPVOID FAR * ppvObj)
{
	return(Site_QueryInterface((IOleClientSite*)((char*)This - sizeof(IOleClientSite)), riid, ppvObj));
}

HRESULT STDMETHODCALLTYPE InPlace_AddRef(IOleInPlaceSite FAR * This)
{
	return(1);
}

HRESULT STDMETHODCALLTYPE InPlace_Release(IOleInPlaceSite FAR * This)
{
	return(1);
}

HRESULT STDMETHODCALLTYPE InPlace_GetWindow(IOleInPlaceSite FAR * This, HWND FAR * lphwnd)
{
	*lphwnd = ((_IOleInPlaceSiteEx FAR*)This)->frame.window;

	return(S_OK);
}

HRESULT STDMETHODCALLTYPE InPlace_ContextSensitiveHelp(IOleInPlaceSite FAR * This, BOOL fEnterMode)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE InPlace_CanInPlaceActivate(IOleInPlaceSite FAR * This)
{
	// Tell the browser we can in place activate
	return(S_OK);
}

HRESULT STDMETHODCALLTYPE InPlace_OnInPlaceActivate(IOleInPlaceSite FAR * This)
{
	// Tell the browser we did it ok
	return(S_OK);
}

HRESULT STDMETHODCALLTYPE InPlace_OnUIActivate(IOleInPlaceSite FAR * This)
{
	return(S_OK);
}

HRESULT STDMETHODCALLTYPE InPlace_GetWindowContext(IOleInPlaceSite FAR * This, LPOLEINPLACEFRAME FAR * lplpFrame, LPOLEINPLACEUIWINDOW FAR * lplpDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	* lplpFrame = (LPOLEINPLACEFRAME) & ((_IOleInPlaceSiteEx*)This)->frame;
	// We have no OLEINPLACEUIWINDOW
	*lplpDoc = 0;

	// Fill in some other info for the browser
	lpFrameInfo->fMDIApp = FALSE;
	lpFrameInfo->hwndFrame = ((_IOleInPlaceFrameEx*)* lplpFrame)->window;
	lpFrameInfo->haccel = 0;
	lpFrameInfo->cAccelEntries = 0;

	
	// InPlace_OnPosRectChange() is called right when a window is first
	// created anyway, so no need to duplicate it here.
//	GetClientRect(lpFrameInfo->hwndFrame, lprcPosRect);
//	GetClientRect(lpFrameInfo->hwndFrame, lprcClipRect);

	return(S_OK);
}

HRESULT STDMETHODCALLTYPE InPlace_Scroll(IOleInPlaceSite FAR * This, SIZE scrollExtent)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE InPlace_OnUIDeactivate(IOleInPlaceSite FAR * This, BOOL fUndoable)
{
	return(S_OK);
}

HRESULT STDMETHODCALLTYPE InPlace_OnInPlaceDeactivate(IOleInPlaceSite FAR * This)
{
	return(S_OK);
}

HRESULT STDMETHODCALLTYPE InPlace_DiscardUndoState(IOleInPlaceSite FAR * This)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE InPlace_DeactivateAndUndo(IOleInPlaceSite FAR * This)
{
	NOTIMPLEMENTED;
}

// Called when the position of the browser object is changed
HRESULT STDMETHODCALLTYPE InPlace_OnPosRectChange(IOleInPlaceSite FAR * This, LPCRECT lprcPosRect)
{
	IOleObject* browserObject;
	IOleInPlaceObject* inplace;

	browserObject = *((IOleObject * *)((char*)This - sizeof(IOleObject*) - sizeof(IOleClientSite)));
	if (SUCCEEDED(browserObject->lpVtbl->QueryInterface(browserObject, &IID_IOleInPlaceObject, (void**)& inplace)))
	{
		inplace->lpVtbl->SetObjectRects(inplace, lprcPosRect, lprcPosRect);
		inplace->lpVtbl->Release(inplace);
	}

	return(S_OK);
}


////////////////////////////////////// IOleInPlaceFrame /////////////////////////////////////////

HRESULT STDMETHODCALLTYPE Frame_QueryInterface(IOleInPlaceFrame FAR * This, REFIID riid, LPVOID FAR * ppvObj)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Frame_AddRef(IOleInPlaceFrame FAR * This)
{
	return(1);
}

HRESULT STDMETHODCALLTYPE Frame_Release(IOleInPlaceFrame FAR * This)
{
	return(1);
}

HRESULT STDMETHODCALLTYPE Frame_GetWindow(IOleInPlaceFrame FAR * This, HWND FAR * lphwnd)
{	
	*lphwnd = ((_IOleInPlaceFrameEx*)This)->window;
	return(S_OK);
}

HRESULT STDMETHODCALLTYPE Frame_ContextSensitiveHelp(IOleInPlaceFrame FAR * This, BOOL fEnterMode)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Frame_GetBorder(IOleInPlaceFrame FAR * This, LPRECT lprectBorder)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Frame_RequestBorderSpace(IOleInPlaceFrame FAR * This, LPCBORDERWIDTHS pborderwidths)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Frame_SetBorderSpace(IOleInPlaceFrame FAR * This, LPCBORDERWIDTHS pborderwidths)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Frame_SetActiveObject(IOleInPlaceFrame FAR * This, IOleInPlaceActiveObject * pActiveObject, LPCOLESTR pszObjName)
{
	return(S_OK);
}

HRESULT STDMETHODCALLTYPE Frame_InsertMenus(IOleInPlaceFrame FAR * This, HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Frame_SetMenu(IOleInPlaceFrame FAR * This, HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject)
{
	return(S_OK);
}

HRESULT STDMETHODCALLTYPE Frame_RemoveMenus(IOleInPlaceFrame FAR * This, HMENU hmenuShared)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Frame_SetStatusText(IOleInPlaceFrame FAR * This, LPCOLESTR pszStatusText)
{
	return(S_OK);
}

HRESULT STDMETHODCALLTYPE Frame_EnableModeless(IOleInPlaceFrame FAR * This, BOOL fEnable)
{
	return(S_OK);
}

HRESULT STDMETHODCALLTYPE Frame_TranslateAccelerator(IOleInPlaceFrame FAR * This, LPMSG lpmsg, WORD wID)
{
	NOTIMPLEMENTED;
}


void UnEmbedBrowserObject(HWND hwnd)
{
	IOleObject** browserHandle;
	IOleObject* browserObject;

	if ((browserHandle = (IOleObject * *)GetWindowLongPtr(hwnd, GWL_USERDATA)))
	{
		browserObject = *browserHandle;
		browserObject->lpVtbl->Close(browserObject, OLECLOSE_NOSAVE);
		browserObject->lpVtbl->Release(browserObject);

		GlobalFree(browserHandle);

		return;
	}

	//Must not get here if EmbedBrowser is called
	_ASSERT(0);
}

#define WEBPAGE_GOBACK		0
#define WEBPAGE_GOFORWARD	1
#define WEBPAGE_GOHOME		2
#define WEBPAGE_SEARCH		3
#define WEBPAGE_REFRESH		4
#define WEBPAGE_STOP		5


long DisplayHTMLPage(HWND hwnd, LPTSTR webPageName)
{
	IWebBrowser2* webBrowser2;
	VARIANT			myURL;
	IOleObject* browserObject;

	browserObject = *((IOleObject * *)GetWindowLongPtr(hwnd, GWL_USERDATA));

	if (SUCCEEDED(browserObject->lpVtbl->QueryInterface(browserObject, &IID_IWebBrowser2, (void**)& webBrowser2)))
	{
		VariantInit(&myURL);
		myURL.vt = VT_BSTR;

#ifndef UNICODE
		{
			wchar_t* buffer;
			DWORD		size;

			size = MultiByteToWideChar(CP_ACP, 0, webPageName, -1, 0, 0);
			if (!(buffer = (wchar_t*)GlobalAlloc(GMEM_FIXED, sizeof(wchar_t) * size))) 
			{
				webBrowser2->lpVtbl->Release(webBrowser2);
				return (-6);
			}
			MultiByteToWideChar(CP_ACP, 0, webPageName, -1, buffer, size);
			myURL.bstrVal = SysAllocString(buffer);
			GlobalFree(buffer);
		}
#else
		myURL.bstrVal = SysAllocString(webPageName);
#endif
		if (!myURL.bstrVal)
		{
			webBrowser2->lpVtbl->Release(webBrowser2);
			return(-7);
		}

		webBrowser2->lpVtbl->put_Silent(webBrowser2, VARIANT_TRUE);
		webBrowser2->lpVtbl->Navigate2(webBrowser2, &myURL, 0, 0, 0, 0);

		SysFreeString(myURL.bstrVal);
		VariantClear(&myURL);

		webBrowser2->lpVtbl->Release(webBrowser2);

		return(0);
	}

	return(-5);
}


void ResizeBrowser(HWND hwnd, DWORD width, DWORD height)
{
	IWebBrowser2* webBrowser2;
	IOleObject* browserObject;

	browserObject = *((IOleObject * *)GetWindowLongPtr(hwnd, GWL_USERDATA));

	if (SUCCEEDED(browserObject->lpVtbl->QueryInterface(browserObject, &IID_IWebBrowser2, (void**)& webBrowser2)))
	{
		webBrowser2->lpVtbl->put_Width(webBrowser2, width);
		webBrowser2->lpVtbl->put_Height(webBrowser2, height);		

		webBrowser2->lpVtbl->Release(webBrowser2);
	}
}

long EmbedBrowserObject(HWND hwnd)
{
	LPCLASSFACTORY		pClassFactory;
	IOleObject* browserObject;
	IWebBrowser2* webBrowser2;
	RECT				rect;
	char* ptr;
	_IOleClientSiteEx* _iOleClientSiteEx;

	
	if (!(ptr = (char*)GlobalAlloc(GMEM_FIXED, sizeof(_IOleClientSiteEx) + sizeof(IOleObject*))))
		return(-1);
		
	_iOleClientSiteEx = (_IOleClientSiteEx*)(ptr + sizeof(IOleObject*));
	_iOleClientSiteEx->client.lpVtbl = &MyIOleClientSiteTable;

	_iOleClientSiteEx->inplace.inplace.lpVtbl = &MyIOleInPlaceSiteTable;

	_iOleClientSiteEx->inplace.frame.frame.lpVtbl = &MyIOleInPlaceFrameTable;

	_iOleClientSiteEx->inplace.frame.window = hwnd;

	_iOleClientSiteEx->ui.ui.lpVtbl = &MyIDocHostUIHandlerTable;

	// Get a pointer to the browser object and lock it down so it doesn't "disappear" 

	pClassFactory = 0;
	if (SUCCEEDED(CoGetClassObject(&CLSID_WebBrowser, CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER, NULL, &IID_IClassFactory, (void**)& pClassFactory)) && pClassFactory)
	{
		// Call the IClassFactory's CreateInstance() to create a browser object
		if (SUCCEEDED(pClassFactory->lpVtbl->CreateInstance(pClassFactory, 0, &IID_IOleObject, &browserObject)))
		{
			// Free the IClassFactory as the browser is created already
			pClassFactory->lpVtbl->Release(pClassFactory);

			*((IOleObject * *)ptr) = browserObject;
			SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG_PTR)ptr);
						
			if (SUCCEEDED(browserObject->lpVtbl->SetClientSite(browserObject, (IOleClientSite*)_iOleClientSiteEx)))
			{
				browserObject->lpVtbl->SetHostNames(browserObject, L"IdProxy Host", 0);

				GetClientRect(hwnd, &rect);
								
				if (SUCCEEDED(OleSetContainedObject((struct IUnknown*)browserObject, TRUE)) && // Let browser object know that it is embedded in an OLE container.
					SUCCEEDED(browserObject->lpVtbl->DoVerb(browserObject, OLEIVERB_SHOW, NULL, (IOleClientSite*)_iOleClientSiteEx, -1, hwnd, &rect)) && //actually put the browser object into our window.
					SUCCEEDED(browserObject->lpVtbl->QueryInterface(browserObject, &IID_IWebBrowser2, (void**)& webBrowser2))) //The actual browser we may use
				{
					//Load the browser fullscreen
					webBrowser2->lpVtbl->put_Left(webBrowser2, 0);
					webBrowser2->lpVtbl->put_Top(webBrowser2, 0);
					webBrowser2->lpVtbl->put_Width(webBrowser2, rect.right);
					webBrowser2->lpVtbl->put_Height(webBrowser2, rect.bottom);
					
					webBrowser2->lpVtbl->Release(webBrowser2);

					// Success
					return(0);
				}
			}
						
			UnEmbedBrowserObject(hwnd);
			return(-4);
		}

		pClassFactory->lpVtbl->Release(pClassFactory); //TODO: Investigate if we need to free it
		GlobalFree(ptr);

		// Can't create an instance of the browser!
		return(-3);
	}

	GlobalFree(ptr);

	// Can't get the web browser's IClassFactory!
	return(-2);
}

void DisconnectEvents(HWND hwnd)
{
	IWebBrowser2* webBrowser2 = NULL;
	IOleObject* browserObject = NULL;
	IConnectionPointContainer* pCPC = NULL;
	IConnectionPoint* pCP = NULL;

	browserObject = *((IOleObject * *)GetWindowLongPtr(hwnd, GWL_USERDATA));

	if (SUCCEEDED(browserObject->lpVtbl->QueryInterface(browserObject, &IID_IWebBrowser2, (void**)& webBrowser2)) &&
		SUCCEEDED(webBrowser2->lpVtbl->QueryInterface(webBrowser2, &IID_IConnectionPointContainer, (void**)& pCPC)) &&
		SUCCEEDED(pCPC->lpVtbl->FindConnectionPoint(pCPC, &DIID_DWebBrowserEvents2, &pCP)))
			pCP->lpVtbl->Unadvise(pCP, &g_adviseCookie);
	
	if (webBrowser2)
		webBrowser2->lpVtbl->Release(webBrowser2);

	if (pCPC)
		pCPC->lpVtbl->Release(pCPC);

	if (pCP)
		pCP->lpVtbl->Release(pCP);
}

void InitializeBrowser(HWND hwnd)
{
	DisconnectEvents(hwnd); //just in case we add a second active instance. Always disconnect
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_SIZE:
		{		
			ResizeBrowser(hwnd, LOWORD(lParam), HIWORD(lParam));
			return(0);
		}

		case WM_CREATE:
		{
			if (EmbedBrowserObject(hwnd)) return(-1);
			InitializeBrowser(hwnd);

			InterlockedIncrement(&g_nBrowserCount);

			return(0);
		}

		case WM_DPICHANGED:
		{
			RECT* const newWinBounds = (RECT*)lParam;
			WORD dpi = HIWORD(wParam);

			SetWindowPos(hwnd,
				NULL,
				newWinBounds->left,
				newWinBounds->top,
				newWinBounds->right - newWinBounds->left,
				newWinBounds->bottom - newWinBounds->top,
				SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);

			break;
		}
		case WM_DESTROY:
		{
			UnEmbedBrowserObject(hwnd);
			InterlockedDecrement(&g_nBrowserCount);

			if (!InterlockedCompareExchange(&g_nBrowserCount, g_nBrowserCount, 0))
				PostQuitMessage(0);

			return(TRUE);
		}
	}

	return(DefWindowProc(hwnd, uMsg, wParam, lParam));
}

void GetBrowserFrameWindowBounds(HWND parent, int *pLeft, int *pTop, int* pWidth, int* pHeight)
{
	RECT rectDesktop;
	
	GetClientRect(parent, &rectDesktop);

	int xScreenResolution = GetSystemMetrics(SM_CXSCREEN);
	int yScreenResolution = GetSystemMetrics(SM_CYSCREEN);

	int width = (int)ceil((double)xScreenResolution * 0.5333333333333333);
	int height = (int)ceil((double)yScreenResolution * 0.7111111111111111);

	if (width < 800)
		width = 800;

	if (height < 600)
		height = 600;

	*pLeft = (rectDesktop.right / 2) - (width / 2);
	*pTop = (rectDesktop.bottom / 2) - (height / 2);
	*pWidth = width;
	*pHeight = height;
}

int ShowBrowserFrame(Platform_t *platform, void* phInstance, pUtf8_t u8UrlStr)
{
	MSG			msg;
	int nCmdShow = SW_SHOWNORMAL;
	HINSTANCE hInstance = (HINSTANCE)phInstance;

	WNDCLASSEX		wc;
	int width, height, top, left;
	UINT dpiX, dpiY;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WindowProc;
	wc.lpszClassName = &ClassName[0];
	RegisterClassEx(&wc);

	HWND hWndDesktop = GetDesktopWindow();
	GetBrowserFrameWindowBounds(hWndDesktop, &left, &top, &width, &height);

	if ((msg.hwnd = CreateWindowEx(0, &ClassName[0], TEXT("Autodesk Signin"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hWndDesktop, NULL, hInstance, 0)))
	{
		SetWindowPos(msg.hwnd, HWND_TOPMOST, left, top, width, height, SWP_SHOWWINDOW);
		DisplayHTMLPage(msg.hwnd, TEXT("https://accounts-dev.autodesk.com/"));

		ShowWindow(msg.hwnd, nCmdShow);
		UpdateWindow(msg.hwnd);
	}

	g_hWnd = msg.hwnd;

	while (GetMessage(&msg, 0, 0, 0) == 1)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnregisterClassW(&ClassName[0], hInstance);

	return 0;
}

void InitializePlatform(Platform_t* platform)
{
	platform->browserFrame.ShowBrowserFrame = ShowBrowserFrame;
	platform->browserFrame.CloseBrowserFrame = NULL;
}

BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		if (FAILED(OleInitialize(NULL)))
			return FALSE;

		break;
	case DLL_THREAD_ATTACH:  break;
	case DLL_THREAD_DETACH:  break;
	case DLL_PROCESS_DETACH: 
		OleUninitialize();
		break;

	}

	return TRUE;
}

int CALLBACK wWinMain(_In_ HINSTANCE hInstance, _In_ HINSTANCE hInstNULL, _In_ LPSTR lpszCmdLine, _In_ int nCmdShow)
{
	return ShowBrowserFrame(hInstance, hInstance, NULL);
}
