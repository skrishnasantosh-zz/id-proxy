#include <Windows.h>
#include <string>
#include <codecvt>

#include "../platform/platform.h"

int main()
{
	const pByte_t *secret = (const pByte_t*)"TestSecret";
	const pByte_t* str = (const pByte_t*)"Some string";

	pByte_t dest[256] = { 0 };
	size_t hashSize = 0;
		
	Platform_t platform;

	LoadPlatform(&platform, GetModuleHandle(NULL));

	platform.crypto.HmacSha1(&platform, dest, 256, &hashSize, str, strlen((const char*)str), secret, strlen(((const char*)secret)));


	platform.Unload(&platform);
}