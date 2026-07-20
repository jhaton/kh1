#include "ppp.h"

typedef struct {
    /* 0x0 */ u32 unFrameCt;
    /* 0x4 */ u8 bSmp;
    /* 0x5 */ u8 bStandby;
    /* 0x6 */ char dummy0[2];
    /* 0x8 */ pppFVECTOR* pvecSmp;
} VRyjDrawShipoly;

INCLUDE_ASM("asm/nonmatchings/ppp/ryjProg/pppRyjDrawShipoly", pppRyjDrawShipolyCalc);

INCLUDE_ASM("asm/nonmatchings/ppp/ryjProg/pppRyjDrawShipoly", pppRyjDrawShipolyDraw);

void pppRyjDrawShipolyCon(pppPObject* pobj, pppCtrlTable* ctbl) {
    VRyjDrawShipoly* shipoly = (VRyjDrawShipoly*)&pobj->val[ctbl->useVal[2]];
    shipoly->pvecSmp = NULL;
    shipoly->unFrameCt = 0;
    shipoly->bSmp = FALSE;
    shipoly->bStandby = TRUE;
}

void pppRyjDrawShipolyDes(pppPObject* pobj, pppCtrlTable* ctbl) {
    VRyjDrawShipoly* shipoly = (VRyjDrawShipoly*)&pobj->val[ctbl->useVal[2]];

    if (shipoly->pvecSmp != NULL) {
        pppFree(ppvEnv, shipoly->pvecSmp);
        shipoly->pvecSmp = NULL;
    }
}

INCLUDE_ASM("asm/nonmatchings/ppp/ryjProg/pppRyjDrawShipoly", func_0019C460);

INCLUDE_ASM("asm/nonmatchings/ppp/ryjProg/pppRyjDrawShipoly", func_0019C618);

INCLUDE_ASM("asm/nonmatchings/ppp/ryjProg/pppRyjDrawShipoly", func_0019C698);

INCLUDE_ASM("asm/nonmatchings/ppp/ryjProg/pppRyjDrawShipoly", func_0019C770);
