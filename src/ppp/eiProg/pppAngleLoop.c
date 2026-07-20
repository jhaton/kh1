#include "ppp.h"

typedef struct {
    /* 0x0 */ pppCDT cdt;
    /* 0x4 */ pppIVECTOR angle;
} PAngleLoop;

typedef struct {
    /* 0x0 */ pppIVECTOR appliedAngle;
    /* 0x4 */ void* activeParams;
} VAngleLoop;

void func_0018C558(PAngleLoop* params, VAngleLoop* angleState) {
    angleState->activeParams = params;
    angleState->appliedAngle.z = angleState->appliedAngle.x = angleState->appliedAngle.y = 0;
}

void pppAngleLoopCon(pppPObject* pobj, pppCtrlTable* ctbl) {
    VAngleLoop* angleState = (VAngleLoop*)&pobj->val[ctbl->useVal[0]];
    angleState->activeParams = NULL;
}

void pppAngleLoopCon2(pppPObject* pobj, pppCtrlTable* ctbl) {
    VAngleLoop* angleState = (VAngleLoop*)&pobj->val[ctbl->useVal[0]];
    PAngleLoop* activeParams = ((VAngleLoop*)&pobj->val[ctbl->useVal[0]])->activeParams;
    
    angleState->appliedAngle.x -= activeParams->angle.x;
    angleState->appliedAngle.y -= activeParams->angle.y;
    angleState->appliedAngle.z -= activeParams->angle.z;
}

void pppAngleLoopCalc(pppPObject* pobj, PAngleLoop* params, pppCtrlTable* ctbl) {
    VAngleLoop* angleState = (VAngleLoop*)&pobj->val[ctbl->useVal[0]];

    if (ppvUserStopPartF != 0 || ppvMng->stop || ppvMng->unk_B4 != 0) {
        return;
    }

    if (angleState->activeParams == NULL) {
        func_0018C558(params, angleState);
    }

    if (params->cdt.time == pobj->time) {
        angleState->appliedAngle.x += params->angle.x;
        angleState->appliedAngle.y += params->angle.y;
        angleState->appliedAngle.z += params->angle.z;
    }
}
