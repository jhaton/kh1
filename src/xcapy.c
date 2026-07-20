#include "common.h"

typedef struct XCapy {
    /* 0x00 */ char unk_00[0x08];
    /* 0x08 */ s32 showSecondaryValue;
    /* 0x0C */ s32 digitCount;
    /* 0x10 */ s32 value;
    /* 0x14 */ s32 secondaryValue;
} XCapy;

typedef struct XCapy2 {
    /* 0x00 */ char unk_00[0x1C];
    /* 0x1C */ s32 distanceMeters;
    /* 0x20 */ char unk_20[0x04];
    /* 0x24 */ s32 screenX;
    /* 0x28 */ s32 screenY;
    /* 0x2C */ s32 showHours;
    /* 0x30 */ char unk_30[0x14];
    /* 0x44 */ s32 subseconds;
    /* 0x48 */ s32 seconds;
    /* 0x4C */ s32 minutes;
    /* 0x50 */ s32 hours;
} XCapy2;

void func_00244048(s32, UNK_TYPE*, s32, s32, f32);
void func_00244130(s32, s32, char*, f32, char*, s32);
void func_00244200(void*);
void func_002465E8(s32*, s32);

extern UNK_TYPE D_0042BC20[];

s32 D_004610F8 = 13;
s32 D_004610FC = 11;

UNK_TYPE D_006632D0;
extern s32 D_006633A0[8];
extern s32 D_006633C0[8];
extern s32 D_006633E0[7];

void func_00244490(XCapy2* timerDisplay) {
    char textColor[3];
    char textBuffer[0x10];

    s32 drawX = timerDisplay->screenX;
    s32 drawY;

    if (timerDisplay->showHours != 0) {
        drawX += 0x67;
    } else {
        drawX += 0x5D;
    }

    drawY = timerDisplay->screenY + 0x2B;

    textColor[0] = 0xE5;
    textColor[1] = 0x4E;
    textColor[2] = 0x19;

    if (timerDisplay->showHours != 0) {
        sprintf(textBuffer, "%02d", timerDisplay->hours);
        func_00244130(drawX, drawY, textColor, 0.80000005f, textBuffer, D_004610F8);
        drawX += 22;
        func_00244130(drawX, drawY, textColor, 0.80000005f, ":", D_004610F8);
        drawX += 11;
    }
    sprintf(textBuffer, "%02d", timerDisplay->minutes);
    func_00244130(drawX, drawY, textColor, 0.80000005f, textBuffer, D_004610F8);
    drawX += 22;
    func_00244130(drawX, drawY, textColor, 0.80000005f, "\'", D_004610F8);
    drawX += 11;
    sprintf(textBuffer, "%02d", timerDisplay->seconds);
    func_00244130(drawX, drawY, textColor, 0.80000005f, textBuffer, D_004610F8);
    drawX += 22;
    func_00244130(drawX, drawY, textColor, 0.80000005f, "\'", D_004610F8);
    drawX += 5;
    func_00244130(drawX, drawY, textColor, 0.80000005f, "\'", D_004610F8);
    drawX += 9;
    sprintf(textBuffer, "%02d", timerDisplay->subseconds);
    func_00244130(drawX, drawY, textColor, 0.80000005f, textBuffer, D_004610F8);
}

void func_002446B0(XCapy2* display) {
    s32 spriteIndex;

    for (spriteIndex = 0; spriteIndex < ARRAY_COUNT(D_006633C0); spriteIndex++) {
        func_00244048(D_006633C0[spriteIndex], &D_006632D0, display->screenX, display->screenY, 1.0f);
    }
    func_00244200(display);
    func_00244490(display);
}

void func_00244730(XCapy2* display) {
    s32 spriteIndex;

    for (spriteIndex = 0; spriteIndex < ARRAY_COUNT(D_006633A0); spriteIndex++) {
        func_00244048(D_006633A0[spriteIndex], &D_006632D0, display->screenX, display->screenY, 1.0f);
    }
    func_00244200(display);
    func_00244490(display);
}

void func_002447B0(XCapy2* display) {
    s32 spriteIndex;

    for (spriteIndex = 0; spriteIndex < ARRAY_COUNT(D_006633E0); spriteIndex++) {
        func_00244048(D_006633E0[spriteIndex], &D_006632D0, display->screenX, display->screenY, 1.0f);
    }
    func_00244200(display);
}

void func_00244828(s32 entryIndex) {
    func_002465E8(&D_0042BC20[entryIndex * 1500], 1);
}

void func_00244858(XCapy* numericDisplay) {
    char textBuffer[0x10];
    u8 textColor[3];

    textColor[0] = 0xFF;
    textColor[1] = 0xFF;
    textColor[2] = 0xFF;

    switch (numericDisplay->digitCount) {
        case 1:
            sprintf(textBuffer, "%01d", numericDisplay->value);
            func_00244130(69, 22, textColor, 1.0f, textBuffer, D_004610F8);
            if (numericDisplay->showSecondaryValue != 0) {
                sprintf(textBuffer, "/");
                func_00244130(84, 22, textColor, 1.0f, textBuffer, D_004610F8);
                sprintf(textBuffer, "%01d", numericDisplay->secondaryValue);
                func_00244130(97, 22, textColor, 1.0f, textBuffer, D_004610F8);
            }
            break;
        case 2:
            sprintf(textBuffer, "%02d", numericDisplay->value);
            func_00244130(69, 22, textColor, 1.0f, textBuffer, D_004610F8);
            if (numericDisplay->showSecondaryValue != 0) {
                sprintf(textBuffer, "/");
                func_00244130(97, 22, textColor, 1.0f, textBuffer, D_004610F8);
                sprintf(textBuffer, "%02d", numericDisplay->secondaryValue);
                func_00244130(110, 22, textColor, 1.0f, textBuffer, D_004610F8);
            }
            break;
        case 3:
            sprintf(textBuffer, "%03d", numericDisplay->value);
            func_00244130(69, 22, textColor, 1.0f, textBuffer, D_004610F8);
            if (numericDisplay->showSecondaryValue != 0) {
                sprintf(textBuffer, "/");
                func_00244130(110, 22, textColor, 1.0f, textBuffer, D_004610F8);
                sprintf(textBuffer, "%03d", numericDisplay->secondaryValue);
                func_00244130(123, 22, textColor, 1.0f, textBuffer, D_004610F8);
            }
            break;
        case 4:
            sprintf(textBuffer, "%04d", numericDisplay->value);
            func_00244130(69, 22, textColor, 1.0f, textBuffer, D_004610F8);
            if (numericDisplay->showSecondaryValue != 0) {
                sprintf(textBuffer, "/");
                func_00244130(123, 22, textColor, 1.0f, textBuffer, D_004610F8);
                sprintf(textBuffer, "%04d", numericDisplay->secondaryValue);
                func_00244130(136, 22, textColor, 1.0f, textBuffer, D_004610F8);
            }
            break;
    }
}

void func_00244B70(XCapy2* distanceDisplay) {
    char textBuffer[0x10];
    u8 textColor[3];
    
    s32 textX = 69;

    textColor[0] = 0xFF;
    textColor[1] = 0xFF;
    textColor[2] = 0xFF;

    sprintf(textBuffer, "%d", distanceDisplay->distanceMeters);
    func_00244130(textX, 22, textColor, 1.0f, textBuffer, D_004610F8);
    textX += strlen(textBuffer) * 13;
    sprintf(textBuffer, "m");
    textX += 2;
    func_00244130(textX, 24, textColor, 1.0f, textBuffer, D_004610FC);
}
