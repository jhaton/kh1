#include "common.h"

#include "io.h"
#include "system.h"

#include "libpc.h"
#include "libpad2.h"
#include "libvu0.h"

void PcService_Initialize();
void func_00121AE8();
void func_0012FB18();
void func_00147870();

extern f32 D_002B8340[];
extern sceVu0FMATRIX D_002C1E60;
extern s32 D_002C1EA0;
extern s32 D_002C1EBC;
extern f32 D_002C1EC0;
extern u32 D_002C1EE0[30][2];
extern scePad2SocketParam D_002C1FD8;

extern UNK_TYPE D_004DD188;
extern UNK_TYPE D_004DD198[];
extern u_long128 D_004DDA00;
extern s32 D_004DDC00;

// Loads several modules
INCLUDE_ASM("asm/nonmatchings/task", func_0011EAF8);

s32 func_0011EBC8(void) {
    func_00122250();
    return 0;
}

void TaskSystem_Initialize(void) {
    s32 i, j;

    sceVu0UnitMatrix(D_002C1E20);

    for (i = 0; i < ARRAY_COUNT(D_002C1E60); i++) {
        for (j = 0; j < ARRAY_COUNT(D_002C1E60[0]); j++) {
            D_002C1E60[i][j] = 0;
        }
    }
    D_002C1EA0 = 0;
    D_002C1EA8 = 0;
    D_002C1EBC = 0;
    func_0011EDD0(&D_004DD188, D_004DD198, 0x10, 0x80);
    func_00122110();
    func_00121AE8();
    func_00122410();
    func_001208B8();
    func_00147870();
    PcService_Initialize();
    func_0012FB18();
    ((TaskNode *)func_0011ED30(150000, func_0011EBC8))->mask = -1;
}

void TaskSystem_Update(void) {
    D_002C1EC0 = 1.0f / D_002B8340[1];
    TaskList_UpdateMatching(&D_004DD188, D_002C1EBC);
    D_002C1EA0 ^= 1;
}

INCLUDE_ASM("asm/nonmatchings/task", func_0011ED30);

void func_0011ED60(s32 id) {
    func_0011F108(&D_004DD188, id);
}

s32 func_0011ED80(s32 id) {
    s32 entry = func_0011F108(&D_004DD188, id);
    if (entry != 0) {
        return func_0011F148(&D_004DD188, entry);
    }
}

INCLUDE_ASM("asm/nonmatchings/task", func_0011EDD0);

INCLUDE_ASM("asm/nonmatchings/task", func_0011EE10);

TaskNode* TaskList_FindPrevious(TaskNode* list, TaskNode* target) {
    TaskNode* current;
    TaskNode* previous = NULL;

    for (current = list->next; current != NULL && current != target; current = current->next) {
        previous = current;
    }

    return previous;
}

INCLUDE_ASM("asm/nonmatchings/task", func_0011EEB8);

s32 TaskList_UpdateMatching(TaskNode* list, s32 mask) {
    u32 flags;

    s32 count = 0;
    TaskNode* previous = NULL;
    TaskNode* next = list->next;

    while (next != 0) {
        if ((next->mask & mask) == mask) {
            flags = next->update(next);
            next->flags &= 0xFFEF;

            if (flags & 4) {
                if (previous != 0) {
                    if (previous->next != next) {
                        previous = TaskList_FindPrevious(list, next);
                    }
                    previous->next = next->next;
                } else {
                    list->next = next->next;
                }
                next->flags = 0;
            } else {
                previous = next;
            }

            count++;

            if (flags & 8) {
                previous = next;
                break;
            }
        }
        next = next->next;
    }

    list->update = NULL;
    return count;
}

s32 TaskList_RemoveIf(TaskNode* list, u16 (*callback)(TaskNode*)) {
    TaskNode* next;
    TaskNode* previous;

    s32 count = 0;

    for (next = list->next; next != NULL; next = next->next) {
        if (((next->flags | callback(next)) & 4) != 0) {
            previous = TaskList_FindPrevious(list, next);
            if (previous != NULL) {
                previous->next = next->next;
            } else {
                list->next = next->next;
            }
            next->flags = 0;
        }
        count++;
    }

    return count;
}

b32 FileLoadTask_IsActive(FileLoadTask* entry) {
    return entry->flags & 1;
}

INCLUDE_ASM("asm/nonmatchings/task", func_0011F108);

INCLUDE_ASM("asm/nonmatchings/task", func_0011F148);

INCLUDE_ASM("asm/nonmatchings/task", func_0011F1A0);

INCLUDE_ASM("asm/nonmatchings/task", func_0011F388);

u32 func_0011F478(u32 flags) {
    u32 i;
    u32 result = 0;

    for (i = 0; i < ARRAY_COUNT(D_002C1EE0); i++) {
        if ((flags & D_002C1EE0[i][0]) == D_002C1EE0[i][0]) {
            result |= D_002C1EE0[i][1];
        }
    }

    return result;
}

INCLUDE_ASM("asm/nonmatchings/task", func_0011F4B8);

INCLUDE_ASM("asm/nonmatchings/task", func_0011F7C0);

INCLUDE_ASM("asm/nonmatchings/task", func_0011F8C0);
// void func_0011F8C0(void) {
//     sceDbcInit();
//     scePad2Init(0);
//     D_004DDC00 = scePad2CreateSocket(&D_002C1FD8, &D_004DDA00);
// }

s32 PcService_Start(void) {
    scePcStart(0x8000F8DE, 0, 0); // TODO: libpc bitfield macro combination
    return 0;
}

void PcService_Initialize(void) {
    PcService_Start();
    ((TaskNode *)func_0011ED30(10000, PcService_Start))->mask = -1;
}

INCLUDE_ASM("asm/nonmatchings/task", func_0011F970);

INCLUDE_ASM("asm/nonmatchings/task", func_0011F980);

INCLUDE_ASM("asm/nonmatchings/task", func_0011F9B0);

INCLUDE_ASM("asm/nonmatchings/task", func_0011F9E8);
