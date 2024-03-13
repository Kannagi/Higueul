

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
	if(this->target == TARGET_65816)
		asm_bru_65816(src1,src2,operator1,operator2,type,clabel);
/*
	if(this->target == TARGET_6502)
		asm_bru_6502(src1,src2,operator1,operator2,type,clabel);
*/

	if(this->target == TARGET_AltairX)
		asm_bru_AltairX(src1,src2,operator1,operator2,type,clabel);
}

void Eagle::asm_call_jump(const EAGLE_VARIABLE &src,int ninst,int type)
{
	if(this->target == TARGET_65816)
		asm_call_jump_65816(src,ninst,type);

	if(this->target == TARGET_AltairX)
		asm_call_jump_AltairX(src,ninst,type);
}

void Eagle::asm_return(const EAGLE_VARIABLE &ret,bool retvoid)
{
	if(this->target == TARGET_65816)
		asm_return_65816(ret,retvoid);

	if(this->target == TARGET_AltairX)
		asm_return_AltairX(ret,retvoid);
}

void Eagle::asm_alu(const EAGLE_VARIABLE &dst,const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2)
{
	if(this->target == TARGET_65816)
		asm_alu_65816(dst,src1,src2,operator1,operator2);

	if(this->target == TARGET_AltairX)
		asm_alu_AltairX(dst,src1,src2,operator1,operator2);
}

void Eagle::asm_falu(const EAGLE_VARIABLE &dst,const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2)
{
	if(this->target == TARGET_AltairX)
		asm_falu_AltairX(dst,src1,src2,operator1,operator2);
}



