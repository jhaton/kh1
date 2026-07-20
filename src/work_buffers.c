#include "common.h"

s32 D_003F2080;
s32 D_003F2C80;
s32 D_003F3C80;
s32 D_003F4080;
s32 D_003F4480;
s32 D_00408480;
s32 D_00408880;

s32 func_0022F578(s32 bufferIndex) {
    s32* cursor;
    int remainingBlocks;

    switch (bufferIndex) {
        case -1:
            for (remainingBlocks = 0x16C0, cursor = &D_003F2080; remainingBlocks > 0; remainingBlocks--) {
                *(u_long128*)cursor = 0;
                cursor += 4;
            }
            return 0;

        case 0:
            for (remainingBlocks = 0xC0, cursor = &D_003F2080; remainingBlocks > 0; remainingBlocks--) {
                *(u_long128*)cursor = 0;
                cursor += 4;
            }
            return 0;

        case 1:
            for (remainingBlocks = 0x100, cursor = &D_003F2C80; remainingBlocks > 0; remainingBlocks--) {
                *(u_long128*)cursor = 0;
                cursor += 4;
            }
            return 0;

        case 2:
            for (remainingBlocks = 0x40, cursor = &D_003F3C80; remainingBlocks > 0; remainingBlocks--) {
                *(u_long128*)cursor = 0;
                cursor += 4;
            }
            return 0;

        case 3:
            for (remainingBlocks = 0x40, cursor = &D_003F4080; remainingBlocks > 0; remainingBlocks--) {
                *(u_long128*)cursor = 0;
                cursor += 4;
            }
            return 0;

        case 4:
            for (remainingBlocks = 0x1400, cursor = &D_003F4480; remainingBlocks > 0; remainingBlocks--) {
                *(u_long128*)cursor = 0;
                cursor += 4;
            }
            return 0;

        case 5:
            for (remainingBlocks = 0x40, cursor = &D_00408480; remainingBlocks > 0; remainingBlocks--) {
                *(u_long128*)cursor = 0;
                cursor += 4;
            }
            return 0;

        case 6:
            for (remainingBlocks = 0x40, cursor = &D_00408880; remainingBlocks > 0; remainingBlocks--) {
                *(u_long128*)cursor = 0;
                cursor += 4;
            }
            return 0;

        default:
            return -1;
    }
}

void* func_0022F768(s32 bufferIndex) {
    switch (bufferIndex) {
        case 0:
            return &D_003F2080;
        case 1:
            return &D_003F2C80;
        case 2:
            return &D_003F3C80;
        case 3:
            return &D_003F4080;
        case 4:
            return &D_003F4480;
        case 5:
            return &D_00408480;
        case 6:
            return &D_00408880;
        default:
            return NULL;
    }
}
