#include <ExDisp.h>
#include "Browser.h"

AdBrowser::AdBrowser(HINSTANCE hInstance, const wchar_t* title) : 
	m_hInstance(hInstance), m_title(title), m_hWnd(nullptr),
	m_storage(this), m_inPlaceFrame(this), m_inPlaceSite(this), 
	m_clientSite(this), m_webBrowserOle(nullptr)
{ }

AdBrowser::~AdBrowser()
{ }

HWND AdBrowser::GetWindowHandle()
{
	return m_hWnd;
}

LRESULT CALLBACK AdBrowser::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	AdBrowser* frame = nullptr;

	switch (uMsg) 
	{
	case WM_CREATE:
		frame = (AdBrowser*)((LPCREATESTRUCT)lParam)->lpCreateParams;
		if (frame != nullptr)	
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)frame);
		break;

	
	default:
		frame = (AdBrowser*)GetWindowLongPtr(hWnd, GWLP_USERDATA);		
		break;
	}

	if (frame == nullptr)
		return DefWindowProc(hWnd, uMsg, wParam, lParam);

	return frame->WindowProcThis(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK AdBrowser::WindowProcThis(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		this->m_hWnd = hWnd;
		InitWebBrowser();
		break;
			
	case WM_DESTROY:
		UninitWebBrowser();
		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void AdBrowser::InitWebBrowser()
{
	HRESULT hr = E_FAIL;

	hr = OleCreate(CLSID_WebBrowser, IID_IOleObject, OLERENDER_DRAW, nullptr, &m_clientSite, &m_storage, (void**)&m_webBrowserOle);

	if (SUCCEEDED(hr))
	{
		RECT rect = { 0 };

		m_webBrowserOle->SetHostNames(L"My Host Name", 0);
		GetClientRect(m_hWnd, &rect);

		hr = OleSetContainedObject(m_webBrowserOle, TRUE);

		if (SUCCEEDED(hr))
		{
			hr = m_webBrowserOle->DoVerb(OLEIVERB_SHOW, NULL, &m_clientSite, -1, m_hWnd, &rect);

			if (SUCCEEDED(hr))
			{
				IWebBrowser2* browser;

				hr = m_webBrowserOle->QueryInterface(IID_IWebBrowser2, (void**)& browser);

				if (SUCCEEDED(hr))
				{
					browser->put_Left(0);
					browser->put_Top(0);
					browser->put_Width(rect.right);
					browser->put_Height(rect.bottom);
					
					browser->Release();
				}
			}
		}
	}	
}

void AdBrowser::UninitWebBrowser()
{

	if (m_webBrowserOle != nullptr)
	{
		if (!IsBadReadPtr(m_webBrowserOle, sizeof(IOleObject*)))
		{
			m_webBrowserOle->Close(OLECLOSE_NOSAVE);
			m_webBrowserOle->Release();
		}
	}
}

void AdBrowser::Navigate(const wchar_t* urlStr)
{
	IWebBrowser2* browser;
	VARIANT			url;	
	HRESULT hr = E_FAIL;

	hr = m_webBrowserOle->QueryInterface(IID_IWebBrowser2, (void**)&browser);

	if (SUCCEEDED(hr))
	{		
		VariantInit(&url);
		url.vt = VT_BSTR;

		url.bstrVal = SysAllocString(urlStr);

		if (url.bstrVal == NULL)
		{
			browser->Release();
			VariantClear(&url);
			MessageBox(m_hWnd, TEXT("Error"), TEXT("Unable to navigate WebBrowser. Cannot allocate string value"), MB_ICONERROR | MB_OK);
			return;
		}

		hr = browser->Navigate2(&url, 0, 0, 0, 0);

		if (FAILED(hr))
		{
			MessageBox(m_hWnd, TEXT("Error"), TEXT("Unable to navigate WebBrowser. Navigate Failed"), MB_ICONERROR | MB_OK);
		}

		SysFreeString(url.bstrVal);
		url.bstrVal = NULL;

		VariantClear(&url);
				
		browser->Release();
	}
}

void AdBrowser::ShowWindow(HWND parent, BOOL modal, const wchar_t* urlStr)
{
	MSG	msg;
	std::wstring className(BROWSER_FRAME_CLASS);	

	if (OleInitialize(NULL) == S_OK)
	{
		WNDCLASSEX		wc;		

		ZeroMemory(&wc, sizeof(WNDCLASSEX));
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.hInstance = m_hInstance;
		wc.lpfnWndProc = WindowProc;
		wc.lpszClassName = &className[0];
		RegisterClassEx(&wc);

		m_hWnd = CreateWindowEx(0, &className[0], BROWSER_FRAME_TITLE, WS_SYSMENU | WS_CAPTION | WS_DLGFRAME,
			CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, HWND_DESKTOP, NULL, m_hInstance, this);

		if (m_hWnd != NULL && m_hWnd != INVALID_HANDLE_VALUE)
		{
			msg.hwnd = m_hWnd;

			if (modal && (parent != NULL && parent != INVALID_HANDLE_VALUE && parent != HWND_DESKTOP))			
				EnableWindow(parent, FALSE);

			::ShowWindow(m_hWnd, SW_SHOWDEFAULT);
			UpdateWindow(m_hWnd);

			Navigate(urlStr);

			while (GetMessage(&msg, 0, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if (modal && (parent != NULL && parent != INVALID_HANDLE_VALUE && parent != HWND_DESKTOP))
				EnableWindow(parent, TRUE);				
		}	

		OleUninitialize();
	}
}
