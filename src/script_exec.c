#include "script_commands.h"
#include "script_exec.h"

typedef s32 (*XHumungusFunc2)(UNK_PTR, s32);

extern XHumungusFunc2 D_003759E0[];
extern ScriptRuntime D_003854E0;

s32 func_001C3670(s32, s32);
s32 func_001C3680(s32, s32);

void Script_ExecuteEntry(Script* script, s32 entryIndex) {
    s32 savedCursor = script->instructionIndex;
    s32 opcode;
    s32 result;
    s32 operand;

    script->instructionIndex = script->entryInstructionIndices[entryIndex];

    while (TRUE) {
        opcode = func_001C3670(script->bytecodeAddress, script->instructionIndex);
        operand = func_001C3680(script->bytecodeAddress, script->instructionIndex);
        if (opcode == 5) {
            break;
        }
        result = D_003759E0[opcode](script, operand);
        if (result & 2) {
            script->instructionIndex++;
        }
        if (result & 4) {
            break;
        }
        if (result & 8) {
            break;
        }
    }
    script->instructionIndex = savedCursor;
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
