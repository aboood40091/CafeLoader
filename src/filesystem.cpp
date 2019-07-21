#include <string>

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <coreinit/debug.h>
#include <coreinit/title.h>

#include "globals.h"
#include "filesystem.h"

FSFileHandle file;

bool getStat(FSClient *client, FSCmdBlock *block,
             const char *path, FSStat *returnedStat,
             int errHandling) {

    std::string sdPath(path);
    char TitleIDString[FS_MAX_FULLPATH_SIZE];
    snprintf(TitleIDString,FS_MAX_FULLPATH_SIZE,"%016llX",OSGetTitleID());

    std::string titleID(TitleIDString);

    if (sdPath.substr(0, 5) != "/vol/") {
        sdPath = "/vol/content/" + sdPath;
    }

    if (sdPath.substr(0, 13) == "/vol/content/") {
        sdPath = "/vol/" + titleID + sdPath.substr(4);
    }

    std::string fpath = "sd:/cafeloader";
    fpath += sdPath;

	returnedStat->flags = (FSStatFlags)0;
	return !exists(fpath.c_str());
}

void checkFileHandle() {
	if (file != 0)
		OSFatal("Blarg");
}

bool getStatFile(FSClient *client, FSCmdBlock *block,
				 FSFileHandle fileHandle, FSStat *returnedStat,
				 int errHandling) {

	if (file == fileHandle) {
		struct stat path_stat;
        if (fstat(fileHandle, &path_stat) < 0)
            return 1;

        returnedStat->size = path_stat.st_size;
		return 0;
	}

	return 1;
}

bool setPosFile(FSClient *client, FSCmdBlock *block,
				FSFileHandle fileHandle, uint32_t fpos,
				int errHandling) {

	if (fileHandle != file)
		return 1;

	int newOffset = lseek(fileHandle, (int)fpos, SEEK_SET);

    if (newOffset == (int)fpos)
        return 0;

    else
        return 1;
}

/* TODO: while the code is finished, I will re-enable this later one
bool openSave(FSClient *client, FSCmdBlock *block,
			  uint8_t accountSlotNo, const char *path,
			  const char *mode,
			  FSFileHandle *fileHandle,
			  int errHandling) {

	std::string sdPath(path);
    char TitleIDString[FS_MAX_FULLPATH_SIZE];
    snprintf(TitleIDString,FS_MAX_FULLPATH_SIZE,"%016llX",OSGetTitleID());

    std::string titleID(TitleIDString);

    if (sdPath.substr(0, 5) != "/vol/") {
        sdPath = "/vol/content/" + sdPath;
    }

    if (sdPath.substr(0, 13) == "/vol/content/") {
        sdPath = "/vol/" + titleID + sdPath.substr(4);
    }

    std::string fpath = "sd:/cafeloader";
    fpath += sdPath;

	if (!exists(fpath.c_str()))
		return 1;

    checkFileHandle();

	FSFileHandle handle = open(fpath.c_str(), O_RDONLY);
	file = handle;
	*fileHandle = handle;

	return 1;
}
*/

bool openFile(FSClient *client, FSCmdBlock *block,
              const char *path, const char *mode,
              FSFileHandle *fileHandle,
              int errHandling) {

    std::string sdPath(path);
    char TitleIDString[FS_MAX_FULLPATH_SIZE];
    snprintf(TitleIDString,FS_MAX_FULLPATH_SIZE,"%016llX",OSGetTitleID());

    std::string titleID(TitleIDString);

    if (sdPath.substr(0, 5) != "/vol/") {
        sdPath = "/vol/content/" + sdPath;
    }

    if (sdPath.substr(0, 13) == "/vol/content/") {
        sdPath = "/vol/" + titleID + sdPath.substr(4);
    }

    std::string fpath = "sd:/cafeloader";
    fpath += sdPath;

	if (!exists(fpath.c_str()))
		return 1;

    checkFileHandle();

	FSFileHandle handle = open(fpath.c_str(), O_RDONLY);
	file = handle;
	*fileHandle = handle;

	return 0;
}

#define MAXIMUM_READ_CHUNK 1024*1024

int readIntoBuffer(FSFileHandle handle, char *buffer, int size, int count) {
    int sizeToRead = size * count;
    char *newBuffer = buffer;
    int curResult = -1;
    int totalSize = 0;
    int toRead = 0;

    while (sizeToRead > 0) {
        if (sizeToRead < MAXIMUM_READ_CHUNK)
            toRead = sizeToRead;

        else
            toRead = MAXIMUM_READ_CHUNK;

        curResult = read(handle, newBuffer, toRead);
        if (curResult < 0)
            return -1;

        if (curResult == 0)
            //EOF
            break;

        newBuffer = (char*)(((uint32_t)newBuffer) + curResult);
        totalSize += curResult;
        sizeToRead -= curResult;
    }

    return totalSize;
}

int readFile(FSClient *client, FSCmdBlock *block,
             char *dest, int size, int count,
             FSFileHandle fileHandle, int flag,
             int errHandling) {

	if (fileHandle != file)
		return -1;

	return readIntoBuffer(fileHandle, dest, size, count);
}

bool writeFile(FSClient *client, FSCmdBlock *block,
			   char *source, int size, int count,
			   FSFileHandle fileHandle, int flag,
			   int errHandling) {

	if (fileHandle != file)
		return 1;

	uint32_t length = size * count;
	write(fileHandle, source, length);

	return 0;
}

bool closeFile(FSClient *client, FSCmdBlock *block,
			   FSFileHandle fileHandle,
			   int errHandling) {

	if (fileHandle != file)
		return 1;

    close(fileHandle);
	file = 0;
	return 0;
}
