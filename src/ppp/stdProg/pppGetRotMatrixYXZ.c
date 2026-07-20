#include "ppp/pppUtil.h"

void pppGetRotMatrixYXZ(sceVu0FMATRIX outputMatrix, pppIVECTOR* angles) {
    sceVu0FMATRIX zRotation, xRotation, combinedRotation, yRotation;

    pppGetRotMatrixZ(zRotation, angles->z);
    pppGetRotMatrixX(xRotation, angles->x);
    sceVu0MulMatrix(combinedRotation, zRotation, xRotation);
    pppGetRotMatrixY(yRotation, angles->y);
    sceVu0MulMatrix(outputMatrix, combinedRotation, yRotation);
}
