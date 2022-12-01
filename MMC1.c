#include "MMC1.h"

mapper MMC1;

BOOL bus_MMC1_read(WORD addr, DWORD* mapped_addr)
{
	if (addr >= 0x8000 && addr <= 0xFFFF)
	{ 
		//(*mapped_addr) = addr & (NROM.n_prg_banks > 1 ? 0x7FFF : 0x3FFF);
		return TRUE;
	}
	return FALSE;
}
BOOL ppu_MMC1_read(WORD addr, DWORD* mapped_addr)
{
	if (addr >= 0x0000 && addr <= 0x1FFF)
	{
		//(*mapped_addr) = addr;
		return TRUE;
	}
	return FALSE;
}
BOOL bus_MMC1_write(WORD addr, DWORD* mapped_addr)
{
	if (addr >= 0x8000 && addr <= 0xFFFF)
	{

	}
	else if (addr >= 0x0FFF && addr <= 0x1FFF)
	{

	}
	else
		return FALSE;

	return TRUE;
}
BOOL ppu_MMC1_write(WORD addr, DWORD* mapped_addr)
{
	if (addr >= 0x0000 && addr <= 0x0FFF)
	{

	}
	else if (addr >= 0x0FFF && addr <= 0x1FFF)
	{

	}
	else
	return FALSE;

	return TRUE;
}

mapper get_MMC1(INT n_prg_banks, INT n_chr_banks)
{
	MMC1.n_prg_banks = n_prg_banks;
	MMC1.n_chr_banks = n_chr_banks;

	MMC1.ppu_map_read = ppu_MMC1_read;
	MMC1.bus_map_read = bus_MMC1_read;

	MMC1.bus_map_write = bus_MMC1_write;
	MMC1.ppu_map_write = ppu_MMC1_write;
	return MMC1;
}

