#pragma once
#include "bus.h";

BYTE ppu_read(WORD addr);
void ppu_write(WORD addr, BYTE data);

BYTE ppu_bus_read(WORD addr);
void ppu_bus_write(WORD addr, BYTE data);

void ppu_connect_cartridge();
void clock_ppu();