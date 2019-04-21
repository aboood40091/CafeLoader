#include <coreinit/context.h>

#ifdef __cplusplus
extern "C" {
#endif

bool handle_crash(uint32_t type, void *handler, OSContext *context);

BOOL DSIHandler_Fatal(OSContext *context);
BOOL ISIHandler_Fatal(OSContext *context);
BOOL ProgramHandler_Fatal(OSContext *context);

void FatalCrashHandler();

#ifdef __cplusplus
}
#endif
