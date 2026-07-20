#include "ppp.h"

typedef struct {
    pppCDT cdt;
    pppIVECTOR angle;
} PAngle;

typedef struct {
    pppIVECTOR angle;
} VAngle;

void pppAngleCalc(pppPObject* pobj, PAngle* params, pppCtrlTable* ctbl) {
    VAngle* angle;

    if (ppvUserStopPartF != 0) {
        return;
    }

    if (ppvMng->stop || ppvMng->unk_B4 != 0) {
        return;
    }

    if (params->cdt.time != pobj->time) {
        return;
    }

    angle = (VAngle*)&pobj->val[ctbl->useVal[0]];
    angle->angle.x += params->angle.x;
    angle->angle.y += params->angle.y;
    angle->angle.z += params->angle.z;
}

void pppAngleCon(pppPObject* pobj, pppCtrlTable* ctbl) {
    VAngle* angle = (VAngle*)&pobj->val[ctbl->useVal[0]];

    angle->angle.x = angle->angle.y = angle->angle.z = 0;
}
