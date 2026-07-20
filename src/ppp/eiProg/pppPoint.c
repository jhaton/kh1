#include "ppp.h"

typedef struct {
    pppCDT cdt;
    pppFVECTOR pos;
} PPoint;

typedef struct {
    pppFVECTOR pos;
} VPoint;

void pppPoint(pppPObject* pobj, PPoint* params, pppCtrlTable* ctbl) {
    VPoint* state;

    if (ppvUserStopPartF != 0) {
        return;
    }

    if (ppvMng->stop || ppvMng->unk_B4 != 0) {
        return;
    }

    if (params->cdt.time != pobj->time) {
        return;
    }

    state = (VPoint*)&pobj->val[ctbl->useVal[0]];
    state->pos.x += params->pos.x;
    state->pos.y += params->pos.y;
    state->pos.z += params->pos.z;
}

void pppPointCon(pppPObject* pobj, pppCtrlTable* ctbl) {
    f32 zero = 0.0f;
    VPoint* state = (VPoint*)&pobj->val[ctbl->useVal[0]];

    state->pos.x = state->pos.y = state->pos.z = zero;
}
