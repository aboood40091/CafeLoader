#include <string>

#include <string.h>
#include <stdlib.h>

#include <coreinit/cache.h>
#include <coreinit/exception.h>
#include <coreinit/memorymap.h>
#include <coreinit/memdefaultheap.h>
#include <coreinit/title.h>
#include <nsysnet/socket.h>
#include <wups.h>
#include <system/memory.h>
#include <utils/logger.h>

#include "globals.h"
#include "handler.h"

WUPS_PLUGIN_NAME("CafeLoader");
WUPS_PLUGIN_DESCRIPTION("Loader for custom code.");
WUPS_PLUGIN_VERSION("v1.3");
WUPS_PLUGIN_AUTHOR("AboodXD");
WUPS_PLUGIN_LICENSE("GPL");

WUPS_FS_ACCESS()
WUPS_ALLOW_KERNEL()

uint8_t gAppStatus = 0;

int exists(const char* fname) {
    int f = open(fname, O_RDONLY);
    if (f < 0)
        return 0;

    close(f);
    return 1;
}

int existsDir(const char* dname) {
    DIR* dir = opendir(dname);
    if (!dir)
        return 0;

    closedir(dir);
    return 1;
}

uint32_t getFileLength(const char* fname) {
    struct stat fileStat;
    stat(fname, &fileStat);
    uint32_t length = fileStat.st_size;

    return length;
}

char* readBuf(const char* fname, int f) {
    char* buffer = 0;
    uint32_t length = getFileLength(fname);

    buffer = new char[length];
    if (buffer)
        read(f, buffer, length);

    return buffer;
}

void KernelCopyData(void* dest, void* source, uint32_t len) {
    ICInvalidateRange(source, len);
    DCFlushRange(source, len);

    WUPS_KernelCopyDataFunction(OSEffectiveToPhysical((uint32_t)dest), OSEffectiveToPhysical((uint32_t)source), len);

    ICInvalidateRange(dest, len);
    DCFlushRange(dest, len);
}

void Patch(char* buffer) {
    uint16_t count = *(uint16_t*)buffer;
    buffer += 2;
    for (uint16_t i = 0; i < count; i++) {
        uint16_t bytes = *(uint16_t*)buffer;
        buffer += 2;
        uint32_t addr = *(uint32_t*)buffer;
        buffer += 4;
        KernelCopyData((void*)addr, buffer, bytes);
        buffer += bytes;
    }
}

void* operator new(size_t size) {
    return MEMAllocFromDefaultHeapEx(size, 4);
}
void* operator new[](size_t size) {
    return MEMAllocFromDefaultHeapEx(size, 4);
}
void* operator new(size_t size, int alignment) {
    return MEMAllocFromDefaultHeapEx(size, alignment);
}
void* operator new[](size_t size, int alignment) {
    return MEMAllocFromDefaultHeapEx(size, alignment);
}
void operator delete(void* ptr) {
    MEMFreeToDefaultHeap(ptr);
}
void operator delete(void* ptr, size_t size) {
    MEMFreeToDefaultHeap(ptr);
}

ON_APPLICATION_START(args) {
    socket_lib_init();
    log_init();

    DEBUG_FUNCTION_LINE("Setting the ExceptionCallbacks\n");
    OSSetExceptionCallbackEx(OS_EXCEPTION_MODE_GLOBAL_ALL_CORES, OS_EXCEPTION_TYPE_DSI, DSIHandler_Fatal);
    OSSetExceptionCallbackEx(OS_EXCEPTION_MODE_GLOBAL_ALL_CORES, OS_EXCEPTION_TYPE_ISI, ISIHandler_Fatal);
    OSSetExceptionCallbackEx(OS_EXCEPTION_MODE_GLOBAL_ALL_CORES, OS_EXCEPTION_TYPE_PROGRAM, ProgramHandler_Fatal);

    if (args.sd_mounted && args.kernel_access) {

        char TitleIDString[20];
        snprintf(TitleIDString, 20, "%016llX", OSGetTitleID());

        std::string patchTitleIDPath = "sd:/cafeloader/";
        patchTitleIDPath += TitleIDString;

        log_printf("patchTitleIDPath: %s", patchTitleIDPath.c_str());

        std::string patchesPath = patchTitleIDPath + "/Patches.hax";
        std::string addrPath = patchTitleIDPath + "/Addr.bin";
        std::string codePath = patchTitleIDPath + "/Code.bin";
        std::string dataPath = patchTitleIDPath + "/Data.bin";

        uint32_t CODE_ADDR;
        uint32_t DATA_ADDR;

        uint32_t length = 0;

        if (exists(patchesPath.c_str())) {
            log_printf("Patches.hax found!");

            int patchesFile = open(patchesPath.c_str(), O_RDONLY);
            char* patchesBuffer = readBuf(patchesPath.c_str(), patchesFile);
            Patch(patchesBuffer);

            close(patchesFile);
            delete[] patchesBuffer;

            log_printf("Loaded Patches.hax!");
        }

        if (exists(addrPath.c_str()) && exists(codePath.c_str()) && exists(dataPath.c_str())) {
            log_printf("Code patches found!");

            int addrFile = open(addrPath.c_str(), O_RDONLY);
            char* addrBuffer = readBuf(addrPath.c_str(), addrFile);
            CODE_ADDR = *(uint32_t*)(addrBuffer + 0);
            DATA_ADDR = *(uint32_t*)(addrBuffer + 4);

            close(addrFile);
            delete[] addrBuffer;

            log_printf("Loaded Addr.bin!");

            int codeFile = open(codePath.c_str(), O_RDONLY);
            char* codeBuffer = readBuf(codePath.c_str(), codeFile);
            length = getFileLength(codePath.c_str());
            KernelCopyData((void*)CODE_ADDR, codeBuffer, length);

            close(codeFile);
            delete[] codeBuffer;

            log_printf("Loaded Code.bin!");

            int dataFile = open(dataPath.c_str(), O_RDONLY);
            char* dataBuffer = readBuf(dataPath.c_str(), dataFile);
            length = getFileLength(dataPath.c_str());
            KernelCopyData((void*)DATA_ADDR, dataBuffer, length);

            close(dataFile);
            delete[] dataBuffer;

            log_printf("Loaded Data.bin!");

            uint32_t debugPtr = (uint32_t)&log_printf_;
            KernelCopyData((void*)(DATA_ADDR - 4), &debugPtr, sizeof(uint32_t));

            log_printf("log_printf_ address: 0x%08X\n", debugPtr);
        }
    }
}

ON_APP_STATUS_CHANGED(status) {
    gAppStatus = status;
}

ON_APPLICATION_ENDING() {
}
