#include <stdio.h>

#include <platform.h>
#include <windows.h>

//extern void ShowBrowserFrame();

int main(int argc, char** argv)
{
	ptError_t error = P_ERROR(0);

	int ok = (P_ERROR_OK(error));

	wprintf(L"%s\n", __PLATFORM__);

	Platform_t platform;

	LoadPlatform(&platform, GetModuleHandle(NULL));

	platform.browserFrame.events.onAfterPageLoad = NULL;
	platform.browserFrame.events.onError = NULL;

	platform.browserFrame.ShowBrowserFrame(&platform, GetModuleHandle(NULL), NULL);
	

	UnloadPlatform(&platform);
}