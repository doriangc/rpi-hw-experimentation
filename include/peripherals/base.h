#pragma once

#if RPI_VERSION == 3
#define PBASE 0x3F000000 // Pi 3 base address
#elif RPI_VERSION == 4
#define PBASE 0xFE000000 // Pi 4 base address
#endif