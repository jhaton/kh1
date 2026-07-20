#include "ppp.h"

typedef struct {
    /* 0x0 */ pppCDT cdt;
    /* 0x4 */ pppFVECTOR scale;
} PScaleLoop;

typedef struct {
    /* 0x0 */ pppFVECTOR appliedScale;
    /* 0xC */ void* activeParams;
} VScaleLoop;

void func_0018C6A0(PScaleLoop* params, VScaleLoop* state) {
    state->activeParams = params;
    state->appliedScale.x = state->appliedScale.y = state->appliedScale.z = 0.0f;
}

void pppScaleLoopCon(pppPObject* pobj, pppCtrlTable* ctbl) {
    VScaleLoop* state = (VScaleLoop*)&pobj->val[ctbl->useVal[0]];
    state->activeParams = NULL;
}

void pppScaleLoopCon2(pppPObject* pobj, pppCtrlTable* ctbl) {
    VScaleLoop* state = (VScaleLoop*)&pobj->val[ctbl->useVal[0]];
    PScaleLoop* activeParams = ((VScaleLoop*)&pobj->val[ctbl->useVal[0]])->activeParams;

    state->appliedScale.x -= activeParams->scale.x;
    state->appliedScale.y -= activeParams->scale.y;
    state->appliedScale.z -= activeParams->scale.z;
}

void pppScaleLoopCalc(pppPObject* pobj, PScaleLoop* params, pppCtrlTable* ctbl) {
    VScaleLoop* state = (VScaleLoop*)&pobj->val[ctbl->useVal[0]];

    if (ppvUserStopPartF != 0 || ppvMng->stop || ppvMng->unk_B4 != 0) {
        return;
    }

    if (state->activeParams == NULL) {
        func_0018C6A0(params, state);
    }

    if (params->cdt.time == pobj->time) {
        state->appliedScale.x += params->scale.x;
        state->appliedScale.y += params->scale.y;
        state->appliedScale.z += params->scale.z;
    }
}
