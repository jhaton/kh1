#include "common.h"
#include "common_funcs.h"

#include "xlamp.h"
#include "xpeanut.h"

#include "gcc/stdio.h"
#include "eekernel.h"
#include "libvu0.h"

// Summon character data structures
typedef struct SummonTip {
    /* 0x00 */ char unk_00[0x48];
    /* 0x48 */ s32 unk_48; // Likely character/world ID
} SummonTip;

typedef struct SummonData {
    /* 0x00 */ s32 unk_00[0x6C / 4];
    /* 0x6C */ SummonTip* tip; // Pointer to summon metadata
} SummonData;

typedef struct SummonStateInner {
    /* 0x00 */ u8 active_flag; // Whether this summon slot is active
    /* 0x01 */ char unk_01[0x7];
} SummonStateInner;

typedef struct SummonState {
    /* 0x00 */ char unk_00[0x98];
    /* 0x98 */ SummonStateInner slots[2]; // Two summon slots available
} SummonState;

typedef struct SummonArray4 {
    /* 0x00 */ s32 unk_00[4]; // Array for 4-element data
} SummonArray4;

typedef union X32 {
    s32 s32;
    u8 u8[4];
} X32;

// Main summon character entity structure
typedef struct SummonCharacter {
    /* 0x000 */ s32 flags;        // Entity state flags
    /* 0x004 */ X32 character_id; // Which summon character this is
    /* 0x008 */ char unk_08[0x8];
    /* 0x010 */ f32 unk_10;
    /* 0x014 */ char unk_14[0x28];
    /* 0x03C */ f32 unk_3C;
    /* 0x040 */ char unk_40[0x90];
    /* 0x0D0 */ sceVu0FMATRIX transform_matrix; // 3D transformation matrix
    /* 0x110 */ char unk_110[0x10];
    /* 0x120 */ sceVu0FVECTOR position; // World position
    /* 0x134 */ char unk_130[0x240];
    /* 0x370 */ u64 status_flags; // Extended status flags
    /* 0x374 */ char unk_378[0xCC];
    /* 0x444 */ s32 animation_state; // Current animation/behavior state
} SummonCharacter;

// Simple entity for fade effects
typedef struct FadeEntity {
    /* 0x00 */ char unk_00[0x30];
    /* 0x30 */ f32 color_rgb[3]; // RGB color values
    /* 0x3C */ f32 alpha;        // Alpha/transparency
} FadeEntity;

// Generic entity structure
typedef struct GameEntity {
    /* 0x00 */ u16 flags;
    /* 0x02 */ char unk_02[0xE];
    /* 0x10 */ f32 timer; // Duration timer
} GameEntity;

// Summon behavior definition
typedef struct SummonBehavior {
    /* 0x00 */ f32 duration;           // How long this summon lasts
    /* 0x04 */ s32 (*behavior_func)(); // Function to call for this summon's behavior
} SummonBehavior;

// Function declarations
s32 func_001009A8();
void* func_00122AF8(s32, s32, s32);
s32 func_00122B70(void*);
extern f32 D_002B8340[];       // Delta time array
extern FadeEntity* D_00532504; // Fade effect entity

// More function declarations for summon system
s32 func_0011EF58(s32*, s32);
f32 func_00120A38(sceVu0FVECTOR);
f32 func_00120A58(f32*);
f32 func_00120AC8(f32);
SummonCharacter* func_001234A0(SummonCharacter*);
void func_00123830(SummonCharacter*, s32);
void func_00123E48(s32, sceVu0FVECTOR);
void func_00124BC8(SummonCharacter*);
void func_00130248(SummonCharacter*, f32, UNK_PTR);
void func_001313A8(SummonCharacter*, UNK_PTR);
void func_00131410(SummonCharacter*);
s32 func_00132160(s32, s32, s32);
void func_001372F8(f32, SummonCharacter*);
void func_00137348(SummonCharacter*);
void func_0013A790(void);
s32 func_0013AFE8(void); // Dumbo summon behavior
s32 func_0013B368(void); // Asset loading behavior
s32 func_0013B6E0(void); // Summon activation behavior
s32 func_0013B890(void); // Cleanup behavior
s32 func_0013B1D0(void);
void func_0013B578(void);
s32 func_0013BD88(s32*, SummonBehavior*, s32);
s32 func_0013BDA0(s32*);
s32 func_00141668(SummonCharacter*, s32);
void func_00157AD8(s32);
s32 func_00157B90(void);
void func_00163638(sceVu0FMATRIX, sceVu0FMATRIX);
void func_00177828(s32);
void func_001778A0(s32);
void func_001778B8(s32);
f32 func_00177D68(void); // Random number generator
void func_001C64E0(UNK_PTR, UNK_PTR, UNK_PTR);
s32 func_001EE090(void);

