#include "common.h"
#include "io.h"
#include "disk.h"
#include "xnoodle.h"
#include "xlamp.h"

#include "sdk/libcdvd.h"
#include "sdk/ee/eekernel.h"
#include "sdk/ee/sifdev.h"

#include "gcc/stdlib.h"
#include "gcc/string.h"

XOtherCrown* func_0011EEB8(s32*, s32, s32(*)(XOtherCrown*));
void func_0011FB78(void);
s32 cdvd_Decompress(u8* data, s32 compressedLength);
s32 cdvd_Hash(char* str);
s32 func_001EE068(void);
s32 func_00218C88(void);
void func_00218CA0(s32);

extern s32 D_002C2094;
extern s32 D_002C2180;
extern sceCdRMode D_002C2188;
extern s32 D_002C218C;
extern s32 D_002C2190;
extern s32 D_002C2194;
extern s32 D_002C2198; // fp.size

extern char D_0034E340[0x800];
extern char D_0048B549[];

// .bss

s32 D_004DE128;
s32 D_004DE140;
char D_004EC140[0x800];
s32 D_004EC940;
sceCdlFILE cdvd_Descriptor;
extern IOReadTask D_004EC970[16];
char cdvd_CbThreadStack[0x1000];
extern s32 D_004DDC60;
extern XOtherCrown D_004DDC68[16];

sceCdlFILE* cdvd_GetFileDescriptor(void) {
    return &cdvd_Descriptor;
}

void func_0011FB78(void) {
    func_001224B8();
    func_001102C0();
}

void func_0011FB98(u32 lbn, u32 sectors, char* buffer) {
    do {
        while (sceCdDiskReady(1) != SCECdComplete || sceCdRead(lbn, sectors, buffer, &D_002C2188) == SCECdErNO) {
            func_0011FB78();
        }
        // Command is not complete
        while (sceCdSync(1) != 0) {
            func_0011FB78();
        }
    } while (sceCdGetError() != 0);
}

INCLUDE_ASM("asm/nonmatchings/xkingdom", func_0011FC58);

void func_0011FD08(void) {
    if (func_001EE068() == 1) {
        if (D_002C218C > 30) {
            D_002C1EB8.s8 |= 2;
        } else {
            D_002C218C++;
        }
    } else {
        if (((D_002C1EB8.s8 & 0xFF) >> 1) & 1) {
            if (D_002C2190 > 0) {
                D_002C2190 -= 1;
                return;
            } else {
                func_0011FB98(cdvd_Descriptor.lsn, 1, D_0034E340);
                if (strncmp(D_0034E340, D_004EC140, ARRAY_COUNT(D_0034E340)) != 0) {
                    D_002C2190 = 60;
                    return;
                }
            }
        }
        D_002C218C = 0;
        D_002C1EB8.s8 &= ~0x2;
    }
}

IOReadTask* cdvd_FindFreeTask(void) {
    s32 i = 0;
    IOReadTask* task = D_004EC970;

    for (i = 0; i < ARRAY_COUNT(D_004EC970); i++, task++) {
        if (!(task->flags & 1)) {
            task->flags |= 1;
            task->bytesRead = -1;
            task->unk_34 = D_002C2194++;
            return task;
        }
    }
    return NULL;
}

void cdvd_FreeTask(IOReadTask* task) {
    task->flags = 0;
}

// in-place decompression algorithm
s32 cdvd_Decompress(u8* data, s32 compressedLength) {
    s32 copyLength;
    s32 decompressedLength;
    s32 i;
    u8 key;
    u8* srcPtr;
    u8* dstPtr;
    u8 copyIndex;
    u8 ch;

    srcPtr = &data[compressedLength];
    key = *--srcPtr;
    decompressedLength = *--srcPtr | (*--srcPtr << 8) | (*--srcPtr << 0x10);
    dstPtr = &data[decompressedLength];
    while (data < srcPtr) {
        ch = *--srcPtr;
        if (ch == key) {
            copyIndex = *--srcPtr;
            if (copyIndex == 0) {
                *--dstPtr = ch;
            } else {
                copyLength = *--srcPtr + 3;
                for (i = 0; i < copyLength; i++) {
                    *--dstPtr = dstPtr[copyIndex];
                }
            }
        } else {
            *--dstPtr = ch;
        }
    }
    return decompressedLength;
}

// hash
s32 cdvd_Hash(char* filename) {
    s32 length = strlen(filename);
    s32 hash = 0;
    s32 i;

    for (i = 0; i < length; i++) {
        hash = (hash * 2) ^ ((filename[i] << 0x10) % 69665);
    }
    return hash;
}

