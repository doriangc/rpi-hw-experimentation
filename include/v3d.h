#include "common.h"

typedef struct {
    u8 control_id; // Control ID Code Byte: ID Code #112
    u32 address; // Control ID Data Record Word: Tile Allocation Memory Address (Bit 0..31) 
    u32 size; // Control ID Data Record Word: Tile Allocation Memory Size (Bytes) (Bit 32..63)
    u32 base_address; //  Control ID Data Record Word: Tile State Data Array Base Address (16-Byte Aligned, Size Of 48 Bytes * Num Tiles) (Bit 64..95)
    u8 width; // Control ID Data Record Byte: Width (In Tiles) (Bit 96..103)
    u8 height; // Control ID Data Record Byte: Height (In Tiles) (Bit 104..111)
    u8 data; // Control ID Data Record Byte: Data Record (Bit 112..119)
} tile_binning_mode_config;

typedef struct {
    u8 control_id;
    u16 left;
    u16 bottom;
    u16 width;
    u16 height;
} clip_window;


typedef struct {
    u8 control_id;
    u8 data8;
    u16 data16;
} config_bits;

typedef struct {
    u8 control_id;
    u16 x;
    u16 y;
} viewport_offset;

typedef struct {
    u8 control_id;
    u32 address;
} no_vertex_shading_state;

typedef struct {
    u8 control_id;
    u8 data;
    u32 length; // Number of vertices
    u32 index; // Index of first vertex
} vertex_array_primitives;

typedef struct {
    tile_binning_mode_config config;
    u8 start_binning; // Control ID Code: Start Tile Binning (Advances State Counter So That Initial State Items Actually Go Into Tile Lists) (B)
    clip_window clip;
    config_bits cBits;
    viewport_offset viewport;
    no_vertex_shading_state state;
    vertex_array_primitives primitive_data;
    u8 flush_all_state; // Flush
} binning_command;

extern const u8 NV_SHADER_STATE_RECORD[];
extern const u8 VERTEX_DATA[];
extern const u8 FRAGMENT_SHADER_CODE[];