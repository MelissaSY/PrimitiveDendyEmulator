#pragma once
#include "bus.h"
#include "NROM.h"
#include "MMC1.h"

//typedef struct cartridge {
//	BYTE(*cart_ppu_read)(WORD addr);
//	void(*cart_ppu_write)(WORD addr, BYTE data);
//
//	BYTE (*cart_bus_read)(WORD addr);
//	void (*cart_bus_write)(WORD addr, BYTE data);
//} cartridge;


BYTE* PRG;
BYTE* CHR;

void read_cartridge(LPCWSTR path);

BOOL cart_ppu_read(WORD addr, BYTE* data);
BOOL cart_ppu_write(WORD addr, BYTE data);

BOOL cart_bus_read(WORD addr, BYTE* data);
BOOL cart_bus_write(WORD addr, BYTE data);