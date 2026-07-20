#include "ppp/pppUtil.h"

void pppGetRotMatrixXYZ(sceVu0FMATRIX outputMatrix, pppIVECTOR* angles) {
    sceVu0FMATRIX zRotation, yRotation, combinedRotation, xRotation;

    pppGetRotMatrixZ(zRotation, angles->z);
    pppGetRotMatrixY(yRotation, angles->y);
    sceVu0MulMatrix(combinedRotation, zRotation, yRotation);
    pppGetRotMatrixX(xRotation, angles->x);
    sceVu0MulMatrix(outputMatrix, combinedRotation, xRotation);
}
