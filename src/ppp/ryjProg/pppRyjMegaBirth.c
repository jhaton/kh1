#include "ppp.h"

typedef struct {
    /* 0x00 */ FMATRIX matPar;
    /* 0x40 */ sceVu0FVECTOR vecForce;
    /* 0x50 */ void* pvParticleData;
    /* 0x54 */ void* pvParticleWmat;
    /* 0x58 */ void* pvParticleColor;
    /* 0x5C */ u32 nParticle;
    /* 0x60 */ u16 ushTimingCt;
    /* 0x62 */ u16 ushIct;
} VRyjMegaBirth;

INCLUDE_ASM("asm/nonmatchings/ppp/ryjProg/pppRyjMegaBirth", pppRyjMegaBirthCalc);

INCLUDE_ASM("asm/nonmatchings/ppp/ryjProg/pppRyjMegaBirth", func_0019ECE8);

INCLUDE_ASM("asm/nonmatchings/ppp/ryjProg/pppRyjMegaBirth", func_0019ED48);

INCLUDE_ASM("asm/nonmatchings/ppp/ryjProg/pppRyjMegaBirth", pppRyjMegaBirthDraw);

void pppRyjMegaBirthCon(pppPObject* pobj, pppCtrlTable* ctbl) {
    VRyjMegaBirth* megaBirth = (VRyjMegaBirth*)&pobj->val[ctbl->useVal[2]];

    sceVu0UnitMatrix(megaBirth->matPar);
    *(u_long128*)megaBirth->vecForce = 0;
    megaBirth->pvParticleData = 0;
    megaBirth->pvParticleWmat = 0;
    megaBirth->pvParticleColor = 0;
    megaBirth->nParticle = 0;
    megaBirth->ushTimingCt = 0;
    megaBirth->ushIct = 0;
    sceVu0UnitMatrix(g_matUnit);
}

void pppRyjMegaBirthDes(pppPObject* pobj, pppCtrlTable* ctbl) {
    VRyjMegaBirth* megaBirth = (VRyjMegaBirth*)&pobj->val[ctbl->useVal[2]];

    if (megaBirth->pvParticleData != NULL) {
        pppFree(ppvEnv, megaBirth->pvParticleData);
        megaBirth->pvParticleData = NULL;
    }
    if (megaBirth->pvParticleWmat != NULL) {
        pppFree(ppvEnv, megaBirth->pvParticleWmat);
        megaBirth->pvParticleWmat = NULL;
    }
    if (megaBirth->pvParticleColor != NULL) {
        pppFree(ppvEnv, megaBirth->pvParticleColor);
        megaBirth->pvParticleColor = NULL;
    }
}
