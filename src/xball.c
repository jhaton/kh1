#include "common.h"
#include "fastmath.h"

f32 vector_dot_product(sceVu0FVECTOR, sceVu0FVECTOR);

extern f32 D_002B8340[];
extern f32 D_002B8344[];

// Calculate vector magnitude/length
f32 vector_magnitude(sceVu0FVECTOR vec) {
    f32 prod = sceVu0InnerProduct(vec, vec);
    return _sqrtf(prod);
}

// Calculate 2D distance (XZ plane only, ignoring Y)
// Useful for ground-based distance calculations
f32 vector_magnitude_2d(sceVu0FVECTOR vec) {
    return _sqrtf(vec[0] * vec[0] + vec[2] * vec[2]);
}

// Create rotation matrix from Euler angles
// Order: Z -> X -> Y rotations
s32 matrix_rotation_zxy(sceVu0FMATRIX result, sceVu0FMATRIX base, sceVu0FVECTOR angles) {
    sceVu0RotMatrixZ(result, base, angles[2]);   // Roll
    sceVu0RotMatrixX(result, result, angles[0]); // Pitch
    sceVu0RotMatrixY(result, result, angles[1]); // Yaw
}

INCLUDE_ASM("asm/nonmatchings/xball", func_00120AC8);
f32 func_00120AC8(f32);

INCLUDE_ASM("asm/nonmatchings/xball", func_00120B40);

INCLUDE_ASM("asm/nonmatchings/xball", func_00120D38);

INCLUDE_ASM("asm/nonmatchings/xball", func_00120DB8);

// Frustum culling test for bounding box (8 vertices)
// Returns 1 if all vertices are within screen bounds, 0 if any are outside
// nonmatch: extra nops
INCLUDE_ASM("asm/nonmatchings/xball", func_00120E48);
// s32 frustum_cull_bbox(sceVu0FMATRIX projection_matrix, sceVu0FVECTOR* bbox_vertices) {
//     sceVu0FVECTOR projected_vertex;
//     s32 i;
//
//     // Test all 8 vertices of bounding box
//     for (i = 0; i < 8; i++) {
//         // Transform vertex to clip space
//         sceVu0ApplyMatrix(projected_vertex, projection_matrix, bbox_vertices[i]);
//
//         // Check if behind near plane
//         if (projected_vertex[3] < 1.0f) {
//             return 0;
//         }
//
//         // Perspective divide
//         projected_vertex[0] /= projected_vertex[3];
//         projected_vertex[1] /= projected_vertex[3];
//
//         // Check if outside screen bounds (PS2 coordinate system)
//         if (projected_vertex[0] < 0.0f || projected_vertex[0] > 4095.0f) {
//             return 0;
//         }
//         if (projected_vertex[1] < 0.0f || projected_vertex[1] > 4095.0f) {
//             return 0;
//         }
//     }
//     return 1;
// }

// Scale vector XZ components by factor, preserve Y and W
void vector_scale_xz(sceVu0FVECTOR result, f32 scale_factor, sceVu0FVECTOR input) {
    result[0] = input[0] * scale_factor;
    result[1] = input[1]; // Preserve Y
    result[2] = input[2] * scale_factor;
    result[3] = input[3]; // Preserve W
}

// Basic visibility/backface culling test
s32 is_facing_camera(sceVu0FMATRIX matrix) {
    return (matrix[0][2] < 0.0f || sceVu0ClipScreen(*matrix) != 0) ? TRUE : FALSE;
}

// Complex 3D interpolation between two line segments
// Likely used for camera paths or smooth object movement between waypoints
s32 interpolate_line_segments(
    f32 interpolation_factor, sceVu0FMATRIX result, sceVu0FMATRIX line1, sceVu0FMATRIX line2
) {
    sceVu0FVECTOR temp_vec;
    sceVu0FVECTOR line1_dir;
    sceVu0FVECTOR line2_dir;
    sceVu0FVECTOR interpolated_dir;
    sceVu0FVECTOR scaled_dir;

    // Calculate direction vectors for both lines
    sceVu0SubVector(line1_dir, line1[1], line1[0]);
    sceVu0SubVector(line2_dir, line2[1], line2[0]);

    // Find intersection point of the two lines
    sceVu0ScaleVector(
        interpolated_dir, line1_dir,
        ((line2[0][1] - line1[0][1]) + line2_dir[1] * (line2[0][0] - line1[0][0]))
            / (line2_dir[0] * line1_dir[1] - line2_dir[1] * line1_dir[0])
    );
    sceVu0AddVector(interpolated_dir, interpolated_dir, line1[0]);

    // Calculate interpolated start point
    sceVu0SubVector(line1_dir, line1[0], interpolated_dir);
    func_001223B0(line1_dir, line1_dir); // Normalize with distance stored in W

    sceVu0SubVector(line2_dir, line2[0], interpolated_dir);
    func_001223B0(line2_dir, line2_dir); // Normalize with distance stored in W

    sceVu0InterVector(temp_vec, line2_dir, line1_dir, interpolation_factor);
    sceVu0Normalize(temp_vec, temp_vec);
    sceVu0ScaleVector(scaled_dir, temp_vec, line1_dir[3] + (line2_dir[3] - line1_dir[3]) * interpolation_factor);
    sceVu0AddVector(result[0], interpolated_dir, scaled_dir);
    result[0][3] = 1.0f;

    // Calculate interpolated end point
    sceVu0SubVector(line1_dir, line1[1], interpolated_dir);
    func_001223B0(line1_dir, line1_dir);

    sceVu0SubVector(line2_dir, line2[1], interpolated_dir);
    func_001223B0(line2_dir, line2_dir);

    sceVu0InterVector(temp_vec, line2_dir, line1_dir, interpolation_factor);
    sceVu0Normalize(temp_vec, temp_vec);
    sceVu0ScaleVector(scaled_dir, temp_vec, line1_dir[3] + (line2_dir[3] - line1_dir[3]) * interpolation_factor);
    sceVu0AddVector(result[1], interpolated_dir, scaled_dir);
    result[1][3] = 1.0f;

    return 1;
}

