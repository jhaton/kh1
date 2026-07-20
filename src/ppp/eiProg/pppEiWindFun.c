#include "ppp.h"

typedef struct {
    /* 0x00 */ pppCDT cdt;
    /* 0x04 */ pppFVECTOR vec;
    /* 0x10 */ f32 len;
    /* 0x14 */ f32 r;
    /* 0x18 */ f32 wv;
    /* 0x1C */ f32 wdv;
    /* 0x20 */ f32 wddv;
} PEiWindFun;

typedef struct {
    /* 0x00 */ pppFVECTOR vec;
    /* 0x0C */ f32 len;
    /* 0x10 */ f32 r;
    /* 0x14 */ f32 cosa;
    /* 0x18 */ f32 wv;
    /* 0x1C */ f32 wdv;
    /* 0x20 */ f32 wddv;
} VEiWindFun;

void pppEiWindFunCon(pppPObject* pobj, pppCtrlTable* ctbl) {
    VEiWindFun* windState = (VEiWindFun*)&pobj->val[ctbl->useVal[0]];

    windState->len = 0;
    windState->r = 0;
    windState->wv = windState->wdv = windState->wddv = 0; 
}

INCLUDE_ASM("asm/nonmatchings/ppp/eiProg/pppEiWindFun", pppEiWindFunCalc);
