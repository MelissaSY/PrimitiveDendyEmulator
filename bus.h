#pragma once
#include "cpu.h"
#include "ppu.h"
#include "cartridge.h"
#include "Logger.h"

LONG clock_counter;

void init_system();
void finish_system();

BYTE cpu_read(WORD addr);
void cpu_write(WORD addr, BYTE data);

void insert_cartridge(LPCWSTR filepath);
void reset_bus();
void clock_bus();