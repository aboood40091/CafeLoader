#include <coreinit/filesystem.h>
#include <wups.h>

#include "globals.h"
#include "filesystem.h"

DECL_FUNCTION(bool, FSOpenFile, FSClient *client, FSCmdBlock *block,
              const char *path, const char *mode,
              FSFileHandle *fileHandle,
              int errHandling) {

    if (clientEnabled == false)
        return real_FSOpenFile(client, block, path, mode, fileHandle, errHandling);

    bool result = 1;
    if((result = openFile(client, block, path, mode, fileHandle, errHandling)) != 1)
        return result;

    return real_FSOpenFile(client, block, path, mode, fileHandle, errHandling);
}

DECL_FUNCTION(bool, FSCloseFile, FSClient *client, FSCmdBlock *block,
			   FSFileHandle fileHandle,
			   int errHandling) {

    if (clientEnabled == false)
        return real_FSCloseFile(client, block, fileHandle, errHandling);

    bool result = 1;
    if((result = closeFile(client, block, fileHandle, errHandling)) != 1)
        return result;

    return real_FSCloseFile(client, block, fileHandle, errHandling);
}

DECL_FUNCTION(int, FSReadFile, FSClient *client, FSCmdBlock *block,
             char *dest, int size, int count,
             FSFileHandle fileHandle, int flag,
             int errHandling) {

    if (clientEnabled == false)
        return real_FSReadFile(client, block, dest, size, count, fileHandle, flag, errHandling);

    int result = -1;
    if((result = readFile(client, block, dest, size, count, fileHandle, flag, errHandling)) != -1)
        return result;

    return real_FSReadFile(client, block, dest, size, count, fileHandle, flag, errHandling);
}

DECL_FUNCTION(bool, FSWriteFile, FSClient *client, FSCmdBlock *block,
			   char *source, int size, int count,
			   FSFileHandle fileHandle, int flag,
			   int errHandling) {

    if (clientEnabled == false)
        return real_FSWriteFile(client, block, source, size, count, fileHandle, flag, errHandling);

    bool result = 1;
    if((result = writeFile(client, block, source, size, count, fileHandle, flag, errHandling)) != 1)
        return result;

    return real_FSWriteFile(client, block, source, size, count, fileHandle, flag, errHandling);
}

DECL_FUNCTION(bool, FSSetPosFile, FSClient *client, FSCmdBlock *block,
				FSFileHandle fileHandle, uint32_t fpos,
				int errHandling) {

    if (clientEnabled == false)
        return real_FSSetPosFile(client, block, fileHandle, fpos, errHandling);

    bool result = 1;
    if((result = setPosFile(client, block, fileHandle, fpos, errHandling)) != 1)
        return result;

    return real_FSSetPosFile(client, block, fileHandle, fpos, errHandling);
}

DECL_FUNCTION(bool, FSGetStatFile, FSClient *client, FSCmdBlock *block,
				 FSFileHandle fileHandle, FSStat *returnedStat,
				 int errHandling) {

    if (clientEnabled == false)
        return real_FSGetStatFile(client, block, fileHandle, returnedStat, errHandling);

    bool result = 1;
    if((result = getStatFile(client, block, fileHandle, returnedStat, errHandling)) != 1)
        return result;

    return real_FSGetStatFile(client, block, fileHandle, returnedStat, errHandling);
}

DECL_FUNCTION(bool, FSGetStat, FSClient *client, FSCmdBlock *block,
             const char *path, FSStat *returnedStat,
             int errHandling) {

    if (clientEnabled == false)
        return real_FSGetStat(client, block, path, returnedStat, errHandling);

    bool result = 1;
    if((result = getStat(client, block, path, returnedStat, errHandling)) != 1)
        return result;

    return real_FSGetStat(client, block, path, returnedStat, errHandling);
}

WUPS_MUST_REPLACE(FSOpenFile,                  WUPS_LOADER_LIBRARY_COREINIT,  FSOpenFile);
WUPS_MUST_REPLACE(FSCloseFile,                 WUPS_LOADER_LIBRARY_COREINIT,  FSCloseFile);
WUPS_MUST_REPLACE(FSReadFile,                  WUPS_LOADER_LIBRARY_COREINIT,  FSReadFile);
WUPS_MUST_REPLACE(FSWriteFile,                 WUPS_LOADER_LIBRARY_COREINIT,  FSWriteFile);
WUPS_MUST_REPLACE(FSSetPosFile,                WUPS_LOADER_LIBRARY_COREINIT,  FSSetPosFile);
WUPS_MUST_REPLACE(FSGetStatFile,               WUPS_LOADER_LIBRARY_COREINIT,  FSGetStatFile);
WUPS_MUST_REPLACE(FSGetStat,                   WUPS_LOADER_LIBRARY_COREINIT,  FSGetStat);
