#include <iomanip>
#include <sstream>
#include <string>
#include <cctype>

#include "platform.h"

using namespace std;

extern "C" pError_t UrlEncode(struct _platform_t* platform, wchar_t* dest, const size_t destSize, const wchar_t* url, const size_t urlSize)
{
	wstringstream escapedStr;
	escapedStr.fill(0);

	escapedStr << hex;

	wstring urlStr(url);

	if (destSize < (urlSize * 3)) //Always check for sufficient buffers
		return P_ERROR_DESTTOOSMALL;

	for (wstring::const_iterator itStart = urlStr.begin(), itEnd = urlStr.end(); itStart != itEnd; ++itStart)
	{
		wstring::value_type c = *itStart;

		if (iswalnum((wint_t)c) || c == L'-' || c == L'_' || c == '.' || c == '~')
		{
			escapedStr << c;
			continue;
		}

		escapedStr << L'%' << setw(2) << (wint_t)c;
	}

	wstring str = escapedStr.str();

	wcscpy(dest, str.c_str());
}