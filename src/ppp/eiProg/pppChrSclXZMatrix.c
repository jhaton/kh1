#include "ppp.h"

typedef struct {
    /* 0x0 */ pppCDT cdt;
    /* 0x4 */ f32 scaleRatio;
    /* 0x8 */ f32 maxScale;
} PChrSclXZMatrix;

void pppChrSclXZMatrixCalc(pppPObject* pobj, PChrSclXZMatrix* params) {
    sceVu0FMATRIX scaleMatrix;

    sceVu0UnitMatrix(scaleMatrix);

    scaleMatrix[0][0] = ppvMng->scaleX + (ppvMng->scaleX * params->scaleRatio);
    if (scaleMatrix[0][0] > params->maxScale) {
        scaleMatrix[0][0] = params->maxScale;
    }

    scaleMatrix[2][2] = scaleMatrix[0][0];

    sceVu0MulMatrix(ppvPObj.next->cmat, scaleMatrix, ppvPObj.next->cmat);
}
