#include <stdio.h>
#include <windows.h>
#include <wincrypt.h>
#include <iostream>
#include <bcrypt.h>
#include <excpt.h>
#include "../platform/platform.h"

//As per cryptographic standards for MS Windows BCrypt as defined in https://csrc.nist.gov/csrc/media/projects/cryptographic-module-validation-program/documents/security-policies/140sp892.pdf

#define NT_SUCCESS(Status)          (((NTSTATUS)(Status)) >= 0)
#define STATUS_UNSUCCESSFUL         ((NTSTATUS)0xC0000001L)

extern "C" pError_t HmacSha1(Platform_t* platform, pByte_t* dest, const size_t destSize, size_t *calculatedSize, const pByte_t* data, const size_t dataLen, const pByte_t* secret, const size_t secretLen)
{
	BCRYPT_ALG_HANDLE hAlg = nullptr;
	BCRYPT_HASH_HANDLE hHash = nullptr;
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	DWORD cbData(0), cbHash(0), cbHashObject(0);
	pByte_t* pHashObject = nullptr;
	pByte_t* pHash = nullptr;
	pError_t ret = P_SUCCESS;

	__try {

		if (platform == nullptr || dest == nullptr || data == nullptr || secret == nullptr)
			return P_ERROR_INVALID_ARG;

		if (destSize == 0)
			return P_ERROR_DESTTOOSMALL;

		if (dataLen == 0 || secretLen == 0)
			return P_ERROR_INVALIDLENGTH;

		__try {

			if (ret == P_SUCCESS && !NT_SUCCESS(status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA1_ALGORITHM, NULL, BCRYPT_ALG_HANDLE_HMAC_FLAG)))
			{
				if (platform->logger.LogW != nullptr)
					platform->logger.LogW(platform, LOG_ERROR, L"BCryptOpenAlgorithmProvider: error code 0x%x", status);

				ret = P_ERROR_CRYPT;
			}

			if (ret == P_SUCCESS && !NT_SUCCESS(status = BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH, (BYTE*)& cbHashObject, sizeof(DWORD), &cbData, 0)))
			{
				if (platform->logger.LogW != nullptr)
					platform->logger.LogW(platform, LOG_ERROR, L"BCryptGetProperty(BCRYPT_OBJECT_LENGTH): error code 0x%x", status);

				ret = P_ERROR_CRYPT;
			}

			if (ret == P_SUCCESS)
			{
				pHashObject = (pByte_t*)calloc(1, cbHashObject);

				if (pHashObject == NULL)
				{
					ret = P_ERROR_OUTOFMEMORY;
					if (platform->logger.LogW != nullptr)
						platform->logger.LogW(platform, LOG_ERROR, L"Unable to allocate %ul bytes", cbHashObject);
				}
			}

			if (ret == P_SUCCESS && !NT_SUCCESS(status = BCryptGetProperty(hAlg, BCRYPT_HASH_LENGTH, (BYTE*)& cbHash, sizeof(DWORD), &cbData, 0)))
			{
				if (platform->logger.LogW != nullptr)
					platform->logger.LogW(platform, LOG_ERROR, L"BCryptGetProperty(BCRYPT_HASH_LENGTH): error code 0x%x", status);

				ret = P_ERROR_CRYPT;
			}

			if (cbHash > destSize)
			{
				if (platform->logger.LogW != nullptr)
					platform->logger.LogW(platform, LOG_ERROR, L"BCryptGetProperty(BCRYPT_HASH_LENGTH): error code 0x%x", status);

				ret = P_ERROR_DESTTOOSMALL;
			}

			if (ret == P_SUCCESS && !NT_SUCCESS(status = BCryptCreateHash(hAlg, &hHash, pHashObject, cbHashObject, (PUCHAR)secret, secretLen, 0)))
			{
				if (platform->logger.LogW != nullptr)
					platform->logger.LogW(platform, LOG_ERROR, L"BCryptCreateHash: error code 0x%x", status);

				ret = P_ERROR_CRYPT;
			}

			if (P_OK(ret) && !NT_SUCCESS(status = BCryptHashData(hHash, (PUCHAR)data, dataLen, 0)))
			{
				if (platform->logger.LogW != nullptr)
					platform->logger.LogW(platform, LOG_ERROR, L"BCryptHashData: error code 0x%x", status);

				ret = P_ERROR_CRYPT;
			}

			if (P_OK(ret) && !NT_SUCCESS(status = BCryptFinishHash(hHash, dest, cbHash, 0)))
			{
				if (platform->logger.LogW != nullptr)
					platform->logger.LogW(platform, LOG_ERROR, L"BCryptFinishHash: error code 0x%x", status);

				ret = P_ERROR_CRYPT;
			}

			*calculatedSize = cbHash;
		}
		__finally
		{
			if (P_OK(ret) && hHash != nullptr && !NT_SUCCESS(status = BCryptDestroyHash(hHash)))
			{
				if (platform->logger.LogW != nullptr)
					platform->logger.LogW(platform, LOG_ERROR, L"BCryptDestroyHash: error code 0x%x", status);

				ret = P_WARN_MEMORYNOTCLEARED;
			}

			if (P_OK(ret) && hAlg != nullptr && !NT_SUCCESS(status = BCryptCloseAlgorithmProvider(hAlg, 0)))
			{
				if (platform->logger.LogW != nullptr)
					platform->logger.LogW(platform, LOG_ERROR, L"BCryptCloseAlgorithmProvider: error code 0x%x. Tr", status);

				ret = P_WARN_MEMORYNOTCLEARED;
			}

			if (pHashObject)
				free(pHashObject);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER) //Handle all system / code exceptions
	{
		//TODO: Log and exit
		ret = P_ERROR_SYSTEM;
	}

	return ret;
}