#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include <cstring>
#include <string>

#include <stdint.h>

#include "Eagle.hpp"

void Eagle::asm_bru_AltairX(const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2,int type,int clabel)
{

}

void Eagle::asm_call_jump_AltairX(const EAGLE_VARIABLE &src,int ninst,int type)
{

}

void Eagle::asm_return_AltairX(const EAGLE_VARIABLE &ret,bool retvoid)
{

}

void Eagle::asm_alu_AltairX(const EAGLE_VARIABLE &dst,const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2)
{
	std::string mnemonic;
	std::string src1value;
	std::string src2value;
	std::string dstvalue;

	std::string str_code1,str_code2;

	bool optimize = true;
	bool operation = false;

	if( (src1.bptr == true) || (src2.bptr == true) )
		optimize = false;

	if(operator1 == '+')
		mnemonic = "add ";

	if(operator1 == '-')
		mnemonic = "sub ";

	if(operator1 == '&')
		mnemonic = "and ";

	if(operator1 == '|')
		mnemonic = "ora ";

	if(operator1 == '^')
		mnemonic = "eor ";

	if(operator1 == '<')
		mnemonic = "asl ";

	if(operator1 == '>')
		mnemonic = "lsr ";

	if(operator1 == '*')
	{

	}

	if(operator1 == '/')
	{

	}

	if(operator1 == '%')
	{

	}

	if(operator1 == '=')
	{
		this->text_code += "ori " + dstvalue + "," + src2value  +",0\n";
	}else
	{
		this->text_code += mnemonic + dstvalue + "," + src1value +"," + src2value+"\n";
	}
}

void Eagle::asm_falu_AltairX(const EAGLE_VARIABLE &dst,const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2)
{

}

