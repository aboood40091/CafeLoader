#include <string.h>

#include <coreinit/debug.h>
#include <coreinit/filesystem.h>
#include <nsysnet/socket.h>

#include "globals.h"
#include "filesocket.h"
#include "filesystem.h"

bool isServerFile(const char *path) {
	uint32_t pathLength = strlen(path);
	uint16_t reply = 0;

	send(fd, "\x0C", 1, 0);
	send(fd, &pathLength, 4, 0);
	send(fd, path, pathLength, 0);
	recv(fd, &reply, 2, 0);

	return reply == 0xCAFE;
}

bool getStat(FSClient *client, FSCmdBlock *block,
             const char *path, FSStat *returnedStat,
             int errHandling) {

	returnedStat->flags = (FSStatFlags)0;
	return !isServerFile(path);
}

void checkFileHandle() {
	if (file != 0) {
		send(fd, "\xFF", 1, 0);
		OSFatal("Blarg");
	}
}

bool getStatFile(FSClient *client, FSCmdBlock *block,
				 FSFileHandle fileHandle, FSStat *returnedStat,
				 int errHandling) {

	if (file == fileHandle) {
		send(fd, "\x08", 1, 0);
		send(fd, &fileHandle, 4, 0);

		recv(fd, &returnedStat->size, 4, 0);
		return 0;
	}

	return 1;
}

bool setPosFile(FSClient *client, FSCmdBlock *block,
				FSFileHandle fileHandle, uint32_t fpos,
				int errHandling) {

	if (fileHandle != file)
		return 1;

	send(fd, "\x09", 1, 0);
	send(fd, &fileHandle, 4, 0);
	send(fd, &fpos, 4, 0);
	return 0;
}

/* TODO: while the code is finished, I will re-enable this later one
bool openSave(FSClient *client, FSCmdBlock *block,
			  uint8_t accountSlotNo, const char *path,
			  const char *mode,
			  FSFileHandle *fileHandle,
			  int errHandling) {

	checkFileHandle();

	send(fd, "\x06", 1, 0);

	uint32_t length = strlen(path);
	send(fd, &length, 4, 0);
	send(fd, path, length, 0);
	send(fd, mode, 1, 0);

	uint32_t handle;
	recv(fd, &handle, 4, 0);

	if (handle != 0) {
		file = handle;
		*fileHandle = handle;
		return 0;
	}

	return 1;
}
*/

bool openFile(FSClient *client, FSCmdBlock *block,
              const char *path, const char *mode,
              FSFileHandle *fileHandle,
              int errHandling) {

	if (!isServerFile(path))
		return 1;

	checkFileHandle();

	send(fd, "\x02", 1, 0);

	uint32_t length = strlen(path);
	send(fd, &length, 4, 0);
	send(fd, path, length, 0);

	uint32_t handle;
	recv(fd, &handle, 4, 0);

	file = handle;
	*fileHandle = handle;
	return 0;
}

int readFile(FSClient *client, FSCmdBlock *block,
             char *dest, int size, int count,
             FSFileHandle fileHandle, int flag,
             int errHandling) {

	if (fileHandle != file)
		return -1;

	send(fd, "\x03", 1, 0);
	send(fd, &fileHandle, 4, 0);
	send(fd, &size, 4, 0);
	send(fd, &count, 4, 0);

	uint32_t filesize;
	uint32_t elementsRead;
	recv(fd, &elementsRead, 4, 0);
	recv(fd, &filesize, 4, 0);
	receiveFile(dest, filesize);

	return elementsRead;
}

bool writeFile(FSClient *client, FSCmdBlock *block,
			   char *source, int size, int count,
			   FSFileHandle fileHandle, int flag,
			   int errHandling) {

	if (fileHandle != file)
		return 1;

	uint32_t length = size * count;
	send(fd, "\x04", 1, 0);
	send(fd, &fileHandle, 4, 0);
	send(fd, &length, 4, 0);

	sendFile(source, length);

	return 0;
}

bool closeFile(FSClient *client, FSCmdBlock *block,
			   FSFileHandle fileHandle,
			   int errHandling) {

	if (fileHandle != file)
		return 1;

	send(fd, "\x05", 1, 0);
	send(fd, &fileHandle, 4, 0);
	file = 0;
	return 0;
}
