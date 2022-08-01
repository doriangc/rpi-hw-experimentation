#pragma once

#include "common.h"
#include "mem.h"

void testRun();

typedef struct {
    u8 control_id;
    u8 num_layers;
} number_of_layers;

// static inline int64_t llroundf(float num)
// {
//     return num < 0 ? num - 0.5 : num + 0.5;
// }

// static inline uint64_t __gen_sfixed(float v, uint32_t start, uint32_t end, uint32_t fract_bits) {
//     const float factor = (1 << fract_bits);
//     const int64_t int_val = llroundf(v * factor);
//     const uint64_t mask = ~0ull >> (64 - (end - start + 1));

//     return (int_val & mask) << start;
// }

// Based on OpenGL driver pack functionality (see Mesa project)

static inline u8* gen_start_tile_binning(u8* cl) {
    const u8 OPCODE = 6;
    cl[0] = OPCODE;

    return cl + 1;
}

static inline u8* gen_flush_vcd_cache(u8* cl) {
    const u8 OPCODE = 19;
    cl[0] = OPCODE;

    return cl + 1;
}

static inline u8* gen_supertile_coordinates(
    u8* cl,
    u8 row_number_in_supertiles,
    u8 column_number_in_supertiles
) {
    const u8 OPCODE = 23;

    cl[0] = OPCODE;

    cl[1] = row_number_in_supertiles;
    cl[2] = column_number_in_supertiles;
}

static inline u8* gen_clear_tile_buffers(
    u8* cl,
    bool clear_z_stencil_buffer,
    bool clear_all_render_targets
) {
    const u8 OPCODE = 25;

    cl[0] = OPCODE;
    cl[1] = clear_z_stencil_buffer << 1 | clear_all_render_targets;

    return cl + 2;
}

static inline u8* gen_end_of_loads(u8* cl) {
    const u8 OPCODE = 26;
    cl[0] = OPCODE;

    return cl + 1;
}

static inline u8* gen_end_of_tile_marker(u8* cl) {
    const u8 OPCODE = 27;
    cl[0] = OPCODE;

    return cl + 1;
}

static inline u8* gen_store_tile_buffer_general(
    u8* cl,
    bool flip_y,
    u8 memory_format,
    u8 buffer_to_store,
    u8 output_image_format,
    u8 decimate_mode,
    u8 dither_mode,
    bool r_b_swap,
    bool channel_reverse,
    bool clear_buffer_being_stored,
    u32 height_in_ub_or_stride,
    u16 height,
    u32 address
) {
    const u8 OPCODE = 29;

    cl[0] = OPCODE;

    cl[1] = flip_y << 7 | memory_format << 4 | buffer_to_store;
    cl[2] = output_image_format << 4 | decimate_mode << 2 | dither_mode;
    cl[3] = r_b_swap << 4 | channel_reverse << 3 | clear_buffer_being_stored << 2 | output_image_format >> 4;
    
    cl[4] = height_in_ub_or_stride;
    cl[5] = height_in_ub_or_stride >> 8;
    cl[6] = height_in_ub_or_stride >> 16;

    cl[7] = height;
    cl[8] = height >> 8;

    cl[9] = address;
    cl[10] = address >> 8;
    cl[11] = address >> 16;
    cl[12] = address >> 24;

    return cl + 13;
}

static inline u8* gen_cfg_bits(
    u8* cl,
    u8 rasterizer_oversample_mode,
    u8 line_rasterization,
    bool enable_depth_offset,
    bool clockwise_primitives,
    bool enable_reverse_facing_primitive,
    bool enable_forward_facing_primitive,
    bool z_updates_enable,
    u8 depth_test_function,
    bool direct3d_wireframe_triangles_mode,
    bool direct3d_provoking_vertex,
    bool direct3d_point_fill_mode,
    bool blend_enable,
    bool stencil_enable,
    bool early_z_updates_enable,
    bool early_z_enable
) {
    const u8 OPCODE = 96;

    cl[0] = OPCODE;
    cl[1] = rasterizer_oversample_mode << 6 | line_rasterization << 4 | enable_depth_offset << 3 |
            clockwise_primitives << 2 | enable_reverse_facing_primitive << 1 | enable_forward_facing_primitive;
    cl[2] = z_updates_enable << 7 | depth_test_function << 4 | direct3d_wireframe_triangles_mode << 3;
    cl[3] = direct3d_provoking_vertex << 5 | direct3d_point_fill_mode << 4 | blend_enable << 3 |
            stencil_enable << 2 | early_z_updates_enable << 1 | early_z_enable;

    return cl + 4;
}

