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

class AdBrowserEventSink;

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
	Platform_t* m_platform;

	BOOL m_initComplete;

	AdBrowserEventSink* m_sink;

public:
	AdBrowserFrame(Platform_t* platform, HINSTANCE hInstance);
	~AdBrowserFrame();

public:
	BOOL OpenBrowser(HWND parent, BOOL modal, const wchar_t* urlStr);
	
public:
	static LRESULT CALLBACK FrameProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static void DpiReCalculateBounds(HWND hWnd, RECT *pRect);

private://MSHTML COM interfaces
	STDMETHODIMP QueryInterface(REFIID riid, void** pObject);
	STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS FAR* pDispParams, VARIANT FAR* pVarResult, EXCEPINFO FAR* pExcepInfo, unsigned int FAR* puArgErr);

		
private: //OLE/COM Helpers
	pError_t InplaceBrowserIe();
	pError_t Resize(DWORD width, DWORD height);

	pError_t SetupSink();

	pError_t SetUrl();

private: //COM Objects
	IDocHostUIHandler *m_docHostUiHandler;
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
