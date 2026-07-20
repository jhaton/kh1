#include "ppp.h"

#include "gcc/math.h"

typedef struct {
    /* 0x00 */ pppCDT cdt;
    /* 0x04 */ s32 m_model;
    /* 0x08 */ f32 m_tu;
    /* 0x0C */ f32 m_dtu;
    /* 0x10 */ f32 m_ddtu;
    /* 0x14 */ f32 m_tv;
    /* 0x18 */ f32 m_dtv;
    /* 0x1C */ f32 m_ddtv;
    /* 0x20 */ u8 m_acalc;
    /* 0x21 */ u8 m_afix;
    /* 0x22 */ u8 m_draw;
    /* 0x23 */ u8 m_fog;
    /* 0x24 */ u8 m_near;
    /* 0x25 */ u8 m_scis;
} PDrawMdlLoopZ;

typedef struct {
    /* 0x0 */ void* params;
    /* 0x4 */ f32 m_tu;
    /* 0x8 */ f32 m_dtu;
    /* 0xC */ f32 m_ddtu;
    /* 0x10 */ f32 m_tv;
    /* 0x14 */ f32 m_dtv;
    /* 0x18 */ f32 m_ddtv;
    /* 0x1C */ f32 m_uv;
} VDrawMdlLoopZ;

INCLUDE_ASM("asm/nonmatchings/ppp/eiProg/pppDrawMdlLoopZ", func_0018CC58);

void pppDrawMdlLoopZCon(pppPObject* pobj, pppCtrlTable* ctbl) {
    ((VDrawMdlLoopZ*)&pobj->val[ctbl->useVal[2]])->params = NULL;
}

void pppDrawMdlLoopZCon2(pppPObject* pobj, pppCtrlTable* ctbl) {
    VDrawMdlLoopZ* loopState = (VDrawMdlLoopZ*)&pobj->val[ctbl->useVal[2]];

    loopState->m_dtu = loopState->m_ddtu = loopState->m_dtv = loopState->m_ddtv = 0;
}

void pppDrawMdlLoopZDes(void) {
}

void pppDrawMdlLoopZCalc(pppPObject* pobj, PDrawMdlLoopZ* params, pppCtrlTable* ctbl) {
    VDrawMdlLoopZ* loopState = (VDrawMdlLoopZ*)&pobj->val[ctbl->useVal[2]];
    PDrawMdlLoopZ* stateParamsView = (PDrawMdlLoopZ*)&pobj->val[ctbl->useVal[2]];

    if (loopState->params == NULL) {
        func_0018CC58(params, loopState);
    }

    if (ppvUserStopPartF != 0 || ppvMng->stop || ppvMng->unk_B4 != 0) {
        return;
    }

    if (params->cdt.time == pobj->time) {
        stateParamsView->m_tu += params->m_dtu;
        stateParamsView->m_dtu += params->m_ddtu;
        stateParamsView->m_tv += params->m_dtv;
        stateParamsView->m_dtv += params->m_ddtv;
    }

    if (!ppvEmptyLoop) {
        loopState->m_dtu += loopState->m_ddtu;
        loopState->m_dtv += loopState->m_ddtv;
        loopState->m_tu += loopState->m_dtu;
        loopState->m_tv += loopState->m_dtv;
        loopState->m_tu = fmodf(loopState->m_tu, 32768.0f);
        loopState->m_tv = fmodf(loopState->m_tv, 32768.0f);
    }
}

INCLUDE_ASM("asm/nonmatchings/ppp/eiProg/pppDrawMdlLoopZ", pppDrawMdlLoopZDraw);