static inline u8* gen_clip_window(
    u8* cl,
    u16 left_pixel_coordinate,
    u16 bottom_pixel_coordinate,
    u16 width_in_pixels,
    u16 height_in_pixels
) {
    const u8 OPCODE = 107;
    
    cl[0] = OPCODE;
    
    cl[1] = left_pixel_coordinate;
    cl[2] = left_pixel_coordinate >> 8;

    cl[3] = bottom_pixel_coordinate;
    cl[4] = bottom_pixel_coordinate >> 8;

    cl[5] = width_in_pixels;
    cl[6] = width_in_pixels >> 8;

    cl[7] = height_in_pixels;
    cl[8] = height_in_pixels >> 8;

    return cl + 9;
}

// static inline u8* gen_viewport_offset(
//     u8* cl,
//     float viewport_center_x_coordinate,
//     float viewport_center_y_coordinate,
//     u16 coarse_x,
//     u16 coarse_y
// ) {
//     const u8 OPCODE = 108;

//     // Convert to fixed point (only way GPU understands floats)
//     u64 fixed_point_viewport_center_x_coordinate = __gen_sfixed(viewport_center_x_coordinate, 0, 21, 8);
//     u64 fixed_point_viewport_center_y_coordinate = __gen_sfixed(viewport_center_y_coordinate, 0, 21, 8);

//     cl[0] = OPCODE;
    
//     cl[1] = fixed_point_viewport_center_x_coordinate;
//     cl[2] = fixed_point_viewport_center_x_coordinate >> 8;
//     cl[3] = coarse_x << 6 | (fixed_point_viewport_center_x_coordinate >> 16);
//     cl[4] = coarse_x >> 2;

//     cl[5] = fixed_point_viewport_center_y_coordinate;
//     cl[6] = fixed_point_viewport_center_y_coordinate >> 8;
//     cl[7] = coarse_y << 6 | (fixed_point_viewport_center_y_coordinate >> 16);
//     cl[8] = coarse_y >> 2;

//     return cl + 9;
// }

static inline u8* gen_number_of_layers(
    u8* cl,
    u8 number_of_layers
) {
    const u8 OPCODE = 119;

    cl[0] = OPCODE;
    cl[1] = number_of_layers - 1;
    return cl + 2;
}

static inline u8* gen_tile_binning_mode_config(
    u8* cl,
    u8 tile_alloc_block_size,
    u8 tile_alloc_initial_block_size,
    bool double_buffer_in_non_ms_mode,
    bool multisample_mode_4x,
    u8 maximum_bpp_of_all_render_targets,
    u8 num_render_targets,
    u16 width_in_pixels,
    u16 height_in_pixels
) {
    // We are assuming that some of these values MUST be smaller than 8 bits so they don't overwrite others during OR,
    // but we can't pass in a data type smaller than one byte (except bools).
    const u8 OPCODE = 120;
    
    cl[0] = OPCODE;
    cl[1] = tile_alloc_block_size << 4 | tile_alloc_initial_block_size << 2;
    cl[2] = double_buffer_in_non_ms_mode << 7 | multisample_mode_4x << 6 | maximum_bpp_of_all_render_targets << 4 | (num_render_targets - 1);
    cl[3] = 0;
    cl[4] = 0;

    cl[5] = (width_in_pixels - 1);
    cl[6] = (width_in_pixels - 1) >> 8;

    cl[7] = (height_in_pixels - 1);
    cl[8] = (height_in_pixels - 1) >> 8;

    return cl + 9;
}

// RENDERING Control List items

