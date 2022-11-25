#include "cartridge.h"

///////////////////////////////////
//cartirdge functions

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
//cartirdge functions
///////////////////////////////////

cartridge init_cartridge()
{
	cartridge cartridge;
	cartridge.cart_ppu_read = cart_ppu_read;
	cartridge.cart_ppu_write = cart_ppu_write;
	cartridge.cart_bus_read = cart_bus_read;
	cartridge.cart_bus_write = cart_bus_write;
	return cartridge;
}
