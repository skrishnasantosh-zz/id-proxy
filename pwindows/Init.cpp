#include "..\platform\platform.h"
#include "BrowserFrame.h"

struct _internal
{
	AdBrowserFrame* frame;
};

pError_t ShowBrowserFrame(struct _platform_t* platform, void* appHandle, void *parentWindow, pUtf8_t u8UrlString)
{
	struct _internal* in = static_cast<struct _internal*>(platform->browserFrame._internal);
	if (in->frame != NULL)
	{
		HWND parent = NULL;

		if (!::IsBadReadPtr(parentWindow, sizeof(HWND)))		
			parent = (HWND)parentWindow;		

		in->frame->OpenBrowser(parent, FALSE, L"");

		return P_SUCCESS;
	}

	return P_ERROR_BROWSER_DISPLAY;
}

pError_t UnloadBrowserFrame(struct _platform_t* platform)
{
	if (platform->browserFrame._internal)
	{
		struct _internal* in = static_cast<struct _internal*>(platform->browserFrame._internal);
		if (in != NULL && in->frame)
			delete in->frame;

		delete platform->browserFrame._internal;

		return P_SUCCESS;
	}

	return P_ERROR_BROWSER_CLEANUP;
}

pError_t InitializePlatform(Platform_t * platform)
{
	HMODULE hModule = GetModuleHandle(NULL);

	platform->browserFrame._internal = new struct _internal();
	AdBrowserFrame* frame = new AdBrowserFrame(platform, hModule);

	struct _internal* in = static_cast<struct _internal*>(platform->browserFrame._internal);

	if (in == NULL)
		return P_ERROR_PLATFORM_INIT;

	in->frame = frame;
	platform->browserFrame.ShowBrowserFrame = ShowBrowserFrame;

	platform->browserFrame.Unload = UnloadBrowserFrame;

	return P_SUCCESS;
}
