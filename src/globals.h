#pragma once

#include <coreinit/filesystem.h>
#include <stdbool.h>
#include <vector>

#define FS_MAX_LOCALPATH_SIZE 511
#define FS_MAX_MOUNTPATH_SIZE 128
#define FS_MAX_FULLPATH_SIZE (FS_MAX_LOCALPATH_SIZE + FS_MAX_MOUNTPATH_SIZE)

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern uint8_t gAppStatus;

int exists(const char* fname);
int existsDir(const char* dname);
uint32_t getFileLength(const char* fname);
char* readBuf(const char* fname, int f);

#ifdef __cplusplus
}
#endif // __cplusplus
