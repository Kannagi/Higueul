

#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include <cstring>
#include <string>

#include <stdint.h>

#include "Eagle.hpp"

#define TYPE_LOOP 0
#define TYPE_WHILE 1
#define TYPE_IF 2

static int asm_address(const EAGLE_VARIABLE &src,std::string &labelp,const std::string &pregister,std::string &srcvalue,std::string &str_code);

void Eagle::asm_bru_65816(const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2,int type,int clabel)
{
	std::string src1value;
	std::string src2value;
	std::string label_adr = ".label_b"+std::to_string(clabel);

	if(type == TYPE_IF) //if
	{
		label_adr = ".label_"+std::to_string(this->ilabel);
		this->ilabel++;
	}

	std::string str_code1,str_code2;
	//----------------
	asm_address(src1,this->label1,"218",src1value,this->text_code);
	asm_address(src2,this->label2,"220",src2value,this->text_code);

	//-------------------

	//--- loop dec
	bool optimize_zero = false;
	if(type == TYPE_LOOP)
	{
		if(src1.type == EAGLE_keywords::IDX)
		{
			this->text_code += "dex \n";
		}else
		if(src1.type == EAGLE_keywords::IDY)
		{
			this->text_code += "dey \n";
		}else
		{
			if(src1.type == EAGLE_keywords::ACC)
			{
				this->text_code += "dea \n";
			}else
			{
				this->text_code += "dec " + src1value +"\n";
			}
		}

		if(src2.bimm == true)
		{
			if( (src2.immediate == 0) && ( (operator1 == '=') || (operator1 == '!') ) )
				optimize_zero = true;
		}
	}

	if(operator1 == '?')
	{
		if( (src2.immediate == 1) && (src2.bimm == true) )
		{
			if(operator2 == '=')
				this->text_code += "bvc " + label_adr +"\n";
			else
				this->text_code += "bvs " + label_adr +"\n";
			return;
		}

		if( (src2.immediate == 0) && (src2.bimm == true) )
		{
			if(operator2 == '=')
				this->text_code += "bcc " + label_adr +"\n";
			else
				this->text_code += "bcs " + label_adr +"\n";
			return;
		}

		if( (src2.immediate == 2) && (src2.bimm == true) )
		{
			if(operator2 == '=')
				this->text_code += "bpl " + label_adr +"\n";
			else
				this->text_code += "bmi " + label_adr +"\n";
			return;
		}

		if( (src2.immediate == 3) && (src2.bimm == true) )
		{
			if(operator2 == '=')
				this->text_code += "beq " + label_adr +"\n";
			else
				this->text_code += "bne " + label_adr +"\n";
			return;
		}


		if(operator2 == '=')
			this->text_code += "bcc " + label_adr +"\n";
		else
			this->text_code += "bcs " + label_adr +"\n";
		return;
	}

	if( (src1.bimm == true) && (src2.bimm == true) )
	{
		if(src1.immediate != 0)
		{
			this->text_code += "\njmp " + label_adr +"\n";
		}
		return;
	}


	//----
	if(optimize_zero == false)
	{
		if(src2.type == EAGLE_keywords::IDX)
		{
			src2value = "222";
			this->text_code += "stx 222\n";
		}else if(src2.type == EAGLE_keywords::IDY)
		{
			src2value = "222";
			this->text_code += "sty 222\n";
		}else if(src2.type == EAGLE_keywords::ACC)
		{
			src2value = "222";
			this->text_code += "sta 222\n";
		}

		if(src1.type == EAGLE_keywords::IDX)
		this->text_code += "txa \n";
		else
		if(src1.type == EAGLE_keywords::IDY)
			this->text_code += "tya \n";
		else
		if(src1.type != EAGLE_keywords::ACC)
			this->text_code += "lda " + src1value +"\n";
	}


	//----------------
	if(operator1 == '&')
	{
		this->text_code += "bit " + src2value +"\n";
		if(operator2 == '!')
		{
			if(type == TYPE_IF)
				this->text_code += "bne " + label_adr +"\n";
			else
				this->text_code += "beq " + label_adr +"\n";
			return;
		}


		if(type == TYPE_IF)
			this->text_code += "beq " + label_adr +"\n";
		else
			this->text_code += "bne " + label_adr +"\n";
		return;
	}


	//----------------
	std::string mne1 = "bcc";
	std::string mne2 = "bcs";

	//if( (var.type == EAGLE_keywords::INT8) || (var.type == EAGLE_keywords::INT16))

	if(optimize_zero == false)
	{
		this->text_code += "cmp " + src2value  +"\n";
	}

	if(operator1 == '=')
	{
		if(type == TYPE_WHILE)
			this->text_code += "beq " + label_adr +"\n";
		else
			this->text_code += "bne " + label_adr +"\n";

	}

	if( (operator1 == '!') && (operator2 == '=') )
	{
		if(type == TYPE_WHILE)
			this->text_code += "bne " + label_adr +"\n";
		else
			this->text_code += "beq " + label_adr +"\n";
	}

	if( (operator1 == '<') && (operator2 == '=') )
	{
		if(type == TYPE_IF)
		{
			this->text_code += "beq .label_a" + std::to_string(this->ilabel) +":\n";
			this->text_code += "cmp " + src2value  +"\n";
			this->text_code += "bcs " + label_adr +"\n";
			this->text_code += ".label_a"+std::to_string(this->ilabel)+":\n";
		}
		else
			this->text_code += "bcc " + label_adr +"\n";
	}else
	if(operator1 == '<')
	{
		if(type == TYPE_IF)
		{
			this->text_code += "bcs " + label_adr +"\n";
		}
		else
			this->text_code += "bcc " + label_adr +"\n";
	}

	if( (operator1 == '>') && (operator2 == '=') )
	{
		if(type == TYPE_IF)
			this->text_code += "bcc " + label_adr +"\n";
		else
			this->text_code += "bcs " + label_adr +"\n";
	}else
	if(operator1 == '>')
	{
		if(type == TYPE_IF)
		{
			this->text_code += "bcc " + label_adr +"\n";
			this->text_code += "cmp " + src2value  +"\n";
			this->text_code += "beq " + label_adr +"\n";
		}
		else
			this->text_code += "bcs " + label_adr +"\n";
	}

}


