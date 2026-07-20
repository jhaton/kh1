#include "ppp.h"

typedef struct {
    /* 0x0 */ pppCDT cdt;
    /* 0x4 */ pppIVECTOR m_dang;
} PAngMove;

typedef struct {
    /* 0x4 */ pppIVECTOR m_dang;
} VAngMove;

void pppAngMoveCalc(pppPObject* pobj, PAngMove* params, pppCtrlTable* ctbl) {
    VAngMove* angle = (VAngMove*)&pobj->val[ctbl->useVal[0]];
    VAngMove* angularVelocity = (VAngMove*)&pobj->val[ctbl->useVal[1]];

    if (ppvUserStopPartF != 0 || ppvMng->stop || ppvMng->unk_B4 != 0) {
        return;
    }

    if (params->cdt.time == pobj->time) {
        angularVelocity->m_dang.x += params->m_dang.x;
        angularVelocity->m_dang.y += params->m_dang.y;
        angularVelocity->m_dang.z += params->m_dang.z;
    }

    angle->m_dang.x += angularVelocity->m_dang.x;
    angle->m_dang.y += angularVelocity->m_dang.y;
    angle->m_dang.z += angularVelocity->m_dang.z;
}

void pppAngMoveCon(pppPObject* pobj, pppCtrlTable* ctbl) {
    VAngMove* angularVelocity = (VAngMove*)&pobj->val[ctbl->useVal[1]];
    angularVelocity->m_dang.x = angularVelocity->m_dang.y = angularVelocity->m_dang.z = 0;
}
