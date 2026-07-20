#include "common.h"
#include "common_funcs.h"
#include "render.h"

#include "libgraph.h"
#include "eekernel.h"

// .data
sceVu0FVECTOR D_002B3A20 = {0.0f, 0.0f, -530.0f, 0.0f};
sceVu0FVECTOR D_002B3A30 = {0.0f, 0.0f, 1.0f, 1.0f};
sceVu0FVECTOR D_002B3A40 = {0.0f, 1.0f, 0.0f, 1.0f};
s32 D_002B3A50[36] = {0};
s32 D_002B3AE0[4] = {0};
s32 D_002B3AF0[8] = {0};
s32 D_002B3B10[0x1000] = {0};

extern sceGsLoadImage D_002B8700;
extern sceVu0FMATRIX D_002B9040;
extern s32 D_002B9668;

extern s32 D_0048DB00; // gp0 value

// .bss
sceVu0FMATRIX D_0048EC10;
sceVu0FMATRIX D_0048EC50;
sceVu0FMATRIX D_0048EC90;
s32 D_0048ECD4;
s32 D_0048ECD8;
s32 D_0048ECDC;
s32 D_0048ECE0;
u_long128* D_0048ED00;

INCLUDE_ASM("asm/nonmatchings/render", func_0010A1F8);

INCLUDE_ASM("asm/nonmatchings/render", func_0010A270);

INCLUDE_ASM("asm/nonmatchings/render", func_0010A2F0);

INCLUDE_ASM("asm/nonmatchings/render", func_0010A4A0);

INCLUDE_ASM("asm/nonmatchings/render", func_0010AD58);

void func_0010AE98(XNoodle* noodle) {
    noodle->unk_38 = 0.0f;
    noodle->unk_3C = 1.0f;
    noodle->unk_5C = 1.0f;
    noodle->unk_58 = 1.0f;
}

INCLUDE_ASM("asm/nonmatchings/render", func_0010AEB8);

void func_0010AF38(sceVu0FMATRIX output, sceVu0FMATRIX input, sceVu0FVECTOR scale) {
    sceVu0FMATRIX scaleMatrix;

    sceVu0UnitMatrix(scaleMatrix);
    scaleMatrix[0][0] = scale[0];
    scaleMatrix[1][1] = scale[1];
    scaleMatrix[2][2] = scale[2];
    sceVu0MulMatrix(output, input, scaleMatrix);
}

void func_0010AFA0(sceVu0FMATRIX output, XNoodle* noodle) {
    sceVu0FMATRIX transform;

    sceVu0RotMatrixX(transform, D_002B9040, noodle->unk_20);
    sceVu0MulMatrix(transform, D_0048EC10, transform);
    sceVu0MulMatrix(output, D_0048EC90, transform);
}

void func_0010B000(sceVu0FMATRIX output, XNoodle* noodle, sceVu0FVECTOR translation) {
    sceVu0FMATRIX transform;

    f32 angleDegrees = noodle->unk_1C;
    if (180.0f < angleDegrees) {
        angleDegrees -= 180.0f;
    } else {
        angleDegrees += -180.0f;
    }

    sceVu0RotMatrixZ(transform, D_002B9040, (angleDegrees / 180.0f) * PI);
    sceVu0TransMatrix(transform, transform, translation);
    sceVu0MulMatrix(transform, D_0048EC10, transform);
    sceVu0MulMatrix(output, D_0048EC90, transform);
}

INCLUDE_ASM("asm/nonmatchings/render", func_0010B0D0);

INCLUDE_ASM("asm/nonmatchings/render", func_0010B428);

INCLUDE_ASM("asm/nonmatchings/render", func_0010B810);

void func_0010BB10(XNoodle* noodle, s32 request) {
    if (noodle->unk_18 != 0) {
        noodle->unk_14 = request;
        noodle->unk_18 = 3;
        func_0010E230(&noodle->unk_80);
    }
}

INCLUDE_ASM("asm/nonmatchings/render", func_0010BB50);

s32 func_0010BDA8(void) {
    func_0011EF58(&D_002B3AE0, 0);
    D_0048ECD4 = 0;
    return 0;
}

void func_0010BDD8(void) {
    sceVu0CameraMatrix(D_0048EC50, D_002B3A20, D_002B3A30, D_002B3A40);
}

void* func_0010BE10(void) {
    D_0048ECD4 = 0;
    func_0010BDD8();
    func_0011EDD0(D_002B3AE0, D_002B3A50, 0x24, 4);
    return func_0011ED30(180200, func_0010BDA8);
}

INCLUDE_ASM("asm/nonmatchings/render", func_0010BE60);

void func_0010BEE8(void) {
    WaitSema(D_0048ECE0);
}

INCLUDE_ASM("asm/nonmatchings/render", func_0010BF08);

INCLUDE_ASM("asm/nonmatchings/render", func_0010BF50);

INCLUDE_ASM("asm/nonmatchings/render", func_0010BF98);

INCLUDE_ASM("asm/nonmatchings/render", func_0010BFD8);

INCLUDE_ASM("asm/nonmatchings/render", func_0010C010);

INCLUDE_ASM("asm/nonmatchings/render", func_0010C0E0);

INCLUDE_ASM("asm/nonmatchings/render", func_0010C110);

void func_0010C158(void) {
    void* fileData = func_00155ED8(0x34, 0xC);
    s32 fileSize = func_00120438("kanji.knj", fileData);
    memcpy(&D_0048ED00, fileData, fileSize);
}

INCLUDE_ASM("asm/nonmatchings/render", func_0010C1A8);

s32 func_0010C1D0(s16 destination, u_long128* source) {
    sceGsSetDefLoadImage(&D_002B8700, destination, 1, SCE_GS_PSMCT32, 0, 0, 8, 2);
    sceGsSyncPath(0, 0);
    FlushCache(WRITEBACK_DCACHE);
    return sceGsExecLoadImage(&D_002B8700, source);
}

s32 func_0010C248(void) {
    if (D_002B9668 < 7) {
        if (D_002B9668 >= 5) {
            return 0;
        }
    }

    sceGsSetDefLoadImage(&D_002B8700, 0x3600, 4, SCE_GS_PSMCT32, 0, 0, 256, 256);
    sceGsSyncPath(0, 0);
    FlushCache(WRITEBACK_DCACHE);
    sceGsExecLoadImage(&D_002B8700, &D_0048ED00);
    return 0;
}
