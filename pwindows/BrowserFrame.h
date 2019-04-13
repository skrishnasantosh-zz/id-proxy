#pragma once
#include <windows.h>
#include <math.h>
#include <exdisp.h>		
#include <exdispid.h>
#include <mshtml.h>		
#include <mshtmhst.h>	
#include <crtdbg.h>		// for _ASSERT()
#include <shcore.h>
#include <ShellScalingApi.h> //For DPI Aware operations
#pragma comment(lib,"Shcore.lib")
#include <string>

#include "..\platform\platform.h"

#define ALWAYS_ON_OBJECT (0x01)

class AdBrowserEventSink;
class AdClientSite;
class AdDocHostUiHandler;
class AdInPlaceFrame;
class AdInPlaceSite;

class AdBrowserFrame
{
private:
	MSG m_msg;
	WNDCLASSEX m_wc;			
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	std::wstring m_windowTitle;
	std::wstring m_lastUrl;

	IWebBrowser2* m_webBrowser;
	IOleObject* m_webBrowserOle;

	Platform_t* m_platform;

	BOOL m_initComplete;	

	AdBrowserEventSink* m_sink;
	AdClientSite* m_clientSite;
	AdDocHostUiHandler* m_docHostUiHandler;	
	AdInPlaceFrame* m_inPlaceFrame;
	AdInPlaceSite* m_inPlaceSite;

public:
	AdBrowserFrame(Platform_t* platform, HINSTANCE hInstance);
	~AdBrowserFrame();

public:
	BOOL OpenBrowser(HWND parent, BOOL modal, const wchar_t* urlStr);

public:
	HWND GetHwnd();
	AdClientSite* GetClientSite();
	AdInPlaceFrame* GetInPlaceFrame();
	AdDocHostUiHandler* GetDocHostUiHandler();
	IWebBrowser2* GetWebBrowser();
	AdInPlaceSite* GetInPlaceSite();
	IOleObject* GetWebBrowserAsOle();
	AdBrowserEventSink* GetSink();

public:
	static LRESULT CALLBACK FrameProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static void DpiReCalculateBounds(HWND hWnd, RECT *pRect);

	pError_t InplaceBrowserIe();

private://MSHTML COM interfaces
	STDMETHODIMP QueryInterface(REFIID riid, void** pObject);
	STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS FAR* pDispParams, VARIANT FAR* pVarResult, EXCEPINFO FAR* pExcepInfo, unsigned int FAR* puArgErr);

		
private: //OLE/COM Helpers	
	pError_t Resize(DWORD width, DWORD height);

	pError_t SetupSink();

	pError_t SetUrl();
};

////////////////

class AdBrowserEventSink : public DWebBrowserEvents2 
{
private:
	volatile ULONG m_refCount;

public:
	AdBrowserEventSink();

public:
	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	STDMETHODIMP GetTypeInfoCount(UINT* pctinfo);
	STDMETHODIMP GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo);
	STDMETHODIMP GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId);
	STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr);
protected:
	bool Event_BeforeNavigate2(LPOLESTR url, LONG Flags, LPOLESTR TargetFrameName, PUCHAR PostData, LONG PostDataSize, LPOLESTR Headers, bool Cancel);
};

////////////////

class AdClientSite : public IOleClientSite {

	AdBrowserFrame* m_frame;

public:

	AdClientSite(AdBrowserFrame* frame);
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();
	
	HRESULT STDMETHODCALLTYPE SaveObject();
	HRESULT STDMETHODCALLTYPE GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker** ppmk);
	HRESULT STDMETHODCALLTYPE GetContainer(LPOLECONTAINER FAR* ppContainer);
	HRESULT STDMETHODCALLTYPE ShowObject();

	HRESULT STDMETHODCALLTYPE OnShowWindow(BOOL fShow);
	HRESULT STDMETHODCALLTYPE RequestNewObjectLayout();
};

/////////////////

class AdDocHostUiHandler : public IDocHostUIHandler
{

	AdClientSite* m_clientSite;
	AdBrowserFrame* m_browserFrame;

public:

	AdDocHostUiHandler(AdBrowserFrame* frame);
	virtual ~AdDocHostUiHandler() {}

public: //COM 

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID FAR* ppvObj);
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();


	HRESULT STDMETHODCALLTYPE ShowContextMenu(DWORD dwID, POINT __RPC_FAR* ppt, IUnknown __RPC_FAR* pcmdtReserved, IDispatch __RPC_FAR* pdispReserved);
	HRESULT STDMETHODCALLTYPE ShowUI(DWORD dwID, IOleInPlaceActiveObject __RPC_FAR* pActiveObject, IOleCommandTarget __RPC_FAR* pCommandTarget, IOleInPlaceFrame  __RPC_FAR* pFrame, IOleInPlaceUIWindow __RPC_FAR* pDoc);
	HRESULT STDMETHODCALLTYPE GetHostInfo(DOCHOSTUIINFO __RPC_FAR* pInfo);
	HRESULT STDMETHODCALLTYPE HideUI();
	HRESULT STDMETHODCALLTYPE UpdateUI();

	HRESULT STDMETHODCALLTYPE EnableModeless(BOOL fEnable);
	HRESULT STDMETHODCALLTYPE OnDocWindowActivate(BOOL fActivate);
	HRESULT STDMETHODCALLTYPE OnFrameWindowActivate(BOOL fActivate);
	HRESULT STDMETHODCALLTYPE ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow __RPC_FAR* pUIWindow, BOOL fRameWindow);
	HRESULT STDMETHODCALLTYPE TranslateAccelerator(LPMSG lpMsg, const GUID __RPC_FAR* pguidCmdGroup, DWORD nCmdID);
	HRESULT STDMETHODCALLTYPE GetOptionKeyPath(LPOLESTR __RPC_FAR* pchKey, DWORD dw);
	HRESULT STDMETHODCALLTYPE GetDropTarget(IDropTarget __RPC_FAR* pDropTarget, IDropTarget __RPC_FAR* __RPC_FAR* ppDropTarget);

	HRESULT STDMETHODCALLTYPE GetExternal(IDispatch** ppDispatch);
	HRESULT STDMETHODCALLTYPE TranslateUrl(DWORD dwTranslate, OLECHAR __RPC_FAR* pchURLIn, OLECHAR __RPC_FAR* __RPC_FAR* ppchURLOut);
	HRESULT STDMETHODCALLTYPE FilterDataObject(IDataObject __RPC_FAR* pDO, IDataObject __RPC_FAR* __RPC_FAR* ppDORet);
};

/////////////////

class AdInPlaceFrame : public IOleInPlaceFrame 
{
private:
	AdBrowserFrame* m_mainFrame;

public:
	AdInPlaceFrame(AdBrowserFrame* frame);

public: //COM

	HRESULT STDMETHODCALLTYPE GetWindow(HWND FAR* lphwnd);
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID FAR* ppvObj);
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

	HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode);
	HRESULT STDMETHODCALLTYPE GetBorder(LPRECT lprectBorder);
	HRESULT STDMETHODCALLTYPE RequestBorderSpace(LPCBORDERWIDTHS pborderwidths);
	HRESULT STDMETHODCALLTYPE SetBorderSpace(LPCBORDERWIDTHS pborderwidths);
	HRESULT STDMETHODCALLTYPE SetActiveObject(IOleInPlaceActiveObject* pActiveObject, LPCOLESTR pszObjName);
	HRESULT STDMETHODCALLTYPE InsertMenus(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths);
	HRESULT STDMETHODCALLTYPE SetMenu(HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject);
	HRESULT STDMETHODCALLTYPE RemoveMenus(HMENU hmenuShared);
	HRESULT STDMETHODCALLTYPE SetStatusText(LPCOLESTR pszStatusText);
	HRESULT STDMETHODCALLTYPE EnableModeless(BOOL fEnable);
	HRESULT STDMETHODCALLTYPE TranslateAccelerator(LPMSG lpmsg, WORD wID);
};

/////////////

class AdInPlaceSite : public IOleInPlaceSite 
{
private:
	AdBrowserFrame* m_frame;

public://Constructor
	AdInPlaceSite(AdBrowserFrame* frame); 

public://COM
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID FAR* ppvObj);
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();
	HRESULT STDMETHODCALLTYPE GetWindow(HWND FAR* lphwnd);
	HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode);
	HRESULT STDMETHODCALLTYPE CanInPlaceActivate();
	HRESULT STDMETHODCALLTYPE OnInPlaceActivate();
	HRESULT STDMETHODCALLTYPE OnUIActivate();

	HRESULT STDMETHODCALLTYPE GetWindowContext(LPOLEINPLACEFRAME FAR* lplpFrame, LPOLEINPLACEUIWINDOW FAR* lplpDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo);
	HRESULT STDMETHODCALLTYPE Scroll(SIZE scrollExtent);

	HRESULT STDMETHODCALLTYPE OnUIDeactivate(BOOL fUndoable);

	HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate();

	HRESULT STDMETHODCALLTYPE DiscardUndoState();

	HRESULT STDMETHODCALLTYPE DeactivateAndUndo();
		
	HRESULT STDMETHODCALLTYPE OnPosRectChange(LPCRECT lprcPosRect);	
};
