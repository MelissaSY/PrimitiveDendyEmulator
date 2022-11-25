#pragma once
#include "cpu.h"
#include "ppu.h"
#include "cartridge.h"

BYTE cpu_read(WORD addr);
void cpu_write(WORD addr, BYTE data);

void insert_cartridge();
void reset_bus();
void clock_bus();