#include "..\platform\platform.h"
#include "BrowserFrame.h"

struct _internal
{
	AdBrowserFrame* frame;
};

pError_t ShowBrowserFrame(struct _platform_t* platform, void* appHandle, pUtf8_t u8UrlString)
{
	struct _internal* in = static_cast<struct _internal*>(platform->browserFrame._internal);
	if (in->frame != NULL)
	{
		in->frame->OpenBrowser(L"");
		return P_ERROR(0);
	}

	return P_ERROR(-1);
}

pError_t UnloadBrowserFrame(struct _platform_t* platform)
{
	if (platform->browserFrame._internal)
	{
		struct _internal* in = static_cast<struct _internal*>(platform->browserFrame._internal);
		if (in != NULL && in->frame)
			delete in->frame;

		delete platform->browserFrame._internal;

		return P_ERROR(0);
	}

	return P_ERROR(-1);
}

extern "C" pError_t InitializePlatform(Platform_t * platform)
{
	HMODULE hModule = GetModuleHandle(NULL);

	platform->browserFrame._internal = new struct _internal();
	AdBrowserFrame* frame = new AdBrowserFrame(platform, hModule);

	struct _internal* in = static_cast<struct _internal*>(platform->browserFrame._internal);

	in->frame = frame;
	platform->browserFrame.ShowBrowserFrame = ShowBrowserFrame;

	platform->browserFrame.Unload = UnloadBrowserFrame;

	return TRUE;
}
