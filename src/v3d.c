#include "common.h"
#include "mailbox.h"
#include "video.h"
#include "v3d.h"
#include "timer.h"
#include "peripherals/base.h"

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

static mailbox_fb_request fb_req;

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

u32 allocateScreenBuffer(u32 xres, u32 yres, u32 bpp) {
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

void configureTileBinningMode() {
    binning_command control_list;

    control_list.config.control_id = 112;
    control_list.config.address = BIN_ADDRESS; // This might not be right (Endianness)
    control_list.config.size = 0x2000;
    control_list.config.base_address = BIN_BASE;
    control_list.config.width = 30;
    control_list.config.height = 17;
    control_list.config.data = AUTO_INITIALIZE_TILE_STATE_DATA_ARRAY;

    control_list.start_binning = 6;

    control_list.clip.control_id = 102;
    control_list.clip.left = 0;
    control_list.clip.bottom = 0;
    control_list.clip.width = 640;
    control_list.clip.height = 480;

    control_list.cBits.control_id = 96;
    control_list.cBits.data8 = Enable_Forward_Facing_Primitive + Enable_Reverse_Facing_Primitive;
    control_list.cBits.data16 = Early_Z_Updates_Enable;

    control_list.viewport.control_id = 103;
    control_list.viewport.x = 0;
    control_list.viewport.y = 0;

    control_list.state.control_id = 65;
    control_list.state.address = &NV_SHADER_STATE_RECORD; // address for the fragment shader

    control_list.primitive_data.control_id = 33;
    control_list.primitive_data.data = Mode_Triangles;
    control_list.primitive_data.length = 3;
    control_list.primitive_data.index = 0;

    control_list.flush_all_state = 5;

    *(reg32*)(PBASE + V3D_BASE + V3D_CT0CA) = &control_list;
    *(reg32*)(PBASE + V3D_BASE + V3D_CT0EA) = &control_list + sizeof(control_list);

    // Hold while not finished
    while(*(reg32*)(PBASE + V3D_BASE + V3D_BFC > 0)) { timer_sleep(2); }
}

void render(u32 buffAddr) {
    render_command renderCommand;

    renderCommand.clear.control_id = 114;
    renderCommand.clear.color = 0xFF00FFFFFF00FFFF;
    renderCommand.clear.clearvgzs = 0;
    renderCommand.clear.clearstencil = 0;

    renderCommand.tConf.control_id = 113;
    renderCommand.tConf.address = buffAddr;
    renderCommand.tConf.width = 640;
    renderCommand.tConf.height = 480;
    renderCommand.tConf.data = 0x0004; // Frame_Buffer_Color_Format_RGBA8888

    renderCommand.coords.control_id = 115;
    renderCommand.coords.row = 0;
    renderCommand.coords.column = 0;

    renderCommand.gen.control_id = 28;
    renderCommand.gen.data16 = 0;
    renderCommand.gen.data32 = 0;

    for (int row=0; row<8; row++) {
        for (int col=0; col<10; col++) {
            process_tile tile = renderCommand.tiles[row*10+col];
            
            tile.coords.control_id = 115;
            tile.coords.row = row;
            tile.coords.column = col;

            tile.bToSublist.control_id = 17;
            tile.bToSublist.address = BIN_ADDRESS + (row * 10 + col) * 32;
            
            tile.store.control_id = 25;
        }
    }

    *(reg32*)(PBASE + V3D_BASE + V3D_CT1CA) = &renderCommand;
    *(reg32*)(PBASE + V3D_BASE + V3D_CT1EA) = &renderCommand + sizeof(renderCommand);
}

void testRun() {
    u32 screenPtr = allocateScreenBuffer(640, 480, 32);
    configureTileBinningMode();
    render(screenPtr);
}

// align 4
// CONTROL_LIST_BIN_STRUCT: ; Control List Of Concatenated Control Records & Data Structure (Binning Mode Thread 0)
//   Tile_Binning_Mode_Configuration BIN_ADDRESS, $2000, BIN_BASE, 10, 8, Auto_Initialise_Tile_State_Data_Array ; Tile Binning Mode Configuration (B) (Address, Size, Base Address, Tile Width, Tile Height, Data)
//   Start_Tile_Binning ; Start Tile Binning (Advances State Counter So That Initial State Items Actually Go Into Tile Lists) (B)

//   Clip_Window 0, 0, SCREEN_X, SCREEN_Y ; Clip Window
//   Configuration_Bits Enable_Forward_Facing_Primitive + Enable_Reverse_Facing_Primitive, Early_Z_Updates_Enable ; Configuration Bits
//   Viewport_Offset 0, 0 ; Viewport Offset
//   NV_Shader_State NV_SHADER_STATE_RECORD ; NV Shader State (No Vertex Shading)
//   Vertex_Array_Primitives Mode_Triangles, 3, 0 ; Vertex Array Primitives (OpenGL)
//   Flush ; Flush (Add Return-From-Sub-List To Tile Lists & Then Flush Tile Lists To Memory) (B)
// CONTROL_LIST_BIN_END: