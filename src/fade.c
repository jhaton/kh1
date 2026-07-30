#include "common.h"

typedef struct {
    /* 0x00 */ char taskHeader[0x10];
    /* 0x10 */ f32 currentTicks;
    /* 0x14 */ f32 intensityPerTick;
    /* 0x18 */ f32 durationTicks;
} FadeTask;

s32 D_002B29CC;
s32 D_002B29D0;

s32 D_002B85C0[4];
s32 D_002B860C;

extern f32 D_002B8340[];
extern u16 D_002B8C70;

f32 D_0048E508;

void func_00106718(void);
void func_00106728(void);
FadeTask* func_0011EEB8(u16*, s32, s32 (*)(FadeTask*));

s32 func_001061D8(void) {
    if (((D_002B85C0[0] == 0) || (D_002B860C == 128)) & 0xFFFF) {
        return 1;
    }
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/fade", func_00106208);

INCLUDE_ASM("asm/nonmatchings/fade", func_00106230);

INCLUDE_ASM("asm/nonmatchings/fade", func_00106380);

INCLUDE_ASM("asm/nonmatchings/fade", func_00106428);

void Fade_ResetIntensity(void) {
    D_0048E508 = 0;
}

s32 FadeTask_UpdateScreenIn(FadeTask* fadeTask) {
    fadeTask->currentTicks += D_002B8340[1];
    if (fadeTask->durationTicks <= fadeTask->currentTicks) {
        D_002B85C0[0] = D_002B85C0[1] = D_002B85C0[2] = D_002B85C0[3] = 128;
        return 4;
    }
    D_002B85C0[0] = D_002B85C0[1] = D_002B85C0[2] = D_002B85C0[3] = fadeTask->intensityPerTick * fadeTask->currentTicks;
    return 0;
}

void Fade_StartScreenIn(s32 duration) {
    FadeTask* fadeTask;
    s32 durationTicks;

    func_00106718();
    func_00106728();
    fadeTask = func_0011EEB8(&D_002B8C70, 0, FadeTask_UpdateScreenIn);
    durationTicks = duration << 1;
    if (duration == 0) {
        duration = 1;
    }
    if (fadeTask != NULL) {
        fadeTask->currentTicks = 0.0f;
        fadeTask->durationTicks = durationTicks;
        fadeTask->intensityPerTick = 128.0f / (duration << 1);
    }
}

s32 FadeTask_UpdateScreenOut(FadeTask* fadeTask) {
    fadeTask->currentTicks -= D_002B8340[1];
    if (fadeTask->currentTicks <= 0) {
        D_002B85C0[0] = D_002B85C0[1] = D_002B85C0[2] = D_002B85C0[3] = 0;
        return 4;
    }
    D_002B85C0[0] = D_002B85C0[1] = D_002B85C0[2] = D_002B85C0[3] = fadeTask->intensityPerTick * fadeTask->currentTicks;
    return 0;
}

void Fade_StartScreenOut(s32 duration) {
    FadeTask* fadeTask = func_0011EEB8(&D_002B8C70, 0, FadeTask_UpdateScreenOut);
    s32 durationTicks = duration << 1;
    if (duration == 0) {
        duration = 1;
    }
    if (fadeTask != NULL) {
        fadeTask->currentTicks = durationTicks;
        fadeTask->intensityPerTick = 128.0f / (duration << 1);
    }
}

void func_00106718(void) {
    D_002B29CC = 0;
}

void func_00106728(void) {
    D_002B29D0 = 0;
}

s32 FadeTask_UpdateOverlayIn(FadeTask* fadeTask) {
    fadeTask->currentTicks += D_002B8340[1];
    if (fadeTask->durationTicks <= fadeTask->currentTicks) {
        D_002B860C = 128;
        return 4;
    }
    D_002B860C = fadeTask->intensityPerTick * fadeTask->currentTicks;
    return 0;
}

void Fade_StartOverlayIn(s32 duration) {
    FadeTask* fadeTask;
    s32 durationTicks;

    func_00106718();
    func_00106728();
    fadeTask = func_0011EEB8(&D_002B8C70, 0, FadeTask_UpdateOverlayIn);
    durationTicks = duration << 1;
    if (duration == 0) {
        duration = 1;
    }
    if (fadeTask != NULL) {
        fadeTask->currentTicks = 0.0f;
        fadeTask->durationTicks = durationTicks;
        fadeTask->intensityPerTick = 128.0f / (duration << 1);
    }
}

s32 FadeTask_UpdateOverlayOut(FadeTask* fadeTask) {
    fadeTask->currentTicks -= D_002B8340[1];
    if (fadeTask->currentTicks <= 0.0f) {
        D_002B860C = 0;
        return 4;
    }
    D_002B860C = fadeTask->intensityPerTick * fadeTask->currentTicks;
    return 0;
}

void Fade_StartOverlayOut(s32 duration) {
    FadeTask* fadeTask;
    f32 maxIntensity;
    s32 durationTicks;

    if (D_002B29CC == 1 || D_002B29D0 == 1) {
        Fade_StartScreenIn(duration);
        func_00106718();
        func_00106728();
        return;
    }

    fadeTask = func_0011EEB8(&D_002B8C70, 0, FadeTask_UpdateOverlayOut);
    durationTicks = duration << 1;
    maxIntensity = 128.0f;
    if (duration == 0) {
        duration = 1;
    }
    if (fadeTask != 0) {
        fadeTask->currentTicks = durationTicks;
        fadeTask->intensityPerTick = maxIntensity / ((f32)(duration << 1));
    }
}
