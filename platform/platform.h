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
#define P_REST_HEADER_VALUELEN 2048

#define P_REST_QPARAM_STRLEN 48
#define P_REST_QVALUE_STRLEN 1024

#define P_SUCCESS P_ERROR(0x00)
#define P_NO_IMPL (0xffffffff)

#define P_ERROR(errC) (errC)
#define P_OK(x) (x == P_SUCCESS)
#define P_ERROR_FAIL (!P_ERROR_OK)

#define P_ERROR_PLATFORM_INIT P_ERROR(-0x01)
#define P_ERROR_INVALIDLENGTH P_ERROR(-0x0102)
#define P_ERROR_DESTTOOSMALL (-0x0103)
#define P_ERROR_PLATFORM_UNINIT P_ERROR(-0x0104)
#define P_ERROR_SYSTEM P_ERROR(-0x0105)

#define P_ERROR_LIBLOAD P_ERROR(-0x03)
#define P_ERROR_BROWSER P_ERROR(-0x04)
#define P_ERROR_BROWSER_LOAD P_ERROR(-0x0400)
#define P_ERROR_BROWSER_INIT P_ERROR(-0x0401)
#define P_ERROR_BROWSER_NAVIGATE P_ERROR(-0x0402)
#define P_ERROR_BROWSER_DISPLAY P_ERROR(-0x0403)
#define P_ERROR_BROWSER_CLEANUP P_ERROR(-0x9999) /* All cleanups have this error code */
#define P_ERROR_WINDOW P_ERROR(0x05)

#define P_ERROR_OUTOFMEMORY (-0x0101)

#define P_ERROR_STRING P_ERROR(-0x06)

#define P_ERROR_POINTER (-0x701)

#define P_ERROR_OPS (-0x09)
#define P_ERROR_CRYPT (-0x902)

#define P_ERROR_INVALID_ARG (-0x801)

#define P_WARN_MEMORYNOTCLEARED (0x10)

#define P_FALSE (0)
#define P_TRUE (!P_FALSE)

#define P_ERR_INSUFFICIENT_MEMORY (-100)

