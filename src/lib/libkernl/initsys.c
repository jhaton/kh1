#include "common.h"
#include "eekernel.h"

#define KSEG0 0x80000000
#define SYSCALL(id) __asm__ volatile("li $3,"#id"\n syscall")

typedef struct {
    s32 syscallNumber;
    void* handler;
} _sysEntry;
static s32* kFindAddress(s32 targetValue);
static void setup(s32 syscallNumber, void (*handler)());

_sysEntry SysEntry = {.syscallNumber = 0x83, .handler = &kFindAddress};
s32 _pad = 0;
s32 __sce_sema_id = 0;
s32 __sce_eh_sema_id = 0;
s32* _SyscallEntry = 0;

static void supplement_crt0(void) {
    struct SemaParam libc_sema;
    struct SemaParam eh_sema;
    
    libc_sema.maxCount = 0x1;
    libc_sema.initCount = 0x1;
    eh_sema.maxCount = 0x1;
    eh_sema.initCount = 0x1;
    __sce_sema_id = CreateSema(&libc_sema);
    __sce_eh_sema_id = CreateSema(&eh_sema);
    return;
}

static s32* kFindAddress(s32 targetValue) {
    s32* address;
    
    address = (s32*)KSEG0;
    while (*address != targetValue) {
        address++;
    }
    return address;
}

static u32 FindAddress(void* targetValue) {
    SYSCALL(0x83);
}

static s32 GetSystemCallTableEntry(void) {
    u32 address;
    
    setup(SysEntry.syscallNumber, SysEntry.handler);
    address = FindAddress(&kFindAddress);
    return address - 0x20C;
}

static void setup(s32 syscallNumber, void (*handler)()) {
    SYSCALL(0x74);
}

static s32* _setup(s32 syscallNumber) {
    SYSCALL(0x74);
    return _SyscallEntry + syscallNumber;
}

void _InitSys(void) {
    supplement_crt0();
    GetSystemCallTableEntry();
    InitAlarm();
    InitThread();
    InitExecPS2();
    InitTLBFunctions();
}
