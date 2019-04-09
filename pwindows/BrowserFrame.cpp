
#include "BrowserFrame.h"
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>

#define BROWSER_FRAME_WIDTH 1024
#define BROWSER_FRAME_HEIGHT 768

const wchar_t* FRAME_CLASS = L"IdProxyClass";


AdBrowserFrame::AdBrowserFrame(Platform_t* platform, HINSTANCE hInstance)	
	:m_hInstance(NULL), m_hWnd(NULL), m_lastUrl(L""), m_msg({0}), m_wc({0}), m_webBrowser(NULL), m_windowTitle(L"")
{
	if (platform == NULL)
	{
		//Log
	}

	m_platform = platform;
	m_hInstance = hInstance;
}

AdBrowserFrame::~AdBrowserFrame()
{
	UnregisterClass(FRAME_CLASS, m_hInstance);
}

BOOL AdBrowserFrame::OpenBrowser(const wchar_t* urlStr)
{
	WNDCLASSEX		wc;
	int width, height, top, left;
	UINT dpiX, dpiY;
	RECT bounds = { 0 };

	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = m_hInstance;
	wc.lpfnWndProc = FrameProc;
	wc.lpszClassName = FRAME_CLASS;
	RegisterClassEx(&wc);

	HWND hWndDesktop = ::GetDesktopWindow();	

	auto dpi = ::GetThreadDpiAwarenessContext();
	::SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);

	m_hWnd = CreateWindowEx(0, FRAME_CLASS, m_windowTitle.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hWndDesktop, NULL, m_hInstance, 0);

	::SetThreadDpiAwarenessContext(dpi);

	if (m_hWnd)
	{	
		DpiReCalculateBounds(m_hWnd, &bounds);

		SetWindowPos(m_hWnd, /*HWND_TOPMOST*/0, bounds.left, bounds.top, bounds.right, bounds.bottom, SWP_SHOWWINDOW);
		//DisplayHTMLPage(m_hWnd, TEXT("https://accounts-dev.autodesk.com/"));

		ShowWindow(m_hWnd, SW_SHOWDEFAULT);
		UpdateWindow(m_hWnd);
	}

	while (GetMessage(&m_msg, m_hWnd, 0, 0) == 1)
	{
		TranslateMessage(&m_msg);
		DispatchMessage(&m_msg);
	}

	return TRUE;
}

LRESULT CALLBACK AdBrowserFrame::FrameProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DPICHANGED:
		RECT rect;
		DpiReCalculateBounds(hWnd, &rect);
		SetWindowPos(hWnd, /*HWND_TOPMOST*/0, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW);
		break;
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
