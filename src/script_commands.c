#include "common.h"

#include "work_buffers.h"
#include "script_commands.h"
#include "display.h"
#include "graphics.h"
#include "script_exec.h"
#include "camera.h"

extern u32 D_002A2740;
extern s32 D_002A2744;
extern s32 D_002A2748;
extern u32 D_002B29C4;
extern u32 D_002B8000;
extern u32 D_002B8004;
extern u32 D_002B8008;
extern u32 D_002B8014;
extern f32 D_002B8348;
extern u32 D_002B834C;
extern s32 D_002B8678;
extern s32 D_002B8680;
extern s32 D_002B8684;
extern u32 D_002B916C;
extern u32 D_002B9170;
extern u32 D_002B9338;
extern u32 D_002B9660;
extern s32 D_002C2104;
extern s32 D_002C2108;
extern s32 D_002C2024;
extern u16 D_002C1EAA;
extern u32 D_00375BC0;
extern ScriptRuntime D_003854E0;
extern u32 D_00388860;
extern u32 D_003889B0;

// Sora/Riku score on Destiny Island
extern s32 D_003C10B8[];

extern XHumungusFunc D_003C10D8[];
extern XHumungusFunc D_003D4A20[];
extern s32* D_003D55DC;

extern u32 D_003F0DD4;
extern s32 D_0041F8BC;

extern char D_0048A3A0[];

extern UNK_PTR D_00624550;
extern UNK_PTR D_00624554;
extern UNK_PTR D_00624658;
extern UNK_PTR D_00624958;
extern UNK_PTR D_00624960;
extern UNK_PTR D_00624970;

extern void func_F20000();

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_LoadEventCameraPreset);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001C8950);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_LoadObjectYawRelativeEventCameraPreset);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001C9380);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetCameraPosition);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetCameraTargetPosition);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetCameraRotationDegrees);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetCameraDistance);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetCameraFovDegrees);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001C9BE0);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001C9C30);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001C9C80);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001C9CD0);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001C9D40);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001C9DB0);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001C9E20);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001C9E70);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001C9EC0);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_StartCameraTargetPositionTransition);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CA090);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_StartCameraRotationTransition);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CA638);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_StartCameraDistanceTransition);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CA7A0);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_StartCameraFovTransition);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CA908);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_StartCameraShake);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_WaitForEventCamera);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CADE0);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CB0E0);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CB1A8);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CB4A0);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CB5C0);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CB7B8);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CB850);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CB8C8);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CB938);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CB998);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CBB90);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_ConfigureCameraOrbit);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_TransitionCameraOrbitOverDuration);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CC130);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_GetCameraPositionY);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CC240);

s32 ScriptCommand_EnablePlayerRelativeCameraYaw(void) {
    if (!(D_002C1EA8 >> 1 & 1)) {
        func_00180038();
    }
    return 2;
}

s32 ScriptCommand_DisablePlayerRelativeCameraYaw(void) {
    if (!(D_002C1EA8 >> 1 & 1)) {
        func_00180080();
    }
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CC338);

void func_001CC520(void) {
    D_003854E0.unk_00 = 0;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CC530);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CC558);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CC588);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CC618);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CC680);

void func_001CC798(s32 value) {
    D_003854E0.unk_3070 &= ~0x100000000;
    D_003854E0.unk_3140 = value;
    *D_003854E0.unk_3144 = value;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CC7D0);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_WaitForTextWindowChoice);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CC950);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_CloseTextWindow);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CCA88);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CCB18);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetTextWindowItemName);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CCEF8);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CCF78);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CD2A8);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CD340);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetTextWindowText);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CD620);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_OpenTextWindow);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CD8E8);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CDA00);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_ReopenTextWindow);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetTextWindowPointerStyle);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetTextWindowPointerPosition);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetTextWindowPointerAngle);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetTextWindowPosition);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetTextWindowSize);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetTextWindowOrientation);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetTextWindowOpenTransition);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetTextWindowCloseTransition);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetTextWindowCharacterDelay);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_WaitForTextWindowInput);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_WaitForTextWindow);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CE0A0);

s32 func_001CE188(Script* script) {
    script->stackTop--;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_ConfigureConversationCameraAndTextWindows);

s32 ScriptCommand_ClearConversationCamera(void) {
    func_0017FED8();
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_FitTextWindowToText);

// Handle score against Riku on Destiny Island
s32 func_001CE858(Script* script) {
    s32 stackTop = script->stackTop;

    D_003C10B8[script->valueStack[stackTop - 1]] = script->valueStack[stackTop];
    script->stackTop -= 2;
    return 2;
}

void func_001CE898(Script* script) {
    script->stackTop--;
    script->valueStack[script->stackTop] += script->valueStack[script->stackTop + 1];
}

void func_001CE8D0(Script* script) {
    script->stackTop--;
    script->valueStack[script->stackTop] -= script->valueStack[script->stackTop + 1];
}

void func_001CE908(Script* script) {
    script->valueStack[script->stackTop] = -script->valueStack[script->stackTop];
}

void func_001CE928(Script* script) {
    script->stackTop--;
    script->valueStack[script->stackTop] *= script->valueStack[script->stackTop + 1];
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CE960);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CE9A0);

void func_001CE9E0(Script* script) {
    script->stackTop--;
    script->valueStack[script->stackTop] = script->valueStack[script->stackTop] == script->valueStack[script->stackTop + 1];
}

void func_001CEA18(Script* script) {
    script->stackTop--;
    script->valueStack[script->stackTop] = script->valueStack[script->stackTop + 1] < script->valueStack[script->stackTop];
}

