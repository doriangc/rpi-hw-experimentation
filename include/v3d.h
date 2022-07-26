#include "common.h"

void testRun();

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

typedef struct {
    u8 control_id;
    u64 color;
    u32 clearvgzs;
    u8 clearstencil;
} clear_colors;

typedef struct {
    u8 control_id;
    u32 address;
    u16 width;
    u16 height;
    u16 data;
} tile_rendering_mode_config;

typedef struct {
    u8 control_id;
    u8 column;
    u8 row;
} tile_coordinates;

typedef struct {
    u8 control_id;
    u16 data16;
    u32 data32;
} store_tile_buffer_general;

typedef struct {
    u8 control_id;
} store_multi_sample;

typedef struct {
    u8 control_id;
    u32 address;
} branch_to_sublist;

typedef struct {
    tile_coordinates coords;
    branch_to_sublist bToSublist;
    store_multi_sample store;
} process_tile;

typedef struct {
    clear_colors clear;
    tile_rendering_mode_config tConf;

    // general store pass.
    tile_coordinates coords;
    store_tile_buffer_general gen;

    process_tile tiles[80];
} render_command;

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