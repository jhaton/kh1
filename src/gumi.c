#include "common.h"
#include "gumi.h"
#include "work_buffers.h"

s32 D_00373790;
s32 D_00373794;
s32 D_00373798;
s32 D_0037379C;
s32 D_00373898;
void* D_0037389C;
s32 D_003738A0;
s32 D_003738A8;

s32 D_00373B00[];
s32 D_00373BC0[];

s32 D_0061BA40;
s32 D_0061BA48[2][2];
s32 D_0061B950;
void* D_0061BA58;

XGumiThing D_0061B960;

extern void func_F20000();
extern void func_001C0370(void*, s32);

s32 func_001BBCD8(s32 index) {
    XGumi* gumi = func_0022F768(4);

    return gumi->unk_9BC0[index];
}

void func_001BBD10(s32 index, s32 value) {
    XGumi* gumi = func_0022F768(4);

    gumi->unk_9BC0[index] = value;
}

void func_001BBD58(double value) {
    asin(value);
}

s32 func_001BBD70(void) {
    if (func_00120640(D_0061B950) != 0) {
        return 0;
    }

    puts("overlay read run");
    FlushCache(0);
    FlushCache(2);
    func_F20000();
    return 4;
}

INCLUDE_ASM("asm/nonmatchings/gumi", func_001BBDD0);

void func_001BBE38(void) {
    XGumi* gumi = func_0022F768(4);

    if (gumi->unk_9B00 == 0) {
        gumi->unk_9B00 = 0x4000;
    }
    if (gumi->unk_9B04 == 0) {
        gumi->unk_9B04 = 0x8000;
    }
    if (gumi->unk_9B08 == 0) {
        gumi->unk_9B08 = 0x400;
    }
    if (gumi->unk_9B0C == 0) {
        gumi->unk_9B0C = 0x2000;
    }
    if (gumi->unk_9B10 == 0) {
        gumi->unk_9B10 = 0x2000;
    }
    if (gumi->unk_9B14 == 0) {
        gumi->unk_9B14 = 0x2000;
    }
    if (gumi->unk_9B18 == 0) {
        gumi->unk_9B18 = 0x1000;
    }
    if (gumi->unk_9B1C == 0) {
        gumi->unk_9B1C = 0x1000;
    }
    if (gumi->unk_9B20 == 0) {
        gumi->unk_9B20 = 0x1000;
    }
}

s32 func_001BBF78(s32 index) {
    XGumi* gumi = func_0022F768(4);
    func_001BBE38();
    return (&gumi->unk_9B00)[index];
}

void func_001BBFC8(s32 index, s32 value) {
    XGumi* gumi = func_0022F768(4);
    (&gumi->unk_9B00)[index] = value;
}

s32 func_001BC010(void) {
    if ((func_001BCC80() & 0x80) == 0) {
        return func_001BC590(49) > 0;
    }
    return FALSE;
}

void func_001BC048(void) {
    XGumi* gumi;
    XGumiBlock* block;

    func_001BD210();
    gumi = (XGumi*)func_0022F768(4);
    func_001C0370(gumi, sizeof(XGumi));

    gumi->unk_0004 = 0x100;
    gumi->unk_0000[0] = 'G';
    gumi->unk_0000[1] = 'U';
    gumi->unk_0000[2] = 'M';
    gumi->unk_0000[3] = 'I';

    func_001BCCA0(gumi);
    func_001BC100(0);

    block = func_001BCB58();
    block->unk_44 = 1;
    block->unk_47 = 1;
    func_001BCBD0(func_001BCB30());
    func_001BCD30(10, func_001BCB88(0));
    func_001BC290(func_001BCB88(0), 0);
}

INCLUDE_ASM("asm/nonmatchings/gumi", func_001BC100);

INCLUDE_ASM("asm/nonmatchings/gumi", func_001BC1E8);

INCLUDE_ASM("asm/nonmatchings/gumi", func_001BC290);

