#include "ppp.h"

typedef struct {
    pppCDT cdt;
    pppFVECTOR vec;
} PMove;

typedef struct {
    pppFVECTOR vec;
} VMove;

void pppMove(pppPObject* pobj, PMove* params, pppCtrlTable* ctbl) {
    VMove* displacement;
    pppFVECTOR* velocity;

    displacement = (VMove*)&pobj->val[ctbl->useVal[0]];
    velocity = (pppFVECTOR*)&pobj->val[ctbl->useVal[1]];
    if (ppvUserStopPartF != 0) {
        return;
    }

    if (ppvMng->stop || ppvMng->unk_B4 != 0) {
        return;
    }

    if (params->cdt.time == pobj->time) {
        velocity->x = velocity->x + params->vec.x;
        velocity->y = velocity->y + params->vec.y;
        velocity->z = velocity->z + params->vec.z;
    }
    displacement->vec.x = displacement->vec.x + velocity->x;
    displacement->vec.y = displacement->vec.y + velocity->y;
    displacement->vec.z = displacement->vec.z + velocity->z;
}

void pppMoveCon(pppPObject* pobj, pppCtrlTable* ctbl) {
    f32 zero = 0.0f;
    VMove* velocity = (VMove*)&pobj->val[ctbl->useVal[1]];

    velocity->vec.x = velocity->vec.y = velocity->vec.z = zero;
}
