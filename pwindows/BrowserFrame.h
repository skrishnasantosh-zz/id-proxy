#pragma once
#include <windows.h>
#include <math.h>
#include <exdisp.h>		
#include <mshtml.h>		
#include <mshtmhst.h>	
#include <crtdbg.h>		// for _ASSERT()
#include <shcore.h>
#include <ShellScalingApi.h> //For DPI Aware operations
#pragma comment(lib,"Shcore.lib")
#include <uchar.h>
#include <string>

#include "..\platform\platform.h"

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

public:
	AdBrowserFrame(Platform_t* platform, HINSTANCE hInstance);
	~AdBrowserFrame();

public:
	BOOL OpenBrowser(const wchar_t* urlStr);
	
public:
	static LRESULT CALLBACK FrameProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static void DpiReCalculateBounds(HWND hWnd, RECT *pRect);
};