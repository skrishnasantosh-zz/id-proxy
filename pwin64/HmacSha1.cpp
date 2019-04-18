#include <stdio.h>
#include <windows.h>
#include <wincrypt.h>
#include <iostream>
#include "../platform/platform.h"

pError_t HmacSha1(Platform_t* platform, pByte_t* dest, const size_t destSize, const pByte_t* str, const size_t strLen, const pByte_t* secret, const size_t secretLen)
{
	if (platform == nullptr || dest == nullptr || str == nullptr || secret == nullptr)
		return P_ERROR_POINTER;

	if (destSize == 0 || strLen == 0 || secretLen == 0)
		return P_ERROR_INVALID_ARG;

	HMAC_INFO hMacInfo = { 0 };
	hMacInfo.HashAlgid = CALG_SHA1;

	HCRYPTPROV hProvider = NULL;
	HCRYPTHASH hHash = NULL;
	HCRYPTKEY hKey = NULL;
	HCRYPTHASH hMac = NULL;
	PBYTE pbHash = NULL;
	DWORD dwDataLen = 0;
	unsigned long ret = P_SUCCESS;
		
	if (!CryptAcquireContext(&hProvider, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
	{
		if (platform->logger.LogW != nullptr)
			platform->logger.LogW(platform, LOG_ERROR, L"Unable to Acquire Cryptographic Context. Windows Error %0x", GetLastError());

		ret = P_ERROR_OPS;
	}

	if (ret == P_SUCCESS && !CryptCreateHash(hProvider, CALG_SHA1, NULL, 0, &hHash))
	{
		if (platform->logger.LogW != nullptr)
			platform->logger.LogW(platform, LOG_ERROR, L"Unable to create Cryptographic hash. Windows Error %0x", GetLastError());

		ret = P_ERROR_OPS;
	}

	if (ret == P_SUCCESS && !CryptHashData(hHash, (BYTE*)secret, secretLen, 0))
	{
		if (platform->logger.LogW != nullptr)
			platform->logger.LogW(platform, LOG_ERROR, L"Unable to create Hash secret. Windows Error %0x", GetLastError());

		ret = P_ERROR_OPS;
	}

	if (ret == P_SUCCESS && !CryptDeriveKey(hProvider, CALG_RC4, hHash, 0, &hKey))
	{
		if (platform->logger.LogW != nullptr)
			platform->logger.LogW(platform, LOG_ERROR, L"Unable to derive Hash key. Windows Error %0x", GetLastError());

		ret = P_ERROR_OPS;
	}

	if (ret == P_SUCCESS && !CryptCreateHash(hProvider, CALG_HMAC, hKey, 0, &hMac))
	{
		if (platform->logger.LogW != nullptr)
			platform->logger.LogW(platform, LOG_ERROR, L"Unable to create Hash HMAC. Windows Error %0x", GetLastError());

		ret = P_ERROR_OPS;
	}

	if (ret == P_SUCCESS && !CryptGetHashParam(hMac, HP_HASHVAL, NULL, &dwDataLen, 0))
	{
		if (platform->logger.LogW != nullptr)
			platform->logger.LogW(platform, LOG_ERROR, L"Unable to get hash length. Windows Error %0x", GetLastError());

		ret = P_ERROR_OPS;
	}

	if (dwDataLen > destSize)	
		ret = P_ERROR_STRING_DESTTOOSMALL;	

	if (ret == P_SUCCESS && !CryptGetHashParam(hMac, HP_HASHVAL, (BYTE*)dest, &dwDataLen, 0))
	{
		if (platform->logger.LogW != nullptr)
			platform->logger.LogW(platform, LOG_ERROR, L"Unable to get hash value. Windows Error %0x", GetLastError());

		ret = P_ERROR_OPS;
	}

	if (hMac)
		CryptDestroyHash(hMac);
	if (hKey)
		CryptDestroyKey(hKey);
	if (hHash)
		CryptDestroyHash(hHash);
	if (hProvider)
		CryptReleaseContext(hProvider, 0);

	return ret;
}