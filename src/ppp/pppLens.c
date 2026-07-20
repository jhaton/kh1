#include "common.h"

#include "ppp/pppUtil.h"
#include "ppp/pppKeLns.h"

#include "libvu0.h"

INCLUDE_ASM("asm/nonmatchings/ppp/pppLens", KeBornRnd2);

INCLUDE_ASM("asm/nonmatchings/ppp/pppLens", func_001A75B0);

INCLUDE_ASM("asm/nonmatchings/ppp/pppLens", func_001A75C8);

INCLUDE_ASM("asm/nonmatchings/ppp/pppLens", func_001A7668);

INCLUDE_ASM("asm/nonmatchings/ppp/pppLens", func_001A76E8);

void KeLnsLp_Init(KeLnsLp* lensState) {
    ppvMng->drawPriority = 2;
    sceVu0UnitMatrix(lensState->zMat);
    lensState->preHidePow = 1.0f;
}

INCLUDE_ASM("asm/nonmatchings/ppp/pppLens", func_001A7C40);

INCLUDE_ASM("asm/nonmatchings/ppp/pppLens", func_001A7F48);

void KeLnsClm_Init(KeLnsClm* column) {
    column->flag = 0;
    column->lp = &ppvDbgTemp;
    column->shp = NULL;
}

INCLUDE_ASM("asm/nonmatchings/ppp/pppLens", func_001A8060);

void KeLnsArnd_Init(KeLnsArnd* surround) {
    surround->shp = NULL;
    surround->lp = &ppvDbgTemp;
}

INCLUDE_ASM("asm/nonmatchings/ppp/pppLens", func_001A8248);

INCLUDE_ASM("asm/nonmatchings/ppp/pppLens", func_001A84B8);

void KeLnsCrn_Init(KeLnsCrn* corner) {
    corner->shp = NULL;
    corner->lp = &ppvDbgTemp;
}

INCLUDE_ASM("asm/nonmatchings/ppp/pppLens", KeLnsFls_Draw);

void KeLnsFls_Init(KeLnsFls* flash) {
    flash->shp = NULL;
    flash->lp = &ppvDbgTemp;
}

INCLUDE_ASM("asm/nonmatchings/ppp/pppLens", func_001A8770);
