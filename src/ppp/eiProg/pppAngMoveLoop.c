#include "ppp.h"

typedef struct {
    /* 0x0 */ pppCDT cdt;
    /* 0x4 */ pppIVECTOR m_dang;
} PAngMoveLoop;

typedef struct {
    /* 0x4 */ pppIVECTOR m_dang;
} VAngMoveLoop;

void pppAngMoveLoopCon(pppPObject* pobj, pppCtrlTable* ctbl) {
    VAngMoveLoop* angularVelocity = (VAngMoveLoop*)&pobj->val[ctbl->useVal[1]];
    angularVelocity->m_dang.x = angularVelocity->m_dang.y = angularVelocity->m_dang.z = 0;
}

void pppAngMoveLoopCon2(pppPObject* pobj, pppCtrlTable* ctbl) {
    pppAngMoveLoopCon(pobj, ctbl);
}

void pppAngMoveLoopCalc(pppPObject* pobj, PAngMoveLoop* params, pppCtrlTable* ctbl) {
    VAngMoveLoop* angle = (VAngMoveLoop*)&pobj->val[ctbl->useVal[0]];
    VAngMoveLoop* angularVelocity = (VAngMoveLoop*)&pobj->val[ctbl->useVal[1]];

    if (ppvUserStopPartF != 0 || ppvMng->stop || ppvMng->unk_B4 != 0) {
        return;
    }

    if (params->cdt.time == pobj->time) {
        angularVelocity->m_dang.x += params->m_dang.x;
        angularVelocity->m_dang.y += params->m_dang.y;
        angularVelocity->m_dang.z += params->m_dang.z;
    }

    if (!ppvEmptyLoop) {
        angle->m_dang.x += angularVelocity->m_dang.x;
        angle->m_dang.y += angularVelocity->m_dang.y;
        angle->m_dang.z += angularVelocity->m_dang.z;
    }
}
