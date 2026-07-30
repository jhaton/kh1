#include "common.h"
#include "common_funcs.h"
#include "sound_rpc.h"
#include "sound.h"

typedef struct SoundEntry {
    /* 0x00 */ s32 soundId;
    /* 0x04 */ s32 context;
    /* 0x08 */ s32 volume;
    /* 0x0C */ s32 bank;
    /* 0x10 */ s32 channel;
    /* 0x14 */ u8 state;
    /* 0x15 */ u8 unk_15;
    /* 0x16 */ u8 mode;
    /* 0x17 */ char unk_17;
} SoundEntry;

typedef struct XScale {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 unk_04;
} XScale; // size = 0x8

typedef struct XShell {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s32 unk_08;
} XShell; // size = 0xC

extern s32 D_002A0514;
extern s32 D_002BD8B4;
extern s32 D_002BD8B8[2];
extern s32 D_002BD8C8;
extern s32 D_002BD8CC;
extern s32 D_002BD8E0;
extern s32 D_002BD8E4;

const char D_00486E40[] = "%3d";
const char D_00486E48[] = "amusic/music";
const char D_00486E58[] = ".dat";
const char D_00486E60[] = "amusic/se";

void (*D_004DCB70)(void);
extern s32 D_004DCB8C;
extern SoundEntry* D_004DCB90;
extern XShell* D_004DCB94;
extern SoundEntry* D_004DCB98;
extern SoundEntry* D_004DCF58;
extern XScale* D_004DD138;
extern s32 D_004DD180;

SoundEntry* SoundEntry_Allocate(s32, s32, s32, s32, s32);

void func_0011C5D8(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5) {
    func_001EE570(arg0, arg1, arg2, arg3, arg4, arg5);
}

s32 func_0011C5F0(s32 channel, s32 targetVolume, s32 duration) {
    return func_001EE618(channel, targetVolume, duration);
}

void* func_0011C608(s32 channel) {
    return func_001EE5D8(channel);
}

void* func_0011C620(void* buffer) {
    return func_001EE660(buffer);
}

void* func_0011C638(void* buffer) {
    return func_001EE6B8(buffer);
}

s32 func_0011C650(void) {
    return func_001EE090();
}

void* func_0011C668(s32 bank, s32 soundId, s32 channel, s32 leftVolume, s32 rightVolume) {
    return func_001EE330(bank, soundId, channel, leftVolume, rightVolume);
}

s32 func_0011C680(s32 bank, s32 soundId, s32 channel) {
    return func_001EE390(bank, soundId, channel);
}

void* func_0011C698(s32 bank, s32 soundId, s32 channel, s32 arg3, s32 arg4) {
    return func_001EE428(bank, soundId, channel, arg3, arg4);
}

void* func_0011C6B0(s32 bank, s32 soundId, s32 channel, s32 leftVolume, s32 rightVolume) {
    return func_001EE478(bank, soundId, channel, leftVolume, rightVolume);
}

void func_0011C6C8(s32 useAllocatedStorage) {
    XScale* scale;
    SoundEntry* entry;
    XShell* shell;
    s32 i;

    if (useAllocatedStorage == 0) {
        D_004DCB90 = &D_004DCB98;
        D_004DCB94 = &D_004DCF58;
        D_004DCB8C = 0x28;
    } else {
        D_004DCB90 = func_00155ED8(0x10, 1);
        D_004DCB94 = func_00155ED8(0x11, 1);
        D_004DCB8C = 0x80;
    }

    entry = D_004DCB90;
    shell = D_004DCB94;
    for (i = 0; i < D_004DCB8C; i++) {
        entry->state = 0;
        shell->unk_00 = 0;
        shell->unk_04 = 0;
        shell->unk_08 = 0;
        entry++;
        shell++;
    }

    scale = &D_004DD138;
    for (i = 0; i < 4; i++) {
        scale->unk_00 = 0;
        scale->unk_04 = 0;
        scale++;
    }
}

void func_0011C7D0(void) {
    D_004DD180 = 0;
}

void func_0011C7E0(void) {
    D_004DD180 = 1;
}

void func_0011C7F0(void) {
    func_0011C7D0();
    func_0011C6C8(0);
    func_001EE5D8(0);
    func_001EE5D8(1);
    func_001EE390(-100, -1, -1);
    func_001EE1E0(0, 0);
    func_001EE1E0(0, 1);
    func_001EE1E0(1, 0);
    func_001EE1E0(1, 1);
    func_001EE1E0(0, -8);
    func_001EE1E0(3, -8);
    func_001EE228(10);
    func_00113A10(0);
    func_001EEC58();
    func_00101528();
}

INCLUDE_ASM("asm/nonmatchings/sound", func_0011C898);

INCLUDE_ASM("asm/nonmatchings/sound", func_0011C938);

s32 func_0011C9B0(void* buffer) {
    return func_00120438("amusic/se000.dat", buffer); // in cdvd.c
}