s32 func_001BC368(void) {
    return sizeof(XGumi);
}

u16 func_001BC370(s32 index) {
    return *(u16*)(func_001BD288(index) + 0x14); // todo: struct
}

u16 func_001BC390(s32 index) {
    return *(u16*)(func_001BD288(index) + 0x16); // todo: struct
}

s32 func_001BC3B0(s32 id) {
    s32 result;

    func_001BD1E8();
    result = func_001C0148(id);
    result = func_001F0E98(result);
    func_001F0DD0();
    return result;
}

s32 func_001BC3F0(s32 id) {
    s32 result;

    func_001BD1E8();
    result = func_001C0150(id);
    result = func_001F0E98(result);
    func_001F0DD0();
    return result;
}

INCLUDE_ASM("asm/nonmatchings/gumi", func_001BC430);

INCLUDE_ASM("asm/nonmatchings/gumi", func_001BC480);
// s32 func_001BC480(s32 arg0) {
//     u8 bVar1;
//     XGumiBlock* pXVar2;
//     s32 lVar3;

//     printf(D_00489CE0, arg0);
//     pXVar2 = func_001BCB58();
//     bVar1 = pXVar2->unk_00[arg0];

//     lVar3 = func_001BC5F8(arg0);

//     if (lVar3 > bVar1) {
//         pXVar2->unk_00[arg0]++;
//     }
//     return lVar3 > bVar1;
// }

INCLUDE_ASM("asm/nonmatchings/gumi", func_001BC4F8);

s32 func_001BC578(void) {
    return func_001BD360();
}

u8 func_001BC590(s32 index) {
    XGumiBlock* block = func_001BCB58();
    return block->unk_00[index];
}

void func_001BC5C0(s32 index, s32 value) {
    XGumiBlock* block = func_001BCB58();
    block->unk_00[index] = value;
}

u8 func_001BC5F8(s32 index) {
    return *(u8*)(func_001BD288(index) + 0xB); // todo: struct
}

INCLUDE_ASM("asm/nonmatchings/gumi", func_001BC618);

INCLUDE_ASM("asm/nonmatchings/gumi", func_001BC638);

INCLUDE_ASM("asm/nonmatchings/gumi", func_001BC6B0);

void func_001BC888(void) {
    D_00373790 = 0;
}

void func_001BC898(s32 enabled) {
    D_00373790 = enabled;
    if (enabled != 0) {
        func_001C0370(&D_0061B960, sizeof(XGumiThing));
        D_0061BA40 = 0;
        D_0061BA48[0][0] = 0;
        D_0061BA48[0][1] = 0;
        D_0061BA48[1][0] = 0;
        D_0061BA48[1][1] = 0;
        func_0010FCE0();
    } else {
        func_0010FCD0();
    }
}

void func_001BC8F8(s32 value) {
    D_0061BA40 = value;
}

void func_001BC908(s32 index, s32 firstValue, s32 secondValue) {
    D_0061BA48[index][0] = firstValue;
    D_0061BA48[index][1] = secondValue;
}

void func_001BC928(void) {
    D_00373790 = 0;
    func_001C01C0(0);
}

s32 func_001BC948(void) {
    return func_00112C70();
}

s32 func_001BC960(void) {
    return D_0037379C;
}

void func_001BC970(void) {
    D_003738A8 = 1;
    D_0037379C = 1;
    func_001BC888();
    puts("Run Gumi Edit");
    func_001BBDD0("gb.x");
    func_001BCAA0(func_001BC948);
    D_00373794++;
}

s32 func_001BC9D8(void) {
    return 0;
}

f32 func_001BC9E0(void) {
    return 0.0f;
}

f32 func_001BC9F0(void) {
    return 0.0f;
}

