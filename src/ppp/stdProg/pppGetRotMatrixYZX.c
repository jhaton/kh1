#include "ppp/pppUtil.h"

void pppGetRotMatrixYZX(sceVu0FMATRIX outputMatrix, pppIVECTOR* angles) {
    sceVu0FMATRIX xRotation, zRotation, combinedRotation, yRotation;

    pppGetRotMatrixX(xRotation, angles->x);
    pppGetRotMatrixZ(zRotation, angles->z);
    sceVu0MulMatrix(combinedRotation, xRotation, zRotation);
    pppGetRotMatrixY(yRotation, angles->y);
    sceVu0MulMatrix(outputMatrix, combinedRotation, yRotation);
}
