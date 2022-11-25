#include "cpu.h"


//does PHP really push status on status on stack with break flag set no matter what? 
//does PLP change break flag?
//wrong CMPs realisation 
//JSR & RTS ??		

//change all bus_read/bus_write in instructions to special_get/slecial_set later on, identiify all gets/sets in addressing modes

#define NEG_FLAG		7
#define OVERFLOW_FLAG	6
#define RESERVED_FLAG	5
#define BREAK_FLAG		4
#define DECIMAL_FLAG	3
#define INTERRUPT_FLAG	2
#define ZERO_FLAG		1
#define CARRY_FLAG		0

//sl - shift left of the lower bit of byte to set the flag, b - byte with set lowest bit
#define RET_FLAG(sl, b, flags)	flags & ~(1<<sl) | (b&0x01) << sl
//gets flag shifting right the flags (return in lowest bit) 
#define GET_FLAG(sr, flags)		(flags >> (7 - sr))&0x01

int period, cycles_num;
WORD cpu_addr, cpu_rel_addr;
BYTE operationCode;
instr_addr ia;

void (*special_set)(void);
void (*special_get)(void);

void page_crossed(WORD prev_addr, WORD current_addr) {
	if ((prev_addr & 0xFF00) != (prev_addr & 0xFF00))
	{
		cycles_num++;
	}
}
//special get
void bus_get() {
	cpu_operand = cpu_read(cpu_addr);
}
//special set
void bus_set() {
	cpu_write(cpu_addr, cpu_operand);
}
void accumulator_get() {
	cpu_operand = cpu_reg.A;
}
void accumulator_set() {
	cpu_reg.A = cpu_operand;
}
void ZFlagSet(BYTE res) {
	cpu_reg.P = RET_FLAG(ZERO_FLAG, ~(res || 0), cpu_reg.P);
}
void NFlagSet(BYTE res) {
	cpu_reg.P = RET_FLAG(NEG_FLAG, res >> 7, cpu_reg.P);
}
void NZFlagSet(BYTE res) {
	ZFlagSet(res);
	NFlagSet(res);
}
BYTE CVFlagSet(WORD res, BYTE operand_1, BYTE operand_2) {
	//Carry flag
	cpu_reg.P = RET_FLAG(CARRY_FLAG, res >> 8, cpu_reg.P);
	//Overflow flag
	cpu_reg.P = RET_FLAG(OVERFLOW_FLAG, ((res ^ operand_1) & (res ^ operand_2) & 0x80) >> 7, cpu_reg.P);
	return res & 0x0011;
}
void pushS(BYTE operand) {
	cpu_write(0x0100 | cpu_reg.SP, operand);
	cpu_reg.SP--;
}
BYTE pullS() {
	return cpu_read(0x100 | ++cpu_reg.SP);
}
//////////////////////////////////////////////////////////////////////
//Logical and arithmetic commands
void ORA() {
	cpu_operand = cpu_read(cpu_addr);
	cpu_reg.A |= cpu_operand;
	NZFlagSet(cpu_reg.A);
}
void AND() {
	cpu_operand = cpu_read(cpu_addr);
	cpu_reg.A &= cpu_operand;
	NZFlagSet(cpu_reg.A);
}
void EOR() {
	cpu_operand = cpu_read(cpu_addr);
	cpu_reg.A ^= cpu_operand;
	NZFlagSet(cpu_reg.A);
}
void ADC() {
	cpu_operand = cpu_read(cpu_addr);
	WORD res = cpu_reg.A + cpu_operand + ((cpu_reg.P >> (7 - CARRY_FLAG)) & 0x01);
	cpu_reg.A = CVFlagSet(res, cpu_reg.A, cpu_operand);
}
void SBC() {
	cpu_operand = cpu_read(cpu_addr);
	WORD res = cpu_reg.A - cpu_operand - ~(cpu_reg.P >> (7 - CARRY_FLAG)) & 0x01;
	cpu_reg.A = CVFlagSet(res, cpu_reg.A, cpu_operand);
}
void CMP() {
	cpu_operand = cpu_read(cpu_addr);
	WORD res = cpu_reg.A - cpu_operand;
	cpu_reg.A = res & 0x0011;
	NZFlagSet(cpu_reg.A);

	cpu_reg.P = RET_FLAG(CARRY_FLAG, cpu_reg.P >> ZERO_FLAG, cpu_reg.P) 
		| RET_FLAG(CARRY_FLAG, res >> 8, cpu_reg.P);
}
void CPX() {
	cpu_operand = cpu_read(cpu_addr);
	WORD res = cpu_reg.X - cpu_operand;
	cpu_reg.X = res & 0x0011;
	NZFlagSet(cpu_reg.X);

	cpu_reg.P = RET_FLAG(CARRY_FLAG, cpu_reg.P >> ZERO_FLAG, cpu_reg.P)
		| RET_FLAG(CARRY_FLAG, res >> 8, cpu_reg.P);
}
void CPY() {
	cpu_operand = cpu_read(cpu_addr);
	WORD res = cpu_reg.Y - cpu_operand;
	cpu_reg.Y = res & 0x0011;
	NZFlagSet(cpu_reg.Y);

	cpu_reg.P = RET_FLAG(CARRY_FLAG, cpu_reg.P >> ZERO_FLAG, cpu_reg.P)
		| RET_FLAG(CARRY_FLAG, res >> 8, cpu_reg.P);
}
void DEC() {
	cpu_operand = cpu_read(cpu_addr);
	cpu_operand--;
	NZFlagSet(cpu_operand);
	cpu_write(cpu_addr, cpu_operand);
}
void DEX() {
	cpu_reg.X--;
	NZFlagSet(cpu_reg.X);
}
void DEY() {
	cpu_reg.Y--;
	NZFlagSet(cpu_reg.Y);
}
void INC() {
	cpu_operand = cpu_read(cpu_addr);
	cpu_operand++;
	NZFlagSet(cpu_operand);
	cpu_write(cpu_addr, cpu_operand);
}
void INX() {
	cpu_reg.X++;
	NZFlagSet(cpu_reg.X);
}
void INY() {
	cpu_reg.Y++;
	NZFlagSet(cpu_reg.Y);
}
void ASL() {
	cpu_operand = cpu_read(cpu_addr);
	WORD res = cpu_operand << 1;
	cpu_reg.P = RET_FLAG(CARRY_FLAG, res >> 8, cpu_reg.P);
	cpu_operand = res & 0x0011;
	NZFlagSet(cpu_operand);
	cpu_write(cpu_addr, cpu_operand);

}
void ROL() {
	cpu_operand = cpu_read(cpu_addr);
	BYTE lastBit = (cpu_operand & 0x80) >> 7;
	cpu_operand = (cpu_operand << 1) | (cpu_reg.P >> CARRY_FLAG) & 0x01;
	cpu_reg.P = RET_FLAG(CARRY_FLAG, lastBit, cpu_reg.P);
	NZFlagSet(cpu_operand);
	cpu_write(cpu_addr, cpu_operand);
}
void LSR() {
	cpu_operand = cpu_read(cpu_addr);
	BYTE firstBit = cpu_operand & 0x01;
	cpu_operand = (cpu_operand >> 1);
	cpu_reg.P = RET_FLAG(CARRY_FLAG, firstBit, cpu_reg.P);
	NZFlagSet(cpu_operand);
	cpu_write(cpu_addr, cpu_operand);
}
void ROR() {
	cpu_operand = cpu_read(cpu_addr);
	BYTE firstBit = cpu_operand & 0x01;
	cpu_operand = (cpu_operand >> 1) | ((cpu_reg.P << (7 - CARRY_FLAG)) & 0x80);
	cpu_reg.P = RET_FLAG(CARRY_FLAG, firstBit, cpu_reg.P);
	NZFlagSet(cpu_operand);
	cpu_write(cpu_addr, cpu_operand);
}
//Move commands
void LDA() {
	cpu_operand = cpu_read(cpu_addr);
	cpu_reg.A = cpu_operand;
	NZFlagSet(cpu_reg.A);
}
void STA() {
	cpu_write(cpu_addr, cpu_reg.A);
}
void LDX() {
	cpu_operand = cpu_read(cpu_addr);
	cpu_reg.X = cpu_operand;
	NZFlagSet(cpu_reg.X);
}
void STX() {
	cpu_write(cpu_addr, cpu_reg.X);
}
void LDY() {
	cpu_operand = cpu_read(cpu_addr);
	cpu_reg.Y = cpu_operand;
	NZFlagSet(cpu_reg.Y);
}
void STY() {
	cpu_write(cpu_addr, cpu_reg.Y);
}
void TAX() {
	cpu_reg.X = cpu_reg.A;
	NZFlagSet(cpu_reg.X);
}
void TXA() {
	cpu_reg.A = cpu_reg.X;
	NZFlagSet(cpu_reg.A);
}
void TAY() {
	cpu_reg.Y = cpu_reg.A;
	NZFlagSet(cpu_reg.Y);
}
void TYA() {
	cpu_reg.A = cpu_reg.Y;
	NZFlagSet(cpu_reg.A);
}
void TSX() {
	cpu_reg.X = cpu_reg.SP;
	NZFlagSet(cpu_reg.X);
}
void TXS() {
	cpu_reg.SP = cpu_reg.X;
}
void PLA() {
	cpu_reg.A = pullS();
	NZFlagSet(cpu_reg.A);
}
void PHA() {
	pushS(cpu_reg.A);
}
void PLP() {
	cpu_reg.P = pullS();
}
void PHP() {
	pushS(cpu_reg.P);
}
//Jump/Flag commands
void branch(BYTE flag, BYTE value) {
	if (GET_FLAG(flag, cpu_reg.P) == value) {
		cycles_num++;
		page_crossed(cpu_reg.PC.W, cpu_rel_addr);
		cpu_reg.PC.W = cpu_rel_addr;
	}
}
void BPL() {
	branch(NEG_FLAG, 0);
}
void BMI() {
	branch(NEG_FLAG, 1);
}
void BVC() {
	branch(OVERFLOW_FLAG, 0);
}
void BVS() {
	branch(OVERFLOW_FLAG, 1);
}
void BCC() {
	branch(CARRY_FLAG, 0);
}
void BCS() {
	branch(CARRY_FLAG, 1);
}
void BNE() {
	branch(ZERO_FLAG, 0);
}
void BEQ() {
	branch(ZERO_FLAG, 1);
}
void BRK() {
	cpu_reg.PC.W++;
	pushS(cpu_reg.PC.B.h);
	pushS(cpu_reg.PC.B.l);

	cpu_reg.P = RET_FLAG(INTERRUPT_FLAG, 0x01, cpu_reg.P);

	pushS(RET_FLAG(BREAK_FLAG, 0x01, cpu_reg.P));

	cpu_reg.PC.B.h = cpu_read(0xFFFE);
	cpu_reg.PC.B.l = cpu_read(0xFFFF);
}
void RTI() {
	cpu_reg.P = pullS();
	cpu_reg.P = RET_FLAG(BREAK_FLAG, 0x00, cpu_reg.P);

	cpu_reg.PC.B.l = pullS();
	cpu_reg.PC.B.h = pullS();
}
void JSR() {
	pushS(cpu_reg.PC.B.h);
	pushS(cpu_reg.PC.B.l);

	cpu_reg.PC.W = cpu_addr;
}
void RTS() {
	cpu_reg.PC.B.l = pullS();
	cpu_reg.PC.B.h = pullS();
	cpu_reg.PC.W++;
}
void JMP() {
	cpu_reg.PC.W = cpu_addr;
}
void BIT() {
	cpu_operand = cpu_read(cpu_addr);
	ZFlagSet(cpu_operand & cpu_reg.A);
	cpu_reg.P = RET_FLAG(NEG_FLAG, cpu_operand >> 7, cpu_reg.P);
	cpu_reg.P = RET_FLAG(OVERFLOW_FLAG, cpu_operand >> 6, cpu_reg.P);
}
void CLC() {
	cpu_reg.P = RET_FLAG(CARRY_FLAG, 0x00, cpu_reg.P);
}
void SEC() {
	cpu_reg.P = RET_FLAG(CARRY_FLAG, 0x01, cpu_reg.P);
}
void CLD() {
	cpu_reg.P = RET_FLAG(DECIMAL_FLAG, 0x00, cpu_reg.P);
}
void SED() {
	cpu_reg.P = RET_FLAG(DECIMAL_FLAG, 0x01, cpu_reg.P);
}
void CLI() {
	cpu_reg.P = RET_FLAG(INTERRUPT_FLAG, 0x00, cpu_reg.P);
}
void SEI() {
	cpu_reg.P = RET_FLAG(INTERRUPT_FLAG, 0x01, cpu_reg.P);
}
void CLV() {
	cpu_reg.P = RET_FLAG(OVERFLOW_FLAG, 0x00, cpu_reg.P);
}
void NOP() {}
//illegal
void JAM() {

}
void SLO() {

}
void ANC() {

}
void RLA() {

}
void SRE() {

}
void ALR() {

}
void RRA() {

}
void SAX() {

}
void SHA() {

}
void LAX() {

}
void DCP() {

}
void ISC() {

}
void ARR() {

}
void ANE() {

}
void TAS() {

}
void LXA() {

}
void LAS() {

}
void SBX() {

}
void USBC() {

}
void SHY() {

}
void SHX() {

}


