#include "../platform/platform.h"
#include "Browser.h"

pError_t HmacSha1(Platform_t* platform, pByte_t* dest, const size_t destSize, const char16_t* str, const char16_t* secret);

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

pError_t UrlEncode(Platform_t* platform, char16_t* dest, const size_t destSize, const char16_t* url)
{
	


	return P_SUCCESS;
}

pError_t UrlDecode(Platform_t* platform, char16_t* dest, const size_t destSize, const char16_t* url)
{
	return P_SUCCESS;
}

pError_t InitializePlatform(Platform_t* platform)
{
	if (platform == NULL)
		return P_ERROR_PLATFORM_INIT;

	platform->browserFrame.ShowBrowserFrame = ShowBrowserFrame;
	
	return P_SUCCESS;
}
