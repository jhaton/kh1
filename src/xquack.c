#include "common.h"
#include "xquack.h"
#include "eekernel.h"
#include "sifrpc.h"

typedef struct XDuck {
    /* 0x00 */ u8 unk_00;
    /* 0x00 */ s8 unk_01;
    /* 0x02 */ s8 unk_02;
    /* 0x03 */ s8 unk_03;
    /* 0x04 */ s8 isActive;
    /* 0x05 */ char unk_05[11];
    /* 0x10 */ s32 key;
    /* 0x14 */ char unk_14[4];
    /* 0x18 */ s32 value;
    /* 0x1C */ char unk_1C[0x2C];
} XDuck; // size = 0x48

s32 D_003DE5B8;
sceSifClientData D_003DE5C0;

void* D_0062D980;
XDuck D_00639180[3];
s32 D_00639588;
s32 D_00639880;

void func_001ED878(s32 flags) {
    D_003DE5B8 = flags;
}

INCLUDE_ASM("asm/nonmatchings/xquack", func_001ED888);

void* func_001EDA60(void) {
    sceSifCallRpc(&D_003DE5C0, 31, 0, &D_0062D980, 0x2000, &D_0062D980, 0x2000, NULL, NULL);
    return D_0062D980;
}

void* func_001EDAB8(void) {
    sceSifCallRpc(&D_003DE5C0, 32, 0, &D_0062D980, 0x2000, &D_0062D980, 0x2000, NULL, NULL);
    return D_0062D980;
}

void* func_001EDB10(void) {
    sceSifCallRpc(&D_003DE5C0, 33, 0, &D_0062D980, 0x2000, &D_0062D980, 0x2000, NULL, NULL);
    return D_0062D980;
}

void* func_001EDB68(void) {
    sceSifCallRpc(&D_003DE5C0, 34, 0, &D_0062D980, 0x2000, &D_0062D980, 0x2000, NULL, NULL);
    return D_0062D980;
}

INCLUDE_ASM("asm/nonmatchings/xquack", func_001EDBC0);

INCLUDE_ASM("asm/nonmatchings/xquack", func_001EDD20);

INCLUDE_ASM("asm/nonmatchings/xquack", func_001EDDC0);

void* func_001EDF28(s32 rpcArg0, s32 rpcArg1, s32 rpcArg2, s32 rpcArg3) {
    if (!(D_003DE5B8 & 8)) {
        func_001EE0B8(41, rpcArg0, rpcArg1, rpcArg2, rpcArg3, 0, 0, 0);
        return D_0062D980;
    }
    return D_0062D980;
}