s32 func_0011C9D0(void) {
    if (func_001EE090()) {
        return 0;
    }

    if (D_004DCB70 != NULL) {
        D_004DCB70();
    }

    return 4;
}

INCLUDE_ASM("asm/nonmatchings/sound", func_0011CA10);

INCLUDE_ASM("asm/nonmatchings/sound", func_0011CB50);

INCLUDE_ASM("asm/nonmatchings/sound", func_0011CC20);

void SoundEntry_Initialize(s32 soundId, s32 channel, s32 context, s32 volume, s32 mode, SoundEntry* entry, s32 bank) {
    entry->bank = bank;
    entry->soundId = soundId;
    entry->channel = channel;
    entry->context = context;
    entry->volume = volume;
    entry->mode = mode;
    entry->unk_15 = 0;
}

SoundEntry* SoundEntry_Allocate(s32 soundId, s32 channel, s32 context, s32 volume, s32 mode) {
    SoundEntry* entry = D_004DCB90;
    s32 i;

    for (i = 0; i < D_004DCB8C; i++, entry++) {
        s32 bank = (soundId < 1000) ? 2 : -1;

        if (entry->state == 0) {
            SoundEntry_Initialize(soundId, channel, context, volume, mode, entry, bank);
            return entry;
        }
    }

    return NULL;
}


INCLUDE_ASM("asm/nonmatchings/sound", func_0011CE58);

void func_0011CEB0(s32 soundId, s32 channel) {
    s32 bank = soundId < 1000 ? 2 : -8;

    func_0011C668(bank, soundId, channel, 0x3FFF, 0x3FFF);
    func_0011CE58(soundId, channel);
}

s32 func_0011CF08(s32 soundId, s32 channel) {
    s32 bank = soundId < 1000 ? 2 : -8;

    return func_0011C680(bank, soundId, channel);
}

void* func_0011CF38(s32 soundId, s32 channel, s32 arg2) {
    s32 bank = soundId < 1000 ? 2 : -8;

    return func_0011C698(bank, soundId, channel, -1, arg2);
}

void* func_0011CF70(s32 soundId, s32 channel) {
    s32 bank = soundId < 1000 ? 2 : -1;

    return func_0011C668(bank, soundId, channel, 0x3FFF, 0x3FFF);
}

s32 func_0011CFA8(s32 soundId, s32 channel) {
    s32 bank = soundId < 1000 ? 2 : -1;

    return func_0011C680(bank, soundId, channel);
}

void* func_0011CFD8(s32 soundId, s32 channel, s32 arg2) {
    s32 bank = soundId < 1000 ? 2 : -1;

    return func_0011C698(bank, soundId, channel, -1, arg2);
}

void func_0011D010(s32 soundId, s32 channel, s32 context, s32 volume) {
    SoundEntry* entry = SoundEntry_Allocate(soundId, channel, context, volume, 0);

    if (entry != NULL) {
        entry->state = 1;
    }
}

void func_0011D040(s32 soundId, s32 channel, s32 context, s32 volume) {
    SoundEntry* entry = SoundEntry_Allocate(soundId, channel, context, volume, 1);

    if (entry != NULL) {
        entry->state = 2;
    }
}

void func_0011D070(s32 soundId, s32 channel, s32 context, s32 volume) {
    SoundEntry* entry = SoundEntry_Allocate(soundId, channel, context, volume, 2);

    if (entry != NULL) {
        entry->state = 1;
    }
}

void func_0011D0A0(s32 soundId, s32 channel, s32 context, s32 volume) {
    SoundEntry* entry = SoundEntry_Allocate(soundId, channel, context, volume, 3);

    if (entry != NULL) {
        entry->state = 2;
    }
}

void func_0011D0D0(s32 soundId, s32 channel, s32 context, s32 volume) {
    SoundEntry* entry = SoundEntry_Allocate(soundId, channel, context, volume, 4);

    if (entry != NULL) {
        entry->state = 2;
    }
}

INCLUDE_ASM("asm/nonmatchings/sound", func_0011D100);

INCLUDE_ASM("asm/nonmatchings/sound", func_0011D180);

INCLUDE_ASM("asm/nonmatchings/sound", func_0011D1F8);

INCLUDE_ASM("asm/nonmatchings/sound", func_0011D278);

INCLUDE_ASM("asm/nonmatchings/sound", func_0011D310);

INCLUDE_ASM("asm/nonmatchings/sound", func_0011D3B8);

INCLUDE_ASM("asm/nonmatchings/sound", func_0011D420);

INCLUDE_ASM("asm/nonmatchings/sound", func_0011D4B8);

INCLUDE_ASM("asm/nonmatchings/sound", func_0011D520);

void* func_0011D598(s32 soundId) {
    return func_0011C668(2, soundId, 0, 0x3FFF, 0x3FFF);
}

void* func_0011D5C0(s32 soundId, s32 reducedVolume) {
    s32 volume = 0x3FFF;
    if (reducedVolume != 0) {
        volume = 0x1FFF;
    }

    return func_0011C6B0(2, soundId, 0, volume, volume);
}

