#include "ppp.h"

typedef struct {
    /* 0x0 */ pppCDT cdt;
    /* 0x4 */ pppFVECTOR vec;
} PAccele;

typedef struct {
    /* 0x0 */ pppFVECTOR vec;
} VAccele;

void pppAcceleCalc(pppPObject* pobj, PAccele* params, pppCtrlTable* ctbl) {
    VAccele* velocity = (VAccele*)&pobj->val[ctbl->useVal[0]];
    VAccele* acceleration = (VAccele*)&pobj->val[ctbl->useVal[1]];

    if (ppvUserStopPartF != 0 || ppvMng->stop || ppvMng->unk_B4 != 0) {
        return;
    }

    if (params->cdt.time == pobj->time) {
        acceleration->vec.x += params->vec.x;
        acceleration->vec.y += params->vec.y;
        acceleration->vec.z += params->vec.z;
    }
    velocity->vec.x += acceleration->vec.x;
    velocity->vec.y += acceleration->vec.y;
    velocity->vec.z += acceleration->vec.z;
}

void pppAcceleCon(pppPObject* pobj, pppCtrlTable* ctbl) {
    VAccele* acceleration = (VAccele*)&pobj->val[ctbl->useVal[1]];
    acceleration->vec.x = acceleration->vec.y = acceleration->vec.z = 0.0f;
}