void func_001CEA50(Script* script) {
    script->stackTop--;
    script->valueStack[script->stackTop] = script->valueStack[script->stackTop] < script->valueStack[script->stackTop + 1] ^ 1;
}

void func_001CEA88(Script* script) {
    script->stackTop--;
    script->valueStack[script->stackTop] = script->valueStack[script->stackTop] < script->valueStack[script->stackTop + 1];
}

void func_001CEAC0(Script* script) {
    script->stackTop--;
    script->valueStack[script->stackTop] = script->valueStack[script->stackTop + 1] < script->valueStack[script->stackTop] ^ 1;
}

void func_001CEAF8(Script* script) {
    script->stackTop--;
    script->valueStack[script->stackTop] = script->valueStack[script->stackTop] != script->valueStack[script->stackTop + 1];
}

void func_001CEB30(Script* script) {
    script->stackTop--;
    script->valueStack[script->stackTop] &= script->valueStack[script->stackTop + 1];
}

void func_001CEB68(Script* script) {
    script->stackTop--;
    script->valueStack[script->stackTop] |= script->valueStack[script->stackTop + 1];
}

void func_001CEBA0(Script* script) {
    script->stackTop--;
    script->valueStack[script->stackTop] ^= script->valueStack[script->stackTop + 1];
}

void func_001CEBD8(Script* script) {
    script->valueStack[script->stackTop] = ~script->valueStack[script->stackTop];
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CEBF8);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CEC30);

s32 func_001CEC68(UNK_PTR context, s32 functionIndex) {
    D_003C10D8[functionIndex](context);
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CEC98);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_WaitTicks);

s32 ScriptCommand_DiscardValue(Script* script) {
    script->stackTop--;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_TurnObjectTowardPosition);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetObjectRotation);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetObjectAnimation);

s32 func_001CF1B0(void) {
    return 2;
}

s32 ScriptCommand_BeginEventMode(void) {
    D_00375BC0 |= 2;
    func_00133538();
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_EnableEventCamera);

s32 func_001CF250(void) {
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_FinishEventMode);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_DisableEventCamera);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_GetSelectedObjectPositionX);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_GetSelectedObjectPositionY);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_GetSelectedObjectPositionZ);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetObjectPosition);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_WaitForObjectAction);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_ShowObject);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_HideObject);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_ShowSelectedObjectShadow);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_HideSelectedObjectShadow);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_EnableSelectedObjectCollision);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_DisableSelectedObjectCollision);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CF638);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_EnableSelectedObjectMotion);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_DisableSelectedObjectMotion);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_FadeScreenIn);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_FadeScreenOut);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_FadeOverlayOut);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_FadeOverlayIn);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CF8A8);

s32 func_001CF918(void) {
    if (D_002B834C == 1) {
        D_002B834C = 4;
    }
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_ResetDisplayAndSoundModes);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetSelectedObjectMotionBlend);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_TransitionToWorldRoomEventDefaultSpawn);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CFA90);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_TransitionToWorldRoomEvent);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_TransitionToRoomSelection);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CFC18);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CFCB0);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CFD08);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_EnableMapGroup);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_DisableMapGroup);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CFDC8);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CFE00);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CFE30);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CFE70);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_RandomIntegerUpTo);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001CFF30);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_TurnSelectedObjectToAngle);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D01D8);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D0308);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D0438);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D04C0);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetInterpolationValueType);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D0590);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D0600);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D0670);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_TransitionInterpolationValueColor);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D0828);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D0870);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_ClearInterpolationValue);

s32 ScriptCommand_ShowCinematicBarsSlowly(void) {
    func_00104200(16);
    D_003854E0.unk_3078 |= 1;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D09A8);

s32 ScriptCommand_HideCinematicBarsSlowly(void) {
    func_00104310(16);
    D_003854E0.unk_3078 &= ~1;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D0A40);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetSelectedObjectMotionFrame);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_ToggleSelectedObjectMotionPause);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_TransitionSelectedObjectColor);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_TransitionSelectedObjectToOpaqueBaseColor);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_RestoreSelectedObjectDefaultColor);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D0DE0);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D0E78);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_LoadEventResource);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D1068);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_WaitForEventResourceLoad);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_ReplaceSelectedObjectMotionResource);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_RestoreSelectedObjectMotionResource);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetSelectedObjectPartMaskBits);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_ClearSelectedObjectPartMaskBits);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_WaitForSelectedObjectTurn);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_TurnSelectedObjectTowardPosition);

s32 ScriptCommand_CaptureScreen(void) {
    func_00109B00();
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_FadeCapturedScreen);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_Noop_Cmd103);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_EnableSelectedObjectBackgroundCollision);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_DisableSelectedObjectBackgroundCollision);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetObjectAnimationWithFlag0x10000);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetObjectPartScale);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_StopSelectedObjectMovementAndSetAnimation);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetSelectedObjectAnimationAtFrame);

s32 ScriptCommand_ShowPartyStatusUi(void) {
    D_002C1EA8 &= ~8;
    return 2;
}

s32 ScriptCommand_HidePartyStatusUi(void) {
    D_002C1EA8 |= 8;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_EnablePlayerControl);

s32 ScriptCommand_DisablePlayerControl(void) {
    D_002C1EA8 |= 4;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D19C8);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D1A20);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D1A98);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D1B10);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D1BD8);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D1C10);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D1C48);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D1C68);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_TransitionSelectedObjectScale);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_ConfigureSelectedObjectMotionPart);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D1E20);

s32 ScriptCommand_GetCurrentWorldIndex(Script* script) {
    s32 value = D_002B8678;
    script->stackTop++;
    script->valueStack[script->stackTop] = value;
    return 2;
}

