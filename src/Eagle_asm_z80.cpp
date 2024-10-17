#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include <cstring>
#include <string>

#include <stdint.h>

#include "Eagle.hpp"

void Eagle::asm_bru_z80(const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2,int type,int clabel)
{

}

void Eagle::asm_call_jump_z80(const EAGLE_VARIABLE &src,int ninst,int type)
{

}

void Eagle::asm_return_z80(const EAGLE_VARIABLE &ret,bool retvoid)
{

}

void Eagle::asm_alu_z80(const EAGLE_VARIABLE &dst,const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2)
{
	std::string mnemonic;
	std::string src1value;
	std::string src2value;
	std::string reg = "b";

	if(operator1 == '+')
		mnemonic = "add ";

	if(operator1 == '-')
		mnemonic = "sub ";

	if(operator1 == '&')
		mnemonic = "and ";

	if(operator1 == '|')
		mnemonic = "or ";

	if(operator1 == '^')
		mnemonic = "xor ";

	if(operator1 == '<')
		mnemonic = "sla ";

	if(operator1 == '>')
		mnemonic = "sra ";


	this->text_code += "ld a," + src1value + "\n";
	this->text_code += "ld b," + src2value + "\n";

	this->text_code += mnemonic + reg +"\n";
}
