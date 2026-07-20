#include "ppp.h"

typedef struct {
    /* 0x0 */ s16 ict;
    /* 0x2 */ u16 aptct;
} VVertexAp;

void pppVertexApCon(pppPObject* pobj, pppCtrlTable* ctbl) {
    VVertexAp* state = (VVertexAp*)&pobj->val[ctbl->useVal[0]];
    
    state->ict = 0;
    state->aptct = 0;
}

INCLUDE_ASM("asm/nonmatchings/ppp/eiProg/pppVertexAp", func_00185F30);

INCLUDE_ASM("asm/nonmatchings/ppp/eiProg/pppVertexAp", pppVertexApCalc);
