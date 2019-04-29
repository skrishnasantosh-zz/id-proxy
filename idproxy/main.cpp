
#include <Windows.h>
#include <string>
#include <codecvt>

#include <time.h>
#include <math.h>
#include <map>
#include <cstdlib>
#include <vector>
#include <cassert>
#include <list>
#include <ctype.h>

#include "../platform/platform.h"

#define MAX_NONCE_TS_CHARS 32

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

using namespace std;

wstring UrlEncode(const wstring& url);

const wchar_t* GetNonce(wchar_t nonce[MAX_NONCE_TS_CHARS], const wchar_t* givenNonce = nullptr)
{	
	static int rndinit = 1;
	const wchar_t* chars = L"abcdefghijklmnopqrstuvwxyz"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ" "0123456789";

	memset(nonce, 0, sizeof(wchar_t) * MAX_NONCE_TS_CHARS);

	if (givenNonce != nullptr) //Test 
	{
		size_t len = MIN(wcslen(givenNonce), MAX_NONCE_TS_CHARS);
		wcsncpy(nonce, givenNonce, len);

		return nonce;
	}

	unsigned int max = wcslen(chars);
	int i, len;

	if (rndinit) {
		srand(time(NULL)); 
		rndinit = 0;
	} 

	len = 15 + floor(rand() * 16.0 / (double)RAND_MAX);

	for (i = 0; i < len; i++) {
		nonce[i] = chars[rand() % max];
	}
	nonce[i] = '\0';
	return (nonce);
}

const wchar_t* GetTimeStamp(wchar_t ts[MAX_NONCE_TS_CHARS], const wchar_t *givenTimeStamp = nullptr)
{
	time_t t;
	
	memset(ts, 0, sizeof(wchar_t) * MAX_NONCE_TS_CHARS);

	if (givenTimeStamp != nullptr)
	{
		size_t len = MIN(wcslen(givenTimeStamp), MAX_NONCE_TS_CHARS);
		wcsncpy(ts, givenTimeStamp, len);

		return ts;
	}

	t = time(NULL);

	wsprintf(ts, L"%lu", (unsigned long long)t);

	return ts;
}

string WideStrToMb(wstring wstr)
{
	if (wstr.empty())
		return string();

	size_t size = 0;
	_locale_t lc = _create_locale(LC_ALL, "en_US.UTF-8");
	errno_t err = _wcstombs_s_l(&size, NULL, 0, &wstr[0], _TRUNCATE, lc);
	std::string ret = std::string(size, 0);
	err = _wcstombs_s_l(&size, &ret[0], size, &wstr[0], _TRUNCATE, lc);
	_free_locale(lc);
	ret.resize(size - 1);

	return ret;
}

wstring MbToWideStr(string str)
{
	size_t len = str.length() + 1;
	std::wstring ret = std::wstring(len, 0);
	size_t size = 0;
	_locale_t lc = _create_locale(LC_ALL, "en_US.UTF-8");
	errno_t retval = _mbstowcs_s_l(&size, &ret[0], len, &str[0], _TRUNCATE, lc);
	_free_locale(lc);
	ret.resize(size - 1);

	return ret;
}

bool OAuthParamsToString(const multimap<string, string>& rawParamMap, string& rawParams, const string& paramsSeperator) 
{
	rawParams.assign("");
	if (rawParamMap.size())
	{
		list<string> keyValueList;
		std::string dummyStr;

		/* Push key-value pairs to a list of strings */
		keyValueList.clear();
		multimap<string, string>::const_iterator itMap = rawParamMap.begin();
		for (; itMap != rawParamMap.end(); itMap++)
		{
			dummyStr.assign(itMap->first);
			dummyStr.append("=");
			if (paramsSeperator == ",")
			{
				dummyStr.append("\"");
			}
			dummyStr.append(itMap->second);
			if (paramsSeperator == ",")
			{
				dummyStr.append("\"");
			}
			keyValueList.push_back(dummyStr);
		}

		/* Sort key-value pairs based on key name */
		keyValueList.sort();

		/* Now, form a string */
		dummyStr.assign("");
		list<string>::iterator itKeyValue = keyValueList.begin();
		for (; itKeyValue != keyValueList.end(); itKeyValue++)
		{
			if (dummyStr.length())
			{
				dummyStr.append(paramsSeperator);
			}
			dummyStr.append(itKeyValue->c_str());
		}
		rawParams.assign(dummyStr);
	}
	return (rawParams.length()) ? true : false;
}