s32 ScriptCommand_GetCurrentRoomIndex(Script* script) {
    s32 value = D_002B8680;
    script->stackTop++;
    script->valueStack[script->stackTop] = value;
    return 2;
}

s32 ScriptCommand_GetCurrentEntranceIndex(Script* script) {
    s32 value = D_002B8684;
    script->stackTop++;
    script->valueStack[script->stackTop] = value;
    return 2;
}

s32 ScriptCommand_ShowMapParts(Script* script) {
    func_001055B8((long)script->valueStack[script->stackTop], 0);
    script->stackTop--;
    return 2;
}

s32 ScriptCommand_HideMapParts(Script* script) {
    func_001055B8((long)script->valueStack[script->stackTop], 1);
    script->stackTop--;
    return 2;
}

s32 ScriptCommand_SetWorldRoomState(Script* script) {
    func_00111408(
        script->valueStack[script->stackTop - 2], script->valueStack[script->stackTop - 1], script->valueStack[script->stackTop]
    );
    script->stackTop -= 3;
    return 2;
}

s32 func_001D2040(Script* script) {
    func_00106380(
        script->valueStack[script->stackTop - 3], script->valueStack[script->stackTop - 2], script->valueStack[script->stackTop - 1],
        script->valueStack[script->stackTop]
    );
    script->stackTop -= 4;
    return 2;
}

s32 func_001D20A0(Script* script) {
    func_00106428(
        script->valueStack[script->stackTop - 3], script->valueStack[script->stackTop - 2], script->valueStack[script->stackTop - 1],
        script->valueStack[script->stackTop]
    );
    script->stackTop -= 4;
    return 2;
}

s32 func_001D2100(void) {
    Fade_ResetIntensity();
    return 2;
}

s32 func_001D2120(Script* script) {
    WorkBuffers_Get(1);
    script->stackTop--;
    return 2;
}

s32 func_001D2158(Script* script) {
    WorkBuffers_Get(1);
    script->stackTop--;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D2190);

s32 ScriptCommand_ShowCinematicBars(void) {
    func_00104200(1);
    D_003854E0.unk_3078 |= 1;
    return 2;
}

s32 ScriptCommand_HideCinematicBars(void) {
    func_00104310(1);
    D_003854E0.unk_3078 &= ~1;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_EnableSelectedObjectColorOverride);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_EnablePartyColorOverride);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_DisableSelectedObjectColorOverride);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_DisablePartyColorOverride);

void func_001D2440(void) {
    func_F20000(0, 0);
}

s32 func_001D2460(void) {
    cdvd_QueueFileLoad(D_0048A3A0, func_F20000, func_001D2440, NULL);
    return 2;
}

s32 func_001D2498(void) {
    D_0041F8BC = 0;
    return 2;
}

s32 func_001D24A8(Script* script) {
    script->stackTop++;
    script->valueStack[script->stackTop] = func_00125BB8(1);
    return 2;
}

s32 func_001D24F0(Script* script) {
    script->stackTop++;
    script->valueStack[script->stackTop] = func_00125BB8(2);
    return 2;
}

s32 func_001D2538(Script* script) {
    script->stackTop++;
    script->valueStack[script->stackTop] = func_00125BB8(4);
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_IsPlayerControllable);

s32 func_001D25D8(void) {
    D_003854E0.unk_3070 |= 0x10000000000;
    return 8;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_EnableAllObjectControl);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_DisableAllObjectControl);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SelectedObjectExists);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D2AF0);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_MoveSelectedObjectToPositionByStepDistance);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_MoveSelectedObjectToPositionAtTwiceSpeed);

s32 func_001D2DB8(Script* script) {
    D_003854E0.unk_3148 |= script->valueStack[script->stackTop];
    script->stackTop--;
    return 2;
}

s32 func_001D2DF0(Script* script) {
    D_003854E0.unk_3148 &= ~script->valueStack[script->stackTop];
    script->stackTop--;
    return 2;
}

s32 func_001D2E30(void) {
    D_002C1EA8 |= 0x10;
    return 2;
}

s32 func_001D2E50(void) {
    D_002C1EA8 &= ~0x10;
    return 2;
}

s32 func_001D2E70(void) {
    D_002A2740 = 1;
    return 2;
}

s32 func_001D2E88(void) {
    D_002A2740 = 0;
    return 2;
}

s32 func_001D2E98(Script* script) {
    D_002A2744 = script->valueStack[script->stackTop];
    script->stackTop--;
    return 2;
}

s32 func_001D2EC0(Script* script) {
    D_002A2748 = script->valueStack[script->stackTop];
    script->stackTop--;
    return 2;
}

// INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D2EE8);
s32 func_001D2EE8(Script* script) {
    script->stackTop++;
    script->valueStack[script->stackTop] = D_002C2104;
    return 2;
}

s32 func_001D2F10(Script* script) {
    script->stackTop++;
    script->valueStack[script->stackTop] = D_002C2024;
    return 2;
}

s32 ScriptCommand_GetPressedInputMask(Script* script) {
    script->stackTop++;
    script->valueStack[script->stackTop] = D_002C2108;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D2F60);

void func_001D3080(void) {
    D_003854E0.unk_3070 &= ~0x200000000000;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_PreloadObjectResources);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_WaitForObjectResources);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SpawnAndSelectObject);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D3198);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D3200);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D3240);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_DistanceBetweenPointsXz);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D33B8);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D3498);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D3570);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D3668);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_StoreDistanceBetweenObjects);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_DistanceBetweenObjects);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D3960);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetSelectedObjectProximityRadiusMode1);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetSelectedObjectProximityRadiusMode2);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetSelectedObjectProximityRadius);

s32 func_001D3AF0(void) {
    return 2;
}

