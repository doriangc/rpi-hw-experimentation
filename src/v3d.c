#include "common.h"
#include "mailbox.h"
#include "video.h"
#include "v3d.h"
#include "timer.h"
#include "peripherals/base.h"
#include "printf.h"

typedef struct {
    mailbox_tag tag;
    u32 xres;
    u32 yres;
} mailbox_fb_size;

typedef struct {
    mailbox_tag tag;
    u32 bpp; 
} mailbox_fb_depth;

typedef struct {
    mailbox_tag tag;
    u32 pitch;
} mailbox_fb_pitch;

typedef struct {
    mailbox_tag tag;
    u32 base;
    u32 screen_size;
} mailbox_fb_buffer;

typedef struct {
    mailbox_fb_size res;
    mailbox_fb_size vres; // virtual resolution
    mailbox_fb_depth depth;
    mailbox_fb_buffer buff;
    mailbox_fb_pitch pitch;
} mailbox_fb_request;

const u32 BIN_ADDRESS = 0x00400000;
const u32 BIN_BASE    = 0x00500000;
const u8 AUTO_INITIALIZE_TILE_STATE_DATA_ARRAY = 0x04;

const u8 Enable_Forward_Facing_Primitive = 0x01; // Configuration_Bits: Enable Forward Facing Primitive
const u8 Enable_Reverse_Facing_Primitive = 0x02; // Configuration_Bits: Enable Reverse Facing Primitive
const u8 Clockwise_Primitives            = 0x04; // Configuration_Bits: Clockwise Primitives
const u8 Enable_Depth_Offset             = 0x08; // Configuration_Bits: Enable Depth Offset
const u8 Antialiased_Points_Lines        = 0x10; // Configuration_Bits: Antialiased Points & Lines (Not Actually Supported)
const u8 Coverage_Read_Type_Level_4_8    = 0x00; // Configuration_Bits: Coverage Read Type = 4*8-Bit Level
const u8 Coverage_Read_Type_Mask_16      = 0x20; // Configuration_Bits: Coverage Read Type = 16-Bit Mask
const u8 Rasteriser_Oversample_Mode_None = 0x00; // Configuration_Bits: Rasteriser Oversample Mode = None
const u8 Rasteriser_Oversample_Mode_4X   = 0x40; // Configuration_Bits: Rasteriser Oversample Mode = 4X
const u8 Rasteriser_Oversample_Mode_16X  = 0x80; // Configuration_Bits: Rasteriser Oversample Mode = 16X

const u16 Early_Z_Updates_Enable = 0x0200; // Configuration_Bits: Early Z Updates Enable

const u8 Mode_Triangles = 0x04;

static u8 binning_command[4096] __attribute__((aligned(16)));
static u8 render_command[4096] __attribute__((aligned(16)));

u32 allocateScreenBuffer(u32 xres, u32 yres, u32 bpp) {
    mailbox_fb_request fb_req;

    fb_req.res.tag.id = RPI_FIRMWARE_FRAMEBUFFER_SET_PHYSICAL_WIDTH_HEIGHT;
    fb_req.res.tag.buffer_size = 8;
    fb_req.res.tag.value_length = 8;
    fb_req.res.xres = xres;
    fb_req.res.yres = yres;

    fb_req.vres.tag.id = RPI_FIRMWARE_FRAMEBUFFER_SET_VIRTUAL_WIDTH_HEIGHT;
    fb_req.vres.tag.buffer_size = 8;
    fb_req.vres.tag.value_length = 8;
    fb_req.vres.xres = xres;
    fb_req.vres.yres = yres;

    fb_req.depth.tag.id = RPI_FIRMWARE_FRAMEBUFFER_SET_DEPTH;
    fb_req.depth.tag.buffer_size = 4;
    fb_req.depth.tag.value_length = 4;
    fb_req.depth.bpp = bpp;

    fb_req.buff.tag.id = RPI_FIRMWARE_FRAMEBUFFER_ALLOCATE;
    fb_req.buff.tag.buffer_size = 8;
    fb_req.buff.tag.value_length = 4;
    fb_req.buff.base = 16;
    fb_req.buff.screen_size = 0;

    fb_req.pitch.tag.id = RPI_FIRMWARE_FRAMEBUFFER_GET_PITCH;
    fb_req.pitch.tag.buffer_size = 4;
    fb_req.pitch.tag.value_length = 4;
    fb_req.pitch.pitch = 0;

    mailbox_process((mailbox_tag *)&fb_req, sizeof(fb_req));
    return fb_req.buff.base;
}

void configure_tile_binning_mode() {
    u8* bcl = &binning_command;

    bcl = gen_number_of_layers(bcl, 1);
    bcl = gen_tile_binning_mode_config(bcl, 0, 0, false, false, 0, 1, 800, 600);
    
    bcl = gen_start_tile_binning(bcl);
    bcl = gen_clip_window(bcl, 0, 0, 800, 600);
    bcl = gen_cfg_bits(bcl, 0, 0, false, false, true, true, true, 7, false, false, false, false, false, false, false);
    bcl = gen_viewport_offset(bcl, 400, 300, 0, 0);

    *(reg32*)(PBASE + V3D_BASE + V3D_CT0CA) = (reg32)(void *)&binning_command;
    *(reg32*)(PBASE + V3D_BASE + V3D_CT0EA) = (reg32)(void *)bcl;

    // Hold while not finished
    while(*(reg32*)(PBASE + V3D_BASE + V3D_BFC > 0)) { timer_sleep(2); }
}

void render(u32 buffAddr) {
    u8* rcl = &render_command;

    rcl = gen_tile_rendering_mode_cfg_common(rcl, 1, 800, 600, 1, false, false, false, false, 0, 0, false);
    rcl = gen_tile_rendering_mode_cfg_clear_colors_part1(rcl, 0, 0xFFFF, 0xFFFF);
    rcl = gen_tile_rendering_mode_cfg_zs_clear_values(rcl, 0, 0);
    rcl = gen_tile_list_initial_block_size(rcl, true, 0);

    rcl = gen_multicore_rendering_tile_list_set_base(rcl, buffAddr, 0);
    rcl = gen_multicore_rendering_supertile_cfg(rcl, 1, 1, 13, 10, 13, 10, 1, false, false);
    rcl = gen_tile_coordinates(rcl, 0, 0);
    
    rcl = gen_end_of_loads(rcl);
    rcl = gen_store_tile_buffer_general(rcl, false, 0, 8, 0, 0, 0, false, false, false, 0, 0, 0);
    rcl = gen_clear_tile_buffers(rcl, true, true);
    rcl = gen_end_of_tile_marker(rcl);

    rcl = gen_tile_coordinates(rcl, 0, 0);
    rcl = gen_end_of_loads(rcl);
    rcl = gen_store_tile_buffer_general(rcl, false, 0, 8, 0, 0, 0, false, false, false, 0, 0, 0);
    rcl = gen_end_of_tile_marker(rcl);

    rcl = gen_flush_vcd_cache(rcl);

    for (int y=0; y<10; y++) {
        for (int x=0; x<13; x++) rcl = gen_supertile_coordinates(rcl, y, x);
    }

    *(reg32*)(PBASE + V3D_BASE + V3D_CT1CA) = (reg32)(void*)&render_command;
    *(reg32*)(PBASE + V3D_BASE + V3D_CT1EA) = (reg32)(void*)rcl;
}

void testRun() {
    u32 screenPtr = allocateScreenBuffer(800, 600, 32);
    printf("Done allocating");
    configure_tile_binning_mode();
    printf("Done binning");
    render(screenPtr);
    printf("Done rendering");
}