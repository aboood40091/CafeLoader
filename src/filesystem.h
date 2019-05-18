#pragma once

#include <coreinit/filesystem.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

bool openFile(FSClient *client, FSCmdBlock *block,
              const char *path, const char *mode,
              FSFileHandle *fileHandle,
              int errHandling);

bool closeFile(FSClient *client, FSCmdBlock *block,
			   FSFileHandle fileHandle,
			   int errHandling);

int readFile(FSClient *client, FSCmdBlock *block,
             char *dest, int size, int count,
             FSFileHandle fileHandle, int flag,
             int errHandling);

bool writeFile(FSClient *client, FSCmdBlock *block,
			   char *source, int size, int count,
			   FSFileHandle fileHandle, int flag,
			   int errHandling);

bool setPosFile(FSClient *client, FSCmdBlock *block,
				FSFileHandle fileHandle, uint32_t fpos,
				int errHandling);

bool getStatFile(FSClient *client, FSCmdBlock *block,
				 FSFileHandle fileHandle, FSStat *returnedStat,
				 int errHandling);

bool getStat(FSClient *client, FSCmdBlock *block,
             const char *path, FSStat *returnedStat,
             int errHandling);

#ifdef __cplusplus
}
#endif // __cplusplus
