#include "ppp/pppUtil.h"

void pppGetRotMatrixZ(sceVu0FMATRIX outputMatrix, u32 angle) {
    f32 sin = D_0036F790[(angle / 16) % 0x1000];
    f32 cos = D_0036F790[((angle + 0x4000) / 16) % 0x1000];

    outputMatrix[0][0] = cos;
    outputMatrix[1][0] = -sin;
    outputMatrix[2][0] = 0.0f;
    outputMatrix[3][0] = 0.0f;
    outputMatrix[0][1] = sin;
    outputMatrix[1][1] = cos;
    outputMatrix[2][1] = 0.0f;
    outputMatrix[3][1] = 0.0f;
    outputMatrix[0][2] = 0.0f;
    outputMatrix[1][2] = 0.0f;
    outputMatrix[2][2] = 1.0f;
    outputMatrix[3][2] = 0.0f;
    outputMatrix[0][3] = 0.0f;
    outputMatrix[1][3] = 0.0f;
    outputMatrix[2][3] = 0.0f;
    outputMatrix[3][3] = 1.0f;
}
