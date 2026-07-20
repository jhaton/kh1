#include "ppp/pppUtil.h"

void pppGetRotMatrixZYX(sceVu0FMATRIX outputMatrix, pppIVECTOR* angles) {
    sceVu0FMATRIX xRotation, yRotation, combinedRotation, zRotation;

    pppGetRotMatrixX(xRotation, angles->x);
    pppGetRotMatrixY(yRotation, angles->y);
    sceVu0MulMatrix(combinedRotation, xRotation, yRotation);
    pppGetRotMatrixZ(zRotation, angles->z);
    sceVu0MulMatrix(outputMatrix, combinedRotation, zRotation);
}
