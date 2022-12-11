#include "ppu.h"

#define PATTERN_LEFT_RIGHT(addr)	((addr >> 12) & 0x0001)	


WORD scan_line = 0;

BOOL is_inserted = FALSE;

BYTE pattern_table[2][4*1024];
BYTE name_table[2][1024];
BYTE palette_table[32];

tile name_table_tile[2];
tile pattern_table_tile[2];
tile screen_tile;

RGB_NES palette[64];

ULONG cycles_num;

BYTE address_latch = 0x00;
BYTE ppu_data_buffer = 0x00;
WORD ppu_address = 0x0000;



typedef union PPUCTRL {
	struct {

		//Nametable select
		BYTE NN : 2;

		//increment mode
		BYTE I : 1;

		//FG tile select
		BYTE S : 1;

		//BG tile select
		BYTE B : 1;

		//sprite height
		BYTE H : 1;

		//ppu master/slave
		BYTE P : 1;

		//enable
		BYTE V : 1;
	};
	BYTE reg;
} PPUCTRL;

typedef union PPUMASK {
	struct {
		//grayscale
		BYTE g : 1;
		//BG left column enable
		BYTE m : 1;
		//column enable
		BYTE M : 1;
		//BG enable
		BYTE b : 1;
		//FG enable
		BYTE s : 1;
		//color empasis
		//red (green on PAL/Dendy)
		BYTE R : 1;
		//color empasis
		//green (red on PAL/Dendy)
		BYTE G : 1;
		//color empasis
		//blue
		BYTE B : 1;
	};
	BYTE reg;
} PPUMASK;

typedef union PPUSTATUS {
	struct {
		BYTE unused : 5;
		//sprite overflow
		BYTE O : 1;
		//sprite 0 hit 
		BYTE S : 1;
		//Vblank
		BYTE V : 1;
	};
	BYTE reg;
} PPUSTATUS;



PPUCTRL control;
PPUMASK mask;
PPUSTATUS status;


tile get_pattern_table(BYTE i, BYTE palette_num);


BYTE ppu_read(WORD addr) 
{
	BYTE data = 0x00;

	addr &= 0x3FFF;

	if (cart_ppu_read(addr, &data))
	{

	}
	//pattern

	/*
	* DCBA98 76543210
	* ---------------
	* 0HRRRR CCCCPTTT
	* |||||| |||||+++- T: Fine Y offset, the row number within a tile
	* |||||| ||||+---- P: Bit plane (0: "lower"; 1: "upper")
	* |||||| ++++----- C: Tile column
	* ||++++---------- R: Tile row
	* |+-------------- H: Half of pattern table (0: "left"; 1: "right")
	* +--------------- 0: Pattern table is at $0000-$1FFF
	*/
	else if (addr >= 0x0000 && addr <= 0x1FFF)
	{
		data = pattern_table[PATTERN_LEFT_RIGHT(addr)][addr & 0x0FFF];
	}
	//nametable
	else if (addr >= 0x2000 && addr <= 0x3EFF)
	{

	}
	//palette
	else if (addr >= 0x3F00 && addr <= 0x3FFF)
	{
		addr &= 0x001F;
		switch (addr)
		{
		case 0x0010:
			addr = 0x0000;
			break;
		case 0x0014:
			addr = 0x0004;
			break;
		case 0x0018:
			addr = 0x0008;
			break;
		case 0x001C:
			addr = 0x000C;
			break;
		default:
			break;
		}
		data = palette_table[addr];
	}

	return data;
}
void ppu_write(WORD addr, BYTE data)
{
	addr &= 0x3FFF;

	if (cart_ppu_write(addr, data))
	{

	}
	//pattern
	else if (addr >= 0x0000 && addr <= 0x1FFF)
	{
		pattern_table[PATTERN_LEFT_RIGHT(addr)][addr & 0x0FFF] = data;
	}
	//nametable
	else if (addr >= 0x2000 && addr <= 0x3EFF)
	{

	}
	//palette
	else if (addr >= 0x3F00 && addr <= 0x3FFF)
	{
		addr &= 0x001F;
		switch (addr)
		{
		case 0x0010:
			addr = 0x0000;
			break;
		case 0x0014:
			addr = 0x0004;
			break;
		case 0x0018:
			addr = 0x0008;
			break;
		case 0x001C:
			addr = 0x000C;
			break;
		default:
			break;
		}
		palette_table[addr] = data;
	}
}

