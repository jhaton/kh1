#include "ppp/pppUtil.h"

void pppGetRotMatrixZXY(sceVu0FMATRIX outputMatrix, pppIVECTOR* angles) {
    sceVu0FMATRIX yRotation, xRotation, combinedRotation, zRotation;

    pppGetRotMatrixY(yRotation, angles->y);
    pppGetRotMatrixX(xRotation, angles->x);
    sceVu0MulMatrix(combinedRotation, yRotation, xRotation);
    pppGetRotMatrixZ(zRotation, angles->z);
    sceVu0MulMatrix(outputMatrix, combinedRotation, zRotation);
}
