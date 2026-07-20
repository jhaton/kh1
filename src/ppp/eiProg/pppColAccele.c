#include "ppp.h"

typedef struct {
    /* 0x0 */ pppHCVECTOR ddcol;
} VColAccele;

typedef struct {
    /* 0x0 */ pppCDT cdt;
    /* 0x4 */ pppHCVECTOR ddcol;
} PColAccele;

void pppColAcceleCalc(pppPObject* pobj, PColAccele* params, pppCtrlTable* ctbl) {
    VColAccele* colorVelocity = (VColAccele*)&pobj->val[ctbl->useVal[0]];
    VColAccele* colorAcceleration = (VColAccele*)&pobj->val[ctbl->useVal[1]];

    if (ppvUserStopPartF != 0 || ppvMng->stop || ppvMng->unk_B4 != 0) {
        return;
    }

    if (params->cdt.time == pobj->time) {
        colorAcceleration->ddcol.r += params->ddcol.r;
        colorAcceleration->ddcol.g += params->ddcol.g;
        colorAcceleration->ddcol.b += params->ddcol.b;
        colorAcceleration->ddcol.a += params->ddcol.a;
    }

    colorVelocity->ddcol.r += colorAcceleration->ddcol.r;
    colorVelocity->ddcol.g += colorAcceleration->ddcol.g;
    colorVelocity->ddcol.b += colorAcceleration->ddcol.b;
    colorVelocity->ddcol.a += colorAcceleration->ddcol.a;
}

void pppColAcceleCon(pppPObject* pobj, pppCtrlTable* ctbl) {
    VColAccele* colorAcceleration = (VColAccele*)&pobj->val[ctbl->useVal[1]];
    colorAcceleration->ddcol.r = colorAcceleration->ddcol.g = colorAcceleration->ddcol.b = colorAcceleration->ddcol.a = 0;
}
