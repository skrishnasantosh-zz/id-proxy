#pragma once

#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

	extern struct _platform_t;

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
		ptError_t (*CheckForUpdates)(struct _platform_t* platform, struct AutoUpdateMetaInf* meta, enum peSupportedPlatforms os, ptVersion_t osVersion, ptVersion_t productVersion);
		ptError_t (*CheckForLatestUpdates)(struct _platform_t* platform, struct AutoUpdateMetaInf* meta, enum peSupportedPlatforms os, ptVersion_t osVersion);
		ptError_t (*InstallUpdate)(struct _platform_t* platform, struct AutoUpdateFileInf* file, pfnProgressNotifier_t progressNotifier);
		ptError_t (*RollbackUpdates)(struct _platform_t* platform, struct AutoUpdateMetaInf* file, pfnProgressNotifier_t progressNotifier);
	};

#ifdef __cplusplus
}
#endif
