#include "ppu.h"

typedef struct RGB {
	BYTE R;
	BYTE G;
	BYTE B;
} RGB_NES;

BYTE nametable[2][1024];
BYTE palette[32];

RGB_NES pallete[64];

INT cycles_num;

BYTE ppu_read(WORD addr) 
{
	BYTE data = 0x00;

	addr &= 0x3FFF;

	if (cart_ppu_read(addr, &data))
	{

	}

	return data;
}
void ppu_write(WORD addr, BYTE data)
{
	addr &= 0x3FFF;

	if (cart_ppu_write(addr, data))
	{

	}
}

BYTE ppu_bus_read(WORD addr)
{
	BYTE data = 0x00;
	switch (addr) 
	{
		//control
	case 0x0000:
		break;
		//mask
	case 0x0001:
		break;
		//status
	case 0x0002:
		break;
		//OAM address
	case 0x0003:
		break;
		//OAM data
	case 0x0004:
		break;
		//scroll
	case 0x0005:
		break;
		//PPU address
	case 0x0006:
		break;
		//PPU data
	case 0x0007:
		break;
	}
	return data;
}
void ppu_bus_write(WORD addr, BYTE data)
{
	switch (addr)
	{
		//control
	case 0x0000:
		break;
		//mask
	case 0x0001:
		break;
		//status
	case 0x0002:
		break;
		//OAM address
	case 0x0003:
		break;
		//OAM data
	case 0x0004:
		break;
		//scroll
	case 0x0005:
		break;
		//PPU address
	case 0x0006:
		break;
		//PPU data
	case 0x0007:
		break;
	}
}

void ppu_connect_cartridge()
{

}

void ppu_read_pallete(LPCWSTR filepath)
{
	HANDLE h_file = INVALID_HANDLE_VALUE;
	DWORD bytes_read;
	h_file = CreateFile(filepath, FILE_READ_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h_file != INVALID_HANDLE_VALUE)
	{
		ReadFile(h_file, &pallete, sizeof(pallete), &bytes_read, NULL);
		bytes_read = 0;
	}
	CloseHandle(h_file);
}

void ppu_clock()
{
	cycles_num++;
	if (cycles_num >= 341)
	{
		cycles_num = 0;
	}
}