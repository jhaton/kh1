#include "ppp.h"

typedef struct {
    /* 0x0 */ pppCDT cdt;
    /* 0x4 */ f32 scaleRatio;
    /* 0x8 */ f32 maxScale;
} PChrSclYMatrix;

void pppChrSclYMatrixCalc(pppPObject* pobj, PChrSclYMatrix* params) {
    sceVu0FMATRIX scaleMatrix;

    sceVu0UnitMatrix(scaleMatrix);

    scaleMatrix[1][1] = ppvMng->scaleY + (ppvMng->scaleY * params->scaleRatio);
    if (scaleMatrix[1][1] > params->maxScale) {
        scaleMatrix[1][1] = params->maxScale;
    }

    sceVu0MulMatrix(ppvPObj.next->cmat, scaleMatrix, ppvPObj.next->cmat);
}