// Overlay function for additional summon logic
extern s32 func_F20000(s32, s32);

// Global data
extern f32 D_002B8340[]; // Delta time
extern s32 D_002C1E00;
extern sceVu0FMATRIX D_002C1E20; // Base transformation matrix
extern u32 D_002C1EA8;           // System flags
extern u32 D_002C1EC8;           // More system flags
extern SummonData* D_002DEC00;   // Current summon data
extern u32 D_002DED20;

// Summon character file names - these load the actual Disney character assets
char* summon_file_names[8] = {
    "xs_dumbo", // Dumbo (elephant)
    "xs_bambi", // Bambi (deer)
    "xs_genie", // Genie (from Aladdin)
    "xs_tink",  // Tinker Bell (fairy)
    "xs_mushu", // Mushu (dragon from Mulan)
    "xs_simba", // Simba (lion from Lion King)
};

// Camera/positioning vectors for summon appearances
sceVu0FVECTOR summon_start_pos = {0.0f, -120.0f, -70.0f, 1.0f};  // Starting position
sceVu0FVECTOR summon_target_pos = {0.0f, -100.0f, -20.0f, 1.0f}; // Target position

// File extensions for summon assets
char* data_extension = ".dbt"; // Data/behavior file
char* model_extension = ".x";  // Model file

// Summon behavior table - defines duration and behavior function for each summon type
SummonBehavior summon_behaviors[4] = {
    {60.0f, func_0013AFE8}, // 60-second duration summons (Dumbo)
    {30.0f, func_0013B368}, // 30-second duration (asset loading phase)
    {60.0f, func_0013B6E0}, // 60-second duration (activation phase)
    {0.0f, func_0013B890},  // Cleanup phase (no duration limit)
};

char* dat_extension; // = ".dat";  // Additional data file extension

// More global variables
extern s32 D_00301080;
extern s32 D_00301088;
extern f32 D_0030108C;
extern f32 D_00301090;
extern s32 D_00301094;
extern SummonState* D_003051EC; // Global summon state
extern u32 D_00375BC0;

// Global summon system state variables
extern f32 interpolation_factor_1;
extern f32 interpolation_factor_2;
extern f32 interpolation_factor_3;
extern sceVu0FVECTOR working_vector_1;
extern sceVu0FVECTOR working_vector_2;
extern sceVu0FVECTOR working_vector_3;
extern sceVu0FVECTOR working_vector_4;
extern f32 current_timer;
extern s32 current_state;
extern s32 behavior_index;
extern s32 animation_timer;
extern SummonCharacter* active_summon;    // Currently active main summon
extern SummonCharacter* secondary_summon; // Secondary summon character
extern sceVu0FVECTOR saved_position;
extern s32 system_state;
extern s32 loading_state;
extern SummonArray4* loaded_data;
extern s32* asset_pointers;
extern s32 current_summon_id;

/**
 * Main summon update function - handles positioning and animation interpolation
 * This is called every frame while a summon is active
 */
