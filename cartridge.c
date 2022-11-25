#include "cartridge.h"
#include <windows.h>

BYTE* PRG;
BYTE* CHR;

BYTE mapper_id = 0;
BYTE prg_banks = 0;
BYTE chr_banks = 0;

typedef struct header
{
	char name[4];
	BYTE prg_rom_chunks;
	BYTE chr_rom_chunks;
	BYTE mapper_1;
	BYTE mapper_2;

	BYTE prg_ram_size;
	BYTE tv_system_1;
	BYTE tv_system_2;
	char unused[5];
} header;

void read_cartridge(LPCWSTR path)
{
	HANDLE h_file = INVALID_HANDLE_VALUE;
	DWORD bytes_read;
	header header;
	
	h_file = CreateFile(path, FILE_READ_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h_file != INVALID_HANDLE_VALUE)
	{
		ReadFile(h_file, &header, sizeof(header), &bytes_read, NULL);

		if (header.mapper_1 & 0x04) 
		{

		}
	}
}


///////////////////////////////////
//cartirdge io functions

//ppu bus
BYTE cart_ppu_read(WORD addr)
{

}
//ppu bus
void cart_ppu_write(WORD addr, BYTE data)
{

}

//main bus
BYTE cart_bus_read(WORD addr) 
{

}
//main bus
void cart_bus_write(WORD addr, BYTE data) 
{

}
//cartirdge io functions
///////////////////////////////////