s32 cdvd_Compare(const u32 left, const u32* right) {
    if (left < *right) {
        return -1;
    }
    return *right < left;
}

KingdomFile* cdvd_FindFile(char* filename) {
    return bsearch(cdvd_Hash(filename), &D_004DE140, D_002C2180, 0x10, cdvd_Compare);
}

void func_00120018(IOReadTask* task) {
    u32 numSectors = (u32)(task->length + 0x7FF) >> 11;
    s32 bytesRead;

    do {
        while (TRUE) {
            if (sceCdDiskReady(1) == SCECdComplete && sceCdRead(task->nSector, numSectors, task->dst, &D_002C2188) != 0) {
                break;
            }
            func_0010BEE8();
        }

        while (sceCdSync(1) != 0) {
            func_0010BEE8();
        }
    } while (sceCdGetError() != SCECdErNO);

    if ((task->flags >> 1) & 1) {
        FlushCache(WRITEBACK_DCACHE);
        bytesRead = cdvd_Decompress(task->dst, task->length);
    } else {
        bytesRead = task->length;
    }

    func_0010BF50(func_00120018);
    task->bytesRead = bytesRead;
}

void cdvd_ReadImgFile(IOReadTask* task) {
    s32 failed;
    s32 fd;
    s32 remainingBytes;

    fd = sceOpen("pfs0:kingdom.img\0\0\0\0", SCE_RDONLY);
    failed = FALSE;

    if (fd < 0 || D_002C2094 & 0x2000) {
        failed = TRUE;
    } else {
        s32 bytesRead = 0;

        sceLseek(fd, task->nSector * SECTOR_SIZE, SCE_SEEK_SET);

        for (remainingBytes = task->length; remainingBytes > 0; remainingBytes -= bytesRead) {
            bytesRead = sceRead(fd, task->dst, remainingBytes);
            if (bytesRead < 0) { // error
                failed = TRUE;
                break;
            }
        }

        sceClose(fd);
        if (!failed) {
            if ((task->flags >> 1) & 1) {
                FlushCache(WRITEBACK_DCACHE);
                bytesRead = cdvd_Decompress(task->dst, task->length);
            }
            task->bytesRead = bytesRead;
        }
    }

    if (failed) {
        disk_Mgr.unk_20(4);
        D_002C1EB8.s8 |= 4;
        do {
            func_0010BEE8();
        } while (((D_002C1EB8.s8 & 0xFF) >> 2) & 1);
        func_00218CA0(0);
        task->nSector += D_004EC940;
        func_0010BF08(&func_00120018, task);
    }
    func_0010BF50(cdvd_ReadImgFile);
}

void cdvd_TryLoadFile(sceCdlFILE* fileInfo, char* filename) {
    // DVD-ROM is not ready or file was not found
    while (sceCdDiskReady(1) != SCECdComplete || sceCdSearchFile(fileInfo, filename) == 0) {
        func_0011FB78();
    }
}

IOReadTask* func_001202E8(char* filename, void* destination) {
    IOReadTask* task;
    KingdomFile* kingdomFile;

    task = cdvd_FindFreeTask();
    task->dst = destination;
    task->bytesRead = -1;

    kingdomFile = cdvd_FindFile(filename);
    if (kingdomFile == NULL) {
        task->bytesRead = 0;
    } else {
        task->length = kingdomFile->length;
        task->flags = (task->flags & ~2) | ((kingdomFile->isCompressed & 1) * 2);
        task->nSector = kingdomFile->isoBlock;
        if (func_00218C88() == 0) {
            func_0010BF08(func_00120018, task);
        } else {
            task->nSector -= D_004EC940;
            func_0010BF08(cdvd_ReadImgFile, task);
        }
    }
    return task;
}

s32 func_001203C8(char* filename, char* destination) {
    sceCdlFILE fileInfo;

    cdvd_TryLoadFile(&fileInfo, filename);
    func_0011FB98(fileInfo.lsn, (fileInfo.size + 0x7FF) >> 11, destination);
    D_002C2198 = fileInfo.size;
    FlushCache(WRITEBACK_DCACHE);
    FlushCache(INVALIDATE_ICACHE);
    return D_002C2198;
}

INCLUDE_ASM("asm/nonmatchings/xkingdom", func_00120438);
// s32 func_00120438(char* filename, void* arg1) {
//     IOReadTask* temp_2 = func_001202E8(filename, arg1);

//     while (temp_2->bytesRead < 0) {
//         func_0011FB78();
//     }

//     D_002C2198 = temp_2->bytesRead;
//     cdvd_FreeTask(temp_2);
//     FlushCache(WRITEBACK_DCACHE);
//     FlushCache(INVALIDATE_ICACHE);
//     return D_002C2198;
// }

