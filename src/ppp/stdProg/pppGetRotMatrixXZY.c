#include "ppp/pppUtil.h"

void pppGetRotMatrixXZY(sceVu0FMATRIX outputMatrix, pppIVECTOR* angles) {
    sceVu0FMATRIX yRotation, zRotation, combinedRotation, xRotation;

    pppGetRotMatrixY(yRotation, angles->y);
    pppGetRotMatrixZ(zRotation, angles->z);
    sceVu0MulMatrix(combinedRotation, yRotation, zRotation);
    pppGetRotMatrixX(xRotation, angles->x);
    sceVu0MulMatrix(outputMatrix, combinedRotation, xRotation);
}
