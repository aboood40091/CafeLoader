#include <stdlib.h>

#include <coreinit/internal.h>
#include <nsysnet/socket.h>
//#include <utils/logger.h>

#include "filesocket.h"
#include "globals.h"

void receiveFile(char *dest, uint32_t filesize) {
    /*
	send(fd, "\x07", 1, 0);
	send(fd, "\x00\x00\x00\x0A", 4, 0);
	char buffer2[10];
	__os_snprintf(buffer2, 10, "%i", filesize);
	send(fd, buffer2, 10, 0);
    */

	uint32_t bytes = 0;
	while (bytes < filesize) {
		uint32_t blarg;
		if ((filesize - bytes) < 0x400) {
			blarg = filesize - bytes;
		} else {
			blarg = 0x400;
		}
        /*
		send(fd, "\x07", 1, 0);
		send(fd, "\x00\x00\x00\x0A", 4, 0);
		char buffer3[10];
		__os_snprintf(buffer3, 10, "%i", blarg);
		send(fd, buffer3, 10, 0);
		*/

		uint32_t num = recv(fd, dest, blarg, MSG_DONTWAIT);

        /*
		send(fd, "\x07", 1, 0);
		send(fd, "\x00\x00\x00\x0A", 4, 0);
		char buffer4[10];
		__os_snprintf(buffer4, 10, "%i", num);
		send(fd, buffer4, 10, 0);
		*/

		if (num == 0xFFFFFFFF) {
			//log_printf("TODO: insert SOLastError() output here");
		} else {
			bytes += num;
			dest += num;
		}
	}
}

void sendFile(char *source, uint32_t filesize) {
	uint32_t bytes = 0;
	while (bytes < filesize) {
		uint32_t num = send(fd, source, filesize - bytes, 0);
		bytes += num;
		source += num;
	}
}
