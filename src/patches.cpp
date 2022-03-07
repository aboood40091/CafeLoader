#include <coreinit/filesystem.h>
#include <wups.h>

#include <vector>
#include <string>
#include <coreinit/title.h>
#include <string.h>

#include "globals.h"

uint64_t latestTID = 0;
bool alreadyMounted = false;
FSMountSource mountSource;
char mountPath[128];

void mountSDCard(FSClient* client, FSCmdBlock* block) {
    if (!alreadyMounted || (OSGetTitleID() != latestTID)) {
        FSStatus ret1 = FSGetMountSource(client, block, FS_MOUNT_SOURCE_SD, &mountSource, -1);
        FSStatus ret2 = FSMount(client, block, &mountSource, mountPath, 128, -1);
        alreadyMounted = true;
        latestTID = OSGetTitleID();
    }
}

constexpr inline uint32_t fromWiiUModeToStd(const char* mode) {
    uint32_t flags = O_RDONLY;
    if (strcmp(mode, "r") == 0) {
        flags = O_RDONLY;
    } else if (strcmp(mode, "r+") == 0) {
        flags = O_RDWR;
    } else if (strcmp(mode, "w") == 0) {
        flags = O_TRUNC | O_CREAT | O_WRONLY;
    } else if (strcmp(mode, "w+") == 0) {
        flags = O_TRUNC | O_CREAT | O_RDWR;
    } else if (strcmp(mode, "a") == 0) {
        flags = O_CREAT | O_APPEND | O_WRONLY;
    } else if (strcmp(mode, "a+") == 0) {
        flags = O_CREAT | O_APPEND | O_RDWR;
    }
    return flags;
}

DECL_FUNCTION(FSStatus, FSOpenFile, FSClient* client, FSCmdBlock* block, const char* path, const char* mode,
              FSFileHandle* fileHandle, int errHandling) {

    if (gAppStatus == WUPS_APP_STATUS_BACKGROUND) {
        return real_FSOpenFile(client, block, path, mode, fileHandle, errHandling);
    }

    mountSDCard(client, block);

    std::string realpath = path;
    if (!realpath.starts_with("/vol/content")) {
        return real_FSOpenFile(client, block, path, mode, fileHandle, errHandling);
    }

    char titleId[20];
    snprintf(titleId, sizeof(titleId), "%016llX", OSGetTitleID());
    std::string sdPath = std::string(mountPath) + "/cafeloader/vol/" + std::string(titleId) + realpath.substr(4);

    int fd = -1;
    FSStatus ret = real_FSOpenFile(client, block, sdPath.c_str(), mode, (FSFileHandle*)&fd, -1);
    if (fd < 0) {
        return real_FSOpenFile(client, block, path, mode, fileHandle, errHandling);
    }

    *fileHandle = fd;
    return FS_STATUS_OK;
}

typedef struct _fs_dev_dir_entry_t {
    void* dev;
    int dirHandle;
} fs_dev_dir_entry_t;

DECL_FUNCTION(FSStatus, FSOpenDir, FSClient* client, FSCmdBlock* block, const char* path, FSDirectoryHandle* handle,
              uint32_t errHandling) {

    if (gAppStatus == WUPS_APP_STATUS_BACKGROUND) {
        return real_FSOpenDir(client, block, path, handle, errHandling);
    }

    mountSDCard(client, block);

    std::string realpath = path;
    if (!realpath.starts_with("/vol/content")) {
        return real_FSOpenDir(client, block, path, handle, errHandling);
    }

    char titleId[20];
    snprintf(titleId, sizeof(titleId), "%016llX", OSGetTitleID());
    std::string sdPath = std::string(mountPath) + "/cafeloader/vol/" + std::string(titleId) + realpath.substr(4);

    int fd = -1;
    real_FSOpenDir(client, block, sdPath.c_str(), (FSDirectoryHandle*)&fd, errHandling);
    if (fd < 0) {
        return real_FSOpenDir(client, block, path, handle, errHandling);
    }

    *handle = fd;
    return FS_STATUS_OK;
}

DECL_FUNCTION(FSStatus, FSGetStat, FSClient* client, FSCmdBlock* block, const char* path, FSStat* returnedStat,
              int errHandling) {

    if (gAppStatus == WUPS_APP_STATUS_BACKGROUND) {
        return real_FSGetStat(client, block, path, returnedStat, errHandling);
    }

    mountSDCard(client, block);

    std::string realpath = path;
    if (!realpath.starts_with("/vol/content")) {
        return real_FSGetStat(client, block, path, returnedStat, errHandling);
    }

    char titleId[20];
    snprintf(titleId, sizeof(titleId), "%016llX", OSGetTitleID());
    std::string sdPath = std::string(mountPath) + "/cafeloader/vol/" + std::string(titleId) + realpath.substr(4);

    int fd = -1;
    real_FSOpenFile(client, block, sdPath.c_str(), "r", (FSFileHandle*)&fd, -1);
    if (fd < 0) {
        return real_FSGetStat(client, block, path, returnedStat, errHandling);
    }

    FSGetStatFile(client, block, fd, returnedStat, errHandling);
    FSCloseFile(client, block, fd, errHandling);
    return FS_STATUS_OK;
}

WUPS_MUST_REPLACE(FSOpenFile, WUPS_LOADER_LIBRARY_COREINIT, FSOpenFile);
WUPS_MUST_REPLACE(FSOpenDir, WUPS_LOADER_LIBRARY_COREINIT, FSOpenDir);
WUPS_MUST_REPLACE(FSGetStat, WUPS_LOADER_LIBRARY_COREINIT, FSGetStat);
