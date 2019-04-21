#include <string>

#include <string.h>
#include <stdlib.h>

#include <coreinit/exception.h>
#include <coreinit/memorymap.h>
#include <coreinit/title.h>
#include <nsysnet/socket.h>
#include <utils/logger.h>
#include <wups.h>

#include "handler.hpp"

#define FS_MAX_LOCALPATH_SIZE           511
#define FS_MAX_MOUNTPATH_SIZE           128
#define FS_MAX_FULLPATH_SIZE            (FS_MAX_LOCALPATH_SIZE + FS_MAX_MOUNTPATH_SIZE)

WUPS_PLUGIN_NAME("CafeLoader");
WUPS_PLUGIN_DESCRIPTION("Loader for custom code.");
WUPS_PLUGIN_VERSION("v1.0");
WUPS_PLUGIN_AUTHOR("AboodXD");
WUPS_PLUGIN_LICENSE("GPL");

WUPS_FS_ACCESS()
WUPS_ALLOW_KERNEL()

uint32_t CODE_ADDR = 0x0F800000; // change this
uint32_t DATA_ADDR = 0x10400000; // change this

int exists(const char *fname) {
    int file = open(fname, O_RDONLY);

    if (file < 0)
        return 0;

    close(file);
    return 1;
}

uint32_t getFileLength(const char *fname) {
    struct stat fileStat;
    stat(fname, &fileStat);
    uint32_t length = fileStat.st_size;

    return length;
}

char* readBuf(const char *fname, int file) {
    char* buffer = 0;
    uint32_t length = getFileLength(fname);

    buffer = (char *)malloc(length);
    if (buffer)
        read(file, buffer, length);

    return buffer;
}

void Patch(char *buffer) {
    uint16_t count = *(uint16_t *)buffer; buffer += 2;
    for (uint16_t i = 0; i < count; i++) {
        uint16_t bytes = *(uint16_t *)buffer; buffer += 2;
        uint32_t addr = *(uint32_t *)buffer; buffer += 4;

        if (addr < 0x10000000)
            WUPS_KernelCopyDataFunction(OSEffectiveToPhysical(addr), OSEffectiveToPhysical((uint32_t)buffer), bytes);
        else
            memcpy((void *)addr, buffer, bytes);

        buffer += bytes;
    }
}

ON_APPLICATION_START(args){
    socket_lib_init();
    log_init();

    DEBUG_FUNCTION_LINE("Setting the ExceptionCallbacks\n");
    OSSetExceptionCallbackEx(OS_EXCEPTION_MODE_GLOBAL_ALL_CORES, OS_EXCEPTION_TYPE_DSI, DSIHandler_Fatal);
    OSSetExceptionCallbackEx(OS_EXCEPTION_MODE_GLOBAL_ALL_CORES, OS_EXCEPTION_TYPE_ISI, ISIHandler_Fatal);
    OSSetExceptionCallbackEx(OS_EXCEPTION_MODE_GLOBAL_ALL_CORES, OS_EXCEPTION_TYPE_PROGRAM, ProgramHandler_Fatal);

    if (args.sd_mounted && args.kernel_access) {
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

            int   patchesFile   = open(patchesPath.c_str(), O_RDONLY);
            char *patchesBuffer = readBuf(patchesPath.c_str(), patchesFile);
            Patch(patchesBuffer);

            close(patchesFile);
            free(patchesBuffer);

            DEBUG_FUNCTION_LINE("Loaded Patches.hax!\n");

            int   codeFile   = open(codePath.c_str(), O_RDONLY);
            char *codeBuffer = readBuf(codePath.c_str(), codeFile);
            length = getFileLength(codePath.c_str());
            WUPS_KernelCopyDataFunction(OSEffectiveToPhysical(CODE_ADDR), OSEffectiveToPhysical((uint32_t)codeBuffer), length);

            close(codeFile);
            free(codeBuffer);

            DEBUG_FUNCTION_LINE("Loaded Code.bin!\n");

            int   dataFile   = open(dataPath.c_str(), O_RDONLY);
            char *dataBuffer = readBuf(dataPath.c_str(), dataFile);
            length = getFileLength(dataPath.c_str());
            memcpy((void *)DATA_ADDR, dataBuffer, length);

            close(dataFile);
            free(dataBuffer);

            DEBUG_FUNCTION_LINE("Loaded Data.bin!\n");

            int   ctorsFile   = open(ctorsPath.c_str(), O_RDONLY);
            char *ctorsBuffer = readBuf(ctorsPath.c_str(), ctorsFile);
            length = getFileLength(ctorsPath.c_str());
            memcpy((void *)(DATA_ADDR + 0x20000), ctorsBuffer, length);

            close(ctorsFile);
            free(ctorsBuffer);

            DEBUG_FUNCTION_LINE("Loaded Ctors.bin!\n");
        }
    }
}
