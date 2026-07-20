#include "ppp.h"

typedef struct {
    /* 0x0 */ pppHCVECTOR col;
} VColMove;

typedef struct {
    /* 0x0 */ pppCDT cdt;
    /* 0x4 */ pppHCVECTOR col;
} PColMove;

void pppColMoveCalc(pppPObject* pobj, PColMove* params, pppCtrlTable* ctbl) {
    VColMove* color = (VColMove*)&pobj->val[ctbl->useVal[0]];
    VColMove* colorVelocity = (VColMove*)&pobj->val[ctbl->useVal[1]];

    if (ppvUserStopPartF != 0 || ppvMng->stop || ppvMng->unk_B4 != 0) {
        return;
    }

    if (params->cdt.time == pobj->time) {
        colorVelocity->col.r += params->col.r;
        colorVelocity->col.g += params->col.g;
        colorVelocity->col.b += params->col.b;
        colorVelocity->col.a += params->col.a;
    }

    color->col.r += colorVelocity->col.r;
    color->col.g += colorVelocity->col.g;
    color->col.b += colorVelocity->col.b;
    color->col.a += colorVelocity->col.a;
}

void pppColMoveCon(pppPObject* pobj, pppCtrlTable* ctbl) {
    VColMove* colorVelocity = (VColMove*)&pobj->val[ctbl->useVal[1]];
    colorVelocity->col.r = colorVelocity->col.g = colorVelocity->col.b = colorVelocity->col.a = 0;
}
