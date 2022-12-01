#include "cartridge.h"
#include <windows.h>

typedef enum file_type {
	NES2_0,
	archaic_iNES,
	iNES,
	iNES0_7
} file_type;

typedef enum mirroring {
	vertical,
	horizontal,
	four_screen
} mirroring;

typedef enum console_type {
	computer = 0,
	vs_system = 1,
	playchoice_10 = 2,
	extended = 3
} console_type;

BOOL SRAM; //battery-backed memory, prg-ram, sram

BYTE* PRG;
BYTE* CHR;

WORD mapper_id = 0;
BYTE n_prg_banks = 0;
BYTE n_chr_banks = 0;


mapper current_mapper;
/*
* 
* ines
Header (16 bytes)
Trainer, if present (0 or 512 bytes)
PRG ROM data (16384 * x bytes)
CHR ROM data, if present (8192 * y bytes)
PlayChoice INST-ROM, if present (0 or 8192 bytes)
PlayChoice PROM, if present (16 bytes Data, 16 bytes CounterOut) (this is often missing, see PC10 ROM-Images for details)
*/

//names of fields are like the ones ines supports

typedef struct header
{
	char name[4];
	BYTE prg_rom_size; //in 16 kb units (ines)
	BYTE chr_rom_size; //in 8 kb units (0 means the board uses CHR RAM) (ines)
	/* 
	76543210
	||||||||
	|||||||+-Mirroring: 0 : horizontal(vertical arrangement) (CIRAM A10 = PPU A11)
	|||||||             1 : vertical(horizontal arrangement) (CIRAM A10 = PPU A10)
	||||||+-- 1 : Cartridge contains battery - backed PRG RAM($6000 - 7FFF) or other persistent memory
	|||||+--- 1 : 512 - byte trainer at $7000 - $71FF(stored before PRG data)
	||||+---- 1 : Ignore mirroring control or above mirroring bit; instead provide four - screen VRAM
	++++---- - Lower nybble of mapper number
	*/
	BYTE flags_6; 
	/*
	76543210
	||||||||
	|||||||+- VS Unisystem	
	||||||+-- PlayChoice-10 (8KB of Hint Screen data stored after CHR data)
	||||++--- If equal to 2, flags 8-15 are in NES 2.0 format
	++++----- Upper nybble of mapper number
	*/
	BYTE flags_7;

	BYTE flags_8; //PRG RAM size ?
	BYTE flags_9;
	BYTE tv_system_1;
	BYTE tv_system_2;
	char unused[5];
} header;

mirroring detect_mirroring(BYTE flag_6)
{
	mirroring mirroring = horizontal;

	if (flag_6 & 0x04)
	{
		mirroring = four_screen;
	}
	else if (flag_6 & 0x01) 
	{
		mirroring = vertical;
	}
	return mirroring;
}

file_type detect_file_type(BYTE flags_7)
{
	BYTE res = flags_7 & 0x0C;
	file_type file_type;
	switch (res)
	{
	case 0x08:
		file_type = NES2_0;
		break;
	case 0x04:
		file_type = archaic_iNES;
		break;
	case 0x00:
		file_type = iNES;
		break;
	default:
		file_type = iNES0_7;
		break;
	}
	return file_type;
}

console_type detect_console_type(BYTE flag_7)
{
	console_type console_type = flag_7 & 0x03;
	return console_type;
}

BOOL detect_SRAM(BYTE flag_6)
{
	return flag_6 & 0x02;
}

void read_cartridge(LPCWSTR path)
{
	HANDLE h_file = INVALID_HANDLE_VALUE;
	DWORD bytes_read;
	header header;
	file_type file_type;
	mirroring mirroring;
	console_type console_type;

	if (PRG != NULL) 
	{
		free(PRG);
	}
	if(CHR != NULL) 
	{
		free(CHR);
	}

	h_file = CreateFile(path, FILE_READ_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h_file != INVALID_HANDLE_VALUE)
	{
		ReadFile(h_file, &header, sizeof(header), &bytes_read, NULL);
		bytes_read = 0;
		if (header.flags_6 & 0x04) 
		{
			SetFilePointer(h_file, 512, NULL, FILE_CURRENT);
		}

		mapper_id |= (header.flags_6 >> 4) | (header.flags_7 & 0xF0);

		file_type = detect_file_type(header.flags_7);
		console_type = detect_console_type(header.flags_7);
		mirroring = detect_mirroring(header.flags_6);
		SRAM = detect_SRAM(header.flags_6);

		switch (file_type)
		{
		case NES2_0:

			break;
		case archaic_iNES:
			break;
		case iNES:
			n_prg_banks = header.prg_rom_size;
			PRG = (BYTE*)calloc(16 * 1024 * n_prg_banks, sizeof(BYTE));
			ReadFile(h_file, &PRG, 16 * 1024 * n_prg_banks, &bytes_read, NULL);

			n_chr_banks = header.chr_rom_size;
			CHR = (BYTE*)calloc(8 * 1024 * n_chr_banks, sizeof(BYTE));
			ReadFile(h_file, &CHR, 8 * 1024 * n_chr_banks, &bytes_read, NULL);
			break;
		default:
			break;
		}
		//mapper detection
		switch (mapper_id)
		{
		case 0:
			current_mapper = get_NROM(n_prg_banks, n_chr_banks);
			break;
		case 1:
			current_mapper = get_MMC1(n_prg_banks, n_chr_banks);
			break;
		default:
			break;
		}

	}
	CloseHandle(h_file);
}


///////////////////////////////////
//cartirdge io functions

//ppu bus
BOOL cart_ppu_read(WORD addr, BYTE* data)
{
	DWORD mapped_addr = 0;
	if (current_mapper.ppu_map_read(addr, &mapped_addr))
	{
		(*data) = CHR[mapped_addr];
		return TRUE;
	}
	else
	return FALSE;
}
//ppu bus
BOOL cart_ppu_write(WORD addr, BYTE data)
{
	DWORD mapped_addr = 0;
	if (current_mapper.ppu_map_write(addr, &mapped_addr))
	{
		CHR[mapped_addr] = data;
		return TRUE;
	}
	else
	return FALSE;
}

//main bus
BOOL cart_bus_read(WORD addr, BYTE* data)
{
	DWORD mapped_addr = 0;
	if (current_mapper.bus_map_read(addr, &mapped_addr))
	{
		(*data) = PRG[mapped_addr];
		return TRUE;
	} 
	else
	return FALSE;
}
//main bus
BOOL cart_bus_write(WORD addr, BYTE data)
{
	DWORD mapped_addr = 0;
	if (current_mapper.bus_map_write(addr, &mapped_addr))
	{
		PRG[mapped_addr] = data;
		return TRUE;
	}
	else
	return FALSE;
}
//cartirdge io functions
///////////////////////////////////

