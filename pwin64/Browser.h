#pragma once

#include <Windows.h>
#include <MsHTML.h>
#include <string>
#include "../platform/platform.h"

#ifndef GWL_USERDATA
#define GWL_USERDATA -21
#endif

#define BROWSER_FRAME_CLASS TEXT("AdBrowserFrame") 
#define BROWSER_FRAME_TITLE TEXT("Login")

class AdBrowser;

class AdBrowserBase : public IUnknown
{
protected:
	AdBrowser* m_browser;
	volatile ULONG m_refCount;

protected:	
	AdBrowserBase(AdBrowser* browser);
	virtual ~AdBrowserBase();

public:
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID FAR* ppvObj);
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();
};

class AdBrowserStorage : public IStorage, AdBrowserBase
{
public:
	AdBrowserStorage(AdBrowser* browser);

public://COM
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID FAR* ppvObj) { return AdBrowserBase::QueryInterface(riid, ppvObj); }
	ULONG STDMETHODCALLTYPE AddRef() { return AdBrowserBase::AddRef(); }
	ULONG STDMETHODCALLTYPE Release() { return AdBrowserBase::Release(); }

	HRESULT STDMETHODCALLTYPE CreateStream(const WCHAR* pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStream** ppstm);
	HRESULT STDMETHODCALLTYPE OpenStream(const WCHAR* pwcsName, void* reserved1, DWORD grfMode, DWORD reserved2, IStream** ppstm);
	HRESULT STDMETHODCALLTYPE CreateStorage(const WCHAR* pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStorage** ppstg);
	HRESULT STDMETHODCALLTYPE OpenStorage(const WCHAR* pwcsName, IStorage* pstgPriority, DWORD grfMode, SNB snbExclude, DWORD reserved, IStorage** ppstg);
	HRESULT STDMETHODCALLTYPE CopyTo(DWORD ciidExclude, IID const* rgiidExclude, SNB snbExclude, IStorage* pstgDest);
	HRESULT STDMETHODCALLTYPE MoveElementTo(const OLECHAR* pwcsName, IStorage* pstgDest, const OLECHAR* pwcsNewName, DWORD grfFlags);
	HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags);
	HRESULT STDMETHODCALLTYPE Revert();
	HRESULT STDMETHODCALLTYPE EnumElements(DWORD reserved1, void* reserved2, DWORD reserved3, IEnumSTATSTG** ppenum);
	HRESULT STDMETHODCALLTYPE DestroyElement(const OLECHAR* pwcsName);
	HRESULT STDMETHODCALLTYPE RenameElement(const WCHAR* pwcsOldName, const WCHAR* pwcsNewName);
	HRESULT STDMETHODCALLTYPE SetElementTimes(const WCHAR* pwcsName, FILETIME const* pctime, FILETIME const* patime, FILETIME const* pmtime);
	HRESULT STDMETHODCALLTYPE SetClass(REFCLSID clsid);
	HRESULT STDMETHODCALLTYPE SetStateBits(DWORD grfStateBits, DWORD grfMask);
	HRESULT STDMETHODCALLTYPE Stat(STATSTG* pstatstg, DWORD grfStatFlag);
};

class AdInPlaceFrame : public IOleInPlaceFrame, AdBrowserBase
{	
public:
	AdInPlaceFrame(AdBrowser* frame);

public:	//COM
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID FAR* ppvObj) { return AdBrowserBase::QueryInterface(riid, ppvObj); }
	ULONG STDMETHODCALLTYPE AddRef() { return AdBrowserBase::AddRef(); }
	ULONG STDMETHODCALLTYPE Release() { return AdBrowserBase::Release(); }

	HRESULT STDMETHODCALLTYPE GetWindow(HWND FAR* lphwnd);
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

class AdInPlaceSite : public IOleInPlaceSite, AdBrowserBase
{
public:
	AdInPlaceSite(AdBrowser* browser);

public: //COM
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID FAR* ppvObj) { return AdBrowserBase::QueryInterface(riid, ppvObj); }
	ULONG STDMETHODCALLTYPE AddRef() { return AdBrowserBase::AddRef(); }
	ULONG STDMETHODCALLTYPE Release() { return AdBrowserBase::Release(); }

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

class AdClientSite : public IOleClientSite, AdBrowserBase
{
public:
	AdClientSite(AdBrowser* browser);

public://COM
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID FAR* ppvObj) { return AdBrowserBase::QueryInterface(riid, ppvObj); }
	ULONG STDMETHODCALLTYPE AddRef() { return AdBrowserBase::AddRef(); }
	ULONG STDMETHODCALLTYPE Release() { return AdBrowserBase::Release(); }

	HRESULT STDMETHODCALLTYPE SaveObject();
	HRESULT STDMETHODCALLTYPE GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker** ppmk);
	HRESULT STDMETHODCALLTYPE GetContainer(LPOLECONTAINER FAR* ppContainer);
	HRESULT STDMETHODCALLTYPE ShowObject();
	HRESULT STDMETHODCALLTYPE OnShowWindow(BOOL fShow);
	HRESULT STDMETHODCALLTYPE RequestNewObjectLayout();
};


class AdBrowser
{
public:
	HWND m_hWnd;
	IOleObject* m_webBrowserOle;

private:
	HINSTANCE m_hInstance;
	std::wstring m_title;

public:
	AdBrowserStorage m_storage;
	AdInPlaceFrame m_inPlaceFrame;
	AdInPlaceSite m_inPlaceSite;	
	AdClientSite m_clientSite;	

public:
	AdBrowser(HINSTANCE hInstance, const wchar_t* title);
	~AdBrowser();

public:
	HWND GetWindowHandle();
	void ShowWindow(HWND parent, BOOL modal, const wchar_t* urlStr);

private:
	LRESULT CALLBACK WindowProcThis(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void InitWebBrowser();
	void UninitWebBrowser();

	void Navigate(const wchar_t* url);

private:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