#ifdef __cplusplus
extern "C" {
#endif

	typedef int pBool_t;
	typedef unsigned long long pLen_t;
	typedef unsigned char pByte_t;
	typedef int pError_t;

	struct _platform_t;


	typedef void (*pfnProgressNotifier_t)(int progressPercent);
	typedef void(*pfnWebRequestCallBack_t)(struct _platform_t* platform, void* response, size_t responseBufferSize, size_t* responseSize, pError_t* error);

	typedef char* pUtf8_t;
	
#define UTF8(text) u8#text
	
	typedef struct {
		int major, minor, revision, patch;
	} ptVersion_t;

	enum peSupportedPlatforms
	{
		Win64,
		AppleOSX64,
		RHEL64
	};	

	enum LogLevel
	{
		LOG_INFO,
		LOG_WARN,
		LOG_ERROR
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

	typedef pError_t(*pfnBrowserEvent_t)(struct _platform_t* platform, struct WebBrowserFrame* frame, const wchar_t* url, const wchar_t* error, int *bShouldCancel);

	struct WebBrowserEventHandler
	{
		pfnBrowserEvent_t onAfterPageLoad;
		pfnBrowserEvent_t onError;
		pfnBrowserEvent_t onFrameWindowClosed;
		pfnBrowserEvent_t onRedirect;
	};

	struct WebBrowserFrame
	{
		pError_t (*ShowBrowserFrame)(struct _platform_t* platform, void* appHandle, void* parentWindow, const wchar_t* u8UrlString); //TODO: pUtf8_t
		pError_t (*CloseBrowserFrame)(struct _platform_t* platform);		

		struct WebBrowserEventHandler events;

		pError_t (*Unload)(struct _platform_t* platform);
		void* _internal;
	};

	////Url
	struct Url
	{
		pError_t(*UrlEncode)(struct _platform_t* platform, char16_t* dest, const size_t destSize, const char16_t* url);
		pError_t(*UrlDecode)(struct _platform_t* platform, char16_t* dest, const size_t destSize, const char16_t* url);
	};

	////Crypto
	struct Crypto
	{
		pError_t(*HmacSha1)(struct _platform_t* platform, pByte_t* dest, const size_t destSize, size_t* calculatedSize, const pByte_t* str, const size_t strLen, const pByte_t* secret, const size_t secretLen);
	};


	////Logger
	struct Logger
	{
		pError_t(*Log)(struct _platform_t* platform, enum LogLevel logLevel, const char16_t* logString);
		pError_t(*LogW)(struct _platform_t* platform, enum LogLevel logLevel, const wchar_t* logString, ...);
	};

	////Strings
	struct Strings
	{
		pError_t(*Utf8ToUtf16)(struct _platform_t* platform, char16_t* dest, const size_t destSize, const pUtf8_t str, const size_t strlen);
		pError_t(*Utf8ToUtf32)(struct _platform_t* platform, char32_t* dest, const size_t destSize, const pUtf8_t str, const size_t strlen);
		pError_t(*Utf16ToUtf8)(struct _platform_t* platform, pUtf8_t dest, const size_t destSize, const char16_t* str, const size_t strlen);
		pError_t(*Utf16ToUtf32)(struct _platform_t* platform, char32_t* dest, const size_t destSize, const char16_t* str, const size_t strlen);
		pError_t(*Utf32ToUtf16)(struct _platform_t* platform, char16_t* dest, const size_t destSize, const char32_t* str, const size_t strlen);
		pError_t(*Utf32ToUtf8)(struct _platform_t* platform, pUtf8_t dest, const size_t destSize, const char32_t* str, const size_t strlen);
				
		pError_t(*Utf8ToWChar)(struct _platform_t* platform, wchar_t* dest, const size_t destSize, const pUtf8_t str, const size_t strlen);
		pError_t(*WCharToUtf8)(struct _platform_t* platform, pUtf8_t dest, const size_t destSize, const wchar_t* str, const size_t strlen);
		pError_t(*Utf16ToWChar)(struct _platform_t* platform, wchar_t* dest, const size_t destSize, const char16_t str, const size_t strlen);
		pError_t(*WCharToUtf16)(struct _platform_t* platform, char16_t* dest, const size_t destSize, const wchar_t* str, const size_t strlen);
		pError_t(*Utf32ToWChar)(struct _platform_t* platform, wchar_t* dest, const size_t destSize, const char32_t *str, const size_t strlen);
		pError_t(*WCharToUtf32)(struct _platform_t* platform, char32_t* dest, const size_t destSize, const wchar_t* str, const size_t strlen);

		
		pError_t(*CharToUtf16)(struct _platform_t* platform, wchar_t* dest, const size_t destSize, const char* str, const size_t strlen);
		
		
		//Statics
		size_t(*StrlenU8)(struct _platform_t* platform, const pUtf8_t str);
		size_t(*StrlenU16)(struct _platform_t* platform, const char16_t* str);
		size_t(*StrlenU32)(struct _platform_t* platform, const char32_t* str);

		//Unload
		pError_t (*Unload)(struct _platform_t* _platform_t);

		void* _internal;
	};

	////REST API

#define HTTP_GET 0
#define HTTP_POST 1
#define HTTP_PUT 2
#define HTTP_DELETE 3
#define HTTP_PATCH 4
	
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
		
	struct WebRequest
	{					
		pError_t(*SendRequest)(struct _platform_t* platform, int httpRequestType, const wchar_t* url, struct HttpHeader* headers, size_t headerCount, void* body, size_t bodySize, void* response, size_t responseBufferSize, size_t *responseSize);
		pError_t(*SendRequestAsync)(struct _platform_t* platform, int httpRequestType, const wchar_t* url, struct HttpHeader* headers, size_t headerCount, void* body, size_t bodySizem, pfnWebRequestCallBack_t callback);
		
		pError_t(*Unload)(struct _platform_t* platform);
		void* _internal;
	};
	
	typedef struct _platform_t {		

		//struct AutoUpdate autoUpdater;
		struct WebBrowserFrame browserFrame;		
		struct WebRequest webRequest;
		struct Strings strings;
		struct Url url;
		struct Logger logger;
		struct Crypto crypto;

		pError_t(*Unload)(struct _platform_t* platform);
		void* _internal;
	} Platform_t;
	//LoadPlatform

	pError_t LoadPlatform(Platform_t* platform, void *appInstanceHandle);
	   	
#ifdef __cplusplus
}
#endif