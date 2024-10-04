#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include <cstring>
#include <string>

#include <stdint.h>

#include "Eagle.hpp"

static void asm_address(const EAGLE_VARIABLE &src,std::string &labelp,const std::string &pregister,std::string &srcvalue,std::string &str_code);

void Eagle::asm_bru_AltairX(const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2,int type,int clabel)
{

}

void Eagle::asm_call_jump_AltairX(const EAGLE_VARIABLE &src,int ninst,int type)
{

}

void Eagle::asm_return_AltairX(const EAGLE_VARIABLE &ret,bool retvoid)
{
	if(retvoid == true)
	{
		this->text_code += "ret\n";
	}else
	{
		if(ret.type == EAGLE_keywords::IDX)
		{
			this->text_code += "move acc,t1\n";
		}

		if(ret.type == EAGLE_keywords::IDY)
		{
			this->text_code += "move acc,t2\n";
		}

		if(ret.type == EAGLE_keywords::BACC)
		{
			this->text_code += "move a0,t0\n";
		}

		if(ret.type != EAGLE_keywords::ACC)
		{
			if(ret.bimm == true)
			{
				this->text_code += "movei a0,"+ std::to_string(ret.immediate&0x3FFFF) +"\n";
			}
			else
			{
				this->text_code += "ldi.p t0,"+ std::to_string(ret.address&0x3FF) +",[rz]\n";
				this->text_code += "moveix "+ std::to_string(ret.address&0xFFFFFF00) +"\n";
				this->text_code += "move acc,t0\n";
			}
		}

		this->text_code += "ret\n";

	}
}

void Eagle::asm_alu_AltairX(const EAGLE_VARIABLE &dst,const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2)
{
	std::string mnemonic;
	std::string src1value;
	std::string src2value;
	std::string dstvalue;

	std::string str_code1,str_code2;

	bool PQope = false;

	uint64_t adrl;
	uint64_t adrh = 0;


	asm_address(src1,this->label1,"t1",src1value,this->text_code);
	asm_address(src2,this->label2,"t2",src2value,this->text_code);


	if(operator1 == '+')
		mnemonic = "add";

	if(operator1 == '-')
		mnemonic = "sub";

	if(operator1 == '&')
		mnemonic = "and";

	if(operator1 == '|')
		mnemonic = "ora";

	if(operator1 == '^')
		mnemonic = "eor";

	if(operator1 == '<')
		mnemonic = "asl";

	if(operator1 == '>')
		mnemonic = "lsr";

	if(operator1 == '*')
		mnemonic = "mul";

	if(operator1 == '/')
		mnemonic = "div";

	if(operator1 == '%')
		mnemonic = "div";
	//---------------
	bool dst_other = false;
	if(dst.type2 == EAGLE_keywords::REGISTER)
	{
		if(dst.address < 8)
			dstvalue = "t" + std::to_string(dst.address+3);
		else
			dstvalue = "s" + std::to_string(dst.address-8);
	}else
	if(dst.type == EAGLE_keywords::IDX)
	{
		dstvalue = "t1";
	}else
	if(dst.type == EAGLE_keywords::IDY)
	{
		dstvalue = "t2";
	}else
	if(dst.type == EAGLE_keywords::ACC)
	{
		dstvalue = "acc";
	}else
	{
		dstvalue = "t0";
		dst_other = true;
	}

	if( (dst.type == EAGLE_keywords::UINT8) || (dst.type == EAGLE_keywords::INT8))
		mnemonic += ".b";

	if( (dst.type == EAGLE_keywords::UINT16) || (dst.type == EAGLE_keywords::INT16))
		mnemonic += ".w";

	if( (dst.type == EAGLE_keywords::UINT32) || (dst.type == EAGLE_keywords::INT32))
		mnemonic += ".l";

	if( (dst.type == EAGLE_keywords::UINT64) || (dst.type == EAGLE_keywords::INT64))
		mnemonic += ".q";


	if(src2.bimm == true)
	{
		adrl = dst.address &0xFF;
		adrh = (dst.address &0xFFFFFF00)>>8;
		src2value = std::to_string(adrl);
		if(adrh > 0)
			mnemonic += "p";
	}


	mnemonic += " ";


	if(operator1 == '*')
	{
		this->text_code += mnemonic + src1value + "," + src2value  +"\n";
		this->text_code += "move " + dstvalue + ",PL\n";
		PQope = true;
	}

	if(operator1 == '/')
	{
		this->text_code += mnemonic + src1value + "," + src2value  +"\n";
		this->text_code += "move " + dstvalue + ",Q\n";
		PQope = true;
	}

	if(operator1 == '%')
	{
		this->text_code += mnemonic + src1value + "," + src2value  +"\n";
		this->text_code += "move " + dstvalue + ",QR\n";
		PQope = true;
	}


	if(operator1 == '=')
	{
		this->text_code += "ori " + dstvalue + "," + src2value  +",0\n";
	}else
	{
		if(PQope == false)
			this->text_code += mnemonic + dstvalue + "," + src1value +"," + src2value+"\n";
	}

	if(src2.bimm == true)
	{
		if(adrh > 0)
			this->text_code += "moveix "+ std::to_string(adrh) +"\n";
	}

	if(dst_other == false)
		return;



	if( (dst.type == EAGLE_keywords::UINT8) || (dst.type == EAGLE_keywords::INT8))
		mnemonic = "sti.bp ";

	if( (dst.type == EAGLE_keywords::UINT16) || (dst.type == EAGLE_keywords::INT16))
		mnemonic = "sti.wp ";

	if( (dst.type == EAGLE_keywords::UINT32) || (dst.type == EAGLE_keywords::INT32))
		mnemonic = "sti.lp ";

	if( (dst.type == EAGLE_keywords::UINT64) || (dst.type == EAGLE_keywords::INT64))
		mnemonic = "sti.qp ";

	if(dst.bptr == false)
	{
		adrl = dst.address &0xFF;
		adrh = (dst.address &0xFFFFFF00)>>8;

		this->text_code += mnemonic + "t0," + std::to_string(adrl) + "[rz]\n";
		this->text_code += "moveix "+ std::to_string(adrh) +"\n";
	}
	else
	{

	}
}

