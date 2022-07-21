#define BUS_ADDRESSES_l2CACHE_ENABLED 0x40000000 // Bus Addresses: disable_l2cache=0
#define BUS_ADDRESSES_l2CACHE_DISABLED 0xC0000000 // Bus Addresses: disable_l2cache=1

.globl NV_SHADER_STATE_RECORD
.align 16 // 128-Bit Align
NV_SHADER_STATE_RECORD:
  .byte 0 // Flag Bits: 0 = Fragment Shader Is Single Threaded, 1 = Point Size Included In Shaded Vertex Data, 2 = Enable Clipping, 3 = Clip Coordinates Header Included In Shaded Vertex Data
  .byte 3 * 4 // Shaded Vertex Data Stride
  .byte 0 // Fragment Shader Number Of Uniforms (Not Used Currently)
  .byte 0 // Fragment Shader Number Of Varyings
  .word BUS_ADDRESSES_l2CACHE_DISABLED + FRAGMENT_SHADER_CODE // Fragment Shader Code Address
  .word 0 // Fragment Shader Uniforms Address
  .word BUS_ADDRESSES_l2CACHE_DISABLED + VERTEX_DATA // Shaded Vertex Data Address (128-Bit Aligned If Including Clip Coordinate Header)

.globl VERTEX_DATA
.align 16 // 128-Bit Align
VERTEX_DATA:
  // Vertex: Top
  .hword 320 * 16 // X In 12.4 Fixed Point
  .hword  32 * 16 // Y In 12.4 Fixed Point
  .word 0b10000 // Z
  .word 0b10000 // 1 / W

  // Vertex: Bottom Left
  .hword  32 * 16 // X In 12.4 Fixed Point
  .hword 448 * 16 // Y In 12.4 Fixed Point
  .word 0b10000 // Z
  .word 0b10000 // 1 / W

  // Vertex: Bottom Right
  .hword 608 * 16 // X In 12.4 Fixed Point
  .hword 448 * 16 // Y In 12.4 Fixed Point
  .word 0b10000 // Z
  .word 0b10000 // 1 / W

.globl FRAGMENT_SHADER_CODE
.align 16 // 128-Bit Align
FRAGMENT_SHADER_CODE:
  // Fill Color Shader
  .word 0x009E7000 //
  .word 0x100009E7 // nop// nop// nop

  .word 0xFFFFFFFF // RGBA White
  .word 0xE0020BA7 // ldi tlbc, 0xFFFFFFFF
  .word 0x009E7000 //
  .word 0x500009E7 // nop// nop// sbdone
  .word 0x009E7000 //
  .word 0x300009E7 // nop// nop// thrend

  .word 0x009E7000 //
  .word 0x100009E7 // nop// nop// nop
  .word 0x009E7000 //
  .word 0x100009E7 // nop// nop// nop