s32 update_summon_position(u16* input_flags) {
    sceVu0FVECTOR temp_pos;
    sceVu0FVECTOR temp_target;
    sceVu0FMATRIX temp_matrix;
    sceVu0FMATRIX rotation_matrix;

    f32 blend_factor;
    f32 rotation_y;
    f32 distance_scale;
    f32 temp_var1;
    f32 temp_var2;

    // Check if summon system is enabled
    if (((D_002C1EA8 >> 20) & 1) == 0) {
        return 4; // Exit if summons disabled
    }

    // Initialize summon positioning on first frame
    if (*input_flags & 0x10) {
        // Get current player/camera transform
        func_00163638(active_summon->transform_matrix, rotation_matrix);
        D_002DECF0[3] = 1.0f;
        D_002DECE0[3] = 1.0f;

        // Transform reference positions to world space
        sceVu0ApplyMatrix(working_vector_2, rotation_matrix, D_002DECE0);
        sceVu0ApplyMatrix(working_vector_3, rotation_matrix, D_002DECF0);

        // Calculate distance from target to current position
        sceVu0SubVector(working_vector_1, summon_target_pos, working_vector_2);
        interpolation_factor_2 = func_00120A38(working_jector_1);

        // Calculate direction vector and normalize
        sceVu0SubVector(working_vector_1, working_vector_3, working_vector_2);
        sceVu0Normalize(working_vector_1, working_vector_1);
        sceVu0ScaleVector(working_vector_1, working_vector_1, interpolation_factor_2);
        sceVu0AddVector(working_vector_3, working_vector_1, working_vector_2);

        // Calculate ground-plane movement vector
        sceVu0SubVector(working_vector_4, working_vector_2, working_vector_3);
        working_vector_4[1] = 0.0f; // Remove Y component for ground movement
        func_001223B0(working_vector_4, working_vector_4);
        interpolation_factor_2 = working_vector_4[3];
        working_vector_4[3] = 1.0f;

        // Calculate distance difference between start and target
        sceVu0SubVector(working_vector_1, summon_start_pos, summon_target_pos);
        interpolation_factor_3 = func_00120A58(working_vector_1) - interpolation_factor_2;

        // Calculate rotation angles for movement
        temp_var1 = atan2f(working_vector_2[0] - working_vector_3[0], working_vector_2[2] - working_vector_3[2]);
        temp_var2 = atan2f(summon_start_pos[0] - summon_target_pos[0], summon_start_pos[2] - summon_target_pos[2]);
        interpolation_factor_1 = func_00120AC8(temp_var2 - temp_var1);

        // Add some randomness to the movement
        if (func_00177D68() < 0.5f) {
            if (interpolation_factor_1 < 0.0f) {
                interpolation_factor_1 += 2 * PI;
            } else {
                interpolation_factor_1 -= 2 * PI;
            }
        }

        // Add slight random variation to angle
        if (func_00177D68() < 0.5f) {
            interpolation_factor_1 += PI / 6; // +30 degrees
        } else {
            interpolation_factor_1 -= PI / 6; // -30 degrees
        }

        current_timer = 40.0f; // Set animation duration
    }

    // Update animation timer
    current_timer -= D_002B8340[1]; // Subtract delta time
    blend_factor = current_timer / 40.0f;
    if (blend_factor < 0.0f) {
        blend_factor = 0.0f;
    }

    // Create smooth easing curve for animation
    blend_factor = (cosf(blend_factor * PI) + 1.0f) * 0.5f;

    // Interpolate position between target and final position
    sceVu0InterVector(temp_target, summon_target_pos, working_vector_3, blend_factor);

    // Calculate current rotation
    rotation_y = func_00120AC8(interpolation_factor_1 * blend_factor);
    sceVu0RotMatrixY(temp_matrix, D_002C1E20, rotation_y);

    // Apply rotation to movement vector
    sceVu0ApplyMatrix(working_vector_1, temp_matrix, working_vector_4);

    // Scale movement based on animation progress
    distance_scale = interpolation_factor_2 + interpolation_factor_3 * blend_factor;
    sceVu0ScaleVector(working_vector_1, working_vector_1, distance_scale);

    // Calculate final position
    sceVu0AddVector(temp_pos, temp_target, working_vector_1);
    temp_pos[3] = 1.0f;
    temp_target[3] = 1.0f;

    // Interpolate Y position separately for smooth vertical movement
    temp_pos[1] = (summon_start_pos[1] * blend_factor) + (working_vector_2[1] * (1.0f - blend_factor));

    // Transform final positions back to local space
    sceVu0ApplyMatrix(D_002DECE0, active_summon->transform_matrix, temp_pos);
    sceVu0ApplyMatrix(D_002DECF0, active_summon->transform_matrix, temp_target);

    return 0; // Continue animation
}

INCLUDE_ASM("asm/nonmatchings/xapple", func_0013AFE8); // Dumbo summon behavior

