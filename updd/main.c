#include <stdio.h>

#include <platform.h>
#include <windows.h>

//extern void ShowBrowserFrame();

int main(int argc, char** argv)
{
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);

	SetProcessDPIAware();

	wprintf(L"%s\n", __PLATFORM__);

	Platform_t platform;

	LoadPlatform(&platform, GetModuleHandle(NULL));

	platform.browserFrame.events.onAfterPageLoad = NULL;
	platform.browserFrame.events.onError = NULL;

	platform.browserFrame.ShowBrowserFrame(&platform, GetModuleHandle(NULL), NULL, TEXT("https://accounts-dev.autodesk.com"));

	platform.Unload(&platform);

	return 0;
}