s32 ScriptCommand_Noop_Cmd201(void) {
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D3B00);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_GetObjectRelativeFacingAngle);

s32 func_001D3EF0(Script* script) {
    func_001E1F78(script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

s32 func_001D3F38(Script* script) {
    func_001E22B0(script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

s32 func_001D3F80(Script* script) {
    func_001E22F0(script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

s32 func_001D3FC8(Script* script) {
    func_001E2628(script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D4010);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D4060);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D40B0);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetObjectAnimationSpeed);

s32 func_001D4190(Script* script) {
    script->stackTop--;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D41A8);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D41F0);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D42E8);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D43E0);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_TurnScriptObjectTowardPosition);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D45E0);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_StopScriptObjectTurning);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D46F0);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D4A30);

s32 func_001D4A98(void) {
    D_003854E0.unk_3070 &= ~0x800000000000;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D4AD0);

s32 func_001D4B38(void) {
    D_003854E0.unk_3070 &= ~0x1000000000000;
    return 2;
}

s32 ScriptCommand_StartCountUpTimer(Script* script) {
    func_00111A00((long)script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D4BB8);
void func_001D4BB8(); // temp for next function

s32 func_001D4C30(Script* script) {
    func_00111B70(script->valueStack[script->stackTop], func_001D4BB8);
    script->stackTop--;
    return 2;
}

s32 ScriptCommand_PauseTimer(void) {
    func_00111BD8();
    return 2;
}

s32 ScriptCommand_ResumeTimer(void) {
    func_00111BF0();
    return 2;
}

s32 ScriptCommand_StopTimerAndGetFrames(Script* script) {
    s32 value = func_00111C08();
    script->stackTop++;
    script->valueStack[script->stackTop] = value;
    return 2;
}

s32 ScriptCommand_SetTimerSeconds(Script* script) {
    func_00111C20(script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

s32 func_001D4D48(Script* script) {
    func_00111C38(script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

s32 func_001D4D90(Script* script) {
    func_00111C60(script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

s32 func_001D4DD8(Script* script) {
    func_00111C50(script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D4E20);

s32 func_001D4E88(void) {
    D_003854E0.unk_3070 &= ~0x2000000000000;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D4EC0);

s32 func_001D4EE8(void) {
    D_00375BC0 &= ~8;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D4F08);

s32 func_001D4FB0(Script* script) {
    func_001E86A0(script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

s32 ScriptCommand_SendObjectMessage(Script* script) {
    s32 value = func_00123880(script->valueStack[script->stackTop - 1]);
    func_00137230(value, 0, script->valueStack[script->stackTop]);
    script->stackTop -= 2;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_GetTotalItemCount);

s32 ScriptCommand_GetStockItemCount(Script* script) {
    script->valueStack[script->stackTop] = D_003010F8->unk_492[script->valueStack[script->stackTop] + 7];
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_AdjustStockItemCount);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D51D0);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D5248);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D52B0);

s32 func_001D5328(void) {
    func_00178140(0);
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D5348);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_DisableSelectedObjectViewCulling);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_ReapplySelectedObjectPosition);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D5420);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D5480);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetSelectedObjectHp);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_GetSelectedObjectHp);

s32 ScriptCommand_AddPartyCharacter(Script* script) {
    Party_AddCharacter(script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

s32 ScriptCommand_RemovePartyCharacter(Script* script) {
    func_0013C7D0(script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D55B8);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D5608);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D5658);

s32 func_001D56A8(void) {
    func_00112A50();
    D_003854E0.unk_3070 |= 0x10000000000;
    return 8;
}

s32 func_001D56E8(void) {
    func_0011C540();
    return 2;
}

s32 ScriptCommand_SnapshotActiveWorkBuffers(void) {
    func_00111198();
    func_0022F808();
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D5730);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_MoveSelectedObjectToPositionAtSpeed);

s32 ScriptCommand_GetPartyMemberCharacterIndex(Script* script) {
    s32 index = script->valueStack[script->stackTop];
    if (index < 4) {
        script->valueStack[script->stackTop] = D_003010F8->unk_48E[index];
    }
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_GetObjectPositionX);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_GetObjectPositionY);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_GetObjectPositionZ);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_GetObjectFacingAngleDegrees);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D5C00);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_GetObjectAnimationFrame);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D5CE8);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D5D58);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_GetPartyMemberPositionX);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_GetPartyMemberPositionY);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_GetPartyMemberPositionZ);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D5E88);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D5F20);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D5FA0);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_StopScriptObjectMovement);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D60A8);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetSecondaryRenderYRotationDegrees);

s32 ScriptCommand_GetPauseMenuSelection(Script* script) {
    script->stackTop++;
    script->valueStack[script->stackTop] = D_003854E0.pauseMenuSelection;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetSelectedObjectInitialPosition);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SinDegreesTimes10000);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_CosDegreesTimes10000);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D6680);

s32 ScriptCommand_IsBattleModeActive(Script* script) {
    s32 value = D_002C1EA8 & 1;

    script->stackTop++;
    script->valueStack[script->stackTop] = value;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D6728);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_GetDamageSourceObjectId);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_EnableAllObjectShadowsAndCollision);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D6828);

s32 ScriptCommand_FadeBlackOverlayOut(Script* script) {
    func_001038B0(script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

s32 ScriptCommand_FadeBlackOverlayIn(Script* script) {
    func_001037C8(script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_DespawnObject);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_EnableScriptObjectTargeting);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_DisableScriptObjectTargeting);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D6B08);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D6B30);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_GetSelectedObjectMotionId);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_GetObjectMotionId);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D6C08);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D6C58);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D6CA8);

s32 func_001D6CD8(void) {
    func_0012C990(0, 0);
    return 2;
}

