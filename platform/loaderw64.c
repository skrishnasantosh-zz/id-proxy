#pragma once

#include "platform.h"


#ifdef PLATFORM_WINDOWS
#include <Windows.h>

#if (NTDDI_VERSION >= NTDDI_WIN8) //Windows 8+ deprecates PathRemoveFileSpec... So check it before using
#include <PathCch.h>
#pragma comment(lib, "Pathcch.lib")
#else
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#endif

#define FILE_STRLEN 1024

extern pError_t Utf8ToUtf16(struct _platform_t* platform, char16_t* dest, const size_t destSize, const char* str, const size_t strlen);
extern pError_t Utf8ToWChar(struct _platform_t* platform, wchar_t* dest, const size_t destSize, const pUtf8_t str, const size_t strlen);

typedef BOOL (*pInitializePlatform_t)(Platform_t* platform);

pError_t UnloadPlatform(Platform_t* platform);

struct _internal
{
	HMODULE library;
};

void InitDefaults(Platform_t* platform)
{
	platform->strings.Utf8ToUtf16 = Utf8ToUtf16;
	platform->strings.Utf8ToWChar = Utf8ToWChar;
}

pError_t LoadPlatform(Platform_t* platform, void* appInstanceHandle)
{
	HANDLE hInstance = (HINSTANCE)appInstanceHandle;
	TCHAR filePath[FILE_STRLEN] = { 0 };
	HRESULT result = S_FALSE;
	DWORD bytesWritten;
	const TCHAR *winDll = TEXT("C:\\Source\\id-proxy\\bin\\Debug\\pwin64.dll");

	if (platform == NULL)
		return P_ERROR_BROWSER_LOAD;

	memset(platform, 0, sizeof(Platform_t));

	platform->_internal = calloc(1, sizeof(struct _internal));

	if (platform->_internal == NULL)
		return P_ERROR(P_ERR_INSUFFICIENT_MEMORY);

	InitDefaults(platform);
		
	GetModuleFileName(hInstance, filePath, FILE_STRLEN);

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	WriteConsole(hConsole, winDll, lstrlen(winDll), &bytesWritten, NULL);

	HMODULE hLib = LoadLibrary(winDll);

	if (hLib != 0)
	{
		FARPROC proc = GetProcAddress(hLib, "InitializePlatform");
		pInitializePlatform_t pInitFunc = (pInitializePlatform_t)proc;
		pInitFunc(platform);		
	}

	platform->Unload = UnloadPlatform;


	return P_ERROR(0);
}

pError_t UnloadPlatform(Platform_t* platform)
{
	if (platform->browserFrame.Unload != NULL)	
		platform->browserFrame.Unload(platform);	

	if (platform->webRequest.Unload != NULL)
		platform->webRequest.Unload(platform);

	if (platform->strings.Unload != NULL)
		platform->strings.Unload(platform);

	if (platform->_internal != NULL)	
		FreeLibrary((HMODULE)platform->_internal);	
	
	return P_SUCCESS;
}

#endif //PLATFORM_WINDOWS
