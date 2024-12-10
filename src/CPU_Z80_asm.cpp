#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include <cstring>
#include <string>

#include <stdint.h>

#include "Eagle.hpp"
#include "CPU_Z80.hpp"

CPU_Z80::CPU_Z80()
{

}

void CPU_Z80::initialize()
{
	// more to come here
}


void CPU_Z80::asm_do_else()
{

}

std::string CPU_Z80::asm_bru(const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2,int type,int clabel,
int &ilabel)
{
	std::string label_adr = ".label_b"+std::to_string(clabel);
	std::string text_code;

	if(type == TYPE_IF) //if
	{
		label_adr = ".label_"+std::to_string(ilabel);
		ilabel++;
	}

	if(src1.type == EAGLE_keywords::IDX)
	{
		text_code += "dex \n";
	}else
	if(src1.type == EAGLE_keywords::IDY)
	{
		text_code += "dey \n";
	}

	text_code += "cp \n";

	text_code += "jp " + label_adr + "\n";

	return text_code;
}

std::string CPU_Z80::asm_call_jump(const EAGLE_VARIABLE &src,int narg,int type,std::string &labelcall)
{
	std::string mnemonic = "jp ",mne1,mne2,saddress,saddress2;
	std::string text_code;

	if(type >= 1)
		mnemonic = "call ";

	for(int i = 0;i < narg;i++)
	{

	}

	//------
	std::string src1value = labelcall;
	if(src.bimm == true)
	{
		src1value = std::to_string(src.immediate);
	}else
	{
		if(src.blabel == false)
		{

			if(src.type == EAGLE_keywords::UINT16)
			{
				text_code += "ld hl," + std::to_string(src.address) +"\n";
				src1value = "(hl)";
			}


			if(src.type == EAGLE_keywords::IDX)
			{
				src1value = "(IX)";
			}
			if(src.type == EAGLE_keywords::IDY)
			{
				src1value = "(IY)";
			}
		}
	}

	text_code += mnemonic + src1value +"\n";

	return text_code;
}


std::string CPU_Z80::asm_return(const EAGLE_VARIABLE &ret, bool retvoid)
{
	std::string text_code;
	if(retvoid == true)
	{
		return "ret\n";

	}else
	{
		if(ret.type == EAGLE_keywords::IDX)
		{
			text_code += "ld a,IX\n";
		}

		if(ret.type == EAGLE_keywords::IDY)
		{
			text_code += "ld a,IY\n";
		}

		if(ret.type != EAGLE_keywords::ACC)
		{
			if(ret.bimm == true)
				text_code += "ld a,"+ std::to_string(ret.immediate&0xFF) +"\n";
			else
				text_code += "lda a,("+ std::to_string(ret.address) +")\n";
		}

		text_code += "ret\n";
	}

	return text_code;
}


std::string CPU_Z80::asm_alu(const EAGLE_VARIABLE &dst,const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2)
{
	std::string mnemonic;
	std::string src1value,dstvalue;
	std::string src2value;
	std::string reg = "a,b";
	std::string text_code;

	if(dst.bimm == true)
	{
		dstvalue = std::to_string(dst.immediate&0xFF);
	}
	else
	{
		dstvalue = "(" + std::to_string(dst.address&0xFF) +")";
	}


	if(src1.bimm == true)
	{
		src1value = std::to_string(src1.immediate&0xFF);
	}
	else
	{
		src1value = "(" + std::to_string(src1.address&0xFF) +")";
	}

	if(src2.bimm == true)
	{
		src2value = std::to_string(src2.immediate&0xFF);
	}else
	{
		src2value = "(" + std::to_string(src2.address&0xFF) +")";
	}

	if(operator1 == '=')
	{
		text_code += "ld a," + src2value + "\n";
		text_code += "ld " + dstvalue + ",a\n";
		return text_code;
	}

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


	if(operator1 == '*')
	{

	}

	if(operator1 == '/')
	{

	}


	text_code += "ld a," + src1value + "\n";
	text_code += "ld b," + src2value + "\n";

	text_code += mnemonic + reg +"\n";

	text_code += "ld " + dstvalue + ",a\n";

	return text_code;
}


