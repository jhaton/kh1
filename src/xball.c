#include "common.h"
#include "fastmath.h"

f32 func_001215A8(sceVu0FVECTOR, sceVu0FVECTOR);

extern f32 D_002B8340[];
extern f32 D_002B8344[];

f32 func_00120A38(sceVu0FVECTOR vector) {
    f32 squaredLength = sceVu0InnerProduct(vector, vector);
    return _sqrtf(squaredLength);
}

f32 func_00120A58(sceVu0FVECTOR vector) {
    return _sqrtf(vector[0] * vector[0] + vector[2] * vector[2]);
}

s32 func_00120A78(sceVu0FMATRIX outputMatrix, sceVu0FMATRIX inputMatrix, sceVu0FVECTOR rotation) {
    sceVu0RotMatrixZ(outputMatrix, inputMatrix, rotation[2]);
    sceVu0RotMatrixX(outputMatrix, outputMatrix, rotation[0]);
    sceVu0RotMatrixY(outputMatrix, outputMatrix, rotation[1]);
}

INCLUDE_ASM("asm/nonmatchings/xball", func_00120AC8);
f32 func_00120AC8(f32);

INCLUDE_ASM("asm/nonmatchings/xball", func_00120B40);

INCLUDE_ASM("asm/nonmatchings/xball", func_00120D38);

INCLUDE_ASM("asm/nonmatchings/xball", func_00120DB8);

// nonmatch: extra nops
INCLUDE_ASM("asm/nonmatchings/xball", func_00120E48);
// s32 func_00120E48(sceVu0FMATRIX arg0, sceVu0FVECTOR* arg1) {
//     sceVu0FVECTOR local_40;
//     s32 i;

//     for (i = 0; i < 8; i++) {
//         sceVu0ApplyMatrix(local_40, arg0, arg1[i]);
//         if (local_40[3] < 1.0f) {
//             return 0;
//         }
//         local_40[0] /= local_40[3];
//         local_40[1] /= local_40[3];
//         if (local_40[0] < 0.0f || local_40[0] > 4095.0f) {
//             return 0;
//         }
//         if (local_40[1] < 0.0f || local_40[1] > 4095.0f) {
//             return 0;
//         }
//     }
//     return 1;
// }

void func_00120F48(f32 scale, sceVu0FVECTOR output, sceVu0FVECTOR input) {
    output[0] = input[0] * scale;
    output[1] = input[1];
    output[2] = input[2] * scale;
    output[3] = input[3];
}

s32 func_00120F78(sceVu0FMATRIX matrix) {
    return (matrix[0][2] < 0.0f || sceVu0ClipScreen(*matrix) != 0) ? TRUE : FALSE;
}

s32 func_00120FC8(f32 blend, sceVu0FMATRIX output, sceVu0FMATRIX firstLine, sceVu0FMATRIX secondLine) {
    sceVu0FVECTOR interpolatedDirection;
    sceVu0FVECTOR firstDirection;
    sceVu0FVECTOR secondDirection;
    sceVu0FVECTOR intersection;
    sceVu0FVECTOR offset;

    sceVu0SubVector(firstDirection, firstLine[1], firstLine[0]);
    sceVu0SubVector(secondDirection, secondLine[1], secondLine[0]);
    sceVu0ScaleVector(
        intersection, firstDirection, ((secondLine[0][1] - firstLine[0][1]) + secondDirection[1] * (secondLine[0][0] - firstLine[0][0])) / (secondDirection[0] * firstDirection[1] - secondDirection[1] * firstDirection[0])
    );
    sceVu0AddVector(intersection, intersection, firstLine[0]);

    sceVu0SubVector(firstDirection, firstLine[0], intersection);
    func_001223B0(firstDirection, firstDirection);

    sceVu0SubVector(secondDirection, secondLine[0], intersection);
    func_001223B0(secondDirection, secondDirection);

    sceVu0InterVector(interpolatedDirection, secondDirection, firstDirection, blend);
    sceVu0Normalize(interpolatedDirection, interpolatedDirection);
    sceVu0ScaleVector(offset, interpolatedDirection, firstDirection[3] + (secondDirection[3] - firstDirection[3]) * blend);
    sceVu0AddVector(output[0], intersection, offset);
    output[0][3] = 1.0f;

    sceVu0SubVector(firstDirection, firstLine[1], intersection);
    func_001223B0(firstDirection, firstDirection);

    sceVu0SubVector(secondDirection, secondLine[1], intersection);
    func_001223B0(secondDirection, secondDirection);

    sceVu0InterVector(interpolatedDirection, secondDirection, firstDirection, blend);
    sceVu0Normalize(interpolatedDirection, interpolatedDirection);
    sceVu0ScaleVector(offset, interpolatedDirection, firstDirection[3] + (secondDirection[3] - firstDirection[3]) * blend);
    sceVu0AddVector(output[1], intersection, offset);
    output[1][3] = 1.0f;

    return 1;
}