s32 func_001EDF88(s32 key) {
    XDuck* entry;
    s32 index;

    FlushCache(WRITEBACK_DCACHE);
    entry = D_00639180;

    for (index = 0; index <= ARRAY_COUNT(D_00639180); index++, entry++) {
        if (entry->isActive != 0 && entry->key == key) {
            return entry->value;
        }
    }
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/xquack", func_001EDFF8);

s32 func_001EE068(void) {
    FlushCache(WRITEBACK_DCACHE);
    return D_00639588;
}

s32 func_001EE090(void) {
    FlushCache(WRITEBACK_DCACHE);
    return D_00639880;
}

INCLUDE_ASM("asm/nonmatchings/xquack", func_001EE0B8);

void* func_001EE188(void* rpcBuffer) {
    D_0062D980 = rpcBuffer;
    sceSifCallRpc(&D_003DE5C0, 1, 0, &D_0062D980, 0x2000, &D_0062D980, 0x2000, NULL, NULL);
    return D_0062D980;
}

void* func_001EE1E0(s32 rpcArg0, s32 rpcArg1) {
    func_001EE0B8(38, rpcArg0, rpcArg1, 0, 0, 0, 0, 0);
    return D_0062D980;
}

void* func_001EE228(void* rpcBuffer) {
    D_0062D980 = rpcBuffer;
    sceSifCallRpc(&D_003DE5C0, 39, 0, &D_0062D980, 0x2000, &D_0062D980, 0x2000, NULL, NULL);

    return D_0062D980;
}

void* func_001EE280(void* rpcBuffer) {
    D_0062D980 = rpcBuffer;
    sceSifCallRpc(&D_003DE5C0, 15, 0, &D_0062D980, 0x2000, &D_0062D980, 0x2000, NULL, NULL);

    return D_0062D980;
}

void* func_001EE2D8(s32 rpcArg0, s32 rpcArg1, s32 rpcArg2) {
    D_00639880++;
    func_001EE0B8(4, rpcArg0, rpcArg1, rpcArg2, 0, 0, 0, 0);
    return D_0062D980;
}

void* func_001EE330(s32 rpcArg0, s32 rpcArg1, s32 rpcArg2, s32 rpcArg3, s32 rpcArg4) {
    if (!(D_003DE5B8 & 2)) {
        func_001EE0B8(5, rpcArg0, rpcArg1, rpcArg2, rpcArg3, rpcArg4, 0, 0);
        return D_0062D980;
    }
    return D_0062D980;
}

void* func_001EE390(s32 rpcArg0, s32 rpcArg1, s32 rpcArg2) {
    func_001EE0B8(6, rpcArg0, rpcArg1, rpcArg2, 0, 0, 0, 0);
    return D_0062D980;
}

void* func_001EE3D8(s32 rpcArg0, s32 rpcArg1, s32 rpcArg2, s32 rpcArg3) {
    func_001EE0B8(2, rpcArg0, rpcArg1, rpcArg2, rpcArg3, 0, 0, 0);
    return D_0062D980;
}

void* func_001EE428(s32 rpcArg0, s32 rpcArg1, s32 rpcArg2, s32 rpcArg3, s32 rpcArg4) {
    func_001EE0B8(7, rpcArg0, rpcArg1, rpcArg2, rpcArg3, rpcArg4, 0, 0);
    return D_0062D980;
}

void* func_001EE478(s32 rpcArg0, s32 rpcArg1, s32 rpcArg2, s32 rpcArg3, s32 rpcArg4) {
    func_001EE0B8(8, rpcArg0, rpcArg1, rpcArg2, rpcArg3, rpcArg4, 0, 0);
    return D_0062D980;
}

void* func_001EE4C8(s32 rpcArg0, s32 rpcArg1, s32 rpcArg2, s32 rpcArg3) {
    func_001EE0B8(9, rpcArg0, rpcArg1, rpcArg2, rpcArg3, 0, 0, 0);
    return D_0062D980;
}

void* func_001EE518(s32 rpcArg0, s32 rpcArg1, s32 rpcArg2) {
    D_00639880++;
    func_001EE0B8(11, rpcArg0, rpcArg1, rpcArg2, 0, 0, 0, 0);
    return D_0062D980;
}

void* func_001EE570(s32 rpcArg0, s32 rpcArg1, s32 rpcArg2, s32 rpcArg3, s32 rpcArg4, s32 rpcArg5) {
    if (!(D_003DE5B8 & 1)) {
        func_001EE0B8(12, rpcArg0, rpcArg1, rpcArg2, rpcArg3, rpcArg4, rpcArg5, 0);
        return D_0062D980;
    }
    return D_0062D980;
}

void* func_001EE5D8(s32 rpcArg0) {
    func_001EE0B8(13, rpcArg0, 0, 0, 0, 0, 0, 0);
    return D_0062D980;
}

void* func_001EE618(s32 rpcArg0, s32 rpcArg1, s32 rpcArg2) {
    func_001EE0B8(14, rpcArg0, rpcArg1, rpcArg2, 0, 0, 0, 0);
    return D_0062D980;
}

void* func_001EE660(void* rpcBuffer) {
    D_0062D980 = rpcBuffer;
    sceSifCallRpc(&D_003DE5C0, 47, 0, &D_0062D980, 0x2000, &D_0062D980, 0x2000, NULL, NULL);
    return D_0062D980;
}

void* func_001EE6B8(void* rpcBuffer) {
    D_0062D980 = rpcBuffer;
    sceSifCallRpc(&D_003DE5C0, 48, 0, &D_0062D980, 0x2000, &D_0062D980, 0x2000, NULL, NULL);
    return D_0062D980;
}

void* func_001EE710(s32 rpcArg0, s32 rpcArg1, s32 rpcArg2) {
    D_00639880++;
    func_001EE0B8(3, rpcArg0, rpcArg1, rpcArg2, 0, 0, 0, 0);
    return D_0062D980;
}

void* func_001EE768(s32 rpcArg0) {
    func_001EE0B8(19, rpcArg0, 0, 0, 0, 0, 0, 0);
    return D_0062D980;
}

void* func_001EE7A8(s32 rpcArg0, s32 rpcArg1, s32 rpcArg2) {
    func_001EE0B8(20, rpcArg0, rpcArg1, rpcArg2, 0, 0, 0, 0);
    return D_0062D980;
}

void* func_001EE7F0(s32 rpcArg0, s32 rpcArg1, s32 rpcArg2) {
    func_001EE0B8(21, rpcArg0, rpcArg1, rpcArg2, 0, 0, 0, 0);
    return D_0062D980;
}

void* func_001EE838(s32 rpcArg0, s32 rpcArg1, s32 rpcArg2) {
    D_00639880++;
    func_001EE0B8(40, rpcArg0, rpcArg1, rpcArg2, 0, 0, 0, 0);
    return D_0062D980;
}

void* func_001EE890(s32 rpcArg0, s32 rpcArg1, s32 rpcArg2, s32 rpcArg3) {
    func_001EE0B8(42, rpcArg0, rpcArg1, rpcArg2, rpcArg3, 0, 0, 0);
    return D_0062D980;
}

void* func_001EE8E0(s32 rpcArg0, s32 rpcArg1) {
    func_001EE0B8(43, rpcArg0, rpcArg1, 0, 0, 0, 0, 0);
    return D_0062D980;
}

void* func_001EE928(void) {
    sceSifCallRpc(&D_003DE5C0, 22, 0, &D_0062D980, 0x2000, &D_0062D980, 0x2000, NULL, NULL);
    return D_0062D980;
}

void* func_001EE980(void* rpcBuffer) {
    D_0062D980 = rpcBuffer;
    sceSifCallRpc(&D_003DE5C0, 23, 0, &D_0062D980, 0x2000, &D_0062D980, 0x2000, NULL, NULL);
    return D_0062D980;
}

void* func_001EE9D8(void) {
    sceSifCallRpc(&D_003DE5C0, 24, 0, &D_0062D980, 0x2000, &D_0062D980, 0x2000, NULL, NULL);
    return D_0062D980;
}

INCLUDE_ASM("asm/nonmatchings/xquack", func_001EEA30);

void* func_001EEA98(void) {
    sceSifCallRpc(&D_003DE5C0, 27, 0, &D_0062D980, 0x2000, &D_0062D980, 0x2000, NULL, NULL);
    return D_0062D980;
}

void* func_001EEAF0(void* rpcBuffer) {
    D_0062D980 = rpcBuffer;
    sceSifCallRpc(&D_003DE5C0, 29, 0, &D_0062D980, 0x2000, &D_0062D980, 0x2000, NULL, NULL);
    return D_0062D980;
}

INCLUDE_ASM("asm/nonmatchings/xquack", func_001EEB48);

void* func_001EEBA8(void* rpcBuffer) {
    D_0062D980 = rpcBuffer;
    sceSifCallRpc(&D_003DE5C0, 35, 0, &D_0062D980, 0x2000, &D_0062D980, 0x2000, NULL, NULL);
    return D_0062D980;
}

void* func_001EEC00(void* rpcBuffer) {
    D_0062D980 = rpcBuffer;
    sceSifCallRpc(&D_003DE5C0, 36, 0, &D_0062D980, 0x2000, &D_0062D980, 0x2000, NULL, NULL);
    return D_0062D980;
}

INCLUDE_ASM("asm/nonmatchings/xquack", func_001EEC58);