//address modes

void imm() {
	cpu_addr = cpu_read(cpu_reg.PC.W);
	cpu_reg.PC.W++;
}
void abs_() {
	cpu_addr = cpu_read(cpu_reg.PC.W);
	cpu_reg.PC.W++;
	cpu_addr |= cpu_read(cpu_reg.PC.W) << 8;
	cpu_reg.PC.W ++;
}
void absX() {
	cpu_addr = cpu_read(cpu_reg.PC.W);
	cpu_reg.PC.W++;
	cpu_addr |= cpu_read(cpu_reg.PC.W) << 8;
	cpu_reg.PC.W++;

	WORD prev_addr = cpu_addr;
	cpu_addr += cpu_reg.X;

	page_crossed(prev_addr, cpu_addr);

	special_get = bus_get;
	special_set = bus_set;
}
void absY() {
	cpu_addr = cpu_read(cpu_reg.PC.W);
	cpu_reg.PC.W++;
	cpu_addr |= (cpu_read(cpu_reg.PC.W) << 8);
	cpu_reg.PC.W++;

	WORD prev_addr = cpu_addr;
	cpu_addr += cpu_reg.Y;

	page_crossed(prev_addr, cpu_addr);

	special_get = bus_get;
	special_set = bus_set;
}
void zpg() {
	cpu_addr = cpu_read(cpu_reg.PC.W) & 0x00FF;
	cpu_reg.PC.W++;

	special_get = bus_get;
	special_set = bus_set;
}
void zpgX() {
	cpu_addr = (cpu_read(cpu_reg.PC.W) + cpu_reg.X) & 0x00FF;
	cpu_reg.PC.W++;

	special_get = bus_get;
	special_set = bus_set;
}
void zpgY() {
	cpu_addr = (cpu_read(cpu_reg.PC.W) + cpu_reg.Y) & 0x00FF;
	cpu_reg.PC.W++;
}
void rel() {
	cpu_rel_addr = cpu_read(cpu_reg.PC.W);
	cpu_reg.PC.W++;

	if (cpu_rel_addr & 0x80) {
		cpu_rel_addr |= 0xFF00;
	}
}
void ind() {
	WORD ptr = cpu_read(cpu_reg.PC.W);
	cpu_reg.PC.W++;
	ptr |= (cpu_read(cpu_reg.PC.W) << 8);
	cpu_reg.PC.W++;

	cpu_addr = cpu_read(ptr);

	if (ptr & 0x00FF == 0x00FF) {
		cpu_addr |= cpu_read(ptr & 0xFF00) << 8;
	}
	else {
		ptr++;
		cpu_addr |= (cpu_read(ptr) << 8);
	}
}
void indX() {
	BYTE temp_addr = cpu_reg.X + cpu_read(cpu_reg.PC.W);
	cpu_reg.PC.W++;
	cpu_addr = cpu_read(temp_addr & 0xFF);
	temp_addr++;
	cpu_addr |= (cpu_read(temp_addr & 0xFF) << 8);
}
void indY() {
	BYTE temp_addr = cpu_read(cpu_reg.PC.W);
	cpu_reg.PC.W++;
	cpu_addr = cpu_read(temp_addr & 0xFF);
	temp_addr++;
	cpu_addr |= (cpu_read(temp_addr & 0xFF) << 8);

	page_crossed(cpu_addr, cpu_addr + cpu_reg.Y);
	cpu_addr += cpu_reg.Y;
}

