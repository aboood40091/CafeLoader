#include <string>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <coreinit/title.h>
#include <nsysnet/socket.h>
#include <utils/logger.h>
#include <wups.h>

#define FS_MAX_LOCALPATH_SIZE           511
#define FS_MAX_MOUNTPATH_SIZE           128
#define FS_MAX_FULLPATH_SIZE            (FS_MAX_LOCALPATH_SIZE + FS_MAX_MOUNTPATH_SIZE)

WUPS_PLUGIN_NAME("CafeLoader");
WUPS_PLUGIN_DESCRIPTION("Loader for custom code.");
WUPS_PLUGIN_VERSION("v1.0");
WUPS_PLUGIN_AUTHOR("AboodXD");
WUPS_PLUGIN_LICENSE("GPL");

WUPS_FS_ACCESS()

void *CODE_ADDR = (void *)0x0F800000; // change this
void *DATA_ADDR = (void *)0x10400000; // change this

int exists(const char *fname) {
    FILE *file;
    if ((file = fopen(fname, "rb"))) {
        fclose(file);
        return 1;
    }

    return 0;
}

uint32_t getFileLength(FILE *file) {
    fseek(file, 0, SEEK_END);
    uint32_t length = ftell(file);
    fseek(file, 0, SEEK_SET);

    return length;
}

char* open(FILE *file) {
    char* buffer = 0;
    uint32_t length = getFileLength(file);

    buffer = (char *)malloc(length * sizeof(char));
    if (buffer)
        fread(buffer, sizeof(char), length, file);

    return buffer;
}

void Patch(char *buffer) {
    uint16_t count = *(uint16_t *)buffer; buffer += 2;
    for (uint16_t i = 0; i < count; i++) {
        uint16_t bytes = *(uint16_t *)buffer; buffer += 2;
        uint32_t addr = *(uint32_t *)buffer; buffer += 4;
        for (uint16_t j = 0; j < bytes; j++) {
            *((uint8_t *)addr) = *(uint8_t *)buffer;
            buffer += 1;
            addr += 1;
        }
    }
}

ON_APPLICATION_START(args){
    socket_lib_init();
    log_init();

    DEBUG_FUNCTION_LINE("ON_APPLICATION_START reached!\n");

    if (args.sd_mounted) {
        char TitleIDString[FS_MAX_FULLPATH_SIZE];
        snprintf(TitleIDString,FS_MAX_FULLPATH_SIZE,"%016llX",OSGetTitleID());

        std::string patchTitleIDPath = "sd:/cafeloader/";
        patchTitleIDPath += TitleIDString;

        DEBUG_FUNCTION_LINE("patchTitleIDPath: %s\n", patchTitleIDPath.c_str());

        std::string patchesPath = patchTitleIDPath + "/Patches.hax";
        std::string codePath    = patchTitleIDPath + "/Code.bin";
        std::string dataPath    = patchTitleIDPath + "/Data.bin";
        std::string ctorsPath   = patchTitleIDPath + "/Ctors.bin";

        uint32_t length = 0;

        if (exists(patchesPath.c_str()) && exists(codePath.c_str()) && exists(dataPath.c_str()) && exists(ctorsPath.c_str())) {
            DEBUG_FUNCTION_LINE("Patches found!\n");

            FILE *patchesFile = fopen(patchesPath.c_str(), "rb");
            char *patchesBuffer = open(patchesFile);
            Patch(patchesBuffer);

            DEBUG_FUNCTION_LINE("Loaded Patches.hax!\n");

            FILE *codeFile = fopen(codePath.c_str(), "rb");
            char *codeBuffer = open(codeFile);
            length = getFileLength(codeFile);
            memcpy((void *)(0xA0000000 + (int)CODE_ADDR), codeBuffer, length);

            DEBUG_FUNCTION_LINE("Loaded Code.bin!\n");

            FILE *dataFile = fopen(dataPath.c_str(), "rb");
            char *dataBuffer = open(dataFile);
            length = getFileLength(dataFile);
            memcpy(DATA_ADDR, dataBuffer, length);

            DEBUG_FUNCTION_LINE("Loaded Data.bin!\n");

            FILE *ctorsFile = fopen(ctorsPath.c_str(), "rb");
            char *ctorsBuffer = open(ctorsFile);
            length = getFileLength(ctorsFile);
            memcpy((void *)((int)DATA_ADDR + 0x20000), ctorsBuffer, length);

            DEBUG_FUNCTION_LINE("Loaded Ctors.bin!\n");

            fclose(patchesFile);
            fclose(codeFile);
            fclose(dataFile);
            fclose(ctorsFile);

            free(patchesBuffer);
            free(codeBuffer);
            free(dataBuffer);
            free(ctorsBuffer);
        }
    }
}
