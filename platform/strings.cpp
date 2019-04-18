#include "platform.h"
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING 0x01

#include <string>
#include <locale>
#include <codecvt>
#include <iostream>
#include <stdlib.h>

extern "C" pError_t Utf8ToUtf16(struct _platform_t* platform, char16_t* dest, const size_t destSize, const char* str, const size_t strlen)
{
	if ((strlen >= destSize) || (destSize <= (strlen * sizeof(char16_t)) + sizeof(char16_t)))
		return P_ERROR_STRING_DESTTOOSMALL;
	
	std::wstring_convert<std::codecvt<int16_t, char, std::mbstate_t>, int16_t> convert;
	std::u16string u16 = reinterpret_cast<const char16_t*>(convert.from_bytes(str).c_str());	

	memcpy(dest, u16.c_str(), u16.size() * sizeof(char16_t));
	dest[u16.size() + 1] = 0x00;
		
	return P_SUCCESS;
}

pError_t Utf8toUtf32(struct _platform_t* platform, char32_t* dest, const size_t destSize, const char* str, const size_t strlen)
{
	if ((strlen >= destSize) || (destSize <= (strlen * sizeof(char32_t)) + sizeof(char32_t)))
		return P_ERROR_STRING_DESTTOOSMALL;

	std::wstring_convert<std::codecvt<int32_t, char, std::mbstate_t>, int32_t> convert;
	std::u32string u32 = reinterpret_cast<const char32_t*>(convert.from_bytes(str).c_str());

	memcpy(dest, u32.c_str(), u32.size() * sizeof(char32_t));
	dest[u32.size() + 1] = 0x00;

	return P_SUCCESS;
}

pError_t Utf16toUtf8(struct _platform_t* platform, char* dest, const size_t destSize, const char16_t* str, const size_t strlen)
{
	return P_NO_IMPL;
}

pError_t Utf16toUtf32(struct _platform_t* platform, char32_t* dest, const size_t destSize, const char16_t* str, const size_t strlen)
{
	return P_NO_IMPL;
}

pError_t Utf32toUtf16(struct _platform_t* platform, char16_t* dest, const size_t destSize, const char32_t* str, const size_t strlen)
{
	return P_NO_IMPL;
}

pError_t Utf32toUtf8(struct _platform_t* platform, char* dest, const size_t destSize, const char32_t* str, const size_t strlen)
{
	return P_NO_IMPL;
}

extern "C" pError_t Utf8ToWChar(struct _platform_t* platform, wchar_t* dest, const size_t destSize, const pUtf8_t str, const size_t strlen)
{
	if ((strlen >= destSize) || (destSize < (strlen * sizeof(wchar_t)) + sizeof(wchar_t)))
		return P_ERROR_STRING_DESTTOOSMALL;

	std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>, wchar_t> convert;
	std::wstring ustr = convert.from_bytes(str).c_str();

	memcpy(dest, ustr.c_str(), ustr.size() * sizeof(wchar_t));
	dest[ustr.size() + 1] = 0;

	return P_SUCCESS;
}

pError_t WCharToUtf8(struct _platform_t* platform, pUtf8_t dest, const size_t destSize, const wchar_t* str, const size_t strlen)
{
	return P_NO_IMPL;
}

pError_t Utf16ToWChar(struct _platform_t* platform, wchar_t* dest, const size_t destSize, const char16_t str, const size_t strlen)
{

	return P_NO_IMPL;
}

pError_t WCharToUtf16(struct _platform_t* platform, char16_t* dest, const size_t destSize, const wchar_t* str, const size_t strlen)
{
	return P_NO_IMPL;
}

pError_t Utf32ToWChar(struct _platform_t* platform, wchar_t* dest, const size_t destSize, const char32_t* str, const size_t strlen)
{
	return P_NO_IMPL;
}

pError_t WCharToUtf32(struct _platform_t* platform, char32_t* dest, const size_t destSize, const wchar_t* str, const size_t strlen)
{
	return P_NO_IMPL;
}

size_t StrlenU8(const pUtf8_t str)
{
	if (str == NULL)
		return 0;

	return P_NO_IMPL; //NOT IMPLEMENTED
}

size_t StrlenU16(struct _platform_t* platform, const char16_t* str)
{
	std::u16string strU16(str);
	return strU16.length();
}

size_t StrlenU32(struct _platform_t* platform, const char32_t* str)
{
	std::u32string strU32(str);
	return strU32.length();
}

pError_t CharToUtf16(struct _platform_t* platform, char16_t* dest, const size_t destSize, const char* str, const size_t strlen)
{
	if (destSize <= (strlen * sizeof(char16_t)) + sizeof(char16_t))
		return P_ERROR_STRING_DESTTOOSMALL;
	
	memset(dest, 0, destSize);

	std::u16string wstr = u"";
	char16_t c16str[3] = u"\0";
	mbstate_t mbs;
	std::string strC(str);

	for (const auto& it : strC) {
		memset(&mbs, 0, sizeof(mbs));//set shift state to the initial state
		memmove(c16str, u"\0\0\0", 3);
		auto ret = mbrtoc16(c16str, &it, 3, &mbs);
		if (ret != (size_t) - 1)
		{
			wstr.append(std::u16string(c16str));
		}
	} 

	//return wstr;

	return P_ERROR_STRING;
}