void impl() { }
void accum()
{
	special_get = accumulator_get;
	special_set = accumulator_set;
}

//no addressing mode
void noAM() { }
/////////////////////////////////////

instr_addr _instr_mode[0x10][0x10] = {
//		    0			     1			2		     3			   4		   5		     6		      7			   8			9		      A			     B		      C		       D		   E		  F  
/*0*/{BRK, impl,7, ORA, indX, 6, JAM, noAM, 0, SLO, indX, 8, NOP, zpg, 3, ORA, zpg, 3, ASL, zpg, 5, SLO, zpg, 5, PHP, impl, 3, ORA, imm, 2, ASL, accum, 2, ANC, imm, 2, NOP, abs_,4, ORA, abs_,4, ASL, abs_,6, SLO, abs_,6},
/*1*/{BPL, rel, 2, ORA, indY, 5, JAM, noAM, 0, SLO, indY, 8, NOP,zpgX, 4, ORA,zpgX, 4, ASL, zpg, 6, SLO,zpgX, 6, CLC, impl, 2, ORA,absY, 4, NOP, impl,  2, SLO,absY, 7, NOP,absX, 4, ORA,absX, 4, ASL,absX, 7, SLO,absX, 7},
/*2*/{JSR, abs_,6, AND, indY, 6, JAM, noAM, 0, RLA, indX, 8, BIT, zpg, 3, AND, zpg, 3, ROL, zpg, 5, RLA, zpg, 5, PLP, impl, 4, AND, imm, 2, ROL, accum, 2, ANC, imm, 2, BIT, abs_,4, AND, abs_,4, ROL, abs_,6, RLA, abs_,6},
/*3*/{BMI, rel, 2, AND, indY, 5, JAM, noAM, 0, RLA, indY, 8, NOP,zpgX, 4, AND,zpgX, 4, ROL,zpgX, 6, RLA,zpgX, 6, SEC, impl, 2, AND,absY, 4, NOP, impl,  2, RLA,absY, 7, NOP,absX, 4, AND,absX, 4, ROL,absX, 7, RLA,absX, 7},
/*4*/{RTI, impl,6, EOR, indX, 6, JAM, noAM, 0, SRE, indX, 8, NOP, zpg, 3, EOR, zpg, 3, LSR, zpg, 5, SRE, zpg, 5, PHA, impl, 3, EOR, imm, 2, LSR, accum, 2, ALR, imm, 2, JMP, abs_,3, EOR, abs_,4, LSR, abs_,6, SRE, abs_,6},
/*5*/{BVC, rel, 2, EOR, indY, 5, JAM, noAM, 0, SRE, indY, 8, NOP,zpgX, 4, EOR,zpgX, 4, LSR,zpgX, 6, SRE,zpgX, 6, CLI, impl, 2, EOR,absY, 4, NOP, impl,  2, SRE,absY, 7, NOP,absX, 4, EOR,absX, 4, LSR,absX, 7, SRE, abs_,7},
/*6*/{RTS, impl,6, ADC, indX, 6, JAM, noAM, 0, RRA, indX, 8, NOP, zpg, 3, ADC, zpg, 3, ROR, zpg, 5, RRA, zpg, 5, PLA, impl, 4, ADC, imm, 2, ROR, accum, 2, ARR, imm, 2, JMP, ind, 5, ADC, abs_,4, ROR, abs_,6, RRA, abs_,6},
/*7*/{BVS, rel, 2, ADC, indY, 5, JAM, noAM, 0, RRA, indY, 8, NOP,zpgX, 4, ADC,zpgX, 4, ROR,zpgX, 6, RRA,zpgX, 6, SEI, impl, 2, ADC,absY, 4, NOP, impl,  2, RRA,absY, 7, NOP,absX, 4, ADC,absX, 4, ROR,absX, 7, RRA,absX, 7},
/*8*/{NOP, imm, 2, STA, indX, 6, NOP, imm,  2, SAX, indX, 6, STY, zpg, 3, STA, zpg, 3, STX, zpg, 3, SAX, zpg, 3, DEY, impl, 2, NOP, imm, 2, TXA, impl,  2, ANE, imm, 2, STY, abs_, 4,STA, abs_,4, STX, abs_,4, SAX, abs_,4},
/*9*/{BCC, rel, 2, STA, indY, 6, JAM, noAM, 0, SHA, indY, 6, STY,zpgX, 4, STA,zpgX, 4, STX,zpgY, 4, SAX,zpgY, 4, TYA, impl, 2, STA,absY, 5, TXS, impl,  2, TAS,absY, 5, SHY,absX, 5, STA,absX, 5, SHX,absY, 5, SHA,absY, 5},
/*A*/{LDY, imm, 2, LDA, indX, 6, LDX, imm,  2, LAX, indX, 6, LDY, zpg, 3, LDA, zpg, 3, LDX, zpg, 3, LAX, zpg, 3, TAY, impl, 2, LDA, imm, 2, TAX, impl,  2, LXA, imm, 2, LDY, abs_,4, LDA, abs_,4, LDX, abs_,4, LAX, abs_,4},
/*B*/{BCS, rel, 2, LDA, indY, 5, JAM, noAM, 0, LAX, indY, 5, LDY,zpgX, 4, LDA,zpgX, 4, LDX,zpgY, 4, LAX,zpgY, 4, CLV, impl, 2, LDA,absY, 4, TSX, impl,  2, LAS,absY, 4, LDY,absX, 4, LDA,absX, 4, LDX,absY, 4, LAX,absY, 4},
/*C*/{CPY, imm, 2, CMP, indX, 6, NOP, imm,  2, DCP, indX, 8, CPY, zpg, 3, CMP, zpg, 3, DEC, zpg, 5, DCP, zpg, 5, INY, impl, 2, CMP, imm, 2, DEX, impl,  2, SBX, imm, 2, CPY, abs_,4, CMP, abs_,4, DEC, abs_,6, DCP, abs_,6},
/*D*/{BNE, rel, 2, CMP, indY, 5, JAM, noAM, 0, DCP, indY, 8, NOP,zpgX, 4, CMP,zpgX, 4, DEC,zpgX, 6, DCP,zpgX, 6, CLD, impl, 2, CMP,absY, 4, NOP, impl,  2, DCP,absY, 7, NOP,absX, 4, CMP,absX, 4, DEC,absX, 7, DCP,absX, 7},
/*E*/{CPX, imm, 2, SBC, indX, 6, NOP, imm,  2, ISC, indX, 8, CPX, zpg, 3, SBC, zpg, 3, INC, zpg, 5, ISC, zpg, 5, INX, impl, 2, SBC, imm, 2, NOP, impl,  2, USBC,imm, 2, CPX, abs_,4, SBC, abs_,4, INC, abs_,6, ISC, abs_,6},
/*F*/{BEQ, rel, 2, SBC, indY, 5, JAM, noAM, 0, ISC, indY, 8, NOP,zpgX, 4, SBC,zpgX, 4, INC,zpgX, 6, ISC,zpgX, 6, SED, impl, 2, SBC,absY, 4, NOP, impl,  2, ISC,absY, 7, NOP,absX, 4, SBC,absX, 4, INC,absX, 7, ISC,absX, 7}
};