static inline u8* gen_tile_rendering_mode_cfg_common(
    u8* cl,
    u8 number_of_render_targets,
    u16 image_width_pixels,
    u16 image_height_pixels,
    u8 internal_depth_type,
    bool early_z_disable,
    bool early_z_test_and_update_direction,
    bool double_buffer_in_non_ms_mode,
    bool multisample_mode_4x,
    u8 maximum_bpp_of_all_render_targets,
    u16 pad,
    bool early_depth_stencil_clear
) {
    const u8 OPCODE = 121;
    const u8 SUB_ID = 0;

    cl[0] = OPCODE;
    cl[1] = (number_of_render_targets - 1) << 4 | SUB_ID;
    
    cl[2] = image_width_pixels;
    cl[3] = image_width_pixels >> 8;

    cl[4] = image_height_pixels;
    cl[5] = image_height_pixels >> 8;

    cl[6] = internal_depth_type << 7 | early_z_disable << 6 | early_z_test_and_update_direction << 5 |
            double_buffer_in_non_ms_mode << 3 | multisample_mode_4x << 2 | maximum_bpp_of_all_render_targets;

    cl[7] = pad << 4 | early_depth_stencil_clear << 3 | internal_depth_type >> 1;

    cl[8] = pad >> 8;

    return cl + 9;
}

// static inline u8* gen_tile_rendering_mode_cfg_zs_clear_values(
//     u8* cl,
//     u8 stencil_clear_val,
//     float z_clear_val
// ) {
//     const u8 OPCODE = 121;
//     const u8 SUB_ID = 2;

//     cl[0] = OPCODE;
//     cl[1] = SUB_ID;
//     cl[2] = stencil_clear_val;
    
//     memcpy(&cl[3], &z_clear_val, sizeof(z_clear_val));

//     // UNUSED values
//     cl[7] = 0;
//     cl[8] = 0;

//     return cl + 9;
// }

static inline u8* gen_tile_rendering_mode_cfg_clear_colors_part1(
    u8* cl,
    u8 render_target_number,
    u32 clear_color_low_32_bits,
    u32 clear_color_next_24_bits
) {
    const u8 OPCODE = 121;
    const u8 SUB_ID = 3;

    cl[0] = OPCODE;
    cl[1] = render_target_number << 4 | SUB_ID;

    cl[2] = clear_color_low_32_bits;
    cl[3] = clear_color_low_32_bits >> 8;
    cl[4] = clear_color_low_32_bits >> 16;
    cl[5] = clear_color_low_32_bits >> 24;

    cl[6] = clear_color_next_24_bits;
    cl[7] = clear_color_next_24_bits >> 8;
    cl[8] = clear_color_next_24_bits >> 16;

    return cl + 9;
}

static inline u8* gen_multicore_rendering_supertile_cfg(
    u8* cl,
    u8 supertile_width_in_tiles,
    u8 supertile_height_in_tiles,
    u8 total_frame_width_in_supertiles,
    u8 total_frame_height_in_supertiles,
    u16 total_frame_width_in_tiles,
    u16 total_frame_height_in_tiles,
    u8 number_of_bin_tile_lists,
    bool supertile_raster_order,
    bool multicore_enable
) {
    const u8 OPCODE = 122;

    cl[0] = OPCODE;

    cl[1] = supertile_width_in_tiles - 1;
    cl[2] = supertile_height_in_tiles - 1;

    cl[3] = total_frame_width_in_supertiles;
    cl[4] = total_frame_height_in_supertiles;

    cl[5] = total_frame_width_in_tiles;
    cl[6] = total_frame_height_in_tiles << 4 | total_frame_width_in_supertiles >> 8;
    cl[7] = total_frame_height_in_tiles >> 4;

    cl[8] = (number_of_bin_tile_lists - 1) << 5 | supertile_raster_order << 4 | multicore_enable;

    return cl + 9;
}

static inline u8* gen_multicore_rendering_tile_list_set_base(
    u8* cl,
    u32 address,
    u8 tile_list_set_number
) {
    const u8 OPCODE = 123;

    cl[0] = OPCODE;

    // Bit of a weird one, this must assume that the lower 4 bits of the address are 0?
    cl[1] = address | tile_list_set_number;

    cl[2] = address >> 8;
    cl[3] = address >> 16;
    cl[4] = address >> 24;

    return cl + 5;
}

static inline u8* gen_tile_coordinates(
    u8* cl,
    u16 tile_column_number,
    u16 tile_row_number
) {
    const u8 OPCODE = 124;

    cl[0] = OPCODE;
    
    cl[1] = tile_column_number;
    cl[2] = tile_row_number << 4 | tile_column_number >> 8;
    cl[3] = tile_row_number >> 4;

    return cl + 4;
}