BYTE ppu_bus_read(WORD addr)
{
	BYTE data = 0x00;
	switch (addr) 
	{
		//control
	case 0x0000:
		data = control.reg;
		break;
		//mask
	case 0x0001:
		data = control.reg;
		break;
		//status
	case 0x0002:
		data = (status.reg & 0xE0) | (ppu_data_buffer & 0x1F);
		status.V = 0;
		address_latch = 0;
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
		data = ppu_data_buffer;
		ppu_data_buffer = ppu_read(ppu_address);

		if (ppu_address > 0x3F00) data = ppu_data_buffer;
		ppu_address++;
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
		control.reg = data;
		break;
		//mask
	case 0x0001:
		mask.reg = data;
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
		if (address_latch == 0)
		{
			ppu_address = (ppu_address & 0x00FF) | data;
			address_latch = 1;
		}
		else
		{
			ppu_address = (ppu_address & 0xFF00) | (data << 8);
			address_latch = 0;
		}
		break;
		//PPU data
	case 0x0007:
		ppu_write(ppu_address, data);
		ppu_address++;
		break;
	}
}

void ppu_connect_cartridge()
{
	is_inserted = TRUE;
}

void ppu_read_pallete(LPCWSTR filepath)
{
	HANDLE h_file = INVALID_HANDLE_VALUE;
	DWORD bytes_read;
	h_file = CreateFile(filepath, FILE_READ_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h_file != INVALID_HANDLE_VALUE)
	{
		ReadFile(h_file, &palette, sizeof(palette), &bytes_read, NULL);
		bytes_read = 0;
	}
	CloseHandle(h_file);
}


void drawpallete(HDC hdc, RECT wnd_rect)
{
	SetBkMode(hdc, TRANSPARENT);

	RECT one_rect;

	LONG width = (wnd_rect.right) / 16;
	LONG height = (wnd_rect.bottom) / 4;

	one_rect.left = 0;
	one_rect.right = wnd_rect.left + width;
	one_rect.top = 0;
	one_rect.bottom = wnd_rect.top + height;

	HBRUSH brush;

	int num = 0;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			brush = CreateSolidBrush(RGB(palette[num].R, palette[num].G, palette[num].B));
			SelectObject(hdc, brush);
			Rectangle(hdc, one_rect.left, one_rect.top, one_rect.right, one_rect.bottom);
			DeleteObject(brush);
			one_rect.left += width;
			one_rect.right += width;
			num++;
		}
		one_rect.left = 0;
		one_rect.right = width;
		one_rect.bottom += height;
		one_rect.top += height;
	}
}

void drawcurrent(HDC hdc, RECT wnd_rect)
{
	if (!is_inserted)
	{
		drawpallete(hdc, wnd_rect);
	}
	else
	{
		drawpattern(hdc, wnd_rect, get_pattern_table(0, 7));
	}
}


RGB_NES get_palette_color(BYTE palette_num, BYTE color_num)
{
	return palette[ppu_read(0x3F00 + (palette_num << 2) + color_num) & 0x3F];
}

tile get_pattern_table(BYTE i, BYTE palette_num)
{
	BYTE lsb, msb, color_num;
	for (WORD n_tile_y = 0; n_tile_y < 16; n_tile_y++)
	{
		for (WORD n_tile_x = 0; n_tile_x < 16; n_tile_x++)
		{
			WORD offset = n_tile_y * 256 + n_tile_x * 16;

			for (BYTE row = 0; row < 8; row++)
			{
				lsb = ppu_read(i * 0x1000 + offset + row);
				msb = ppu_read(i * 0x1000 + offset + row + 8);
				for (BYTE col = 0; col < 8; col++)
				{
					color_num = (lsb & 0x01) + (msb & 0x01);
					lsb >>= 1;
					msb >>= 1;

					pattern_table_tile[i].tile[n_tile_x * 8 + (7 - col)][n_tile_y * 8 + row] = get_palette_color(palette_num, color_num);
				}
			}
		}
	}
	return pattern_table_tile[i];
}


void ppu_clock()
{
	if (scan_line == -1 && cycles_num == 1)
	{
		status.V = 0;
	}



	if (scan_line == 241 && cycles_num == 1)
	{
		status.V = 1;
		if (control.V)
		{
			ppu_nmi = TRUE;
		}
	}

	cycles_num++;
	if (cycles_num >= 341)
	{
		cycles_num = 0;
		scan_line++;

	}
}




void ppu_reset()
{
	frame_complete = FALSE;
}
//must be called only once
void ppu_init()
{
	ppu_nmi = FALSE;

	name_table_tile[0] = initialize_tile(256, 240);
	name_table_tile[1] = initialize_tile(256, 240);

	screen_tile = initialize_tile(256, 240);

	pattern_table_tile[0] = initialize_tile(128, 128);
	pattern_table_tile[1] = initialize_tile(128, 128);
}

void ppu_finish()
{
	finish_tile(&name_table_tile[0], 256, 240);
	finish_tile(&name_table_tile[1], 256, 240);

	finish_tile(&screen_tile, 256, 240);

	finish_tile(&pattern_table_tile[0], 128, 128);
	finish_tile(&pattern_table_tile[1], 128, 128);
}