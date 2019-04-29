#pragma once

#include <uchar.h>
#include "../platform/platform.h"

#ifdef __cplusplus
extern "C" {
#endif

////Crypto
pError_t HmacSha1(Platform_t* platform, pByte_t* dest, const size_t destSize, const char16_t* str, const char16_t* secret);

////Browser
pError_t ShowBrowserFrame(Platform_t* platform, void* appHandle, void* parentWindow, const wchar_t* url);

////REST Api helper
pError_t SendRequest(struct _platform_t* platform, int httpRequestType, const wchar_t* url, struct HttpHeader* headers, size_t headerCount, void* body, size_t bodySize, void* response, size_t responseBufferSize, size_t* responseSize);

#ifdef __cplusplus
}
#endif