static inline u8* gen_tile_list_initial_block_size(
    u8* cl,
    bool use_auto_chained_tile_lists,
    u8 size_of_first_block_in_chained_tile_lists
) {
    const u8 OPCODE = 126;

    cl[0] = OPCODE;
    cl[1] = use_auto_chained_tile_lists << 2 | size_of_first_block_in_chained_tile_lists;

    return cl + 2;
}

extern const u8 NV_SHADER_STATE_RECORD[];
extern const u8 VERTEX_DATA[];
extern const u8 FRAGMENT_SHADER_CODE[];

#define V3D_BASE 0x00C04000
#define V3D_IDENT0  0x00000 // V3D Identification 0 (V3D Block Identity)
#define V3D_IDENT1  0x00004 // V3D Identification 1 (V3D Configuration A)
#define V3D_IDENT2  0x00008 // V3D Identification 2 (V3D Configuration B)
#define V3D_IDENT3  0x0000C // V3D Identification 3 (V3D Configuration C)
#define V3D_SCRATCH 0x00010 // V3D Scratch Register
#define V3D_L2CACTL 0x00020 // V3D L2 Cache Control
#define V3D_SLCACTL 0x00024 // V3D Slices Cache Control
#define V3D_INTCTL  0x00030 // V3D Interrupt Control
#define V3D_INTENA  0x00034 // V3D Interrupt Enables
#define V3D_INTDIS  0x00038 // V3D Interrupt Disables
#define V3D_CT0CS   0x00100 // V3D Control List Executor Thread 0 Control & Status
#define V3D_CT1CS   0x00104 // V3D Control List Executor Thread 1 Control & Status
#define V3D_CT0EA   0x00108 // V3D Control List Executor Thread 0 End Address
#define V3D_CT1EA   0x0010C // V3D Control List Executor Thread 1 End Address
#define V3D_CT0CA   0x00110 // V3D Control List Executor Thread 0 Current Address
#define V3D_CT1CA   0x00114 // V3D Control List Executor Thread 1 Current Address
#define V3D_CT0RA0  0x00118 // V3D Control List Executor Thread 0 Return Address
#define V3D_CT1RA0  0x0011C // V3D Control List Executor Thread 1 Return Address
#define V3D_CT0LC   0x00120 // V3D Control List Executor Thread 0 List Counter
#define V3D_CT1LC   0x00124 // V3D Control List Executor Thread 1 List Counter
#define V3D_CT0PC   0x00128 // V3D Control List Executor Thread 0 Primitive List Counter
#define V3D_CT1PC   0x0012C // V3D Control List Executor Thread 1 Primitive List Counter
#define V3D_PCS     0x00130 // V3D Pipeline Control & Status
#define V3D_BFC     0x00134 // V3D Binning Mode Flush Count
#define V3D_RFC     0x00138 // V3D Rendering Mode Frame Count
#define V3D_BPCA    0x00300 // V3D Current Address Of Binning Memory Pool
#define V3D_BPCS    0x00304 // V3D Remaining Size Of Binning Memory Pool
#define V3D_BPOA    0x00308 // V3D Address Of Overspill Binning Memory Block
#define V3D_BPOS    0x0030C // V3D Size Of Overspill Binning Memory Block
#define V3D_BXCF    0x00310 // V3D Binner Debug
#define V3D_SQRSV0  0x00410 // V3D Reserve QPUs 0-7
#define V3D_SQRSV1  0x00414 // V3D Reserve QPUs 8-15
#define V3D_SQCNTL  0x00418 // V3D QPU Scheduler Control
#define V3D_SQCSTAT 0x0041C // V3D QPU Scheduler State
#define V3D_SRQPC   0x00430 // V3D QPU User Program Request Program Address
#define V3D_SRQUA   0x00434 // V3D QPU User Program Request Uniforms Address
#define V3D_SRQUL   0x00438 // V3D QPU User Program Request Uniforms Length
#define V3D_SRQCS   0x0043C // V3D QPU User Program Request Control & Status
#define V3D_VPACNTL 0x00500 // V3D VPM Allocator Control
#define V3D_VPMBASE 0x00504 // V3D VPM Base (User) Memory Reservation
#define V3D_PCTRC   0x00670 // V3D Performance Counter Clear
#define V3D_PCTRE   0x00674 // V3D Performance Counter Enables
#define V3D_PCTR0   0x00680 // V3D Performance Counter Count 0
#define V3D_PCTRS0  0x00684 // V3D Performance Counter Mapping 0
#define V3D_PCTR1   0x00688 // V3D Performance Counter Count 1
#define V3D_PCTRS1  0x0068C // V3D Performance Counter Mapping 1
#define V3D_PCTR2   0x00690 // V3D Performance Counter Count 2
#define V3D_PCTRS2  0x00694 // V3D Performance Counter Mapping 2
#define V3D_PCTR3   0x00698 // V3D Performance Counter Count 3
#define V3D_PCTRS3  0x0069C // V3D Performance Counter Mapping 3
#define V3D_PCTR4   0x006A0 // V3D Performance Counter Count 4
#define V3D_PCTRS4  0x006A4 // V3D Performance Counter Mapping 4
#define V3D_PCTR5   0x006A8 // V3D Performance Counter Count 5
#define V3D_PCTRS5  0x006AC // V3D Performance Counter Mapping 5
#define V3D_PCTR6   0x006B0 // V3D Performance Counter Count 6
#define V3D_PCTRS6  0x006B4 // V3D Performance Counter Mapping 6
#define V3D_PCTR7   0x006B8 // V3D Performance Counter Count 7
#define V3D_PCTRS7  0x006BC // V3D Performance Counter Mapping 7
#define V3D_PCTR8   0x006C0 // V3D Performance Counter Count 8
#define V3D_PCTRS8  0x006C4 // V3D Performance Counter Mapping 8
#define V3D_PCTR9   0x006C8 // V3D Performance Counter Count 9
#define V3D_PCTRS9  0x006CC // V3D Performance Counter Mapping 9
#define V3D_PCTR10  0x006D0 // V3D Performance Counter Count 10
#define V3D_PCTRS10 0x006D4 // V3D Performance Counter Mapping 10
#define V3D_PCTR11  0x006D8 // V3D Performance Counter Count 11
#define V3D_PCTRS11 0x006DC // V3D Performance Counter Mapping 11
#define V3D_PCTR12  0x006E0 // V3D Performance Counter Count 12
#define V3D_PCTRS12 0x006E4 // V3D Performance Counter Mapping 12
#define V3D_PCTR13  0x006E8 // V3D Performance Counter Count 13
#define V3D_PCTRS13 0x006EC // V3D Performance Counter Mapping 13
#define V3D_PCTR14  0x006F0 // V3D Performance Counter Count 14
#define V3D_PCTRS14 0x006F4 // V3D Performance Counter Mapping 14
#define V3D_PCTR15  0x006F8 // V3D Performance Counter Count 15
#define V3D_PCTRS15 0x006FC // V3D Performance Counter Mapping 15
#define V3D_DBCFG   0x00E00 // V3D Configure
#define V3D_DBSCS   0x00E04 // V3D S Control & Status
#define V3D_DBSCFG  0x00E08 // V3D S Configure
#define V3D_DBSSR   0x00E0C // V3D S SR
#define V3D_DBSDR0  0x00E10 // V3D SD R0
#define V3D_DBSDR1  0x00E14 // V3D SD R1
#define V3D_DBSDR2  0x00E18 // V3D SD R2
#define V3D_DBSDR3  0x00E1C // V3D SD R3
#define V3D_DBQRUN  0x00E20 // V3D QPU Run
#define V3D_DBQHLT  0x00E24 // V3D QPU Halt
#define V3D_DBQSTP  0x00E28 // V3D QPU Step
#define V3D_DBQITE  0x00E2C // V3D QPU Interrupt Enables
#define V3D_DBQITC  0x00E30 // V3D QPU Interrupt Control
#define V3D_DBQGHC  0x00E34 // V3D QPU GHC
#define V3D_DBQGHG  0x00E38 // V3D QPU GHG
#define V3D_DBQGHH  0x00E3C // V3D QPU GHH
#define V3D_DBGE    0x00F00 // V3D PSE Error Signals
#define V3D_FDBGO   0x00F04 // V3D FEP Overrun Error Signals
#define V3D_FDBGB   0x00F08 // V3D FEP Interface Ready & Stall Signals, FEP Busy Signals
#define V3D_FDBGR   0x00F0C // V3D FEP Internal Ready Signals
#define V3D_FDBGS   0x00F10 // V3D FEP Internal Stall Input Signals
#define V3D_ERRSTAT 0x00F20 // V3D Miscellaneous Error Signals (VPM, VDW, VCD, VCM, L2C)