void Eagle::asm_call_jump_65816(const EAGLE_VARIABLE &src,int ninst,int type)
{
	std::string src1value = this->labelcall,tmp;
	std::string mnemonic = "jmp ",mne1,mne2,saddress;
	if(type >= 1)
		mnemonic = "jsr ";

	std::string srcvalue,pbeg,pend;

	for(int i = 0;i < ninst;i++)
	{
		EAGLE_VARIABLE var = this->arg[i];

		if(var.type == EAGLE_keywords::IDX)
		{
			this->text_code += "stx @" + src1value + "..arg" + std::to_string(i) +"\n";
		}else
		if(var.type == EAGLE_keywords::IDY)
		{
			this->text_code += "sty @" + src1value + "..arg" + std::to_string(i) +"\n";
		}else
		if(var.type == EAGLE_keywords::ACC)
		{
			this->text_code += "sta @" + src1value + "..arg" + std::to_string(i) +"\n";
		}else
		{
			if( (var.type == EAGLE_keywords::UINT8) || (var.type == EAGLE_keywords::INT8))
			{
				mne1 = "lda ";
				mne2 = "sta @";
			}
			else
			{
				mne1 = "ldx ";
				mne2 = "stx @";
			}
			if(var.bptr == true)
			{
				asm_address(var,this->labelarg[i],"218",saddress,this->text_code);
			}else
			if(var.blabel == true)
			{
				if(var.token1 == '$')
					saddress = "#" + this->labelarg[i];
				else if(var.token1 == '#')
					saddress = "#:" + this->labelarg[i];
				else
					saddress = this->labelarg[i];
			}
			else if(var.token1 == '$')
			{
				mne1 = "ldx ";
				mne2 = "stx @";
				saddress = "#" + std::to_string(var.address&0xFFFF);
			}
			else if(var.token1 == '#')
				saddress = "#" + std::to_string(var.address>>16);
			else if(var.bimm == false)
				saddress = std::to_string(var.address);
			else
				saddress = "#" + std::to_string(var.immediate&0xFFFF);

			if(var.token1 == '@')
				saddress = "(" + saddress + ")";

			this->text_code += mne1 + saddress + "\n";
			this->text_code += mne2 + src1value + "..arg" + std::to_string(i) + "\n";


		}
	}

	if(src.bimm == true)
	{
		src1value = std::to_string(src.immediate);
	}else
	{
		if(src.blabel == false)
		{
			tmp = std::to_string(src.address);
			if(src.type == EAGLE_keywords::IDX)
			{
				this->text_code += "stx 222\n";
				tmp = "222";
			}
			if(src.type == EAGLE_keywords::IDY)
			{
				this->text_code += "sty 222\n";
				tmp = "222";
			}
			src1value =  "(" + tmp + ")";
			if(type >= 1)
			{
				this->text_code += "phx\nldx #0\n";
				pend = "plx\n";
			}

		}
	}

	this->text_code += mnemonic + src1value +"\n" + pend;
}

