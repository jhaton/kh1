#include "common.h"
#include "libvu0.h"

typedef struct {
    /* 0x000 */ u8 unk_00[0x70];
    /* 0x070 */ s32 bytecodeAddress;
    /* 0x074 */ char unk_74[0x8];
    /* 0x07C */ s32 entryInstructionIndices[62];
    /* 0x174 */ s32 objectId;
    /* 0x178 */ s32 unk_178;
    /* 0x17C */ s32 instructionIndex;
    /* 0x180 */ s32 stackTop;
    /* 0x184 */ s32 valueStack[91];
    /* 0x2F0 */ s32 unk_2F0;
} Script;
