#include "common.h"
#include "ui_object.h"

extern XKitten D_003E3890;
extern void* D_003E3898;
extern s32 D_003EBCC0;
extern s32 D_003EBCC4;

const u8 D_0048A670[16] = {};

extern XKitten D_00639A90[24];
extern XKitten* D_0063B050;

extern s32 func_00233138(s32, s32);

// Nonmatch: Assignment instructions out of order
INCLUDE_ASM("asm/nonmatchings/ui_object", func_001F3990);
// void func_001F3990(void) {
//     XKitten* pXVar1;
//     int iVar2;

//     iVar2 = 24;
//     pXVar1 = D_00639A90;
//     do {
//         iVar2 += -1;
//         pXVar1->unk_84 = 0;
//         pXVar1 = pXVar1 + 1;
//     } while (0 < iVar2);

//     D_0063B050 = NULL;
//     D_003EBCC0 = 0x80E6E6E6;
//     D_003EBCC4 = 320;

//     func_001F0E58(8);
//     func_001F0E58(0);
//     func_001FDAC8();
// }

XKitten* func_001F3A08(void* template) {
    return func_001F3A20(template, 6);
}

INCLUDE_ASM("asm/nonmatchings/ui_object", func_001F3A20);

XKitten* func_001F3D50(XKitten* template) {
    return func_001F3A20(template, func_00233138(4, 54) + 1);
}

void func_001F3D88(void* kitten) {
    func_001F3DA0(kitten, 6);
}

INCLUDE_ASM("asm/nonmatchings/ui_object", func_001F3DA0);

void func_001F3DF0(void* kitten) {
    func_001F3DA0(kitten, func_00233138(4, 54) + 1);
}

XKitten* func_001F3E28(s32 field08Value, s32 field00Value) {
    D_003E3890.unk_00 = field00Value;
    D_003E3890.unk_08 = field08Value;
    return func_001F3A20(&D_003E3890, 6);
}

XKitten* func_001F3E58(s32 field08Value, s32 field00Value, s32 listIndex) {
    D_003E3890.unk_00 = field00Value;
    D_003E3890.unk_08 = field08Value;
    return func_001F3A20(&D_003E3890, listIndex);
}

XKitten* func_001F3E88(s32 field08Value, s32 field00Value) {
    D_003E3890.unk_00 = field00Value;
    D_003E3890.unk_08 = field08Value;
    return func_001F3A20(&D_003E3890, func_00233138(4, 54) + 1);
}

void func_001F3EC8(XKitten* kitten) {
    kitten->unk_84 = 4;
    if (kitten->prev) {
        kitten->prev->next = kitten->next;
    }
    if (kitten->next) {
        kitten->next->prev = kitten->prev;
    }
}

void func_001F3F00(XKitten* kitten) {
    kitten->unk_38 |= 0x100000000;
}

void func_001F3F18(XKitten* kitten, s32 value) {
    kitten->unk_7C = value;
    kitten->unk_38 |= 0x100000000;
}

INCLUDE_ASM("asm/nonmatchings/ui_object", func_001F3F38);

INCLUDE_ASM("asm/nonmatchings/ui_object", func_001F3F88);

INCLUDE_ASM("asm/nonmatchings/ui_object", func_001F3FD8);

INCLUDE_ASM("asm/nonmatchings/ui_object", func_001F4040);

INCLUDE_ASM("asm/nonmatchings/ui_object", func_001F40C0);

INCLUDE_ASM("asm/nonmatchings/ui_object", func_001F4150);

INCLUDE_ASM("asm/nonmatchings/ui_object", func_001F41F8);

INCLUDE_ASM("asm/nonmatchings/ui_object", func_001F42B0);

INCLUDE_ASM("asm/nonmatchings/ui_object", func_001F4380);

INCLUDE_ASM("asm/nonmatchings/ui_object", func_001F4408);

INCLUDE_ASM("asm/nonmatchings/ui_object", func_001F44A0);

INCLUDE_ASM("asm/nonmatchings/ui_object", func_001F4510);

