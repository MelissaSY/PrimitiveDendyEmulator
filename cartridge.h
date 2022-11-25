#pragma once
#include "ppu.h"
#include "cpu.h"

typedef struct cartridge {
	BYTE(*cart_ppu_read)(WORD addr);
	void(*cart_ppu_write)(WORD addr, BYTE data);

	BYTE (*cart_bus_read)(WORD addr);
	void (*cart_bus_write)(WORD addr, BYTE data);
} cartridge;

//BYTE cart_ppu_read(WORD addr);
//void cart_ppu_write(WORD addr, BYTE data);
//
//BYTE cart_bus_read(WORD addr);
//void cart_bus_write(WORD addr, BYTE data);