#include "ppp.h"

typedef struct {
    /* 0x00 */ void* pvParticleData;
    /* 0x04 */ void* pvParticleColor;
    /* 0x08 */ u32 nParticle;
    /* 0x0C */ f32 fAlphaScale;
    /* 0x10 */ u16 ushTimingCt;
    /* 0x12 */ u8 bActive;
} VRyjMegaBirthModelFilter; // size = 0x14

sceVu0FVECTOR g_vecCamDirM;

INCLUDE_ASM("asm/nonmatchings/ppp/ryjProg/pppRyjMegaBirthModelFilter", pppRyjMegaBirthModelFilterCalc);

void func_001A69A8(void) {
}

INCLUDE_ASM("asm/nonmatchings/ppp/ryjProg/pppRyjMegaBirthModelFilter", func_001A69B0);

INCLUDE_ASM("asm/nonmatchings/ppp/ryjProg/pppRyjMegaBirthModelFilter", pppRyjMegaBirthModelFilterDraw);

void pppRyjMegaBirthModelFilterSta(void) {
    sceVu0SubVector(g_vecCamDirM, ppvAt, ppvEye);
    sceVu0Normalize(g_vecCamDirM, g_vecCamDirM);
    g_vecCamDirM[3] = 1.0f;
}

void pppRyjMegaBirthModelFilterCon(pppPObject* pobj, pppCtrlTable* ctbl) {
    VRyjMegaBirthModelFilter* modelFilter = (VRyjMegaBirthModelFilter*)&pobj->val[ctbl->useVal[2]];

    modelFilter->pvParticleData = NULL;
    modelFilter->pvParticleColor = NULL;
    modelFilter->nParticle = 0;
    modelFilter->fAlphaScale = 1.0f;
    modelFilter->ushTimingCt = 0;
    modelFilter->bActive = 1;
    
    ppvMng->unk_B3 = 1;
}

void pppRyjMegaBirthModelFilterDes(pppPObject* pobj, pppCtrlTable* ctbl) {
    VRyjMegaBirthModelFilter* modelFilter = (VRyjMegaBirthModelFilter*)&pobj->val[ctbl->useVal[2]];

    if (modelFilter->pvParticleData != NULL) {
        pppFree(ppvEnv, modelFilter->pvParticleData);
        modelFilter->pvParticleData = NULL;
    }
    if (modelFilter->pvParticleColor != NULL) {
        pppFree(ppvEnv, modelFilter->pvParticleColor);
        modelFilter->pvParticleColor = NULL;
    }
}
