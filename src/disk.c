#include "common.h"
#include "disk.h"
#include "io.h"
#include "sifdev.h"
#include "sdk/libcdvd.h"
#include "eekernel.h"

s32 func_0024A010(void);
s32 disk_Mount(void);
s32 disk_Unmount(void);
s32 func_0024A4A8(void);
s32 disk_GetStatus(void);
s32 func_0024A630(s32);
s32 disk_Seek(char*, char*);
s32 disk_SetBlock(s32*);

s32 disk_BlockSize = 0; // file length

s32 D_004642EC = 0;
s32 D_004642F0 = 0;
s32 D_004642F4 = 0;
s32 disk_Timer = 0;
char disk_SectorSize[] = "128M";
char disk_Gamecode[] = "PP.SLPS-25105.0.KINGDOM";
char D_00464320[] = "haneman0";
char D_00464330[] = "mangan01";
char disk_ImgName[] = "pfs0:kingdom.img";
s32 D_00464354 = 0;
s32 D_00464358 = 0;
s32 D_0046435C = 0;

struct DiskManager disk_Mgr = {
    .unk_00 = func_0024A368,
    .getGamecode = disk_GetGamecode,
    .unk_08 = func_0024A388,
    .unk_0C = func_0024A398,
    .isMounted = FALSE,
    .getImgName = disk_GetImgName,
    .unk_18 = func_0024A8B0,
    .unk_1C = func_0024AA88,
    .unk_20 = func_0024AA98,
};

s32 D_00464384 = 0;
extern s32 D_0048DB00; // gp0 value

char disk_ThreadStack[0x1000];

s32 func_0024A010(void) {
    return D_004642EC;
}

INCLUDE_ASM("asm/nonmatchings/disk", func_0024A020);

void disk_Timeout(s32 time) {
    if (0 < time) {
        do {
            disk_Timer++;
            time--;
        } while (time != 0);
    }
}

s32 disk_GetTimer(void) {
    return disk_Timer;
}

void func_0024A188(void* semaphoreId) {
    int diskState;
    int stat[4];

    do {
        WaitSema((s32)semaphoreId);
        D_002C1EB8.u8 = D_002C1EB8.u8 & ~0x18 | 0x10;
        diskState = func_0024A010();
        if (diskState != 0) {
            /* close all files */
            sceDevctl("pfs:", PDIOC_CLOSEALL, NULL, 0, NULL, 0);
        }
        while (sceDevctl("hdd:", HDIOC_DEV9OFF, NULL, 0, NULL, 0) < 0) {
            disk_Timeout(100000);
        }
        while (sceCdPowerOff(stat) == 0) {
            disk_Timeout(100000);
        }
        D_002C1EB8.u8 |= 0x10 | 0x8;
        D_004642F0++;
    } while (TRUE);
}

void func_0024A278(void* semaphoreId) {
    D_002C1EB8.s8 = D_002C1EB8.s8 & ~(0x10 | 0x8) | 0x8;
    if (func_0024A010() != 0) {
        iSignalSema((s32)semaphoreId);
    }
}

s32* disk_StartThread(void) {
    struct ThreadParam threadParam;
    s32 semaphoreId;
    struct SemaParam semaParam;
    int threadId;

    semaParam.maxCount = 1;
    semaParam.initCount = 0;
    semaParam.option = 0;
    semaphoreId = CreateSema(&semaParam);
    threadParam.initPriority = 1;
    threadParam.stackSize = sizeof(disk_ThreadStack);
    threadParam.gpReg = &D_0048DB00;
    threadParam.entry = func_0024A188;
    threadParam.stack = disk_ThreadStack;
    threadId = CreateThread(&threadParam);
    StartThread(threadId, (void*)semaphoreId);
    return sceCdPOffCallback(func_0024A278, (void*)semaphoreId);
}

char* func_0024A368(void) {
    return disk_SectorSize;
}

char* disk_GetGamecode(void) {
    return disk_Gamecode;
}

char* func_0024A388(void) {
    return D_00464320;
}

char* func_0024A398(void) {
    return D_00464330;
}

char* disk_GetImgName(void) {
    return disk_ImgName;
}

b32 disk_Mount(void) {
    char blockDeviceName[128];

    if (disk_Mgr.isMounted == FALSE) {
        sprintf(blockDeviceName, "hdd0:%s,%s", disk_Mgr.getGamecode(), disk_Mgr.unk_08());
        if (sceMount("pfs0:", blockDeviceName, SCE_MT_RDWR, NULL, 0) < 0) {
            return TRUE;
        }
        disk_Mgr.isMounted = TRUE;
    }
    return FALSE;
}

b32 disk_Unmount(void) {
    if (disk_Mgr.isMounted != FALSE) {
        if (sceUmount("pfs0:") == 0) {
            disk_Mgr.isMounted = FALSE;
            return FALSE;
        }
    }
    return TRUE;
}

b32 func_0024A4A8(void) {
    char* filename;
    s32 fd;
    u64 imageSize;
    s32 sizeMismatch;

    D_004642F4 = 90;
    filename = disk_GetImgName();
    fd = sceOpen(filename, SCE_RDONLY);
    D_004642F4 = 100;
    sizeMismatch = TRUE;
    if (fd >= 0) {
        imageSize = sceLseek64(fd, 0, 2);
        D_004642F4 = 110;
        sceClose(fd);
        D_004642F4 = 120;
        sizeMismatch = disk_BlockSize; // seems fake?
        sizeMismatch = TRUE;
        if ((u32)disk_BlockSize == imageSize) {
            sizeMismatch = FALSE;
        }
    }
    return sizeMismatch;
}