s32 func_0011D5F0(s32 soundId) {
    return func_0011C680(2, soundId, 0);
}

INCLUDE_ASM("asm/nonmatchings/sound", func_0011D610);

void func_0011D978(s32* leftVolume, s32* rightVolume) {
    s32 clampedRightVolume;

    if (*leftVolume < 0) {
        *leftVolume = 0;
    }
    if (*rightVolume < 0) {
        *rightVolume = 0;
    }
    if (*leftVolume < 0x4000) {
        clampedRightVolume = *rightVolume;
    } else {
        *leftVolume = 0x3FFF;
        clampedRightVolume = *rightVolume;
    }
    if (clampedRightVolume > 0x3FFF) {
        *rightVolume = 0x3FFF;
    }
}

void func_0011D9C8(s32* leftVolume, s32* rightVolume, s32 adjustment, s32 mode) {
    if ((mode == 2) && (D_002A0514 != 0)) {
        *leftVolume = 0;
        *rightVolume = 0;
    } else if (adjustment == 0x3FFF) {
        *leftVolume = 0x3FFF;
        *rightVolume = 0x3FFF;
    } else {
        *leftVolume = *leftVolume + adjustment;
        *rightVolume = *rightVolume + adjustment;
        func_0011D978(leftVolume, rightVolume);
    }
}

INCLUDE_ASM("asm/nonmatchings/sound", func_0011DA50);

INCLUDE_ASM("asm/nonmatchings/sound", func_0011DAC0);

INCLUDE_ASM("asm/nonmatchings/sound", func_0011DB18);

INCLUDE_ASM("asm/nonmatchings/sound", func_0011DBF8);

INCLUDE_ASM("asm/nonmatchings/sound", func_0011DCB8);

INCLUDE_ASM("asm/nonmatchings/sound", func_0011DFF0);

void func_0011E048(s32 arg0) {
    func_001EDD20(arg0);
    func_0011DFF0(arg0);
}

INCLUDE_ASM("asm/nonmatchings/sound", func_0011E070);

s32 func_0011E168(s32 arg0) {
    D_002BD8E4 = func_0011E070(arg0);
    return D_002BD8E4;
}

INCLUDE_ASM("asm/nonmatchings/sound", func_0011E190);

void func_0011E2A8(void) {
    func_0011C5F0(0, 0, 960);
    func_0011C5D8(1, 0, 0x3000, 240, 0, 240);
    D_002BD8B4 = 1;
}

void func_0011E2F8(void) {
    func_0011C5F0(1, 0, 720);
    func_0011C5D8(0, 0, 0x3000, 480, 1, 240);
    D_002BD8B4 = 0;
}

void func_0011E340(void) {
    D_002BD8E0 = 1;
}

void func_0011E350(void) {
    D_002BD8E0 = 0;
}

INCLUDE_ASM("asm/nonmatchings/sound", func_0011E360);

void func_0011E418(void) {
    func_00101528();
}

INCLUDE_ASM("asm/nonmatchings/sound", func_0011E430);

INCLUDE_ASM("asm/nonmatchings/sound", func_0011E520);
void func_0011E520();

INCLUDE_ASM("asm/nonmatchings/sound", func_0011E578);

s32 func_0011E5E8(UNK_TYPE unused0, UNK_TYPE unused1, s32 value) {
    return func_0011CA10(0, value, D_002BD8C8, func_0011E520);
}

INCLUDE_ASM("asm/nonmatchings/sound", func_0011E618);
void func_0011E618();

s32 func_0011E6C0(UNK_TYPE unused0, UNK_TYPE unused1, s32 value) {
    return func_0011CA10(1, value, D_002BD8CC, func_0011E618);
}

INCLUDE_ASM("asm/nonmatchings/sound", func_0011E6F0);

void func_0011E7C8(s32 callback, void* buffer) {
    func_0011E6F0(0, 0x3000, 0, callback, 0, buffer);
}

void func_0011E7F8(s32 arg0, s32 arg1, s32 arg2, void* arg3) {
    func_0011E6F0(1, arg0, arg1, arg2, 0, arg3);
}

void func_0011E830(s32 callback, void* callbackArg) {
    func_0011E6F0(0, 0x3000, 0, callback, 1, callbackArg);
}

INCLUDE_ASM("asm/nonmatchings/sound", func_0011E860);

INCLUDE_ASM("asm/nonmatchings/sound", func_0011E900);

void* func_0011E9A8(s32 slot, s32 arg1, s32 arg2, void* callback) {
    D_002BD8B8[slot] = -1;
    D_004DCB70 = callback;
    func_001EE1E0(1, slot);
    func_001EE518(slot, arg1, arg2);
    return func_0011ED30(10000, func_0011C9D0);
}

INCLUDE_ASM("asm/nonmatchings/sound", func_0011EA28);