const pError_t SignOAuth1(Platform_t* platform, wstring& oAuthSignature, const wstring& httpMethod, const wstring& url, const wstring& consumerKey, const wstring& consumerSecret, const wstring& token, const wstring& tokenSecret, const wstring& testNonce, const wstring& testTs)
{
	string rawParams;
	string paramsSeperator("&");
	string signature;
	multimap<string, string> params;
	
	if (consumerKey.length() == 0 || consumerSecret.length() == 0 || httpMethod.length() == 0)
		return P_ERROR_INVALID_ARG;

	/* Initially empty signature */
	oAuthSignature.assign(L"");

	string conKey = WideStrToMb(consumerKey);
	string conSecret = WideStrToMb(consumerSecret);
	
	printf("\n Consumer Key : %s", conKey.c_str());
	printf("\n Consumer Secret : %s\n", conSecret.c_str());

	params.insert(multimap<string, string>::value_type("oauth_consumer_key", conKey));
	params.insert(multimap<string, string>::value_type("oauth_nonce", WideStrToMb(testNonce)));
	params.insert(multimap<string, string>::value_type("oauth_signature_method", "HMAC-SHA1"));
	params.insert(multimap<string, string>::value_type("oauth_timestamp", WideStrToMb(testTs)));
	params.insert(multimap<string, string>::value_type("oauth_token", ""));
	params.insert(multimap<string, string>::value_type("oauth_version", "1.0"));
	
	OAuthParamsToString(params, rawParams, paramsSeperator);

	printf("\n Params : %s\n", rawParams.c_str());

	wchar_t encoded[10 * 1024] = { 0 };


	wstring rawParamsWide = MbToWideStr(rawParams);

	platform->httpHelpers.UrlEncode(platform, encoded, 10 * 1024, rawParamsWide.c_str(), rawParamsWide.length());

	wstring encodedParams(encoded);

	rawParams = WideStrToMb(encodedParams);

	signature.assign(WideStrToMb(httpMethod));
	signature.append(paramsSeperator);

	memset(encoded, 0, 10 * 1024);
	platform->httpHelpers.UrlEncode(platform, encoded, 10 * 1024, url.c_str(), url.length());

	wstring encodedUrl(encoded);

	signature.append(WideStrToMb(encodedUrl));
	signature.append(paramsSeperator);

	signature.append(rawParams);

	string secret;
	string key = WideStrToMb(consumerSecret);

	secret.assign(key);
	secret.append(paramsSeperator);

	if (tokenSecret.length() > 0)
	{
		memset(encoded, 0, 10 * 1024);
		platform->httpHelpers.UrlEncode(platform, encoded, 10 * 1024, tokenSecret.c_str(), tokenSecret.length());
		wstring encodedTokenSecret(encoded);

		secret.assign(WideStrToMb(encodedTokenSecret));
	}

	pByte_t hmac[1024] = { 0 };
	pByte_t base64[1024] = { 0 };
		
	size_t hmacSize = 0;

	platform->crypto.HmacSha1(platform, hmac, 1024, &hmacSize, (const pByte_t*)signature.c_str(), signature.length(), (const pByte_t*)secret.c_str(), secret.length());	
	platform->httpHelpers.Base64Encode(platform, base64, 1024, hmac, hmacSize);

	memset(hmac, 0, 1024);

	wstring hmacBase64(MbToWideStr(string((const char*)base64)));

	memset(encoded, 0, 10 * 1024);

	platform->httpHelpers.UrlEncode(platform, encoded, 10 * 1024, hmacBase64.c_str(), hmacBase64.length());

	return P_SUCCESS;

	/* Build a string using key-value pairs */
	
	//paramsSeperator = "&";

	//

	//getStringFromOAuthKeyValuePairs(rawKeyValuePairs, rawParams, paramsSeperator);
	//LOG(LogLevelDebug, "Normalized parameters: " << rawParams);

	///* Start constructing base signature string. Refer http://dev.twitter.com/auth#intro */
	//switch (eType)
	//{
	//case Http::Head:
	//{
	//	sigBase.assign("HEAD&");
	//}
	//break;

	//case Http::Get:
	//{
	//	sigBase.assign("GET&");
	//}
	//break;

	//case Http::Post:
	//{
	//	sigBase.assign("POST&");
	//}
	//break;

	//case Http::Delete:
	//{
	//	sigBase.assign("DELETE&");
	//}
	//break;

	//case Http::Put:
	//{
	//	sigBase.assign("PUT&");
	//}
	//break;

	//default:
	//{
	//	return false;
	//}
	//break;
	//}
	//sigBase.append(PercentEncode(rawUrl));
	//sigBase.append("&");
	//sigBase.append(PercentEncode(rawParams));
	//LOG(LogLevelDebug, "Signature base string: " << sigBase);

	///* Now, hash the signature base string using HMAC_SHA1 class */
	//CHMAC_SHA1 objHMACSHA1;
	//std::string secretSigningKey;
	//unsigned char strDigest[Defaults::BUFFSIZE_LARGE];

	//memset(strDigest, 0, Defaults::BUFFSIZE_LARGE);

	///* Signing key is composed of consumer_secret&token_secret */
	//secretSigningKey.assign(PercentEncode(mConsumer->secret()));
	//secretSigningKey.append("&");
	//if (mToken && mToken->secret().length())
	//{
	//	secretSigningKey.append(PercentEncode(mToken->secret()));
	//}

	//objHMACSHA1.HMAC_SHA1((unsigned char*)sigBase.c_str(),
	//	sigBase.length(),
	//	(unsigned char*)secretSigningKey.c_str(),
	//	secretSigningKey.length(),
	//	strDigest);

	///* Do a base64 encode of signature */
	//std::string base64Str = base64_encode(strDigest, 20 /* SHA 1 digest is 160 bits */);
	//LOG(LogLevelDebug, "Signature: " << base64Str);

	///* Do an url encode */
	//oAuthSignature = PercentEncode(base64Str);
	//LOG(LogLevelDebug, "Percent-encoded Signature: " << oAuthSignature);

	//return (oAuthSignature.length()) ? true : false;
}