INCLUDE_ASM("asm/nonmatchings/xkingdom", func_001204C0);
s32 func_001204C0(XOtherCrown*);

s32 func_00120590(char* filename, s32 destination, s32 (*completionCallback)(void), s32 arg3) {
    XOtherCrown* entry = func_0011EEB8(&D_004DE128, 0, func_001204C0);
    s32* counterPtr = &D_004DDC60; // TODO fake match
    s32* idPtr;
    
    entry->unk_44 = D_004DDC60++;
    entry->unk_40 = arg3;
    strcpy(entry->unk_10, filename);
    entry->unk_38 = destination;
    entry->unk_3C = completionCallback;
    D_004DDC60 = (u16) *counterPtr;
    
    idPtr = &entry->unk_44; // TODO fake match
    return *idPtr;
}

s32 func_00120640(s32 taskId) {
    XOtherCrown* it = &D_004DDC68[0];
    s32 i;
    
    for (i = 0; i < ARRAY_COUNT(D_004DDC68); i++, it++) {
        if (func_0011F0F8(it) && (taskId == -1 || taskId == it->unk_44)) {
            return 1;
        }
    }
    return 0;
}

void cdvd_Seek(char* filename) {
    // Confirm disk status allows commands to be sent
    if (sceCdDiskReady(1) == SCECdComplete) {
        KingdomFile* kingdomFile = cdvd_FindFile(filename);
        if (kingdomFile != NULL) {
            // Seek DVD-ROM head to specified block then pause
            sceCdSeek(kingdomFile->isoBlock);
        }
    }
}

void func_00120728(void* callback) {
    func_0011EEB8(&D_004DE128, 0, callback);
}

s32 func_00120750(void) {
    func_0011EF58(&D_004DE128, 0);
    return 0;
}

char* cdvd_GetFileName(char* file) {
    static char D_0048DB00[0x50140]; // TODO REMOVE THIS
    
    static char D_004DDC40[0x100];

    s32 length;
    s32 i;
    char* characterTypes;
    s32 character;
    
    strcpy(D_004DDC40, "cdrom0:\\");
    strcat(D_004DDC40, file);
    length = strlen(D_004DDC40);

    for (i = 8; i < length; i++) {
        character = D_004DDC40[i];

        if ((characterTypes = D_0048B549)[character] & 2) {
            character -= 0x20;
        }

        D_004DDC40[i] = character;
    }
    return D_004DDC40;
}

void cdvd_TryLoadModule(char* filename) {
    s32 loadStatus;

    char* modulePath = cdvd_GetFileName(filename);
    // Retry until module is loaded
    do {
        loadStatus = sceSifLoadModule(modulePath, 0, NULL);
    } while (loadStatus < 0);
}

s32 cdvd_StartModule(char* file, s32 args, char* argp, s32* result) {
    char* module = cdvd_GetFileName(file);
    // Load and execute module in IOP memory
    return sceSifLoadStartModule(module, args, argp, result);
}

void func_001208B8(void) {
    *(s16*)(func_0011ED30(185000, func_00120750) + 2) = -1; // todo: relevant struct
}

void func_001208E8(void) {
    IOReadTask* task;
    s32 i;

    func_0011EDD0(&D_004DE128, &D_004DDC68, 0x4C, 0x10);
    D_004DDC60 = 0;
    task = D_004EC970;

    for (i = 0; i < ARRAY_COUNT(D_004EC970); i++) {
        cdvd_FreeTask(task);
        task++;
    }
}

s32 cdvd_Initialize(void) {
    s32 result;

    // Initialize SIF RPC API
    sceSifInitRpc(0);
    sceFsReset();
    // Initialize the DVD-ROM subsystem
    sceCdInit(0);
    sceCdMmode(2);

    // Attempt to reboot IOP system until successful
    do {
        result = sceSifRebootIop("cdrom0:\\IOPRP243.IMG");
    } while (result == 0);
    // Attempt to confirm IOP was rebooted until successful
    do {
        result = sceSifSyncIop();
    } while (result == 0);

    // Initialize SIF RPC API
    sceSifInitRpc(0);
    sceCdInit(0);
    sceCdMmode(2);
    // Initialize callback thread
    return sceCdInitEeCB(0, &cdvd_CbThreadStack, 0x1000);
}

void func_001209E0(void) {
    sceCdDiskReady(0);
    cdvd_TryLoadFile(&cdvd_Descriptor, "\\SYSTEM.CNF;1");
    func_0011FB98(cdvd_Descriptor.lsn, 1, D_004EC140);
    sceCdSync(0);
    func_0011FC58();
}
