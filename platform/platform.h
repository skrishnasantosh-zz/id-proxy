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

#include <uchar.h>

#define P_PATH_MAX_STRLEN 1024
#define P_FILE_MAX_STRLEN 256
#define P_ERROR_MAX_STRLEN 1024
#define P_GUID_MAX_STRLEN 33
#define P_HASH_MAX_STRLEN 64

#define P_REST_HEADER_KEYLEN 48
#define P_REST_HEADER_VALUELEN 256

#define P_REST_QPARAM_STRLEN 48
#define P_REST_QVALUE_STRLEN 1024


#define P_ERROR(errC) (errC)
#define P_ERROR_OK(x) (x == 0)
#define P_ERROR_FAIL (!P_ERROR_OK)

#define P_SUCCESS P_ERROR(0x00)

#define P_ERROR_PLATFORM_INIT P_ERROR(0x01)
#define P_ERROR_PLATFORM_UNINIT P_ERROR(0x02)
#define P_ERROR_LIBLOAD P_ERROR(0x03)
#define P_ERROR_BROWSER P_ERROR(0x04)
#define P_ERROR_BROWSER_INIT P_ERROR(0x0401)
#define P_ERROR_BROWSER_NAVIGATE P_ERROR(0x0402)
#define P_ERROR_BROWSER_DISPLAY P_ERROR(0x0403)
#define P_ERROR_BROWSER_CLEANUP P_ERROR(0x9999) /* All cleanups have this error code */
#define P_ERROR_WINDOW P_ERROR(0x05)

#define P_FALSE (0)
#define P_TRUE (!P_FALSE)

#define P_ERR_INSUFFICIENT_MEMORY (-100)

#ifdef __cplusplus
extern "C" {
#endif

	typedef int pBool_t;
	typedef unsigned long long pLen_t;

	typedef void (*pfnProgressNotifier_t)(int progressPercent);
	typedef char* pUtf8_t;
	
#define UTF8(text) u8"text"

	typedef int pError_t;

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
	struct _platform_t;
	struct RestApiHelper;

	////Web Browser
		
	struct WebBrowserFrame;
	
	enum BrowserEventTypes
	{
		PageLoaded,
		PageUnloaded,
		UrlChanged,
	};

	typedef pError_t(*pfnBrowserEvent_t)(struct _platform_t* platform, struct WebBrowserFrame* frame, const pUtf8_t* url, const pUtf8_t* error, int *bShouldCancel);

	struct WebBrowserEventHandler
	{
		pfnBrowserEvent_t onAfterPageLoad;
		pfnBrowserEvent_t onError;
		pfnBrowserEvent_t onFrameWindowClosed;
		pfnBrowserEvent_t onRedirect;
	};

	struct WebBrowserFrame
	{
		pError_t (*ShowBrowserFrame)(struct _platform_t* platform, void* appHandle, void* parentWindow, pUtf8_t u8UrlString);
		pError_t (*CloseBrowserFrame)(struct _platform_t* platform);		

		struct WebBrowserEventHandler events;

		pError_t (*Unload)(struct _platform_t* platform);
		void* _internal;
	};

	////Strings
	struct Strings
	{
		size_t(*Utf8toUtf16)(struct _platform_t* platform, char16_t* dest, const size_t destSize, const char* str, const size_t strlen);
		size_t(*Utf8toUtf32)(struct _platform_t* platform, char32_t* dest, const size_t destSize, const char* str, const size_t strlen);
		size_t(*Utf16toUtf8)(struct _platform_t* platform, char* dest, const size_t destSize, const char16_t* str, const size_t strlen);
		size_t(*Utf16toUtf32)(struct _platform_t* platform, char32_t* dest, const size_t destSize, const char16_t* str, const size_t strlen);
		size_t(*Utf32toUtf16)(struct _platform_t* platform, char16_t* dest, const size_t destSize, const char32_t* str, const size_t strlen);
		size_t(*Utf32toUtf8)(struct _platform_t* platform, char* dest, const size_t destSize, const char32_t* str, const size_t strlen);
		
		size_t(*CharToWideChar)(struct _platform_t* platform, wchar_t* dest, const size_t destSize, const char* str, const size_t strlen);
		size_t(*WideCharToChar)(struct _platform_t* platform, char* dest, const size_t destSize, const wchar_t* str, const size_t strlen);

		pError_t (*Unload)(struct platform_t* _platform_t);

		void* _internal;
	};

	////REST API

	struct HttpHeader
	{
		wchar_t headerKey[P_REST_HEADER_KEYLEN];
		wchar_t headerValue[P_REST_HEADER_VALUELEN];
	};

	struct HttpQuery
	{
		wchar_t queryParam[P_REST_QPARAM_STRLEN];
		wchar_t queryValue[P_REST_QVALUE_STRLEN];
	};

	struct OAuth1
	{
		pError_t(*Get)(const wchar_t* fullUrl, const wchar_t* consumerKey, const wchar_t* consumerSecret, const wchar_t *token, const wchar_t* tokenSecret, struct HttpHeader* additionalHeaders, size_t headerCount);
	};

	struct OAuth2
	{
		pError_t(*Get)(const wchar_t* fullUrl, const wchar_t* clientId, const wchar_t* clientSecret, const wchar_t* token, const wchar_t* tokenSecret, struct HttpHeader* additionalHeaders, size_t headerCount);
	};

	struct RestApiHelper
	{
		struct OAuth1 oAuth1;
		struct OAuth2 oAuth2;
						
		pError_t(*Get)(const wchar_t* fullUrl, struct Headers* additionalHeaders, size_t headerCount);

		pError_t(*Unload)(struct platform_t* platform);
		void* _internal;
	};
	
	typedef struct _platform_t {		

		//struct AutoUpdate autoUpdater;
		struct WebBrowserFrame browserFrame;		
		struct RestApiHelper restApi;
		struct Strings strings;

		pError_t(*Unload)(struct platform_t* platform);
		void* _internal;
	} Platform_t;
	//LoadPlatform

	extern pError_t LoadPlatform(Platform_t* platform, void *appInstanceHandle);
	
#ifdef __cplusplus
}
#endif