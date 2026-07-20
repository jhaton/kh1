#include "ppp.h"

typedef struct {
    /* 0x0 */ pppCDT cdt;
    /* 0x4 */ pppHCVECTOR col;
} PColor;

typedef struct {
    /* 0x0 */ pppHCVECTOR col;
    /* 0x8 */ pppCVECTOR release_col;
} VColor;

INCLUDE_ASM("asm/nonmatchings/ppp/eiProg/pppColor", pppColorCalc);

void pppColorCon(pppPObject* pobj, pppCtrlTable* ctbl) {
    VColor* colorState = (VColor*)&pobj->val[ctbl->useVal[0]];
    
    colorState->col.r = colorState->col.g = colorState->col.b = colorState->col.a = 0;
}