INCLUDE_ASM("asm/nonmatchings/xball", func_00121200);

INCLUDE_ASM("asm/nonmatchings/xball", func_00121340);

INCLUDE_ASM("asm/nonmatchings/xball", func_001213B0);

void func_00121480(f32 planeA, f32 planeB, f32 planeC, f32 shadowScale, sceVu0FMATRIX outputMatrix, sceVu0FVECTOR lightPosition) {
    sceVu0DropShadowMatrix(outputMatrix, lightPosition, planeA, planeB, planeC, 0);
    sceVu0ScaleVectorXYZ(outputMatrix[3], outputMatrix[3], shadowScale);
}

void func_001214C8(sceVu0FMATRIX output, sceVu0FMATRIX input) {
    output[0][0] = input[0][0];
    output[0][1] = input[1][0];
    output[0][2] = input[2][0];
    output[0][3] = 0.0f;
    output[1][0] = input[0][1];
    output[1][1] = input[1][1];
    output[1][2] = input[2][1];
    output[1][3] = 0.0f;
    output[2][0] = input[0][2];
    output[2][1] = input[1][2];
    output[2][2] = input[2][2];
    output[2][3] = 0.0f;
    output[3][0] = 0.0f;
    output[3][1] = 0.0f;
    output[3][2] = 0.0f;
    output[3][3] = 1.0f;
}

INCLUDE_ASM("asm/nonmatchings/xball", func_00121538);

INCLUDE_ASM("asm/nonmatchings/xball", func_001215A8);

f32 func_001216A8(sceVu0FVECTOR firstVector, sceVu0FVECTOR secondVector) {
    return acosf(func_001215A8(firstVector, secondVector));
}

INCLUDE_ASM("asm/nonmatchings/xball", func_001216C8);

INCLUDE_ASM("asm/nonmatchings/xball", func_001217C0);

INCLUDE_ASM("asm/nonmatchings/xball", func_00121830);

void func_00121878(sceVu0FVECTOR direction, sceVu0FVECTOR rotation, sceVu0FVECTOR unused) {
    rotation[0] = atan2f(-direction[1], func_00120A58(direction));
    rotation[1] = atan2f(direction[0], direction[2]);
    rotation[2] = 0.0f;
    rotation[3] = 1.0f;
}

INCLUDE_ASM("asm/nonmatchings/xball", func_001218E0);

f32 func_00121970(f32 remainingTime) {
    if (remainingTime > 0.0f) {
        remainingTime -= D_002B8340[1];
        if (remainingTime < 0.0f) {
            remainingTime = 0.0f;
        }
    }
    return remainingTime;
}

void func_001219A8(sceVu0FVECTOR firstVector, sceVu0FVECTOR secondVector) {
    sceVu0FVECTOR horizontalFirst;
    sceVu0FVECTOR horizontalSecond;

    sceVu0CopyVector(horizontalFirst, firstVector);
    sceVu0CopyVector(horizontalSecond, secondVector);
    horizontalFirst[1] = 0.0f;
    horizontalSecond[1] = 0.0f;
    func_001215A8(horizontalFirst, horizontalSecond);
}