s32 func_001D6D00(void) {
    func_0012C990(0, 1);
    return 2;
}

s32 func_001D6D28(void) {
    func_0012C990(1, 0);
    return 2;
}

s32 func_001D6D50(void) {
    func_0012C990(1, 1);
    return 2;
}

s32 func_001D6D78(void) {
    func_0012C990(2, 0);
    return 2;
}

s32 func_001D6DA0(void) {
    func_0012C990(2, 1);
    return 2;
}

s32 func_001D6DC8(void) {
    func_0012C990(6, 0);
    return 2;
}

s32 func_001D6DF0(void) {
    func_0012C990(6, 1);
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D6E18);

s32 func_001D6EA8(Script* script) {
    s32 value = D_002DED08;

    script->stackTop = script->stackTop + 1;
    if (value == 2) {
        script->valueStack[script->stackTop] = 1;
    } else {
        script->valueStack[script->stackTop] = 0;
    }
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_EnableScriptObjectPush);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_DisableScriptObjectPush);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetTreasureOpenedFlag);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_GetTreasureOpenedFlag);

s32 func_001D7028(Script* script) {
    script->stackTop++;
    script->valueStack[script->stackTop] = D_003854E0.unk_315C;
    return 2;
}

s32 func_001D7050(Script* script) {
    script->stackTop++;
    script->valueStack[script->stackTop] = D_003854E0.unk_3160;
    return 2;
}

s32 func_001D7078(Script* script) {
    script->stackTop++;
    script->valueStack[script->stackTop] = D_003854E0.unk_3164;
    return 2;
}

s32 func_001D70A0(Script* script) {
    script->stackTop++;
    script->valueStack[script->stackTop] = D_003854E0.unk_3168;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D70C8);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D7178);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D71A0);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D7250);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetPartyMembers);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D7358);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetGameSpeedPercent);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D7730);

s32 ScriptCommand_GetEventSourceObjectId(Script* script) {
    script->stackTop++;
    script->valueStack[script->stackTop] = D_003854E0.eventSourceObjectId;
    return 2;
}

s32 ScriptCommand_GetEventSourceMessageCode(Script* script) {
    script->stackTop++;
    script->valueStack[script->stackTop] = D_003854E0.eventSourceMessageCode;
    return 2;
}

s32 ScriptCommand_SetEventSourceContext(Script* script) {
    D_003854E0.eventSourceObjectId = script->valueStack[script->stackTop - 1];
    D_003854E0.eventSourceMessageCode = script->valueStack[script->stackTop];
    script->stackTop -= 2;
    return 2;
}

s32 func_001D7808(Script* script) {
    func_0014EE50(script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

s32 ScriptCommand_EnableSpawnGroup(Script* script) {
    func_0014EE78(script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

s32 ScriptCommand_DisableAllSpawnGroups(void) {
    int i = 1;
    int j;

    while (i < 0x20) {
        j = i + 1;
        func_0014EE50(i);
        i = j;
    }
    return 2;
}

s32 ScriptCommand_EnableAllSpawnGroups(void) {
    int i = 1;
    int j;

    while (i < 0x20) {
        j = i + 1;
        func_0014EE78(i);
        i = j;
    }
    return 2;
}

s32 ScriptCommand_SetTextObjectId(Script* script) {
    D_002B8000 = script->valueStack[script->stackTop];
    script->stackTop--;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_GetPartyMemberCount);

s32 func_001D7998(Script* script) {
    func_001452E0(0, script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

s32 func_001D79E0(Script* script) {
    func_001452E0(1, script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

s32 func_001D7A28(void) {
    func_00145398();
    return 2;
}

s32 func_001D7A48(void) {
    func_00112B60();
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D7A68);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D7B60);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D7C00);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D7D10);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D7D70);

s32 ScriptCommand_ApplyCurrentWorldProgressionRoomStates(Script* script) {
    func_00111580(script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

s32 func_001D7E18(void) {
    func_00249F88();
    return 2;
}

s32 func_001D7E38(void) {
    if (func_0024A000() == 0) {
        return 4;
    }
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D7E68);

s32 func_001D7EC8(void) {
    func_00104178();
    return 2;
}

s32 func_001D7EE8(void) {
    func_001009A8();
    return 2;
}

s32 func_001D7F08(void) {
    func_001009D8();
    return 2;
}

s32 func_001D7F28(Script* script) {
    func_001807B0(script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

s32 func_001D7F70(void) {
    func_00180848();
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D7F90);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D7FD8);

s32 func_001D8020(Script* script) {
    func_0023E0B0(script->valueStack[script->stackTop], 1);
    script->stackTop--;
    return 2;
}

s32 ScriptCommand_HidePartyStatusComponent(Script* script) {
    func_0023E0B0(script->valueStack[script->stackTop], 3);
    script->stackTop--;
    return 2;
}

s32 func_001D80B0(Script* script) {
    func_0023E0B0(script->valueStack[script->stackTop], 1);
    script->stackTop--;
    return 2;
}

s32 func_001D80F8(Script* script) {
    func_0023E0B0(script->valueStack[script->stackTop], 0);
    script->stackTop--;
    return 2;
}

s32 func_001D8140(void) {
    D_002C1EA8 &= ~0x8000000;
    return 2;
}

s32 func_001D8168(void) {
    D_002C1EA8 |= 0x8000000;
    return 2;
}

s32 func_001D8188(Script* script) {
    script->valueStack[script->stackTop] = func_00105AE0((long)script->valueStack[script->stackTop]);
    return 2;
}

s32 func_001D81E0(Script* script) {
    func_00105B38(script->valueStack[script->stackTop - 1], script->valueStack[script->stackTop]);
    script->stackTop -= 2;
    return 2;
}

s32 ScriptCommand_EnableWorkBufferSnapshot(void) {
    func_001116A8();
    return 2;
}

s32 ScriptCommand_DisableWorkBufferSnapshot(void) {
    func_001116B8();
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D8270);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D8380);

s32 ScriptCommand_DisableInputSuppression(void) {
    D_002B9660 = 0;
    return 2;
}

s32 ScriptCommand_EnableInputSuppression(void) {
    D_002B9660 = 1;
    return 2;
}

s32 ScriptCommand_EnableObjectScriptEvents(void) {
    D_003854E0.unk_3070 |= 0x800000000000000;
    return 2;
}

s32 ScriptCommand_DisableObjectScriptEvents(void) {
    D_003854E0.unk_3070 &= ~0x800000000000000;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D8490);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D8500);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D8570);

s32 func_001D85C0(void) {
    D_002C1EA8 &= ~0x800;
    return 2;
}

s32 func_001D85E0(void) {
    D_002C1EA8 |= 0x800;
    return 2;
}

s32 func_001D8600(void) {
    D_002C1EA8 &= ~0x200;
    return 2;
}

s32 func_001D8620(void) {
    D_002C1EA8 |= 0x200;
    return 2;
}

s32 ScriptCommand_EnableObjectMessages(void) {
    D_002C1EA8 &= ~0x400;
    return 2;
}

s32 ScriptCommand_DisableObjectMessages(void) {
    D_002C1EA8 |= 0x400;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D8680);

s32 ScriptCommand_IsPlayerInDefaultActionState(Script* script) {
    script->stackTop = script->stackTop + 1;
    script->valueStack[script->stackTop] = func_00125B90(1);
    return 2;
}

s32 func_001D8748(Script* script) {
    script->stackTop++;
    script->valueStack[script->stackTop] = D_003854E0.unk_3380;
    return 2;
}

s32 func_001D8770(void) {
    D_00388860 = 0;
    return 2;
}

s32 ScriptCommand_SuppressConditionalModelPartVariants(void) {
    D_002C1EA8 |= 0x1000;
    return 2;
}

s32 ScriptCommand_AllowConditionalModelPartVariants(void) {
    D_002C1EA8 &= ~0x1000;
    return 2;
}

s32 func_001D87C0(Script* script) {
    func_001F0A90(script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

s32 ScriptCommand_QueueWorldImageLoad(Script* script) {
    func_001128F0(script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D8850);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D8928);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D8A00);

s32 func_001D8A30(Script* script) {
    script->stackTop--;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D8A48);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_RefreshSelectedObjectBackgroundColor);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D8AF0);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D8B30);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_RegisterObjectMessageMapping);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D8CD0);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_GetObjectMessageCode);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_GetRemainingSpawnGroupObjectCount);