void Eagle::asm_return_65816(const EAGLE_VARIABLE &ret,bool retvoid)
{
	if(retvoid == true)
	{
		if( (this->mmap&0x7F0000) == 0)
			this->text_code += "rts\n";
		else
			this->text_code += "rtl\n";
	}else
	{
		if(ret.type == EAGLE_keywords::IDX)
		{
			this->text_code += "txa\n";
		}

		if(ret.type == EAGLE_keywords::IDY)
		{
			this->text_code += "tya\n";
		}

		if(ret.type != EAGLE_keywords::ACC)
		{
			if(ret.bimm == true)
				this->text_code += "lda #"+ std::to_string(ret.immediate&0xFFFF) +"\n";
			else
				this->text_code += "lda "+ std::to_string(ret.address) +"\n";
		}

		if( (this->mmap&0x7F0000) == 0)
			this->text_code += "rts\n";
		else
			this->text_code += "rtl\n";
	}

}

void Eagle::asm_alu_65816(const EAGLE_VARIABLE &dst,const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2)
{
	std::string mnemonic,flag;
	std::string src1value;
	std::string src2value;

	std::string str_code1,str_code2;

	int adr1size,adr2size;

	bool optimize = true;
	bool operation = false;

	if( (src1.bptr == true) || (src2.bptr == true) )
		optimize = false;

	if( (src1.token1 == '@') || (src2.token1 == '@') )
		optimize = false;

	adr1size = asm_address(src1,this->label1,"218",src1value,this->text_code);
	adr2size = asm_address(src2,this->label2,"220",src2value,this->text_code);

	if(operator1 == '+')
	{
		mnemonic = "adc ";
		flag = "clc\n";
	}

	if(operator1 == '-')
	{
		mnemonic = "sbc ";
		flag = "sec\n";
	}

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

	//move =
	if(operator1 == '=')
	{
		if(optimize == true)
		{
			if(operator2 == '?')
			{
				this->text_code  += "ldy " + src2value +"\n";
				this->text_code  += "sty " + src1value +"\n";
				return;
			}

			if(operator2 == '+')
			{
				this->text_code  += "rep #$20\n";
				this->text_code  += "lda " + src2value +"\n";
				this->text_code  += "sta " + src1value +"\n";
				this->text_code  += "sep #$20\n";
				return;
			}


			if( (dst.type == EAGLE_keywords::IDX) && (src2.type == EAGLE_keywords::ACC) )
			{
				this->text_code  += "tax\n";
				return;
			}

			if( (dst.type == EAGLE_keywords::IDY) && (src2.type == EAGLE_keywords::ACC) )
			{
				this->text_code  += "tay\n";
				return;
			}

			if( (dst.type == EAGLE_keywords::ACC) && (src2.type == EAGLE_keywords::IDX) )
			{
				this->text_code  += "txa\n";
				return;
			}

			if( (dst.type == EAGLE_keywords::ACC) && (src2.type == EAGLE_keywords::IDY) )
			{
				this->text_code  += "tya\n";
				return;
			}


			if( adr2size < 0x10000 )
			{
				if(dst.type == EAGLE_keywords::IDX)
				{
					this->text_code  += "ldx " + src2value +"\n";
					return;
				}

				if(dst.type == EAGLE_keywords::IDY)
				{
					this->text_code  += "ldy " + src2value +"\n";
					return;
				}
			}


			if(dst.type != EAGLE_keywords::ACC)
			if( (src2.bimm == true) && (src2.immediate == 0) && (dst.address < 0x10000))
			{
				if( (dst.type == EAGLE_keywords::UINT8) || (dst.type == EAGLE_keywords::INT8) )
				{
					this->text_code  += "stz " + src1value +"\n";
					return;
				}
			}

			if( adr1size < 0x10000 )
			{
				if(src2.type == EAGLE_keywords::IDX)
				{
					this->text_code  += "stx " + src1value +"\n";
					return;
				}

				if(src2.type == EAGLE_keywords::IDY)
				{
					this->text_code  += "sty " + src1value +"\n";
					return;
				}
			}
		}

		if(src2.type != EAGLE_keywords::ACC)
			this->text_code  += "lda " + src2value +"\n";

		operation = true;
	}

	//optimize += imm / -= imm
	if(optimize == true)
	if( (dst.type == src1.type) && (src2.bimm == true) )
	{

		if(src1.type == EAGLE_keywords::IDX)
		{
			if(src2.immediate < 5)
			{
				if(operator1 == '+')
				{
					for(int i = 0;i < src2.immediate;i++)
						this->text_code += "inx\n";
					return;
				}

				if(operator1 == '-')
				{
					for(int i = 0;i < src2.immediate;i++)
						this->text_code += "dex\n";
					return;
				}
			}
		}else
		if(src1.type == EAGLE_keywords::IDY)
		{
			if(src2.immediate < 5)
			{
				if(operator1 == '+')
				{
					for(int i = 0;i < src2.immediate;i++)
						this->text_code += "iny\n";
					return;
				}

				if(operator1 == '-')
				{
					for(int i = 0;i < src2.immediate;i++)
						this->text_code += "dey\n";
					return;
				}
			}

		}else
		if(src1.type == EAGLE_keywords::ACC)
		{
			if(src2.immediate < 3)
			{
				if(operator1 == '+')
				{
					for(int i = 0;i < src2.immediate;i++)
						this->text_code += "ina\n";
					return;
				}

				if(operator1 == '-')
				{
					for(int i = 0;i < src2.immediate;i++)
						this->text_code += "dea\n";
					return;
				}
			}
		}else
		{
			if(src1.address < 0x10000)
			if( (src2.immediate < 3) && ( (dst.address == src1.address) ) )
			{
				if(operator1 == '+')
				{
					for(int i = 0;i < src2.immediate;i++)
						this->text_code += "inc " + src1value +"\n";
					return;
				}

				if(operator1 == '-')
				{
					for(int i = 0;i < src2.immediate;i++)
						this->text_code += "dec " + src1value +"\n";
					return;
				}
			}
		}
	}

	//------------------
	if(operator1 != '=')
	{
		if(src2.type == EAGLE_keywords::IDX)
		{
			src2value = "222";
			this->text_code += "stx 222\n";
		}else if(src2.type == EAGLE_keywords::IDY)
		{
			src2value = "222";
			this->text_code += "sty 222\n";
		}else if(src2.type == EAGLE_keywords::ACC)
		{
			src2value = "222";
			this->text_code += "sta 222\n";
		}

		if(src1.type == EAGLE_keywords::IDX)
			this->text_code += "txa \n";
		else
		if(src1.type == EAGLE_keywords::IDY)
			this->text_code += "tya \n";
		else
		if(src1.type != EAGLE_keywords::ACC)
			this->text_code += "lda " + src1value +"\n";
	}

	//shift
	if( (operator1 == '>') || (operator1 == '<') )
	{

		if(src2.bimm == true)
		{
			if(operator1 == '>')
			{
				for(int i = 0;i < src2.immediate;i++)
					this->text_code  += "lsr\n";
			}else
			{
				for(int i = 0;i < src2.immediate;i++)
					this->text_code  += "asl\n";
			}
		}else
		{
			if(operator1 == '>')
				this->text_code += "jsl higueul_stdlib_lsr\n";
			else
				this->text_code += "jsl higueul_stdlib_asl\n";
		}

		operation = true;
	}

		//mul
	if(operator1 == '*')
	{
		if(src2.bimm == true)
		{
			switch(src2.immediate)
			{
				case 0:
					this->text_code += "lda #0\n";
				break;

				case 1:
					//this->text_code += "lda #0\n";
				break;


				case 2:
					this->text_code += "clc\n";
					this->text_code += "adc " + src1value +"\n";
				break;

				case 3:
					this->text_code += "asl\n";
					this->text_code += "clc\n";
					this->text_code += "adc " + src1value +"\n";
				break;


				case 4:
					this->text_code += "asl\n";
					this->text_code += "asl\n";
				break;

				case 5:
					this->text_code += "asl\n";
					this->text_code += "asl\n";
					this->text_code += "clc\n";
					this->text_code += "adc " + src1value +"\n";
				break;

				case 6:
					this->text_code += "asl\n";
					this->text_code += "asl\n";
					this->text_code += "clc\n";
					this->text_code += "adc " + src1value +"\n";
					this->text_code += "clc\n";
					this->text_code += "adc " + src1value +"\n";
				break;

				case 7:
					this->text_code += "asl\n";
					this->text_code += "asl\n";
					this->text_code += "asl\n";
					this->text_code += "sec\n";
					this->text_code += "sbc " + src1value +"\n";
				break;

				case 8:
					this->text_code += "asl\n";
					this->text_code += "asl\n";
					this->text_code += "asl\n";
				break;

				case 9:
					this->text_code += "asl\n";
					this->text_code += "asl\n";
					this->text_code += "asl\n";
					this->text_code += "clc\n";
					this->text_code += "adc " + src1value +"\n";
				break;

				case 10:
					this->text_code += "asl\n";
					this->text_code += "asl\n";
					this->text_code += "asl\n";
					this->text_code += "clc\n";
					this->text_code += "adc " + src1value +"\n";
					this->text_code += "clc\n";
					this->text_code += "adc " + src1value +"\n";
				break;


				default:

				break;

			}


		}else
		{
			this->text_code += "jsl higueul_stdlib_mul\n";
		}

		operation = true;
	}

	//div
	if(operator1 == '/')
	{
		if(src2.bimm == true)
		{

		}else
		{
			this->text_code += "jsl higueul_stdlib_div\n";
		}

		operation = true;
	}

	if(operation == false)
	this->text_code += flag + mnemonic + src2value + "\n";

	//----------------------DST--------------------------------------

	if(dst.type == EAGLE_keywords::IDX)
	{
		this->text_code += "tax\n";
	}else
	if(dst.type == EAGLE_keywords::IDY)
	{
		this->text_code += "tay\n";
	}else
	{
		if(dst.bptr == true)
		{
			bool py = false;
			bool px = false;

			uint64_t value = 0;

			//arg2
			if(dst.ptr2.bimm == true)
			{
				value = dst.ptr2.value&0xFFFF;
			}else
			{

				if(dst.ptr2.type == EAGLE_keywords::IDX)
				{
					px = true;
				}else
				if(dst.ptr2.type == EAGLE_keywords::IDY)
				{
					py = true;
				}else
				{
					this->text_code  += "ldx " + std::to_string(dst.ptr2.value&0xFFFF) +"\n";
					px = true;
				}
			}

			std::string cptr1,cptr2;
			if(dst.token1 == '@')
			{
				cptr1 = "[";
				cptr2 = "]";
			}
			if(dst.token2 == '@')
			{
				cptr1 = "(";
				cptr2 = ")";
			}

			//arg1
			if(dst.ptr1.bimm == true)
			{
				this->text_code += "sta " + cptr1 + std::to_string(value+dst.ptr1.value) + cptr2;
			}else
			{
				if(dst.ptr1.token2 == ':')
					this->text_code += "sta " + this->label0;
				else
					this->text_code += "sta " + cptr1 + std::to_string(value+dst.ptr1.value)+ cptr2;
			}

			if(dst.ptr2_exist == true)
			{
				if(px == true)
					this->text_code += ",X";

				if(py == true)
					this->text_code += ",Y";
			}

			this->text_code += "\n";

		}else
		if(dst.type != EAGLE_keywords::ACC)
			this->text_code += "sta "+ std::to_string(dst.address) +"\n";


	}

}

