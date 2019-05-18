#include <stdio.h>
#include <string.h>

#include <coreinit/debug.h>
#include <coreinit/exception.h>
#include <utils/logger.h>

#include "handler.h"

// From DiiBuggerWUPS:
// https://github.com/Maschell/DiiBuggerWUPS

OSContext crashContext;
uint32_t crashType;

bool handle_crash(uint32_t type, void *handler, OSContext *context) {
    memcpy((char *)&crashContext, (const char *)context, sizeof(OSContext));
    crashType = type;
    context->srr0 = (uint32_t)handler;
    return true;
}

void FatalCrashHandler() {
    char buffer[0x400];
    snprintf(buffer, 0x400,
             "An exception of type %i occurred:\n\n"
             "r0: %08X r1: %08X r2: %08X r3: %08X r4: %08X\n"
             "r5: %08X r6: %08X r7: %08X r8: %08X r9: %08X\n"
             "r10:%08X r11:%08X r12:%08X r13:%08X r14:%08X\n"
             "r15:%08X r16:%08X r17:%08X r18:%08X r19:%08X\n"
             "r20:%08X r21:%08X r22:%08X r23:%08X r24:%08X\n"
             "r25:%08X r26:%08X r27:%08X r28:%08X r29:%08X\n"
             "r30:%08X r31:%08X\n\n"
             "CR: %08X LR: %08X CTR:%08X XER:%08X\n"
             "SRR0:%08X SRR1:%08X EX0:%08X EX1:%08X\n",
             crashType,
             crashContext.gpr[0],
             crashContext.gpr[1],
             crashContext.gpr[2],
             crashContext.gpr[3],
             crashContext.gpr[4],
             crashContext.gpr[5],
             crashContext.gpr[6],
             crashContext.gpr[7],
             crashContext.gpr[8],
             crashContext.gpr[9],
             crashContext.gpr[10],
             crashContext.gpr[11],
             crashContext.gpr[12],
             crashContext.gpr[13],
             crashContext.gpr[14],
             crashContext.gpr[15],
             crashContext.gpr[16],
             crashContext.gpr[17],
             crashContext.gpr[18],
             crashContext.gpr[19],
             crashContext.gpr[20],
             crashContext.gpr[21],
             crashContext.gpr[22],
             crashContext.gpr[23],
             crashContext.gpr[24],
             crashContext.gpr[25],
             crashContext.gpr[26],
             crashContext.gpr[27],
             crashContext.gpr[28],
             crashContext.gpr[29],
             crashContext.gpr[30],
             crashContext.gpr[31],
             crashContext.cr,
             crashContext.lr,
             crashContext.ctr,
             crashContext.xer,
             crashContext.srr0,
             crashContext.srr1,
             crashContext.dsisr,
             crashContext.dar
            );

    DEBUG_FUNCTION_LINE("%s", buffer);
    OSFatal(buffer);
}

BOOL DSIHandler_Fatal(OSContext *context) {
    return handle_crash(OS_EXCEPTION_TYPE_DSI, (void*)FatalCrashHandler,context);
}
BOOL ISIHandler_Fatal(OSContext *context) {
    return handle_crash(OS_EXCEPTION_TYPE_ISI, (void*)FatalCrashHandler,context);
}
BOOL ProgramHandler_Fatal(OSContext *context) {
    return handle_crash(OS_EXCEPTION_TYPE_PROGRAM, (void*)FatalCrashHandler,context);
}