void func_001D8DB8(void) {
    D_003854E0.unk_3070 &= ~0x2000000000000000;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_LoadCharacterModelVariantAsync);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetWorldFlag);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_WaitForCharacterModelVariantLoad);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D8F00);

void func_001D8F80(void) {
    D_003854E0.unk_3070 &= ~0x4000000000000000;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_DestroySpawnGroupObjectsAsync);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_WaitForSpawnGroupObjectDestruction);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D9058);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D90B0);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D93B8);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D9720);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D9C28);

s32 func_001D9DD0(Script* script) {
    func_00156BA0(script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

s32 func_001D9E18(Script* script) {
    func_00156B70(script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

s32 func_001D9E60(void) {
    D_002C1EA8 &= ~0x8000;
    return 2;
}

s32 func_001D9E88(void) {
    D_002C1EA8 |= 0x8000;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D9EA8);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_InitializeAllBattleSystems);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001D9F60);

s32 func_001D9F88(Script* script) {
    script->valueStack[script->stackTop] = func_00158C30(script->valueStack[script->stackTop]);
    return 2;
}

s32 func_001D9FE0(void) {
    D_003854E0.unk_3078 |= 1;
    return 2;
}

s32 func_001DA000(void) {
    D_003854E0.unk_3078 &= ~1;
    D_00375BC0 &= ~1;
    D_002B8348 = 1.0;
    D_002C1EA8 &= ~0x400;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DA060);

s32 func_001DA0F0(Script* script) {
    func_0013C1E0(script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

s32 func_001DA138(Script* script) {
    func_0013C1F0(script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

s32 func_001DA180(Script* script) {
    func_0013C148(script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

s32 func_001DA1C8(Script* script) {
    if (script->valueStack[script->stackTop] != 0) {
        func_0012C990(3, 1);
    } else {
        func_0012C990(3, 0);
    }
    script->stackTop--;
    return 2;
}

s32 func_001DA228(Script* script) {
    if (script->valueStack[script->stackTop] != 0) {
        func_0012C990(4, 1);
    } else {
        func_0012C990(4, 0);
    }
    script->stackTop--;
    return 2;
}

s32 func_001DA288(Script* script) {
    if (script->valueStack[script->stackTop] != 0) {
        func_0012C990(5, 1);
    } else {
        func_0012C990(5, 0);
    }
    script->stackTop--;
    return 2;
}

s32 ScriptCommand_SetCharacterEquippedWeaponItemId(Script* script) {
    func_0014A070(script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

s32 ScriptCommand_EnablePauseMenuOverride(void) {
    func_00114400();
    return 2;
}

s32 ScriptCommand_DisablePauseMenuOverride(void) {
    func_00114410();
    return 2;
}

s32 func_001DA370(void) {
    func_00112AD8();
    return 2;
}

s32 func_001DA390(void) {
    if (func_00110EA8() == 2) {
        return 2;
    }
    return 4;
}

s32 func_001DA3C0(Script* script) {
    func_00110E38(script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

s32 func_001DA408(void) {
    func_00110E78();
    return 2;
}

s32 func_001DA428(void) {
    if (func_00110EA8() != 4) {
        return 4;
    }
    return 2;
}

s32 func_001DA458(Script* script) {
    func_001114B8(script->valueStack[script->stackTop - 1], script->valueStack[script->stackTop]);
    script->stackTop -= 2;
    return 2;
}

s32 func_001DA4A8(Script* script) {
    func_001424E8(script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

s32 func_001DA4F0(Script* script) {
    func_0013C230(script->valueStack[script->stackTop - 1], script->valueStack[script->stackTop]);
    script->stackTop -= 2;
    return 2;
}

s32 func_001DA540(Script* script) {
    script->valueStack[script->stackTop] = func_0013C280(script->valueStack[script->stackTop]);
    return 2;
}

s32 func_001DA598(Script* script) {
    func_00157158(script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DA5E0);

s32 func_001DA638(Script* script) {
    func_00105510(script->valueStack[script->stackTop - 1], script->valueStack[script->stackTop]);
    script->stackTop -= 2;
    return 2;
}

s32 func_001DA688(Script* script) {
    func_00105560(script->valueStack[script->stackTop - 1], script->valueStack[script->stackTop]);
    script->stackTop -= 2;
    return 2;
}

s32 ScriptCommand_GetElapsedTimeSeconds(Script* script) {
    script->stackTop++;
    script->valueStack[script->stackTop] = func_001EF300();
    return 2;
}

s32 func_001DA720(void) {
    func_00112190();
    return 2;
}

s32 func_001DA740(void) {
    func_001121A8();
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DA760);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DA940);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DAAA8);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DAB88);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_MoveScriptObjectToPositionOverDuration);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DAD58);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_RotateScriptObjectByAnglesOverDuration);

s32 ScriptCommand_OpenPartySelectionMenu(void) {
    GameState_RefreshAvailableFlags();
    PartyMenu_RequestOpen();
    return 6;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DAF98);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_EnableAnimatedScreenFilter);

s32 ScriptCommand_DisableAnimatedScreenFilter(void) {
    func_00103730();
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_GetSelectedObjectStateId);

s32 ScriptCommand_FadeRenderChannelOut(Script* script) {
    func_00108488(script->valueStack[script->stackTop - 1], script->valueStack[script->stackTop]);
    script->stackTop -= 2;
    return 2;
}

s32 ScriptCommand_FadeRenderChannelIn(Script* script) {
    func_001084F0(script->valueStack[script->stackTop - 1], script->valueStack[script->stackTop]);
    script->stackTop -= 2;
    return 2;
}

void func_001DB168(void) {
    D_003854E0.unk_3070 &= ~0x200000000000;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DB198);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DB218);

void func_001DB238(void) {
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DB240);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DB298);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DB2E8);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DB340);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DB3A0);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DB3F8);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DB450);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DB4A8);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DB4F8);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DB578);