/**
 * Clean up summon entities after use
 */
void cleanup_summon_entities(void) {
    // Clean up different asset types loaded for summons
    if (asset_pointers[2] - asset_pointers[1] > 0) {
        func_001560C8((s32)asset_pointers + asset_pointers[1]); // Clean up type 1 assets
    }
    if (asset_pointers[3] - asset_pointers[2] > 0) {
        func_001560C8((s32)asset_pointers + asset_pointers[2]); // Clean up type 2 assets
    }
    if (asset_pointers[4] - asset_pointers[3] > 0) {
        func_00177908((s32)asset_pointers + asset_pointers[3]); // Clean up type 3 assets
    }
    FlushCache(INVALIDATE_ICACHE); // Invalidate instruction cache after cleanup
    loading_state = 4;             // Mark as cleaned up
}

INCLUDE_ASM("asm/nonmatchings/xapple", func_0013B1D0); // Setup summon entities

/**
 * Load summon assets - loads model, animation, and behavior data for a summon character
 */
s32 load_summon_assets(void) {
    char filename_buffer[0x20];

    // Check if we're in the right state and no other loading is happening
    if (loading_state != 2 || (func_001EE090() != 0)) {
        return 0; // Not ready to load
    }

    func_00157B90();   // Prepare for loading
    loading_state = 3; // Set to loading state

    // Load behavior data file (.dbt)
    sprintf(filename_buffer, "%s%s", summon_file_names[system_state], data_extension);
    func_00120590(filename_buffer, asset_pointers, NULL, 0);

    // Load model file (.x)
    sprintf(filename_buffer, "%s%s", summon_file_names[system_state], model_extension);
    func_00120590(filename_buffer, &func_F20000, &cleanup_summon_entities, 0);

    // Schedule entity setup function
    func_0011ED30(47000, &func_0013B1D0);

    return 1; // Loading started successfully
}

INCLUDE_ASM("asm/nonmatchings/xapple", func_0013B480); // Fade effect behavior

INCLUDE_ASM("asm/nonmatchings/xapple", func_0013B578); // Summon cleanup and reset

/**
 * Activate loaded summon - called after assets are loaded to make summon appear
 */
s32 activate_summon(void) {
    SummonCharacter* current_entity;

    // Check if system is busy or not ready
    if (((D_00375BC0 >> 2) & 1) != 0 || loading_state != 4 || func_001EE090() != 0) {
        return 0; // Not ready to activate
    }

    func_0013B578(); // Clean up any previous summon state
    secondary_summon = NULL;
    current_entity = NULL;

    // Find the secondary summon character entity (character ID 7)
    do {
        current_entity = func_001234A0(current_entity); // Get next summon entity
        if (current_entity == NULL) {
            break;
        }
        if (current_entity->character_id.u8[2] == 7) { // Check if this is character type 7
            secondary_summon = current_entity;
            break;
        }
    } while (TRUE);

    // Set up secondary summon if found
    if (secondary_summon != NULL) {
        if ((asset_pointers[2] - asset_pointers[1]) > 0) {
            func_001313A8(secondary_summon, (s32)asset_pointers + asset_pointers[1]);
            func_00130248(secondary_summon, 0.0f, 0); // Reset animation state
            secondary_summon->flags &= ~0x30;         // Clear visibility flags
        }
        secondary_summon->animation_state = 0; // Reset animation
    }

    // Set up main summon based on game state
    if ((((u32)D_002C1EC8 >> 8) % 4) != 1) { // Normal activation
        if (asset_pointers[3] - asset_pointers[2] > 0) {
            func_001313A8(active_summon, (s32)asset_pointers + asset_pointers[2]);
            func_00130248(active_summon, 0.0f, 0); // Reset animation state
        }
        // Set up overlay behavior if available
        func_F20000(
            0, ((asset_pointers[4] - asset_pointers[3]) <= 0) ? NULL : ((s32)asset_pointers + asset_pointers[3])
        );
    } else {                                            // Special activation mode
        func_00124BC8(secondary_summon);                // Special setup for secondary
        func_00131410(active_summon);                   // Special setup for main
        func_00130248(active_summon, 0.0f, 0x40000000); // Special animation mode
    }

    return 1; // Activation successful
}