void Eagle::asm_falu_AltairX(const EAGLE_VARIABLE &dst,const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2)
{

}


static void asm_address(const EAGLE_VARIABLE &src,std::string &labelp,const std::string &pregister,std::string &srcvalue,std::string &str_code)
{

	if(src.bptr == false)
	{
		if(src.blabel == true)
		{

			if(src.token1 == '$')
				srcvalue = labelp;
			else
			{
				srcvalue = pregister;
				//str_code = "ld t0," + labelp
			}

		}else
		{
			srcvalue = std::to_string(src.address);

			if(src.token1 == '$')
				srcvalue = std::to_string(src.address&0xFFFF);

			if(src.type2 == EAGLE_keywords::REGISTER)
			{
				if(src.address < 8)
					srcvalue = "t" + std::to_string(src.address+3);
				else
					srcvalue = "s" + std::to_string(src.address-8);
			}else
			if(src.type == EAGLE_keywords::IDX)
			{
				srcvalue = "t1";
			}else
			if(src.type == EAGLE_keywords::IDY)
			{
				srcvalue = "t2";
			}else
			if(src.type == EAGLE_keywords::ACC)
			{
				srcvalue = "acc";
			}else
			if(src.type == EAGLE_keywords::BACC)
			{
				srcvalue = "t0";
			}

		}


	}
	else
	{
		bool py = false;
		bool px = false;

		//arg2
		if(src.ptr2.bimm == true)
		{
			str_code += "ldx #" + std::to_string(src.ptr2.value&0xFFFF) +"\n";
			px = true;
		}else
		{
			if(src.ptr2.type == EAGLE_keywords::IDX)
			{
				px = true;
			}else
			if(src.ptr2.type == EAGLE_keywords::IDY)
			{
				py = true;
			}else
			if(src.ptr2.type == EAGLE_keywords::ACC)
			{
				px = true;
				str_code += "tax\n";
			}else
			if(src.ptr2.type != EAGLE_keywords::UNKNOW)
			{
				px = true;
				str_code += "lda " + std::to_string(src.ptr2.value) +"\n";
				str_code += "tax\n";
			}
		}

		//arg1
		if(src.ptr1.bimm == true)
		{
			srcvalue  = std::to_string(src.ptr1.value);

		}else
		{
			srcvalue = "("+ pregister + ")";

			if(src.ptr1.type == EAGLE_keywords::IDX)
			{
				str_code += "stx "+ pregister +"\n";
			}else
			if(src.ptr1.type == EAGLE_keywords::IDY)
			{
				str_code += "sty "+ pregister +"\n";
			}else
			if(src.ptr1.type == EAGLE_keywords::ACC)
			{
				str_code += "sta "+ pregister +"\n";
			}else
			{
				if(src.ptr1.token2 == ':')
				{
					srcvalue = labelp;
					if(src.ptr1.token1 == '$')
						srcvalue = "#" + labelp;

					if(src.ptr1.token1 == '#')
						srcvalue = "#:" + labelp;
				}else
				{
					srcvalue = std::to_string(src.ptr1.value);

					if(src.ptr1.token1 == '$')
						srcvalue = "#" + std::to_string(src.ptr1.value&0xFFFF);

					if(src.ptr1.token1 == '#')
						srcvalue = "#:" + std::to_string(src.ptr1.value>>16);
				}
			}
		}

		if(src.ptr2_exist == true)
		{
			if(px == true)
				srcvalue += ",X";

			if(py == true)
				srcvalue += ",Y";
		}
	}

	if(src.token1 == '@')
		srcvalue = "[" + srcvalue + "]";

	if(src.token2 == '@')
		srcvalue = "(" + srcvalue + ")";
}

