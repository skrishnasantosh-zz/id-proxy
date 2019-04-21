#include "../platform/platform.h"
#include "Browser.h"
#include <varargs.h>

extern "C" pError_t HmacSha1(Platform_t* platform, pByte_t* dest, const size_t destSize, size_t* calculatedSize, const pByte_t* str, const size_t strLen, const pByte_t* secret, const size_t secretLen);
pError_t LogW(struct _platform_t* platform, enum LogLevel logLevel, const wchar_t* logString, ...)
{
	wchar_t szLogText[10 * 1024] = { 0 };

	va_list args;
	va_start(args, logString);

	vswprintf_s(szLogText, 10 * 1024, logString, args);

	wprintf(L"\n%s", szLogText);

	va_end(args);

	return P_SUCCESS;
}

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
	platform->crypto.HmacSha1 = HmacSha1;
	platform->logger.LogW = LogW;
	
	return P_SUCCESS;
}
