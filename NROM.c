#include "NROM.h"

mapper NROM;


BOOL bus_NROM_read(WORD addr, DWORD* mapped_addr)
{
	if (addr >= 0x8000 && addr <= 0xFFFF)
	{
		/*
		* cases: NROM-128 - 1 bank (0x0000-0x3FFF) and mirror (0x4000-0x7FFF)
		*		 NROM-256 - 1 bank + 1 bank (0x0000 - 0x7FFF) 
		*/
		(*mapped_addr) = addr & (NROM.n_prg_banks > 1 ? 0x7FFF : 0x3FFF);
		return TRUE;
	}
	return FALSE;
}
BOOL ppu_NROM_read(WORD addr, DWORD* mapped_addr) 
{
	if (addr >= 0x0000 && addr <= 0x1FFF)
	{
		(*mapped_addr) = addr;
		return TRUE;
	}
	return FALSE;
}
BOOL bus_NROM_write(WORD addr, DWORD* mapped_addr)
{
	if (addr >= 0x8000 && addr <= 0xFFFF)
	{
		(*mapped_addr) = addr & (NROM.n_prg_banks > 1 ? 0x7FFF : 0x3FFF);
		return TRUE;

	}
	return FALSE;
}
BOOL ppu_NROM_write(WORD addr, DWORD* mapped_addr) 
{
	if (addr >= 0x0000 && addr <= 0x1FFF)
	{
		if (NROM.n_chr_banks == 0) 
		{
			(*mapped_addr) = addr;
			return TRUE;
		}
	}
	return FALSE;
}

mapper get_NROM(INT n_prg_banks, INT n_chr_banks)
{
	NROM.n_prg_banks = n_prg_banks;
	NROM.n_chr_banks = n_chr_banks;

	NROM.bus_map_read = bus_NROM_read;
	NROM.ppu_map_read = ppu_NROM_read;
	NROM.bus_map_write = bus_NROM_write;
	NROM.ppu_map_write = ppu_NROM_write;

	return NROM;
}

