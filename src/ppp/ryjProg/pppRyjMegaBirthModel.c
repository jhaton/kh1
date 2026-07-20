#include "ppp.h"

typedef struct {
    /* 0x00 */ pppFVECTOR vecForce;
    /* 0x10 */ char unk_10[0x4];
    /* 0x14 */ void* pvParticleData;
    /* 0x18 */ void* pvParticleWmat;
    /* 0x1C */ void* pvParticleColor;
    /* 0x20 */ u32 nParticle;
    /* 0x24 */ u16 ushTimingCt;
    /* 0x26 */ u16 ushIct;
} VRyjMegaBirthModel;

INCLUDE_ASM("asm/nonmatchings/ppp/ryjProg/pppRyjMegaBirthModel", pppRyjMegaBirthModelCalc);

INCLUDE_ASM("asm/nonmatchings/ppp/ryjProg/pppRyjMegaBirthModel", func_001A26D0);

INCLUDE_ASM("asm/nonmatchings/ppp/ryjProg/pppRyjMegaBirthModel", func_001A2798);

INCLUDE_ASM("asm/nonmatchings/ppp/ryjProg/pppRyjMegaBirthModel", pppRyjMegaBirthModelDraw);

INCLUDE_ASM("asm/nonmatchings/ppp/ryjProg/pppRyjMegaBirthModel", pppRyjMegaBirthModelCon);

void pppRyjMegaBirthModelDes(pppPObject* pobj, pppCtrlTable* ctbl) {
    VRyjMegaBirthModel* birthModel = (VRyjMegaBirthModel*)&pobj->val[ctbl->useVal[2]];
    
    if (birthModel->pvParticleData != NULL) {
        pppFree(ppvEnv, birthModel->pvParticleData);
        birthModel->pvParticleData = NULL;
    }
    if (birthModel->pvParticleWmat != NULL) {
        pppFree(ppvEnv, birthModel->pvParticleWmat);
        birthModel->pvParticleWmat = NULL;
    }
    if (birthModel->pvParticleColor != NULL) {
        pppFree(ppvEnv, birthModel->pvParticleColor);
        birthModel->pvParticleColor = NULL;
    }
}

INCLUDE_ASM("asm/nonmatchings/ppp/ryjProg/pppRyjMegaBirthModel", func_001A2F20);
