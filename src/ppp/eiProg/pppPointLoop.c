#include "ppp.h"

typedef struct {
    /* 0x0 */ pppCDT cdt;
    /* 0x4 */ pppFVECTOR offset;
} PPointLoop;

typedef struct {
    /* 0x0 */ pppFVECTOR appliedOffset;
    /* 0xC */ void* activeParams;
} VPointLoop;

void func_0018C410(PPointLoop* params, VPointLoop* state) {
    state->activeParams = params;
    state->appliedOffset.x = state->appliedOffset.y = state->appliedOffset.z = 0.0f;
}

void pppPointLoopCon(pppPObject* pobj, pppCtrlTable* ctbl) {
    VPointLoop* state = (VPointLoop*)&pobj->val[ctbl->useVal[0]];
    state->activeParams = NULL;
}

void pppPointLoopCon2(pppPObject* pobj, pppCtrlTable* ctbl) {
    VPointLoop* state = (VPointLoop*)&pobj->val[ctbl->useVal[0]];
    PPointLoop* activeParams = ((VPointLoop*)&pobj->val[ctbl->useVal[0]])->activeParams;

    state->appliedOffset.x -= activeParams->offset.x;
    state->appliedOffset.y -= activeParams->offset.y;
    state->appliedOffset.z -= activeParams->offset.z;
}

void pppPointLoopCalc(pppPObject* pobj, PPointLoop* params, pppCtrlTable* ctbl) {
    VPointLoop* state = (VPointLoop*)&pobj->val[ctbl->useVal[0]];

    if (ppvUserStopPartF != 0 || ppvMng->stop || ppvMng->unk_B4 != 0) {
        return;
    }

    if (state->activeParams == NULL) {
        func_0018C410(params, state);
    }

    if (params->cdt.time == pobj->time) {
        state->appliedOffset.x += params->offset.x;
        state->appliedOffset.y += params->offset.y;
        state->appliedOffset.z += params->offset.z;
    }
}
