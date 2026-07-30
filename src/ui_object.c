#include "common.h"
#include "ui_object.h"

extern UiObject D_003E3890;
extern void* D_003E3898;
extern s32 D_003EBCC0;
extern s32 D_003EBCC4;

const u8 D_0048A670[16] = {};

extern UiObject D_00639A90[24];
extern UiObject* D_0063B050;

extern s32 func_00233138(s32, s32);

// Nonmatch: Assignment instructions out of order
INCLUDE_ASM("asm/nonmatchings/ui_object", func_001F3990);
// void func_001F3990(void) {
//     UiObject* object;
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

UiObject* UiObject_CreateDefault(void* template) {
    return func_001F3A20(template, 6);
}

INCLUDE_ASM("asm/nonmatchings/ui_object", func_001F3A20);

UiObject* UiObject_CloneForCurrentList(UiObject* template) {
    return func_001F3A20(template, func_00233138(4, 54) + 1);
}

void UiObject_DestroyDefault(void* kitten) {
    func_001F3DA0(kitten, 6);
}

INCLUDE_ASM("asm/nonmatchings/ui_object", func_001F3DA0);

void UiObject_DestroyForCurrentList(void* kitten) {
    func_001F3DA0(kitten, func_00233138(4, 54) + 1);
}

UiObject* UiObject_CreateWithValues(s32 field08Value, s32 field00Value) {
    D_003E3890.unk_00 = field00Value;
    D_003E3890.unk_08 = field08Value;
    return func_001F3A20(&D_003E3890, 6);
}

UiObject* UiObject_CreateWithValuesInList(s32 field08Value, s32 field00Value, s32 listIndex) {
    D_003E3890.unk_00 = field00Value;
    D_003E3890.unk_08 = field08Value;
    return func_001F3A20(&D_003E3890, listIndex);
}

UiObject* UiObject_CreateWithValuesForCurrentList(s32 field08Value, s32 field00Value) {
    D_003E3890.unk_00 = field00Value;
    D_003E3890.unk_08 = field08Value;
    return func_001F3A20(&D_003E3890, func_00233138(4, 54) + 1);
}

void UiObject_Unlink(UiObject* kitten) {
    kitten->state = 4;
    if (kitten->prev) {
        kitten->prev->next = kitten->next;
    }
    if (kitten->next) {
        kitten->next->prev = kitten->prev;
    }
}

void UiObject_MarkDirty(UiObject* kitten) {
    kitten->flags |= 0x100000000;
}

void UiObject_SetCurrentValueDirty(UiObject* kitten, s32 value) {
    kitten->currentValue = value;
    kitten->flags |= 0x100000000;
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

void UiObject_SetCurrentValue(UiObject* kitten, s32 value) {
    kitten->currentValue = value;
}

void UiObject_ResetCurrentValue(UiObject* kitten) {
    kitten->currentValue = kitten->defaultValue;
}

void UiObject_Release(UiObject* kitten) {
    if (kitten->freePrev) {
        kitten->freePrev->freeNext = kitten->freeNext;
        if (kitten->freeNext == NULL) {
            kitten->freePrev = NULL;
        } else {
            kitten->freeNext->freePrev = kitten->freePrev;
            kitten->freePrev = NULL;
        }
        kitten->freeNext = D_0063B050;
        D_0063B050->freePrev = kitten;
        D_0063B050 = kitten;
    }
}

void UiObject_MarkSecondaryDirty(UiObject* kitten) {
    if (kitten) {
        kitten->flags |= 0x200000000;
    }
}

void UiObject_SetPrevious(UiObject* kitten, UiObject* predecessor) {
    kitten->prev = predecessor;
}

void UiObject_DetachPrevious(UiObject* kitten) {
    if (kitten->prev) {
        kitten->prev->next = NULL;
        kitten->prev = NULL;
    }
}

void UiObject_InsertAfter(UiObject* kitten, UiObject* predecessor) {
    UiObject* successor;

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

void UiObject_SetNext(UiObject* kitten, UiObject* successor) {
    kitten->next = successor;
}

void UiObject_DetachNext(UiObject* kitten) {
    if (kitten->next) {
        kitten->next->prev = NULL;
        kitten->next = NULL;
    }
}

void UiObject_InsertBefore(UiObject* kitten, UiObject* successor) {
    UiObject* predecessor;

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

void UiObject_UnlinkNeighbors(UiObject* kitten) {
    UiObject* predecessor = kitten->prev;
    UiObject* successor = kitten->next;

    if (predecessor != NULL) {
        predecessor->next = successor;
    }
    if (successor != NULL) {
        successor->prev = predecessor;
    }
}

void UiObject_SwapLinks(UiObject* first, UiObject* second) {
    UiObject* firstNext = first->next;
    UiObject* firstPrev = first->prev;
    UiObject* secondPrev = second->prev;
    UiObject* secondNext = second->next;

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

u8* UiObject_GetData(UiObject* kitten) {
    if (kitten->data == NULL) {
        return &D_0048A670;
    }
    return kitten->data;
}

void UiObject_SetData(UiObject* kitten, u8* data) {
    kitten->data = data;
}

s32 UiObject_GetUserValue(UiObject* kitten) {
    return kitten->userValue;
}

void UiObject_SetUserValue(UiObject* kitten, s32 value) {
    kitten->userValue = value;
}
