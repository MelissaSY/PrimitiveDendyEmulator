#include "ppu.h"

BYTE nametable[2][1024];
BYTE palette[32];

BYTE ppu_read(WORD addr) 
{
	BYTE data = 0x00;

	addr &= 0x3FFF;

	return data;
}
void ppu_write(WORD addr, BYTE data)
{
	addr &= 0x3FFF;
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

void ppu_clock()
{

}