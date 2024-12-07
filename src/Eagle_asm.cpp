

#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include <cstring>
#include <string>

#include <stdint.h>

#include "Eagle.hpp"
#include "Eagle_asm_z80.hpp"

void Eagle::asm_bru(const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2,int type,int clabel)
{
	if(this->target == TARGET_65816)
		asm_bru_65816(src1,src2,operator1,operator2,type,clabel);

	if( (this->target == TARGET_6502) | (this->target == TARGET_65C02) | (this->target == TARGET_HuC6520) )
		asm_bru_6502(src1,src2,operator1,operator2,type,clabel);

	if(this->target == TARGET_Z80)
		this->asm_z80->asm_bru(src1,src2,operator1,operator2,type,clabel);

	if(this->target == TARGET_AltairX)
		asm_bru_AltairX(src1,src2,operator1,operator2,type,clabel);
}

void Eagle::asm_call_jump(const EAGLE_VARIABLE &src,int ninst,int type)
{
	if(this->target == TARGET_65816)
		asm_call_jump_65816(src,ninst,type);

	if( (this->target == TARGET_6502) | (this->target == TARGET_65C02) | (this->target == TARGET_HuC6520) )
		asm_call_jump_6502(src,ninst,type);

	if(this->target == TARGET_Z80)
		this->asm_z80->asm_call_jump(src, ninst, type, this->labelcall);

	if(this->target == TARGET_AltairX)
		asm_call_jump_AltairX(src,ninst,type);
}

void Eagle::asm_return(const EAGLE_VARIABLE &ret,bool retvoid)
{
	if(this->target == TARGET_65816)
		asm_return_65816(ret,retvoid);

	if( (this->target == TARGET_6502) | (this->target == TARGET_65C02) | (this->target == TARGET_HuC6520) )
		asm_return_6502(ret,retvoid);

	if(this->target == TARGET_Z80)
		this->asm_z80->asm_return(ret,retvoid);

	if(this->target == TARGET_AltairX)
		asm_return_AltairX(ret,retvoid);
}

void Eagle::asm_alu(const EAGLE_VARIABLE &dst,const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2)
{
	if(this->target == TARGET_65816)
		asm_alu_65816(dst,src1,src2,operator1,operator2);

	if( (this->target == TARGET_6502) | (this->target == TARGET_65C02) | (this->target == TARGET_HuC6520) )
		asm_alu_6502(dst,src1,src2,operator1,operator2);

	if(this->target == TARGET_Z80)
		this->asm_z80->asm_alu(dst,src1,src2,operator1,operator2);

	if(this->target == TARGET_AltairX)
		asm_alu_AltairX(dst,src1,src2,operator1,operator2);
}




