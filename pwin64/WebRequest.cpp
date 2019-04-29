#include <Windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")

#include "../platform/platform.h"

#define HTTP_AGENT L"Autodesk Identity Desktop Client 1.0"

pError_t SendRequest(struct _platform_t* platform, int httpRequestType, const wchar_t* url, struct HttpHeader* headers, size_t headerCount, void* body, size_t bodySize, void* response, size_t responseBufferSize, size_t* responseSize)
{
	pError_t status = P_SUCCESS;
	HINTERNET hSession = nullptr, hConnect = nullptr, hRequest = nullptr;
	TCHAR szHttpType[8] = { 0 };

	if (url == nullptr || platform == nullptr)
		return P_ERROR_INVALID_ARG;

	if (httpRequestType < HTTP_GET || httpRequestType > HTTP_PATCH)
		return P_ERROR_INVALID_ARG;

	hSession = WinHttpOpen(HTTP_AGENT, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	
	if (hSession != nullptr && hSession != INVALID_HANDLE_VALUE)
		hConnect = WinHttpConnect(hSession, url, INTERNET_DEFAULT_HTTPS_PORT, 0); //Always HTTPS
	else
		if (platform->logger.LogW != nullptr)
			platform->logger.LogW(platform, LOG_ERROR, L"Unable to create HTTP session. Windows Returned 0x%x", GetLastError());

	if (hConnect != nullptr && hConnect != INVALID_HANDLE_VALUE)
	{
		switch (httpRequestType)
		{
		case HTTP_GET:
			wcscpy_s(szHttpType, 8, L"GET");
			break;
		case HTTP_POST:
			wcscpy_s(szHttpType, 8, L"POST");
			break;
		case HTTP_PUT:
			wcscpy_s(szHttpType, 8, L"PUT");
			break;
		case HTTP_DELETE:
			wcscpy_s(szHttpType, 8, L"DELETE");
			break;
		case HTTP_PATCH:
			wcscpy_s(szHttpType, 8, L"PATCH");
			break;
		default:
			break;
		}

		if (szHttpType[0] != 0)		
			hRequest = WinHttpOpenRequest(hConnect, szHttpType, NULL, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
	}
	else
		if (platform->logger.LogW != nullptr)
			platform->logger.LogW(platform, LOG_ERROR, L"Unable to connect Win HTTP. Windows Returned 0x%x", GetLastError());
		
	if (hRequest != nullptr && hRequest != INVALID_HANDLE_VALUE)
	{
		if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0))
		{
			if (platform->logger.LogW != nullptr)
				platform->logger.LogW(platform, LOG_ERROR, L"Unable to send HTTP Request. Windows Returned 0x%x", GetLastError());
		}
	}
	else
		if (platform->logger.LogW != nullptr)
			platform->logger.LogW(platform, LOG_ERROR, L"Unable to Open HTTP Request. Windows Returned 0x%x", GetLastError());



	return status;
}