void func_001DB5D0(); // temp for next function
INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DB5D0);

s32 func_001DB618(void) {
    func_0011ED30(50002, func_001DB5D0);
    D_003854E0.unk_3078 |= 4;
    return 2;
}

s32 func_001DB658(void) {
    D_003854E0.unk_3078 &= ~4;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DB678);

s32 func_001DB6C8(Script* script) {
    func_0013C858(script->valueStack[script->stackTop - 1], script->valueStack[script->stackTop]);
    script->stackTop -= 2;
    return 2;
}

s32 func_001DB718(Script* script) {
    D_002B8004 = script->valueStack[script->stackTop];
    script->stackTop--;
    return 2;
}

s32 func_001DB740(Script* script) {
    D_002B8008 = script->valueStack[script->stackTop];
    script->stackTop--;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DB768);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetDynamicTextGlyphLookup);

s32 func_001DB810(Script* script) {
    D_002B8014 = script->valueStack[script->stackTop];
    script->stackTop--;
    return 2;
}

s32 func_001DB838(Script* script) {
    func_0013C338(script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

s32 func_001DB880(void) {
    D_00375BC0 &= ~4;
    D_003854E0.unk_3070 &= ~0x2000000000;
    return 2;
}

s32 func_001DB8C8(Script* script) {
    func_001BC480(script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_FinishObjectAnimation);

s32 func_001DB970(void) {
    if (D_002C1EAA & 1) {
        return 4;
    }
    return 2;
}

s32 ScriptCommand_WaitForSummonEnd(void) {
    if (D_002C1EA8 & 0x6000) {
        return 4;
    }
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DB9B0);

s32 ScriptCommand_WaitForAllInterfaceTasks(void) {
    if (func_00235F50() != 0) {
        return 4;
    }
    return 2;
}

s32 func_001DBA30(Script* script) {
    s32 value = D_003F0DD4;
    script->stackTop++;
    script->valueStack[script->stackTop] = value;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_GrantItem);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DBAE8);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_TransitionSelectedObjectToPaletteColor);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DBC40);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DBC88);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DBD18);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DBDA8);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DBE38);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SampleSelectedObjectBackgroundColorAtPosition);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DBF58);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DBFC8);

s32 ScriptCommand_SetObjectEnabled(Script* script) {
    func_00131B90(script->valueStack[script->stackTop - 1], script->valueStack[script->stackTop]);
    script->stackTop -= 2;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DC090);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DC0E8);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DC140);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DC198);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_SetScriptObjectStateId);

s32 func_001DC278(void) {
    D_002B9338 = 0;
    return 2;
}

s32 ScriptCommand_EnableImmediateRoomLoadCompletion(void) {
    D_002B9338 = 1;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_GetItemCategory);

