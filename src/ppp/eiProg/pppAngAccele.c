#include "ppp.h"

typedef struct {
    /* 0x0 */ pppCDT cdt;
    /* 0x4 */ pppIVECTOR ddang;
} PAngAccele;

typedef struct {
    /* 0x0 */ pppIVECTOR ddang;
} VAngAccele;

void pppAngAcceleCalc(pppPObject* pobj, PAngAccele* params, pppCtrlTable* ctbl) {
    VAngAccele* angularVelocity = (VAngAccele*)&pobj->val[ctbl->useVal[0]];
    VAngAccele* angularAcceleration = (VAngAccele*)&pobj->val[ctbl->useVal[1]];

    if (ppvUserStopPartF != 0 || ppvMng->stop || ppvMng->unk_B4 != 0) {
        return;
    }

    if (params->cdt.time == pobj->time) {
        angularAcceleration->ddang.x += params->ddang.x;
        angularAcceleration->ddang.y += params->ddang.y;
        angularAcceleration->ddang.z += params->ddang.z;
    }
    angularVelocity->ddang.x += angularAcceleration->ddang.x;
    angularVelocity->ddang.y += angularAcceleration->ddang.y;
    angularVelocity->ddang.z += angularAcceleration->ddang.z;
}

void pppAngAcceleCon(pppPObject* pobj, pppCtrlTable* ctbl) {
    VAngAccele* angularAcceleration = (VAngAccele*)&pobj->val[ctbl->useVal[1]];
    angularAcceleration->ddang.x = angularAcceleration->ddang.y = angularAcceleration->ddang.z = 0;
}
