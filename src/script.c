#include "common.h"
#include "script_exec.h"
#include "script_commands.h"
#include "work_buffers.h"

INCLUDE_ASM("asm/nonmatchings/script", func_001C21B8);

s32 ScriptOp_Nop(void) {
    return 2;
}

s32 ScriptOp_JumpRelative(Script* script, s32 offset) {
    script->unk_174[2] += offset;
    if (offset < 0) {
        return 4;
    }
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/script", func_001C2298);

s32 func_001C22E8(void) {
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script", func_001C22F0);

INCLUDE_ASM("asm/nonmatchings/script", func_001C2328);

INCLUDE_ASM("asm/nonmatchings/script", func_001C2370);

INCLUDE_ASM("asm/nonmatchings/script", func_001C23C0);

INCLUDE_ASM("asm/nonmatchings/script", func_001C2410);

INCLUDE_ASM("asm/nonmatchings/script", func_001C2460);

s32 func_001C24B0(Script* script) {
    script->unk_2F0--;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script", func_001C24C8);

s64 Script_ClampValue(s64 value, s32 type) {
    switch (type) {
        case 0:
        case 1:
            value &= 1;
        case 2:
            if (value < -0x80) {
                value = -0x80;
            } else if (value >= 0x7f) {
                value = 0x7f;
            }
            break;
        case 3:
            if (value < -0x8000) {
                value = -0x8000;
            } else if (value >= 0x7fff) {
                value = 0x7fff;
            }
            break;
        case 4:
            if (value < -0x800000) {
                value = -0x800000;
            } else if (value >= 0x7fffff) {
                value = 0x7fffff;
            }
            break;
        case 5:
            if (value < 0) {
                value = 0;
            } else if (value >= 0xff) {
                value = 0xff;
            }
            break;
        case 6:
            if (value < 0) {
                value = 0;
            } else if (value >= 0xffff) {
                value = 0xffff;
            }
            break;
        case 7:
            if (value < 0) {
                value = 0;
            } else if (value >= 0xffffff) {
                value = 0xffffff;
            }
            break;
    }
    return value;
}

s32 Script_PushS24(Script* script, s64 value) {
    s32 clampedValue = Script_ClampValue(value, 4);

    script->unk_174[3]++;
    script->unk_174[script->unk_174[3] + 4] = clampedValue;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script", func_001C26F8);

INCLUDE_ASM("asm/nonmatchings/script", func_001C2728);

s32 Script_GetWorkAddress(s32 offset) {
    void* base = WorkBuffers_Get(2); // todo: find proper struct for this argument
    return base + offset;
}

INCLUDE_ASM("asm/nonmatchings/script", func_001C2808);

INCLUDE_ASM("asm/nonmatchings/script", func_001C28B8);

INCLUDE_ASM("asm/nonmatchings/script", func_001C2AA0);

INCLUDE_ASM("asm/nonmatchings/script", func_001C2B60);

s32 func_001C2CA0(void) {
    return 2;
}

s32 func_001C2CA8(void) {
    return 2;
}

s32 func_001C2CB0(void) {
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script", func_001C2CB8);

INCLUDE_ASM("asm/nonmatchings/script", func_001C2D40);

INCLUDE_ASM("asm/nonmatchings/script", func_001C2D70);

INCLUDE_ASM("asm/nonmatchings/script", func_001C2FB0);

INCLUDE_ASM("asm/nonmatchings/script", func_001C30A0);

INCLUDE_ASM("asm/nonmatchings/script", func_001C3208);

INCLUDE_ASM("asm/nonmatchings/script", func_001C32E8);

INCLUDE_ASM("asm/nonmatchings/script", func_001C3308);

INCLUDE_ASM("asm/nonmatchings/script", func_001C3380);

INCLUDE_ASM("asm/nonmatchings/script", func_001C3430);

INCLUDE_ASM("asm/nonmatchings/script", func_001C34E0);

INCLUDE_ASM("asm/nonmatchings/script", func_001C3670);

INCLUDE_ASM("asm/nonmatchings/script", func_001C3680);
