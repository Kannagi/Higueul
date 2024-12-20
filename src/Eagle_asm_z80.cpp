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
	std::string label_adr = ".label_b"+std::to_string(clabel);

	if(type == TYPE_IF) //if
	{
		label_adr = ".label_"+std::to_string(this->ilabel);
		this->ilabel++;
	}

	if(src1.type == EAGLE_keywords::IDX)
	{
		this->text_code += "dex \n";
	}else
	if(src1.type == EAGLE_keywords::IDY)
	{
		this->text_code += "dey \n";
	}

	this->text_code += "cp \n";

	this->text_code += "jp " + label_adr + "\n";
}

void Eagle::asm_call_jump_z80(const EAGLE_VARIABLE &src,int ninst,int type)
{
	std::string mnemonic = "jp ",mne1,mne2,saddress,saddress2;
	if(type >= 1)
		mnemonic = "call ";


	for(int i = 0;i < ninst;i++)
	{

	}

	//------
	std::string src1value = this->labelcall;
	if(src.bimm == true)
	{
		src1value = std::to_string(src.immediate);
	}else
	{
		if(src.blabel == false)
		{

			if(src.type == EAGLE_keywords::UINT16)
			{
				this->text_code += "ld hl," + std::to_string(src.address) +"\n";
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

	this->text_code += mnemonic + src1value +"\n";
}

void Eagle::asm_return_z80(const EAGLE_VARIABLE &ret,bool retvoid)
{
	if(retvoid == true)
	{
		this->text_code += "ret\n";

	}else
	{
		if(ret.type == EAGLE_keywords::IDX)
		{
			this->text_code += "ld a,IX\n";
		}

		if(ret.type == EAGLE_keywords::IDY)
		{
			this->text_code += "ld a,IY\n";
		}

		if(ret.type != EAGLE_keywords::ACC)
		{
			if(ret.bimm == true)
				this->text_code += "ld a,"+ std::to_string(ret.immediate&0xFF) +"\n";
			else
				this->text_code += "lda a,("+ std::to_string(ret.address) +")\n";
		}

		this->text_code += "ret\n";
	}
}

void Eagle::asm_alu_z80(const EAGLE_VARIABLE &dst,const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2)
{
	std::string mnemonic;
	std::string src1value,dstvalue;
	std::string src2value;
	std::string reg = "a,b";

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
		this->text_code += "ld a," + src2value + "\n";
		this->text_code += "ld " + dstvalue + ",a\n";
		return;
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


	this->text_code += "ld a," + src1value + "\n";
	this->text_code += "ld b," + src2value + "\n";

	this->text_code += mnemonic + reg +"\n";

	this->text_code += "ld " + dstvalue + ",a\n";
}
