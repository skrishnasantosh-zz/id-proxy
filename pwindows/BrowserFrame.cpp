
#include "BrowserFrame.h"
#include <winuser.h>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
#include <ExDispid.h>

#define BROWSER_FRAME_WIDTH 1024
#define BROWSER_FRAME_HEIGHT 768

AdBrowserFrame* g_frame;

typedef struct {
	DLGITEMTEMPLATE dlgItem; 
	WORD ch, c, t, dummy, cd;
} DialogItem;

typedef struct _tDlgData{
	DLGTEMPLATE dlgData; 
	WORD m, c;
	WCHAR title[8];
	WORD pt;
	WCHAR font[14];
} Dialog;

std::string IIDToString(REFIID riid);
const IID IID_IDocHostUIHandler = { 0xbd3f23c0, 0xd43e, 0x11CF, {0x89, 0x3b, 0x00, 0xaa, 0x00, 0xbd, 0xce, 0x1a} }; //COM Identifier for IDocHostUIHandler

AdBrowserFrame::AdBrowserFrame(Platform_t* platform, HINSTANCE hInstance)
	:m_hInstance(nullptr), m_hWnd(nullptr), m_lastUrl(L""), m_msg({ 0 }), m_wc({ 0 }), m_webBrowser(nullptr), m_windowTitle(L""), m_initComplete(FALSE),
	 m_docHostUiHandler(nullptr), m_sink(nullptr)
{
	
	if (platform == nullptr)
	{
		//Log
		throw; //Throw exception in This case
	}

	m_platform = platform;
	m_hInstance = hInstance;

	if (SUCCEEDED(::OleInitialize(NULL)))
		m_initComplete = TRUE;
}

AdBrowserFrame::~AdBrowserFrame()
{
	if (m_initComplete)
		::OleUninitialize();	
}

BOOL AdBrowserFrame::OpenBrowser(HWND parent, BOOL modal, const wchar_t* urlStr)
{	
	::SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
	
	Dialog dtp = { {DS_MODALFRAME | DS_3DLOOK | DS_SETFONT | DS_CENTER | WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE, 0, 0/*Zero Sub Elements*/, 0, 0, 478, 296},
		0, 0, L"Preview", 8, L"MS Sans Serif"};

	g_frame = this;
	DialogBoxIndirect(m_hInstance, (DLGTEMPLATE*)& dtp, parent, FrameProc);

	::SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);	

  return TRUE;
}

HWND AdBrowserFrame::GetHwnd()
{
	return m_hWnd;
}

AdClientSite* AdBrowserFrame::GetClientSite()
{
	return m_clientSite;
}

AdInPlaceFrame* AdBrowserFrame::GetInPlaceFrame()
{
	return m_inPlaceFrame;
}

AdDocHostUiHandler* AdBrowserFrame::GetDocHostUiHandler()
{
	return m_docHostUiHandler;
}

IWebBrowser2* AdBrowserFrame::GetWebBrowser()
{
	return m_webBrowser;
}

AdInPlaceSite* AdBrowserFrame::GetInPlaceSite()
{
	return m_inPlaceSite;
}

IOleObject* AdBrowserFrame::GetWebBrowserAsOle() 
{
	return m_webBrowserOle;
}

AdBrowserEventSink* AdBrowserFrame::GetSink()
{
	return m_sink;
}