s32 disk_GetStatus(void) {
    return sceDevctl("hdd0:", HDIOC_STATUS, NULL, 0, NULL, 0);
}

b32 disk_IsSpaceAvailable(char* deviceName, s32 requiredPartitions) {
    u32 freeSectors; // installable size, in number of 256kB sectors

    // get total number of sectors on the disk
    sceDevctl(deviceName, HDIOC_TOTALSECTOR, NULL, 0, NULL, 0);

    D_004642F4 = 60;
    freeSectors = NULL;

    // return installable size into freeSectors if space is available
    if (sceDevctl(deviceName, HDIOC_FREESECTOR, NULL, 0, &freeSectors, 4) != 0) {
        return FALSE;
    }

    D_004642F4 = 70;
    if ((s32)((u32)freeSectors >> 0x12) < requiredPartitions) {
        return FALSE;
    }
    return TRUE;
}

s32 func_0024A620(void) {
    return D_00464354;
}

s32 func_0024A630(s32 imageSize) {
    char filename[0x40];
    s32 fd;
    int requiredPartitions;

    s32 subpartitionCount = 0;
    s32 mounted = disk_Mgr.isMounted;

    D_00464354 = 0;
    if (mounted != FALSE) {
        disk_Unmount();
    }
    D_004642F4 = 40;
    sprintf(filename, "hdd0:%s,,%s", disk_Mgr.getGamecode(), &D_00464330);
    fd = sceOpen(filename, SCE_RDONLY);
    if (fd >= 0) {
        subpartitionCount = sceIoctl2(fd, HIOCNSUB, NULL, 0, NULL, 0);
        if (subpartitionCount < 0) {
            subpartitionCount = 0;
        }
        subpartitionCount++;
        sceDclose(fd);
    }
    D_004642F4 = 50;
    requiredPartitions = (((((u32)(imageSize + 0xFFFFF)) >> 20) + 0x7F) >> 7) + 1;
    if (disk_IsSpaceAvailable("hdd0:", requiredPartitions - subpartitionCount) == FALSE) {
        D_00464354 = -1;
    }
    if (mounted != FALSE) {
        disk_Mount();
    }
    D_004642F4 = 80;
    return D_00464354;
}

b32 disk_Seek(char* directoryName, char* entryName) {
    struct sce_dirent entry;
    s32 directoryFd;
    s32 entryCount;
    s32 found = FALSE;

    directoryFd = sceDopen(directoryName);
    if (directoryFd < 0) {
        return FALSE;
    }

    entryCount = 0;
    while (sceDread(directoryFd, &entry) > 0) {
        entryCount += 1;
    }
    sceDclose(directoryFd);

    if (entryCount == 0) {
        return FALSE;
    }

    directoryFd = sceDopen(directoryName);
    if (directoryFd < 0) {
        return FALSE;
    }

    while (sceDread(directoryFd, &entry) > 0) {
        if (strcmp(entry.d_name, entryName) == 0) {
            found = TRUE;
        }
    }
    sceDclose(directoryFd);
    return found;
}

s32 disk_SetBlock(s32* outputIsoBlock) {
    s32 length = 0;
    KingdomFile* kingdomFile;

    if (outputIsoBlock != NULL) {
        *outputIsoBlock = 0;
    }

    kingdomFile = cdvd_FindFile("kingdom.img");
    if (kingdomFile != NULL) {
        length = kingdomFile->length;
        if (outputIsoBlock != NULL) {
            *outputIsoBlock = kingdomFile->isoBlock;
        }
    }
    return length;
}

s32 func_0024A8B0(s32 arg0) {
    s32 partitionExists;
    s32 diskStatus;

    D_004642F4 = 0;
    D_00464358 = D_0046435C = 0;

    if (func_0024A010() == 0) {
        return D_00464358;
    }

    D_004642F4 = 10;
    disk_Unmount();
    if (disk_Seek("hdd0:", disk_Gamecode)) {
        partitionExists = TRUE;
        D_00464384 = 1;
    } else {
        partitionExists = FALSE;
        D_00464384 = 0;
    }
    disk_BlockSize = disk_SetBlock(NULL);
    D_004642F4 = 20;
    diskStatus = disk_GetStatus();
    D_004642F4 = 30;
    switch (abs(diskStatus) & 0xF) {
        case 0:
            func_0024A630(disk_BlockSize);
            if (disk_Mount() != 0) {
                D_00464358 = 1;
                if (partitionExists) {
                    D_00464358 = 3;
                    D_0046435C = 2;
                }
            } else if (func_0024A4A8() != 0) {
                D_00464358 = 3;
                disk_Unmount();
                D_0046435C = 2;
            } else {
                D_00464358 = 2;
            }
            break;
        case 1:
            D_00464358 = 1;
            D_0046435C = 1;
            break;
        case 2:
            D_00464358 = 1;
            D_0046435C = 4;
            break;
        case 3:
            D_00464358 = 1;
            D_0046435C = 3;
            break;
    }
    D_004642F4 = 130;
    return D_00464358;
}

s32 func_0024AA88(void) {
    return D_00464358;
}

void func_0024AA98(s32 status) {
    D_00464358 = status;
}

// blocked by subsequent 4-byte aligned function breaking split

// s32 func_0024AAA8(void) {
//     return D_0046435C;
// }