INCLUDE_ASM("asm/nonmatchings/gumi", func_001BCA00);
// void func_001BCA00(void) {
//     D_003738A8 = 1;
//     D_00373898 = func_001BC9D8;
//     D_0037389C = func_001BC9E0;
//     D_003738A0 = func_001BC9F0;
//     D_0037379C = 2;
//     func_001C20B8();
//     func_00233968();
//     func_001BC888();
//     puts(D_00489D10);
//     func_001BBDD0(D_00489D20);
//     D_00373798++;
// }

void func_001BCAA0(void* callback) {
    D_0061BA58 = callback;
}

void* func_001BCAB0(void) {
    return D_0061BA58;
}

INCLUDE_ASM("asm/nonmatchings/gumi", func_001BCAC0);

void func_001BCAE8(void) {
    XGumi* gumi = func_0022F768(4);

    if ((*(s32*)gumi->unk_0000 != 'IMUG') || (gumi->unk_0004 != 0x100)) {
        func_001BC048();
    }
}

s32* func_001BCB30(void) {
    XGumi* gumi;

    func_001BCAE8();
    gumi = func_0022F768(4);
    return &gumi->unk_0008;
}

XGumiBlock* func_001BCB58(void) {
    XGumi* gumi;

    func_001BCAE8();
    gumi = func_0022F768(4);
    return &gumi->unk_9A78;
}

INCLUDE_ASM("asm/nonmatchings/gumi", func_001BCB88);

INCLUDE_ASM("asm/nonmatchings/gumi", func_001BCBD0);

INCLUDE_ASM("asm/nonmatchings/gumi", func_001BCC08);

INCLUDE_ASM("asm/nonmatchings/gumi", func_001BCC58);

INCLUDE_ASM("asm/nonmatchings/gumi", func_001BCC80);

void func_001BCCA0(XGumi* gumi) {
    gumi->unk_9B00 = 0x4000;
    gumi->unk_9B04 = 0x8000;
    gumi->unk_9B08 = 0x400;
    gumi->unk_9B0C = 0x2000;
    gumi->unk_9B10 = 0x2000;
    gumi->unk_9B14 = 0x2000;
    gumi->unk_9B18 = 0x1000;
    gumi->unk_9B1C = 0x1000;
    gumi->unk_9B20 = 0x1000;
    gumi->unk_9B24 = 0;
}

INCLUDE_ASM("asm/nonmatchings/gumi", func_001BCD30);

void func_001BCD98(void) {
    XGumiBlock* block = func_001BCB58();

    block->unk_31 += 1;
    func_001BCAC0(0, 4);
    func_001BCAC0(1, 4);
    func_001BCAC0(2, 4);
    func_001BCAC0(3, 4);
    func_001BCAC0(4, 4);
    func_001BCAC0(5, 4);
    func_001BCAC0(6, 4);
    func_001BCAC0(7, 4);
    func_001BCAC0(8, 4);
    func_001BCAC0(9, 4);
    func_001BCAC0(10, 3);
    func_001BCAC0(11, 2);
    func_001BCAC0(12, 4);
    func_001BCAC0(13, 4);
    func_001BCAC0(14, 4);
    func_001BCAC0(15, 31);
    func_001BCAC0(16, 31);
    func_001BCAC0(17, 31);
    func_001BCAC0(18, 31);
    func_001BCAC0(19, 31);
    func_001BCAC0(20, 31);
    func_001BCAC0(21, 31);
    func_001BCAC0(22, 31);
    func_001BCAC0(23, 31);
    func_001BCAC0(24, 31);
    func_001BCAC0(34, -1);
    func_001BCAC0(35, -1);
    func_001BCAC0(36, -1);
    func_001BCAC0(37, -1);
    func_001BCAC0(38, -1);
}

s32 func_001BCF28(s32 index) {
    XGumi* gumi = func_0022F768(4);

    switch (index) {
        case 0:
            return gumi->unk_9AF8;
        case 1:
            return gumi->unk_9AFC;
        default:
            return 0;
    }
}

INCLUDE_RODATA("asm/nonmatchings/gumi", D_00489D28);

INCLUDE_ASM("asm/nonmatchings/gumi", func_001BCF98);