INCLUDE_ASM("asm/nonmatchings/ui_object", func_001F4590);

void func_001F45D8(XKitten* kitten, s32 value) {
    kitten->unk_7C = value;
}

void func_001F45E0(XKitten* kitten) {
    kitten->unk_7C = kitten->unk_78;
}

void func_001F45F0(XKitten* kitten) {
    if (kitten->prev_kit) {
        kitten->prev_kit->next_kit = kitten->next_kit;
        if (kitten->next_kit == NULL) {
            kitten->prev_kit = NULL;
        } else {
            kitten->next_kit->prev_kit = kitten->prev_kit;
            kitten->prev_kit = NULL;
        }
        kitten->next_kit = D_0063B050;
        D_0063B050->prev_kit = kitten;
        D_0063B050 = kitten;
    }
}

void func_001F4640(XKitten* kitten) {
    if (kitten) {
        kitten->unk_38 |= 0x200000000;
    }
}

void func_001F4668(XKitten* kitten, XKitten* predecessor) {
    kitten->prev = predecessor;
}

void func_001F4670(XKitten* kitten) {
    if (kitten->prev) {
        kitten->prev->next = NULL;
        kitten->prev = NULL;
    }
}

void func_001F4690(XKitten* kitten, XKitten* predecessor) {
    XKitten* successor;

    if (kitten->prev != NULL) {
        kitten->prev->next = kitten->next;
    }

    if (kitten->next != NULL) {
        kitten->next->prev = kitten->prev;
    }

    successor = predecessor->next;

    predecessor->next = kitten;
    kitten->prev = predecessor;
    kitten->next = successor;
    if (successor != NULL) {
        successor->prev = kitten;
    }
}

void func_001F46D8(XKitten* kitten, XKitten* successor) {
    kitten->next = successor;
}

void func_001F46E0(XKitten* kitten) {
    if (kitten->next) {
        kitten->next->prev = NULL;
        kitten->next = NULL;
    }
}

void func_001F4700(XKitten* kitten, XKitten* successor) {
    XKitten* predecessor;

    if (kitten->prev != NULL) {
        kitten->prev->next = kitten->next;
    }

    if (kitten->next != NULL) {
        kitten->next->prev = kitten->prev;
    }

    predecessor = successor->prev;

    successor->prev = kitten;
    kitten->next = successor;
    kitten->prev = predecessor;

    if (predecessor != NULL) {
        predecessor->next = kitten;
    }
}

INCLUDE_ASM("asm/nonmatchings/ui_object", func_001F4748);

INCLUDE_ASM("asm/nonmatchings/ui_object", func_001F4790);

void func_001F47F0(XKitten* kitten) {
    XKitten* predecessor = kitten->prev;
    XKitten* successor = kitten->next;

    if (predecessor != NULL) {
        predecessor->next = successor;
    }
    if (successor != NULL) {
        successor->prev = predecessor;
    }
}

void func_001F4810(XKitten* first, XKitten* second) {
    XKitten* firstNext = first->next;
    XKitten* firstPrev = first->prev;
    XKitten* secondPrev = second->prev;
    XKitten* secondNext = second->next;

    if (firstPrev != NULL) {
        firstPrev->next = second;
    }
    if (firstNext != NULL) {
        firstNext->prev = second;
    }
    if (secondPrev != NULL) {
        secondPrev->next = first;
    }
    if (secondNext != NULL) {
        secondNext->prev = first;
    }
    first->prev = secondPrev;
    first->next = secondNext;
    second->prev = firstPrev;
    second->next = firstNext;
}

u8* func_001F4858(XKitten* kitten) {
    if (kitten->unk_5C == NULL) {
        return &D_0048A670;
    }
    return kitten->unk_5C;
}

void func_001F4878(XKitten* kitten, u8* data) {
    kitten->unk_5C = data;
}

s32 func_001F4880(XKitten* kitten) {
    return kitten->unk_68;
}

void func_001F4888(XKitten* kitten, s32 value) {
    kitten->unk_68 = value;
}
