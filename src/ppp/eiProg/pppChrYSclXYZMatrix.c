#include "ppp.h"


typedef struct {
    /* 0x0 */ pppCDT cdt;
    /* 0x4 */ f32 scaleRatio;
    /* 0x8 */ f32 maxScale;
} PChrYSclXYZMatrix;

void pppChrYSclXYZMatrixCalc(pppPObject* pobj, PChrYSclXYZMatrix* params) {
    sceVu0FMATRIX scaleMatrix;

    sceVu0UnitMatrix(scaleMatrix);

    scaleMatrix[1][1] = ppvMng->scaleY + (ppvMng->scaleY * params->scaleRatio);
    if (scaleMatrix[1][1] > params->maxScale) {
        scaleMatrix[1][1] = params->maxScale;
    }

    /* Bug? */
    scaleMatrix[1][1] = scaleMatrix[2][2];
    scaleMatrix[0][0] = scaleMatrix[1][1];

    sceVu0MulMatrix(ppvPObj.next->cmat, scaleMatrix, ppvPObj.next->cmat);
}
