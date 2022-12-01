#include "bus.h"

BYTE RAM[2048];

long clock_counter = 0;


BYTE cpu_read(WORD addr) {
	BYTE data;
	if(cart_bus_read(addr, &data))
	{

	}
	else if ((addr&0xFF00) <= 0x1F00) 
	{
		data = RAM[addr & 0x07FF];
	}
	else if (addr >= 0x2000 && addr <= 0x3FFF)
	{
		data = ppu_bus_read(addr & 0x0007);
	}

	return data;
};
void cpu_write(WORD addr, BYTE data) {
	if(cart_bus_write(addr, data)) 
	{

	}
	else if (addr >= 0x0000 && addr <= 0x1FFF)
	{
		RAM[addr & 0x07FF] = data;
	}
	else if (addr >= 0x2000 && addr <= 0x3FFF) 
	{
		ppu_bus_write(addr & 0x0007, data);
	}
};

void reset_bus()
{
	reset_cpu();
	clock_counter = 0;
}

void clock_bus()
{

}

void insert_cartridge(LPCWSTR filepath)
{
	read_cartridge(filepath);
	ppu_connect_cartridge(filepath);
}