void reset_cpu() {
	cpu_reg.A = 0;
	cpu_reg.X = 0;
	cpu_reg.Y = 0;
	cpu_reg.P = 0;
	cpu_reg.P |= 1 << RESERVED_FLAG | 1 << ZERO_FLAG;
	cpu_reg.SP = 0xFF;
	cpu_reg.PC.B.l = cpu_read(0xFFFC);
	cpu_reg.PC.B.h = cpu_read(0xFFFD);
	//Frame period must be added
}

void clock_cpu() {
	operationCode = cpu_read(cpu_reg.PC.W++);
	ia = _instr_mode[operationCode & 0xF0][operationCode & 0x0F];
	ia.mode();
	ia.instr();
}

void execute_cpu() {

}

void nmi_cpu() {
	pushS(cpu_reg.PC.B.h);
	pushS(cpu_reg.PC.B.l);

	cpu_reg.P = RET_FLAG(BREAK_FLAG, 0x00, cpu_reg.P);
	cpu_reg.P = RET_FLAG(INTERRUPT_FLAG, 0x01, cpu_reg.P);

	pushS(cpu_reg.P);

	cpu_reg.PC.B.l = cpu_read(0xFFFA);
	cpu_reg.PC.B.h = cpu_read(0xFFFB);
	cycles_num = 8;
}
void irq_cpu() {
	if (GET_FLAG(INTERRUPT_FLAG, cpu_reg.P) & 0x01) {
		pushS(cpu_reg.PC.B.h);
		pushS(cpu_reg.PC.B.l);

		cpu_reg.P = RET_FLAG(BREAK_FLAG, 0x00, cpu_reg.P);
		cpu_reg.P = RET_FLAG(INTERRUPT_FLAG, 0x01, cpu_reg.P);

		pushS(cpu_reg.P);

		cpu_reg.PC.B.l = cpu_read(0xFFFE);
		cpu_reg.PC.B.h = cpu_read(0xFFFF);
		cycles_num = 7;
	}
}