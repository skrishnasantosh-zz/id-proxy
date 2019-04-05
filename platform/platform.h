#pragma once


#if defined(_WIN32) 
#define __PLATFORM__ L"Windows"
#define PLATFORM_WINDOWS 0x01
#elif defined(__APPLE__)
#define __PLATFORM__ L"OSX"
#define PLATFORM_MAC 0x01
#elif defined(__linux__)
#define __PLATFORM__ L"Linux"
#define PLATFORM_LINUX 0x01
#elif defined(__unix__)
#define __PLATFORM__ L"Unix"
#define PLATFORM_LINUX 0x01
#endif

#define P_PATH_MAX_STRLEN 1024
#define P_FILE_MAX_STRLEN 256
#define P_ERROR_MAX_STRLEN 1024
#define P_GUID_MAX_STRLEN 33
#define P_HASH_MAX_STRLEN 64

#define P_ERROR(errC) {errC}
#define P_ERROR_OK(x) (x.code == 0)
#define P_ERROR_FAIL !P_ERROR_OK

#define P_FALSE 0
#define P_TRUE !P_FALSE

#ifdef __cplusplus
extern "C" {
#endif

	typedef int pBool_t;
	typedef unsigned long long pLen_t;

	typedef void (*pfnProgressNotifier_t)(int progressPercent);
	typedef char* pUtf8_t;

	typedef struct {
		long code;	
	}ptError_t;

	struct _platform_t;

	typedef struct {
		int major, minor, revision, patch;
	} ptVersion_t;

	enum peSupportedPlatforms
	{
		Win64,
		AppleOSX64,
		RHEL64
	};	

	struct AutoUpdate;
	struct Platform_t;

	////Web Browser
		
	struct WebBrowserFrame;

	enum BrowserEventTypes
	{
		PageLoaded,
		PageUnloaded,
		UrlChanged,
	};

	typedef void (*pfnBrowserEvent_t)(struct _platform_t* platform, struct WebBrowserFrame* frame, const pUtf8_t* url, const pUtf8_t* error, int *bShouldCancel);

	struct WebBrowserEventHandler
	{
		pfnBrowserEvent_t onAfterPageLoad;
		pfnBrowserEvent_t onError;
		pfnBrowserEvent_t onFrameWindowClosed;
		pfnBrowserEvent_t onRedirect;
	};

	struct WebBrowserFrame
	{
		ptError_t(*ShowBrowserFrame)(struct _platform_t* platform, void* appHandle, pUtf8_t u8UrlString);
		ptError_t(*CloseBrowserFrame)(struct _platform_t* platform);		

		struct WebBrowserEventHandler events;
	};

	struct PlatformInternal
	{
		void* moduleHandle;
		void* windowHandle;
		void* browserHandle;
	};

	typedef struct _platform_t {		

		//struct AutoUpdate autoUpdater;
		struct WebBrowserFrame browserFrame;

		struct PlatformInternal internals;

	} Platform_t;
	//LoadPlatform

	extern void LoadPlatform(Platform_t* platform, void *appInstanceHandle);
	extern void UnloadPlatform(Platform_t* platform);
	
#ifdef __cplusplus
}
#endif