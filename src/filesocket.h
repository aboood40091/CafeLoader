#pragma once

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void receiveFile(char *out, uint32_t length);
void sendFile(char *src, uint32_t length);

#ifdef __cplusplus
}
#endif // __cplusplus