s32 func_001DC368(Script* script) {
    script->stackTop++;
    script->valueStack[script->stackTop] = func_001F0A80();
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DC3B0);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DC3D8);

s32 func_001DC428(Script* script) {
    s32 value = D_002B9170;
    script->stackTop++;
    script->valueStack[script->stackTop] = value;
    return 2;
}

s32 func_001DC450(Script* script) {
    s32 value = D_002B916C;
    script->stackTop++;
    script->valueStack[script->stackTop] = value;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DC478);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DC4D0);

void func_001DC538(void) {
    D_003854E0.unk_3078 &= ~0x20;
}

s32 func_001DC558(Script* script) {
    if (func_00149E38(script->valueStack[script->stackTop], func_001DC538)) {
        D_003854E0.unk_3078 |= 0x20;
    }
    script->stackTop--;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DC5C0);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_ShowTimerDisplay);

s32 func_001DC8A8(Script* script) {
    func_001ED6F0(script->valueStack[script->stackTop], 1);
    script->stackTop--;
    return 2;
}

s32 ScriptCommand_HideTimerDisplay(void) {
    func_00111F00();
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DC910);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DC960);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DC9D0);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DCA28);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DCA98);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DCB08);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DCB70);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DCBD8);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DCC48);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DCC98);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DCCE8);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DCD38);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DCD90);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DCDE0);

s32 func_001DCE30(void) {
    D_00375BC0 |= 0x10;
    return 2;
}

s32 func_001DCE50(void) {
    D_00375BC0 &= ~0x10;
    return 2;
}

s32 func_001DCE70(void) {
    func_001ED878(0);
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DCE90);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DCEE8);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DCFA8);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DD118);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DD170);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DD1E0);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DD250);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DD288);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DD2C0);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DD318);

s32 func_001DD370(void) {
    D_002C1EA8 |= 0x20;
    return 2;
}

s32 func_001DD390(void) {
    D_002C1EA8 &= ~0x20;
    return 2;
}

s32 func_001DD3B0(void) {
    D_003854E0.unk_3078 &= ~0x80;
    return 2;
}

s32 func_001DD3D0(void) {
    D_003854E0.unk_3078 |= 0x80;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DD3F0);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DD430);

s32 func_001DD480(void) {
    D_002C1EA8 |= 0x200000;
    return 2;
}

s32 func_001DD4A0(void) {
    D_002C1EA8 &= ~0x200000;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DD4C8);

s32 func_001DD520(Script* script) {
    script->stackTop++;
    script->valueStack[script->stackTop] = D_003889B0;
    return 2;
}

void func_001DD548(void) {
    D_003854E0.unk_3078 &= ~0x100;
}

s32 func_001DD568(void) {
    D_003854E0.unk_3078 |= 0x100;
    func_0011E830(func_001DD548, func_F20000);
    return 2;
}

s32 func_001DD5B0(void) {
    D_003854E0.unk_3078 |= 0x100;
    func_0011E830(func_001DD548, &D_003D55DC);
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DD5F8);

// Nonmatch: Regswap
INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DD620);
// void func_001DD620(UNK_TYPE arg0, UNK_TYPE arg1, s32 arg2) {
//     func_00177908(arg2);
//     D_003854E0.unk_3078 &= ~0x200;
// }

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DD658);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DD6E0);

INCLUDE_ASM("asm/nonmatchings/script_commands", ScriptCommand_GetCharacterEquippedWeaponItemId);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DD750);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DD7B0);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DD7F8);

s32 func_001DD848(Script* script) {
    func_0013BF20(script->valueStack[script->stackTop]);
    script->stackTop--;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DD890);

s32 func_001DD990(void) {
    func_00125B40(1);
    return 2;
}

s32 func_001DD9B0(void) {
    func_00125B40(0);
    return 2;
}

s32 func_001DD9D0(void) {
    s32 i, j = 0;

    do {
        i = j + 1;
        func_001BC4F8(j, 99);
        j = i;
    } while (i < 0x47);
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DDA18);

s32 ScriptCommand_GetGameOverContinueFlag(Script* script) {
    s32 value = D_002B29C4;

    script->stackTop++;
    script->valueStack[script->stackTop] = value;
    return 2;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DDAA0);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DDAF0);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DDB58);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DDC08);

s32 func_001DDCB8(UNK_PTR context, s32 index) {
    return D_003D4A20[index](context);
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DDCE8);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DDFA0);

void func_001DE098(void) {
    D_003854E0.unk_3380++;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DE0B0);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DE1B8);

s32 func_001DE338(void) {
    TaskList_UpdateMatching(&D_00624960, 0);
    return 0;
}

s32 func_001DE360(void) {
    s32 result;

    D_00624550 = func_00155ED8(0x32, 2);
    D_00624554 = D_00624550 + 512000;
    func_0011EDD0(&D_00624960, &D_00624970, 0x50, 0x100);
    result = func_0011ED30(49999, func_001DE338);
    D_00624958 = NULL;
    return result;
}

void func_001DE3E0(UNK_PTR bufferStart, UNK_PTR bufferEnd) {
    D_00624550 = bufferStart;
    D_00624554 = bufferEnd;
}

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DE3F8);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DE518);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DE5F0);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DE680);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DE750);

INCLUDE_ASM("asm/nonmatchings/script_commands", func_001DE7C0);

void func_001DEDF8(void) {
    func_00177908(D_00624550);
    D_003854E0.unk_3070 &= ~0x100000000000;
}

s32 func_001DEE48(void) {
    return cdvd_QueueFileLoad(&D_00624658, D_00624550, func_001DEDF8, NULL);
}
