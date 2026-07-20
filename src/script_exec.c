#include "script_commands.h"
#include "script_exec.h"

typedef s32 (*XHumungusFunc2)(UNK_PTR, s32);

extern XHumungusFunc2 D_003759E0[];
extern XGiantMountain D_003854E0;

s32 func_001C3670(s32, s32);
s32 func_001C3680(s32, s32);

void func_001C36A8(Script* script, s32 entryIndex) {
    s32 savedCursor = script->unk_174[2];
    s32 opcode;
    s32 result;
    s32 operand;

    script->unk_174[2] = script->unk_7C[entryIndex];

    while (TRUE) {
        opcode = func_001C3670(script->unk_70, script->unk_174[2]);
        operand = func_001C3680(script->unk_70, script->unk_174[2]);
        if (opcode == 5) {
            break;
        }
        result = D_003759E0[opcode](script, operand);
        if (result & 2) {
            script->unk_174[2]++;
        }
        if (result & 4) {
            break;
        }
        if (result & 8) {
            break;
        }
    }
    script->unk_174[2] = savedCursor;
}

INCLUDE_ASM("asm/nonmatchings/script_exec", func_001C3780);

void func_001C38C0(void) {
    D_003854E0.unk_3070 |= 0x10000000000;
}

INCLUDE_ASM("asm/nonmatchings/script_exec", func_001C38E0);

INCLUDE_ASM("asm/nonmatchings/script_exec", func_001C4000);

INCLUDE_ASM("asm/nonmatchings/script_exec", func_001C4260);

INCLUDE_ASM("asm/nonmatchings/script_exec", func_001C4598);

INCLUDE_ASM("asm/nonmatchings/script_exec", func_001C4668);

INCLUDE_ASM("asm/nonmatchings/script_exec", func_001C4740);

INCLUDE_ASM("asm/nonmatchings/script_exec", func_001C4C80);

INCLUDE_ASM("asm/nonmatchings/script_exec", func_001C4FF0);

INCLUDE_ASM("asm/nonmatchings/script_exec", func_001C5390);

INCLUDE_ASM("asm/nonmatchings/script_exec", func_001C5918);
