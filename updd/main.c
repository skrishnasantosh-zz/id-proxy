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

	char16_t test[20] = { 0 };
	const char* u8str = u8"Test U8";

	printf("\n Error : 0x%0x\n", platform.strings.Utf8ToWChar(&platform, test, 12, u8"Trail", 5));

	platform.Unload(&platform);
	
	wprintf(L"String is : %s\n", test);

	return 0;
}