static int asm_address(const EAGLE_VARIABLE &src,std::string &labelp,const std::string &pregister,std::string &srcvalue,std::string &str_code)
{
	int adr = src.address;

	if(src.token1 == '$')
		adr &= 0xFFFF;

	if(src.token1 == '#')
		adr &= 0xFF;

	if(src.bimm == true)
	{
		srcvalue = "#" + std::to_string(src.immediate&0xFFFF);

		adr = 0;
	}else if(src.bptr == false)
	{
		if(src.blabel == true)
		{
			srcvalue = labelp;
			if(src.token1 == '$')
				srcvalue = "#" + labelp;

			if(src.token1 == '#')
				srcvalue = "#:" + labelp;
		}else
		{
			srcvalue = std::to_string(src.address);

			if(src.token1 == '$')
				srcvalue = "#" + std::to_string(src.address&0xFFFF);

			if(src.token1 == '#')
				srcvalue = "#:" + std::to_string(src.address>>16);
		}
	}
	else
	{
		bool py = false;
		bool px = false;
		uint64_t value = 0;
		adr = 0;

		//arg2
		if(src.ptr2.bimm == true)
		{
			value = src.ptr2.value&0xFFFF;
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
			srcvalue  = std::to_string(src.ptr1.value+value);

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
					if(value != 0)
					{
						str_code += "ldx #"+ std::to_string(value) +"\n";
						px = true;
					}
					srcvalue = labelp;
					if(src.ptr1.token1 == '$')
						srcvalue = "#" + labelp;

					if(src.ptr1.token1 == '#')
						srcvalue = "#:" + labelp;
				}else
				{
					srcvalue = std::to_string(src.ptr1.value+value);

					if(src.ptr1.token1 == '$')
						srcvalue = "#" + std::to_string( (src.ptr1.value+value)&0xFFFF);

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

	return adr;
}
