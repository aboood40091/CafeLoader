#include <string>

#include <string.h>
#include <stdlib.h>

#include <coreinit/cache.h>
#include <coreinit/exception.h>
#include <coreinit/memorymap.h>
#include <coreinit/title.h>
#include <nsysnet/socket.h>
#include <utils/logger.h>
#include <wups.h>

#include "globals.h"
#include "handler.h"

WUPS_PLUGIN_NAME("CafeLoader");
WUPS_PLUGIN_DESCRIPTION("Loader for custom code.");
WUPS_PLUGIN_VERSION("v1.0");
WUPS_PLUGIN_AUTHOR("AboodXD");
WUPS_PLUGIN_LICENSE("GPL");

WUPS_FS_ACCESS()
WUPS_ALLOW_KERNEL()

uint8_t gAppStatus = 0;

int exists(const char *fname) {
    overrideLoading = true;
    int f = open(fname, O_RDONLY);
    overrideLoading = false;

    if (f < 0)
        return 0;

    close(f);
    return 1;
}

uint32_t getFileLength(const char *fname) {
    struct stat fileStat;
    stat(fname, &fileStat);
    uint32_t length = fileStat.st_size;

    return length;
}

char* readBuf(const char *fname, int f) {
    char* buffer = 0;
    uint32_t length = getFileLength(fname);

    buffer = (char *)malloc(length);
    if (buffer)
        read(f, buffer, length);

    return buffer;
}

void KernelCopyData(void *dest, void *source, uint32_t len) {
    ICInvalidateRange(source, len);
    DCFlushRange(source, len);

    WUPS_KernelCopyDataFunction(OSEffectiveToPhysical((uint32_t)dest), OSEffectiveToPhysical((uint32_t)source), len);

    ICInvalidateRange(dest, len);
    DCFlushRange(dest, len);
}

void Patch(char *buffer) {
    uint16_t count = *(uint16_t *)buffer; buffer += 2;
    for (uint16_t i = 0; i < count; i++) {
        uint16_t bytes = *(uint16_t *)buffer; buffer += 2;
        uint32_t addr = *(uint32_t *)buffer; buffer += 4;
        KernelCopyData((void *)addr, buffer, bytes); buffer += bytes;
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

        std::string addrPath    = patchTitleIDPath + "/Addr.bin";
        std::string patchesPath = patchTitleIDPath + "/Patches.hax";
        std::string codePath    = patchTitleIDPath + "/Code.bin";
        std::string dataPath    = patchTitleIDPath + "/Data.bin";
        std::string ctorsPath   = patchTitleIDPath + "/Ctors.bin";

        uint32_t CODE_ADDR;
        uint32_t DATA_ADDR;

        uint32_t length = 0;

        if (exists(addrPath.c_str()) && exists(patchesPath.c_str()) && exists(codePath.c_str()) && exists(dataPath.c_str()) && exists(ctorsPath.c_str())) {
            DEBUG_FUNCTION_LINE("Patches found!\n");

            int   addrFile   = open(addrPath.c_str(), O_RDONLY);
            char *addrBuffer = readBuf(addrPath.c_str(), addrFile);
            CODE_ADDR = *(uint32_t *)(addrBuffer + 0);
            DATA_ADDR = *(uint32_t *)(addrBuffer + 4);

            close(addrFile);
            free(addrBuffer);

            DEBUG_FUNCTION_LINE("Loaded Addr.bin!\n");

            int   patchesFile   = open(patchesPath.c_str(), O_RDONLY);
            char *patchesBuffer = readBuf(patchesPath.c_str(), patchesFile);
            Patch(patchesBuffer);

            close(patchesFile);
            free(patchesBuffer);

            DEBUG_FUNCTION_LINE("Loaded Patches.hax!\n");

            int   codeFile   = open(codePath.c_str(), O_RDONLY);
            char *codeBuffer = readBuf(codePath.c_str(), codeFile);
            length = getFileLength(codePath.c_str());
            KernelCopyData((void *)CODE_ADDR, codeBuffer, length);

            close(codeFile);
            free(codeBuffer);

            DEBUG_FUNCTION_LINE("Loaded Code.bin!\n");

            int   dataFile   = open(dataPath.c_str(), O_RDONLY);
            char *dataBuffer = readBuf(dataPath.c_str(), dataFile);
            length = getFileLength(dataPath.c_str());
            KernelCopyData((void *)DATA_ADDR, dataBuffer, length);

            close(dataFile);
            free(dataBuffer);

            DEBUG_FUNCTION_LINE("Loaded Data.bin!\n");

            int   ctorsFile   = open(ctorsPath.c_str(), O_RDONLY);
            char *ctorsBuffer = readBuf(ctorsPath.c_str(), ctorsFile);
            length = getFileLength(ctorsPath.c_str());
            KernelCopyData((void *)(DATA_ADDR + 0x20004), ctorsBuffer, length);

            close(ctorsFile);
            free(ctorsBuffer);

            DEBUG_FUNCTION_LINE("Loaded Ctors.bin!\n");

            uint32_t debugPtr = (uint32_t)&log_printf_;
            KernelCopyData((void *)(DATA_ADDR + 0x20000), &debugPtr, 4);

            DEBUG_FUNCTION_LINE("log_printf_ addres: 0x%08X\n", debugPtr);
        }
    }
}

ON_APP_STATUS_CHANGED(status) {
	gAppStatus = status;
}
