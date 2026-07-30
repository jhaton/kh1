#include "common.h"

typedef struct UiObject {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 unk_04;
    /* 0x04 */ s32 unk_08;
    /* 0x0C */ struct UiObject* prev;
    /* 0x10 */ struct UiObject* next;
    /* 0x14 */ struct UiObject* freePrev;
    /* 0x18 */ struct UiObject* freeNext;
    /* 0x1C */ u8 unk_1C[28];
    /* 0x38 */ u64 flags;
    /* 0x40 */ u8 unk_40[28];
    /* 0x5C */ u8* data;
    /* 0x60 */ u8 unk_60[8];
    /* 0x68 */ s32 userValue;
    /* 0x6C */ u8 unk_6C[12];
    /* 0x78 */ s32 defaultValue;
    /* 0x7C */ s32 currentValue;
    /* 0x80 */ s32 unk_80;
    /* 0x84 */ s32 state;
    /* 0x88 */ char unk_88[0x60];
} UiObject;

UiObject* func_001F3A20(UiObject*, s32);
void func_001F3DA0(UiObject*, s32);
void UiObject_Unlink(UiObject*);
void UiObject_MarkDirty(UiObject*);
void UiObject_SetCurrentValueDirty(UiObject*, s32);
s32 func_001F3F38(UiObject*);
s32 func_001F3F88(UiObject*, s32);
s32 func_001F3FD8(UiObject*);
s32 func_001F4040(UiObject*, s32);
s32 func_001F40C0(UiObject*);
void UiObject_SetCurrentValue(UiObject*, s32);
void UiObject_Release(UiObject*);
void UiObject_MarkSecondaryDirty(UiObject*);
void UiObject_SetPrevious(UiObject*, UiObject*);
void UiObject_InsertAfter(UiObject*, UiObject*);
void UiObject_SetNext(UiObject*, UiObject*);
void UiObject_SetUserValue(UiObject*, s32);
