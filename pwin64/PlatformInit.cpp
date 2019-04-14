#include "../platform/platform.h"
#include "Browser.h"

pError_t ShowBrowserFrame(Platform_t* platform, void* appHandle, void* parentWindow, const wchar_t* url)
{

	HINSTANCE hInstance = static_cast<HINSTANCE>(appHandle);
	HWND hWndParent = static_cast<HWND>(parentWindow);

	AdBrowser* frame = new AdBrowser(hInstance, L"Test");
	frame->ShowWindow(hWndParent, TRUE, url);

	delete frame;

	UNREFERENCED_PARAMETER(platform);

	return P_SUCCESS;
}

pError_t InitializePlatform(Platform_t* platform)
{
	if (platform == NULL)
		return P_ERROR_PLATFORM_INIT;

	platform->browserFrame.ShowBrowserFrame = ShowBrowserFrame;
	
	return P_SUCCESS;
}