INCLUDE_ASM("asm/nonmatchings/xball", func_00121200);

INCLUDE_ASM("asm/nonmatchings/xball", func_00121340);

INCLUDE_ASM("asm/nonmatchings/xball", func_001213B0);

// Create drop shadow projection matrix
void create_drop_shadow_matrix(
    f32 light_x, f32 light_y, f32 light_z, f32 scale, sceVu0FMATRIX result, sceVu0FVECTOR ground_plane
) {
    sceVu0DropShadowMatrix(result, ground_plane, light_x, light_y, light_z, 0);
    sceVu0ScaleVectorXYZ(result[3], result[3], scale);
}

// Matrix transpose operation (3x3 rotation part only)
void matrix_transpose_3x3(sceVu0FMATRIX result, sceVu0FMATRIX input) {
    result[0][0] = input[0][0];
    result[0][1] = input[1][0];
    result[0][2] = input[2][0];
    result[0][3] = 0.0f;
    result[1][0] = input[0][1];
    result[1][1] = input[1][1];
    result[1][2] = input[2][1];
    result[1][3] = 0.0f;
    result[2][0] = input[0][2];
    result[2][1] = input[1][2];
    result[2][2] = input[2][2];
    result[2][3] = 0.0f;
    result[3][0] = 0.0f;
    result[3][1] = 0.0f;
    result[3][2] = 0.0f;
    result[3][3] = 1.0f;
}

INCLUDE_ASM("asm/nonmatchings/xball", func_00121538);

INCLUDE_ASM("asm/nonmatchings/xball", vector_dot_product);

// Calculate angle between two vectors using dot product
f32 vector_angle_between(sceVu0FVECTOR vec1, sceVu0FVECTOR vec2) {
    return acosf(vector_dot_product(vec1, vec2));
}

INCLUDE_ASM("asm/nonmatchings/xball", func_001216C8);

INCLUDE_ASM("asm/nonmatchings/xball", func_001217C0);

INCLUDE_ASM("asm/nonmatchings/xball", func_00121830);

// Convert 3D direction vector to Euler angles (pitch/yaw)
// Roll is set to 0 - common for camera or character facing calculations
void vector_to_euler_angles(sceVu0FVECTOR direction, sceVu0FVECTOR angles, sceVu0FVECTOR unused) {
    angles[0] = atan2f(-direction[1], vector_magnitude_2d(direction)); // Pitch (up/down)
    angles[1] = atan2f(direction[0], direction[2]);                    // Yaw (left/right)
    angles[2] = 0.0f;                                                  // Roll (always 0)
    angles[3] = 1.0f;
}

INCLUDE_ASM("asm/nonmatchings/xball", func_001218E0);

// Threshold/clamp function with offset
// If value > 0, subtract threshold, clamp to 0 minimum
f32 apply_threshold(f32 value) {
    if (value > 0.0f) {
        value -= D_002B8340[1]; // Subtract threshold value
        if (value < 0.0f) {
            value = 0.0f;
        }
    }
    return value;
}

// Calculate dot product of two vectors flattened to XZ plane (Y = 0)
// Useful for 2D ground-based calculations
void vector_dot_product_2d(sceVu0FVECTOR vec1, sceVu0FVECTOR vec2) {
    sceVu0FVECTOR flattened1;
    sceVu0FVECTOR flattened2;

    sceVu0CopyVector(flattened1, vec1);
    sceVu0CopyVector(flattened2, vec2);
    flattened1[1] = 0.0f; // Remove Y component
    flattened2[1] = 0.0f; // Remove Y component
    vector_dot_product(flattened1, flattened2);
}
