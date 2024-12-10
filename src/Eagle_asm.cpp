

#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include <cstring>
#include <string>

#include <stdint.h>

#include "Eagle.hpp"

void Eagle::asm_bru(const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2,int type,int clabel)
{
	switch(this->target)
	{
		case TARGET_65816:
			asm_bru_65816(src1,src2,operator1,operator2,type,clabel);
		break;

		case TARGET_6502:
		case TARGET_65C02:
		case TARGET_HuC6520:
			asm_bru_6502(src1,src2,operator1,operator2,type,clabel);
		break;

		case TARGET_Z80:
			this->text_code += cpu_z80.asm_bru(src1,src2,operator1,operator2,type,clabel,this->ilabel);
		break;

		case TARGET_AltairX:
			asm_bru_AltairX(src1,src2,operator1,operator2,type,clabel);
		break;
	}
}

void Eagle::asm_call_jump(const EAGLE_VARIABLE &src,int ninst,int type)
{
	switch(this->target)
	{
		case TARGET_65816:
			asm_call_jump_65816(src,ninst,type);
		break;

		case TARGET_6502:
		case TARGET_65C02:
		case TARGET_HuC6520:
			asm_call_jump_6502(src,ninst,type);
		break;

		case TARGET_Z80:
			this->text_code += cpu_z80.asm_call_jump(src,ninst,type,this->labelcall);
		break;

		case TARGET_AltairX:
			asm_call_jump_AltairX(src,ninst,type);
		break;
	}
}

void Eagle::asm_return(const EAGLE_VARIABLE &ret,bool retvoid)
{
	switch(this->target)
	{
		case TARGET_65816:
			asm_return_65816(ret,retvoid);
		break;

		case TARGET_6502:
		case TARGET_65C02:
		case TARGET_HuC6520:
			asm_return_6502(ret,retvoid);
		break;

		case TARGET_Z80:
			this->text_code += cpu_z80.asm_return(ret,retvoid);
		break;

		case TARGET_AltairX:
			asm_return_AltairX(ret,retvoid);
		break;
	}
}

void Eagle::asm_alu(const EAGLE_VARIABLE &dst,const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2)
{
	switch(this->target)
	{
		case TARGET_65816:
			asm_alu_65816(dst,src1,src2,operator1,operator2);
		break;

		case TARGET_6502:
		case TARGET_65C02:
		case TARGET_HuC6520:
			asm_alu_6502(dst,src1,src2,operator1,operator2);
		break;

		case TARGET_Z80:
			this->text_code += cpu_z80.asm_alu(dst,src1,src2,operator1,operator2);
		break;

		case TARGET_AltairX:
			asm_alu_AltairX(dst,src1,src2,operator1,operator2);
		break;
	}
}

void Eagle::asm_do_else()
{
	switch(this->target)
	{
		case TARGET_65816:
		break;

		case TARGET_6502:
		case TARGET_65C02:
		case TARGET_HuC6520:
		break;

		case TARGET_Z80:
			cpu_z80.asm_do_else();
		break;

		case TARGET_AltairX:
		break;
	}
}




