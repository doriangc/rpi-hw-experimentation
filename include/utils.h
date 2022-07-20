#pragma once

#include "common.h"

extern void delay (u64 ticks);
extern void put32 (u64 address, u32 value);
extern unsigned int get32 (u64 address);