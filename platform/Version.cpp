#include "platform.h"

#ifdef _MSC_VER
#pragma warning(disable:4996) //remove CRT warning for sscanf
#endif

#include <cstdio>
#include <string>
#include <iostream>


namespace Autodesk::IdProxy::Platform
{

	Version::Version(std::string version)
	{
		//TODO: Validate format of version

		auto ret = std::sscanf(version.c_str(), "%d.%d.%d.%d", &m_major, &m_minor, &m_revision, &m_build);
		if (ret == EOF)
		{
			//version not converted
			//TODO: Do something
		}
	}

	void Version::ToString(std::string& version)
	{
		version = "";
	}
};
