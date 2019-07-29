#pragma once

#include <stdbool.h>
#include <coreinit/filesystem.h>

#define FS_MAX_LOCALPATH_SIZE           511
#define FS_MAX_MOUNTPATH_SIZE           128
#define FS_MAX_FULLPATH_SIZE            (FS_MAX_LOCALPATH_SIZE + FS_MAX_MOUNTPATH_SIZE)

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern uint8_t gAppStatus;

extern bool openFileCalledByCafeLoader;
extern bool closeFileCalledByCafeLoader;
extern bool readFileCalledByCafeLoader;
extern bool writeFileCalledByCafeLoader;
extern bool setPosFileCalledByCafeLoader;
extern bool getStatFileCalledByCafeLoader;
extern bool getStatCalledByCafeLoader;
extern bool openSaveCalledByCafeLoader;
extern bool overrideLoading;

int exists(const char *fname);
uint32_t getFileLength(const char *fname);
char* readBuf(const char *fname, int f);

#ifdef __cplusplus
}
#endif // __cplusplus
