#include <coreinit/filesystem.h>
#include <wups.h>

#include "globals.h"
#include "filesystem.h"

bool openFileCalledByCafeLoader = false;
bool closeFileCalledByCafeLoader = false;
bool readFileCalledByCafeLoader = false;
bool writeFileCalledByCafeLoader = false;
bool setPosFileCalledByCafeLoader = false;
bool getStatFileCalledByCafeLoader = false;
bool getStatCalledByCafeLoader = false;
bool openSaveCalledByCafeLoader = false;

bool overrideLoading = false;

DECL_FUNCTION(bool, FSOpenFile, FSClient *client, FSCmdBlock *block,
              const char *path, const char *mode,
              FSFileHandle *fileHandle,
              int errHandling) {

    if (gAppStatus == WUPS_APP_STATUS_BACKGROUND) {
        return real_FSOpenFile(client, block, path, mode, fileHandle, errHandling);
    }

    if (openFileCalledByCafeLoader == true) {
        if (overrideLoading == false) openFileCalledByCafeLoader = false;
        return real_FSOpenFile(client, block, path, mode, fileHandle, errHandling);
    }

    openFileCalledByCafeLoader = true;
    bool result = 1;
    if((result = openFile(client, block, path, mode, fileHandle, errHandling)) != 1) {
        openFileCalledByCafeLoader = false;
        return result;
    }

    openFileCalledByCafeLoader = false;
    return real_FSOpenFile(client, block, path, mode, fileHandle, errHandling);
}

DECL_FUNCTION(bool, FSCloseFile, FSClient *client, FSCmdBlock *block,
              FSFileHandle fileHandle,
              int errHandling) {

    if (gAppStatus == WUPS_APP_STATUS_BACKGROUND) {
        return real_FSCloseFile(client, block, fileHandle, errHandling);
    }

    if (closeFileCalledByCafeLoader == true) {
        if (overrideLoading == false) closeFileCalledByCafeLoader = false;
        return real_FSCloseFile(client, block, fileHandle, errHandling);
    }

    closeFileCalledByCafeLoader = true;
    bool result = 1;
    if((result = closeFile(client, block, fileHandle, errHandling)) != 1) {
        closeFileCalledByCafeLoader = false;
        return result;
    }

    closeFileCalledByCafeLoader = false;
    return real_FSCloseFile(client, block, fileHandle, errHandling);
}

DECL_FUNCTION(int, FSReadFile, FSClient *client, FSCmdBlock *block,
              char *dest, int size, int count,
              FSFileHandle fileHandle, int flag,
              int errHandling) {

    if (gAppStatus == WUPS_APP_STATUS_BACKGROUND) {
        return real_FSReadFile(client, block, dest, size, count, fileHandle, flag, errHandling);
    }

    if (readFileCalledByCafeLoader == true) {
        if (overrideLoading == false) readFileCalledByCafeLoader = false;
        return real_FSReadFile(client, block, dest, size, count, fileHandle, flag, errHandling);
    }

    readFileCalledByCafeLoader = true;
    int result = -1;
    if((result = readFile(client, block, dest, size, count, fileHandle, flag, errHandling)) != -1) {
        readFileCalledByCafeLoader = false;
        return result;
    }

    readFileCalledByCafeLoader = false;
    return real_FSReadFile(client, block, dest, size, count, fileHandle, flag, errHandling);
}

DECL_FUNCTION(bool, FSWriteFile, FSClient *client, FSCmdBlock *block,
              char *source, int size, int count,
              FSFileHandle fileHandle, int flag,
              int errHandling) {

    if (gAppStatus == WUPS_APP_STATUS_BACKGROUND) {
        return real_FSWriteFile(client, block, source, size, count, fileHandle, flag, errHandling);
    }

    if (writeFileCalledByCafeLoader == true) {
        if (overrideLoading == false) writeFileCalledByCafeLoader = false;
        return real_FSWriteFile(client, block, source, size, count, fileHandle, flag, errHandling);
    }

    writeFileCalledByCafeLoader = true;
    bool result = 1;
    if((result = writeFile(client, block, source, size, count, fileHandle, flag, errHandling)) != 1) {
        writeFileCalledByCafeLoader = false;
        return result;
    }

    writeFileCalledByCafeLoader = false;
    return real_FSWriteFile(client, block, source, size, count, fileHandle, flag, errHandling);
}

