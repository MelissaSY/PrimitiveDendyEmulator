#include "ppu_helper.h"

tile initialize_tile(WORD x, WORD y)
{
	tile tile;
	tile.tile = (RGB_NES**)calloc(x, sizeof(RGB_NES*));
	for (WORD i = 0; i < x; i++) 
	{
		tile.tile[i] = (RGB_NES*)calloc(y, sizeof(RGB_NES));
	}
	tile.x = x;
	tile.y = y;
	return tile;
}

void finish_tile(tile* tile, WORD x, WORD y)
{
	if (tile->tile != NULL) 
	{
		for (WORD i = 0; i < x; i++)
		{
			free(tile->tile[i]);
		}
		if (tile->tile != NULL)
		{
			free(tile->tile);
		}
	}
}



void drawpattern(HDC hdc, RECT wnd_rect, tile tile)
{
	SetBkMode(hdc, TRANSPARENT);

	WORD x = tile.x;
	WORD y = tile.y;

	RECT one_rect;

	LONG width = (wnd_rect.right) / tile.x;
	LONG height = (wnd_rect.bottom) / tile.y;

	one_rect.left = 0;
	one_rect.right = wnd_rect.left + width;
	one_rect.top = 0;
	one_rect.bottom = wnd_rect.top + height;

	HBRUSH brush;

	for (int i = 0; i < tile.x; i++)
	{
		for (int j = 0; j < tile.y; j++)
		{
			brush = CreateSolidBrush(RGB(tile.tile[i][j].R, tile.tile[i][j].G, tile.tile[i][j].B));
			SelectObject(hdc, brush);
			Rectangle(hdc, one_rect.left, one_rect.top, one_rect.right, one_rect.bottom);
			DeleteObject(brush);

			one_rect.left += width;
			one_rect.right += width;
			//num++;
		}
		one_rect.left = 0;
		one_rect.right = width;
		one_rect.bottom += height;
		one_rect.top += height;
	}

}