/**
 * Final cleanup - called when summon duration expires
 */
s32 finish_summon(void) {
    // Check if we're in special mode or overlay is still running
    if ((((D_002C1EC8 >> 8) % 4) == 1) || (func_F20000(1, 0) == 0)) {
        func_0013A790();       // Final cleanup
        D_002C1EA8 &= ~0x2000; // Clear summon system flag
        D_00301088 = 0;        // Reset summon state
        return 1;              // Finished
    }
    return 0; // Still running
}

/**
 * Main summon system update - manages the state machine for summon lifecycle
 */
s32 update_summon_system(u16* input_flags) {
    // Initialize summon behavior state machine on first call
    if (*input_flags & 0x10) {
        func_0013BD88(&behavior_index, summon_behaviors, 4); // Set up behavior table
    }

    func_0011EF58(&animation_timer, 0); // Update animation timers

    // Continue until behavior state machine is complete
    return (func_0013BDA0(&behavior_index) != 0) ? 0 : 4;
}

/**
 * Load summon configuration data
 */
void load_summon_config(s32 character_id) {
    func_00132160(
        character_id,
        (s32)loaded_data->unk_00 + loaded_data->unk_00[1], // Pointer to character data
        ((loaded_data->unk_00[3] - loaded_data->unk_00[2]) <= 0)
            ? 0
            : ((s32)loaded_data->unk_00 + loaded_data->unk_00[2]) // Optional extra data
    );
}

/**
 * Callback when configuration loading is complete
 */
void on_config_loaded(void) {
    loading_state = 2; // Ready for asset loading
}

INCLUDE_ASM("asm/nonmatchings/xapple", func_0013B9B8); // Asset loading completion handler

/**
 * Calculate summon power/cost based on character and world state
 */
f32 calculate_summon_cost(void) {
    return (D_002DEC00->tip->unk_48 + ((s32)(D_002DED20 >> 0xA) & 7)) * 3000;
}

/**
 * Initialize a summon character - main entry point for summoning
 * @param summon_entity The summon character entity to initialize
 * @param character_id Which Disney character to summon (0-5)
 */
void* initialize_summon(s32 summon_entity, s32 character_id) {
    char filename_buffer[0x20];

    // Enable summon system
    D_002C1EA8 |= 0x102000; // Set summon active flags

    // Set up character ID and state
    D_00301088 = (D_00301088 & (~7)) | (character_id & 7) | 8;
    D_00301080 = &D_003051EC->slots[character_id].active_flag; // Point to character's slot
    D_00301094 |= 1 << character_id;                           // Mark character as available

    // Calculate and store summon cost
    D_0030108C = D_00301090 = calculate_summon_cost();

    // Set up the summon entity
    func_00141668(summon_entity, -*(&D_003051EC->slots[character_id].active_flag));
    active_summon = summon_entity;
    system_state = character_id;

    // Prepare rendering systems
    func_00157AD8(1); // Initialize render state 1
    func_00157AD8(2); // Initialize render state 2
    func_00157AD8(3); // Initialize render state 3

    loading_state = 0; // Start loading process

    // Allocate memory for asset data
    loaded_data = func_00155ED8(0x34, 0xC);

    // Load character configuration file
    sprintf(filename_buffer, "%s%s", summon_file_names[character_id], dat_extension);
    func_00120590(filename_buffer, loaded_data, func_0013B9B8, 0);

    // Set up behavior system timing
    func_0011EDD0(&animation_timer, &animation_timer, 0x18, 8);

    // Schedule the main update function
    return func_0011ED30(52000, update_summon_system);
}

/**
 * Emergency cleanup for summon system - called when player exits area, etc.
 */
void emergency_summon_cleanup(void) {
    if ((D_002C1EA8 >> 13) & 1) {            // If summon system is active
        func_F20000(2, 0);                   // Emergency cleanup overlay call
        func_0011ED80(update_summon_system); // Remove update function
        func_0023E0B0(0, 1);                 // Reset render state 0->1
        func_0023E0B0(1, 2);                 // Reset render state 1->2
        func_0023E0B0(2, 2);                 // Reset render state 2->2
    }
}
