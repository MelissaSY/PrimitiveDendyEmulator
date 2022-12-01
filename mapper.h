#pragma once
#include <Windows.h>

typedef struct mapper {
	int n_chr_banks;
	int n_prg_banks;

	BOOL(*bus_map_read)(WORD addr, DWORD* mapped_addr);
	BOOL(*ppu_map_read)(WORD addr, DWORD* mapped_addr);
	BOOL(*bus_map_write)(WORD addr, DWORD* mapped_addr);
	BOOL(*ppu_map_write)(WORD addr, DWORD* mapped_addr);
} mapper;