LRESULT CALLBACK AdBrowserFrame::FrameProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DPICHANGED:
	{
		RECT rect;
		DpiReCalculateBounds(hWnd, &rect);
		SetWindowPos(hWnd, /*HWND_TOPMOST*/0, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW);
		break;
	}
	case WM_INITDIALOG:
	{
		g_frame->InplaceBrowserIe();

		return TRUE;
	}
	}
	
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void AdBrowserFrame::DpiReCalculateBounds(HWND hWnd, RECT* pRect)
{
	MONITORINFOEX monitorInfo = {0};
	DEVMODE devMode = { 0 };
	HMONITOR hMonitor = NULL;
	HWND hDesktop = ::GetDesktopWindow();
	RECT rectDesktop;
	
	hMonitor = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

	monitorInfo.cbSize = sizeof(MONITORINFOEX);

	::GetMonitorInfo(hMonitor, &monitorInfo);
	
	int logicalWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
	int logicalHeight = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
		
	devMode.dmSize = sizeof(DEVMODE);
	
	::EnumDisplaySettings(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &devMode);

	int physicalWidth = devMode.dmPelsWidth;
	int physicalHeight = devMode.dmPelsHeight;

	double hScaleFactor = ((double)physicalWidth) / ((double)logicalWidth);
	double vScaleFactor = ((double)physicalHeight) / ((double)logicalHeight);
		
	int width = BROWSER_FRAME_WIDTH * hScaleFactor;
	int height = BROWSER_FRAME_HEIGHT * vScaleFactor;

	pRect->left = (physicalWidth/2) - (width / 2);
	pRect->top = (physicalHeight /2) - (height / 2);

	pRect->right = width;
	pRect->bottom = height;	
}

STDMETHODIMP AdBrowserFrame::QueryInterface(REFIID riid, void** pObject)
{
	std::string s = IIDToString(riid);

	if (!memcmp((const void*)& riid, (const void*)& IID_IUnknown, sizeof(GUID)) ||
		!memcmp((const void*)& riid, (const void*)& IID_IDispatch, sizeof(GUID)) ||
		!memcmp((const void*)& riid, (const void*)& IID_IDocHostUIHandler, sizeof(GUID))) 
	{
		*pObject = m_docHostUiHandler;
		return S_OK;
	}

	pObject = NULL;

	return E_NOINTERFACE;
}

STDMETHODIMP AdBrowserFrame::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, unsigned int* puArgErr)
{
	
	return NOERROR;
}

pError_t AdBrowserFrame::InplaceBrowserIe()
{	
	RECT             rect;

	m_inPlaceFrame = new AdInPlaceFrame(this);
	m_clientSite = new AdClientSite(this);
	m_docHostUiHandler = new AdDocHostUiHandler(this);
	m_inPlaceSite = new AdInPlaceSite(this);
	
	if (SUCCEEDED(::OleCreate(CLSID_WebBrowser, IID_IOleObject, 0x01, NULL, m_clientSite, NULL, (void**)& m_webBrowserOle)))
	{
		m_webBrowserOle->SetHostNames(L"Host Name", NULL);
		GetClientRect(m_hWnd, &rect);

		if (SUCCEEDED(OleSetContainedObject(static_cast<IUnknown*>(m_webBrowserOle), TRUE)))
		{
			if (SUCCEEDED(m_webBrowserOle->DoVerb(OLEIVERB_SHOW, NULL, m_clientSite, -1, m_hWnd, &rect)))
			{
				if (SUCCEEDED(m_webBrowserOle->QueryInterface(IID_IWebBrowser2, reinterpret_cast<void**>(&m_webBrowser))))
				{
					m_webBrowser->put_Left(0);
					m_webBrowser->put_Top(0);
					m_webBrowser->put_Width(rect.right);
					m_webBrowser->put_Height(rect.bottom);

					m_webBrowser->Release();
				}
			}
		}
	}


	return P_ERROR_BROWSER_INIT;
}

pError_t AdBrowserFrame::Resize(DWORD width, DWORD height)
{
	return pError_t();
}

pError_t AdBrowserFrame::SetupSink()
{
	return pError_t();
}

pError_t AdBrowserFrame::SetUrl()
{
	return pError_t();
}

//Helpers

std::string IIDToString(REFIID riid) {
	char buf[34] = { 0 };

	for (int i = 0; i < 16; i++) 
		sprintf_s(buf + i * 2, 33, "%02x", *(((char*)& riid) + i));	

	return buf;
};
