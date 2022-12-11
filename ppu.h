#pragma once
#include "bus.h"
#include "ppu_helper.h"

BOOL readonly;
BOOL frame_complete;
BOOL ppu_nmi;

void ppu_init();

void ppu_reset();
void ppu_clock();

BYTE ppu_read(WORD addr);
void ppu_write(WORD addr, BYTE data);

BYTE ppu_bus_read(WORD addr);
void ppu_bus_write(WORD addr, BYTE data);

void ppu_connect_cartridge();
void ppu_read_pallete(LPCWSTR filepath);

void drawcurrent(HDC DC, RECT wnd_rect);


void ppu_finish();