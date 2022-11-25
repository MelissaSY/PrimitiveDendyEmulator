#pragma once
#include <Windows.h>
#include "bus.h"

struct reg {
	//accumulator
	BYTE A;
	//index X
	BYTE X;
	//index Y
	BYTE Y;
	//stack pointer
	BYTE SP;
	//processor flags: Neg-oVerfow-*reserved*-Break-Decimal-Interrupt-Zero-Carry
	BYTE P;
	//указатель команды
	union PC
	{
		struct {
			BYTE l;
			BYTE h;
		} B;
		WORD W;
	} PC;
} cpu_reg;
//struct for instruction table
typedef struct instruction {
	//instruction address
	void (*instr)(void);
	//adrressing mode
	void (*mode)(void);
	BYTE cycles_num;

} instr_addr;

BYTE cpu_operand;

void reset_cpu();
void clock_cpu();