DECL_FUNCTION(bool, FSSetPosFile, FSClient *client, FSCmdBlock *block,
              FSFileHandle fileHandle, uint32_t fpos,
              int errHandling) {

    if (gAppStatus == WUPS_APP_STATUS_BACKGROUND) {
        return real_FSSetPosFile(client, block, fileHandle, fpos, errHandling);
    }

    if (setPosFileCalledByCafeLoader == true) {
        if (overrideLoading == false) setPosFileCalledByCafeLoader = false;
        return real_FSSetPosFile(client, block, fileHandle, fpos, errHandling);
    }

    setPosFileCalledByCafeLoader = true;
    bool result = 1;
    if((result = setPosFile(client, block, fileHandle, fpos, errHandling)) != 1) {
        setPosFileCalledByCafeLoader = false;
        return result;
    }

    setPosFileCalledByCafeLoader = false;
    return real_FSSetPosFile(client, block, fileHandle, fpos, errHandling);
}

DECL_FUNCTION(bool, FSGetStatFile, FSClient *client, FSCmdBlock *block,
              FSFileHandle fileHandle, FSStat *returnedStat,
              int errHandling) {

    if (gAppStatus == WUPS_APP_STATUS_BACKGROUND) {
        return real_FSGetStatFile(client, block, fileHandle, returnedStat, errHandling);
    }

    if (getStatFileCalledByCafeLoader == true) {
        if (overrideLoading == false) getStatFileCalledByCafeLoader = false;
        return real_FSGetStatFile(client, block, fileHandle, returnedStat, errHandling);
    }

    getStatFileCalledByCafeLoader = true;
    bool result = 1;
    if((result = getStatFile(client, block, fileHandle, returnedStat, errHandling)) != 1) {
        getStatFileCalledByCafeLoader = false;
        return result;
    }

    getStatFileCalledByCafeLoader = false;
    return real_FSGetStatFile(client, block, fileHandle, returnedStat, errHandling);
}

DECL_FUNCTION(bool, FSGetStat, FSClient *client, FSCmdBlock *block,
              const char *path, FSStat *returnedStat,
              int errHandling) {

    if (gAppStatus == WUPS_APP_STATUS_BACKGROUND) {
        return real_FSGetStat(client, block, path, returnedStat, errHandling);
    }

    if (getStatCalledByCafeLoader == true) {
        if (overrideLoading == false) getStatCalledByCafeLoader = false;
        return real_FSGetStat(client, block, path, returnedStat, errHandling);
    }

    getStatCalledByCafeLoader = true;
    bool result = 1;
    if((result = getStat(client, block, path, returnedStat, errHandling)) != 1) {
        getStatCalledByCafeLoader = false;
        return result;
    }

    getStatCalledByCafeLoader = false;
    return real_FSGetStat(client, block, path, returnedStat, errHandling);
}

DECL_FUNCTION(bool, SAVEOpenFile, FSClient *client, FSCmdBlock *block,
              uint8_t accountSlotNo, const char *path,
              const char *mode,
              FSFileHandle *fileHandle,
              int errHandling) {

    if (gAppStatus == WUPS_APP_STATUS_BACKGROUND) {
        return real_SAVEOpenFile(client, block, accountSlotNo, path, mode, fileHandle, errHandling);
    }

    if (openSaveCalledByCafeLoader == true) {
        if (overrideLoading == false) openSaveCalledByCafeLoader = false;
        return real_SAVEOpenFile(client, block, accountSlotNo, path, mode, fileHandle, errHandling);
    }

    openSaveCalledByCafeLoader = true;
    bool result = 1;
    if((result = openSave(client, block, accountSlotNo, path, mode, fileHandle, errHandling)) != 1) {
        openSaveCalledByCafeLoader = false;
        return result;
    }

    openSaveCalledByCafeLoader = false;
    return real_SAVEOpenFile(client, block, accountSlotNo, path, mode, fileHandle, errHandling);
}

WUPS_MUST_REPLACE(FSOpenFile,                  WUPS_LOADER_LIBRARY_COREINIT,  FSOpenFile);
WUPS_MUST_REPLACE(FSCloseFile,                 WUPS_LOADER_LIBRARY_COREINIT,  FSCloseFile);
WUPS_MUST_REPLACE(FSReadFile,                  WUPS_LOADER_LIBRARY_COREINIT,  FSReadFile);
WUPS_MUST_REPLACE(FSWriteFile,                 WUPS_LOADER_LIBRARY_COREINIT,  FSWriteFile);
WUPS_MUST_REPLACE(FSSetPosFile,                WUPS_LOADER_LIBRARY_COREINIT,  FSSetPosFile);
WUPS_MUST_REPLACE(FSGetStatFile,               WUPS_LOADER_LIBRARY_COREINIT,  FSGetStatFile);
WUPS_MUST_REPLACE(FSGetStat,                   WUPS_LOADER_LIBRARY_COREINIT,  FSGetStat);
WUPS_MUST_REPLACE(SAVEOpenFile,                WUPS_LOADER_LIBRARY_NN_SAVE,   SAVEOpenFile);
