#include "ppp.h"

typedef struct {
    pppCDT cdt;
    pppFVECTOR scale;
} PScale;

typedef struct {
    pppFVECTOR scale;
} VScale;

void pppScaleCalc(pppPObject* pobj, PScale* params, pppCtrlTable* ctbl) {
    VScale* state;

    if (ppvUserStopPartF != 0) {
        return;
    }

    if (ppvMng->stop || ppvMng->unk_B4 != 0) {
        return;
    }

    if (params->cdt.time != pobj->time) {
        return;
    }

    state = (VScale*)&pobj->val[ctbl->useVal[0]];
    state->scale.x += params->scale.x;
    state->scale.y += params->scale.y;
    state->scale.z += params->scale.z;
}

void pppScaleCon(pppPObject* pobj, pppCtrlTable* ctbl) {
    f32 zero = 0.0f;
    VScale* state = (VScale*)&pobj->val[ctbl->useVal[0]];

    state->scale.x = state->scale.y = state->scale.z = zero;
}
