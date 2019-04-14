#pragma once

#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

	struct _platform_t;

	struct AutoUpdateFileInf
	{
		pLen_t fileLen;
		char fileName[P_FILE_MAX_STRLEN];
		char targetPath[P_PATH_MAX_STRLEN];
		char fileHashSha256[P_GUID_MAX_STRLEN];
		char fileKey[P_GUID_MAX_STRLEN];
		
		struct AutoUpdateFileInf* pNext;
	};

	struct AutoUpdateMetaInf
	{
		struct AutoUpdateFileInf* pFirst;
		pLen_t fileCount;
	};

	struct AutoUpdate
	{
		pError_t (*CheckForUpdates)(struct _platform_t* platform, struct AutoUpdateMetaInf* meta, enum peSupportedPlatforms os, ptVersion_t osVersion, ptVersion_t productVersion);
		pError_t (*CheckForLatestUpdates)(struct _platform_t* platform, struct AutoUpdateMetaInf* meta, enum peSupportedPlatforms os, ptVersion_t osVersion);
		pError_t (*InstallUpdate)(struct _platform_t* platform, struct AutoUpdateFileInf* file, pfnProgressNotifier_t progressNotifier);
		pError_t (*RollbackUpdates)(struct _platform_t* platform, struct AutoUpdateMetaInf* file, pfnProgressNotifier_t progressNotifier);
	};

#ifdef __cplusplus
}
#endif
