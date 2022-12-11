#pragma once
#include <Windows.h>
typedef struct RGB {
	BYTE R;
	BYTE G;
	BYTE B;
} RGB_NES;

typedef struct tile {
	RGB_NES** tile;
	WORD x;
	WORD y;
} tile;

tile initialize_tile(WORD x,  WORD y);
void finish_tile(tile* tile, WORD x, WORD y);
void drawpattern(HDC hdc, RECT wnd_rect, tile tile);