//
//	GET / SignIn ? openid.claimed_id = http % 3A % 2F % 2Fspecs.openid.net % 2Fauth % 2F2.0 % 2Fidentifier_select & 
//				   openid.identity = http % 3A % 2F % 2Fspecs.openid.net % 2Fauth % 2F2.0 % 2Fidentifier_select & 
//				   openid.assoc_handle = % 7B636910386006193171 % 7D % 7B % 2BnoBmw % 3D % 3D % 7D % 7B32 % 7D & 
//				   openid.return_to = http % 3A % 2F % 2Faccounts - samples.autodesk.com % 2FUser % 2FAuthenticate % 3FreturnUrl % 3D % 252FContent % 252Fhtml % 252FcloseWindow.htm % 26displayMode % 3D1 % 26requestMode % 3D0 % 26loginType % 3D1 % 26Provider % 3D1 % 26IFrameRefreshUrl % 3D % 252FContent % 252Fhtml % 252Foxygen_refresh_response.html % 26LanguagePreference % 3D0 % 26LinkedProvider % 3D0 % 26SuppressMktOptChk % 3Dfalse % 26LDAPAuthenticationContext % 3D0 % 26AuthType % 3D0 % 26AuthViewMode % 3D0 % 26ParentEmailMode % 3D0 % 26ParentShouldReceiveEmail % 3Dtrue % 26EducationLicenseType % 3Dfalse % 26EducationRole % 3Dfalse % 26dnoa.userSuppliedIdentifier % 3Dhttps % 253A % 252F % 252Faccounts - dev.autodesk.com & openid.realm = http % 3A % 2F % 2Faccounts - samples.autodesk.com % 2F & 
//				openid.mode = checkid_setup & openid.ns = http % 3A % 2F % 2Fspecs.openid.net % 2Fauth % 2F2.0 & openid.ns.alias3 = http % 3A % 2F % 2Fspecs.openid.net % 2Fextensions % 2Foauth % 2F1.0 & openid.alias3.consumer = oxygen.autodesk.com & openid.alias3.scope = accounts - samples.autodesk.com & openid.ns.alias4 = http % 3A % 2F % 2Fopenid.net % 2Fsrv % 2Fax % 2F1.0 & openid.alias4.mode = fetch_request & openid.ns.alias5 = http % 3A % 2F % 2Fspecs.openid.net % 2Fextensions % 2Fui % 2F1.0 & openid.alias5.mode = desktop HTTP / 1.1

#include "platform.h"
#include "UrlBuilder.h"
#include <string>
#include <map>

using namespace std;

UrlBuilder::UrlBuilder(u16string environment)
{
	m_urlBase = u"https://www.accounts-" + environment + u".autodesk.com/SignIn?"; //TODO: Do Not Hardcode
}

UrlBuilder::~UrlBuilder()
{

}

void UrlBuilder::AddParam(const u16string key, const u16string value, bool escape)
{
	(escape);
	//TODO: escape
	m_params.insert(pair<u16string, u16string>(key, value));
}
	
bool UrlBuilder::Build(u16string& url)
{
	map<u16string, u16string>::iterator it;
	size_t len = m_params.size();
	size_t pos = 0;

	for (it = m_params.begin(); it != m_params.end(); it++)
	{
		url.append(it->first);
		url.append(u"=");
		url.append(it->second);

		pos++;

		if (pos == len)
			break;

		url.append(u"&");
	}

	return true;
}