int main()
{
	const pByte_t *secret = (const pByte_t*)"TestSecret";
	const pByte_t* str = (const pByte_t*)"Some string";

	pByte_t dest[256] = { 0 };
	pByte_t base64Dest[256] = { 0 };
	size_t hashSize = 0;
	wstring signature;
		
	Platform_t platform;

	LoadPlatform(&platform, GetModuleHandle(NULL));

	auto error = platform.crypto.HmacSha1(&platform, dest, 256, &hashSize, str, strlen((const char*)str), secret, strlen(((const char*)secret)));
	if (P_OK(error))
	{
		error = platform.httpHelpers.Base64Encode(&platform, base64Dest, 256, dest, hashSize);
		if (P_OK(error))
		{
			printf("\nBase64 Encoded Hash : %s\n", base64Dest);
		}
	}

	wchar_t nonce[32] = { 0 };

	wprintf(L"\n%s\n", GetNonce(nonce));
	wprintf(L"\n%s\n", GetTimeStamp(nonce));

	wprintf(L"\n%d\n", platform.httpHelpers.Base64Encode(&platform, dest, 256, (pByte_t*)"Santosh", 7));
	
	SignOAuth1(&platform, signature, L"GET", L"https://accounts-dev.autodesk.com/", L"mycloud-dev.autodesk.com", L"DevAutode$k", L"", L"", L"OFzCNkyeNyq", L"1556417022");

	platform.Unload(&platform);
}