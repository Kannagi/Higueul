

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

static std::string srcvalue16a;
static std::string srcvalue16b;
static int tsrc;
//static void asm_address(const EAGLE_VARIABLE &src,std::string &labelp,const std::string &pregister,std::string &srcvalue,std::string &str_code);

void Eagle::asm_bru_6502(const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2,int type,int clabel)
{
	std::string src1value;
	std::string src2value;
	std::string label_adr = ".label_b"+std::to_string(clabel);
	std::string str_md16;

	if(type == TYPE_IF) //if
	{
		label_adr = ".label_"+std::to_string(this->ilabel);
		this->ilabel++;
	}

	std::string str_code1,str_code2;
	//----------------
	tsrc = 0;
	asm_address(src1,this->label1,"218",src1value,this->text_code);
	tsrc = 1;
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
				this->text_code += "sec \nsbc #1\n";
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

	bool bit02 = false;
	if(operator1 == '&')
	{
		if(this->target == TARGET_6502)
		{
			if(src2value[0] == '#')
			{
				bit02 = true;
			}
		}
	}

	bool mode16sc1 = false;
	if(
		(src1.type == EAGLE_keywords::INT16) || (src1.type == EAGLE_keywords::UINT16) ||
		(src1.ptr_type == EAGLE_keywords::INT16) || (src1.ptr_type == EAGLE_keywords::UINT16)
	)
		mode16sc1 = true;


	bool mode16sc2 = false;
	if(
		(src2.type == EAGLE_keywords::INT16) || (src2.type == EAGLE_keywords::UINT16) ||
		(src2.ptr_type == EAGLE_keywords::INT16) || (src2.ptr_type == EAGLE_keywords::UINT16)
	)
		mode16sc2 = true;


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
		{
			if(bit02 == false)
			{
				this->text_code += "lda " + src1value +"\n";

				if(mode16sc1 == true)
					str_md16 = "lda " + srcvalue16a  +"\n";
			}
		}
	}

	bool mode16 = false;
	//----------------
	if(operator1 == '&')
	{
		if(bit02 == true)
		{
			this->text_code += "lda " + src2value +"\n";
			this->text_code += "bit " + src1value +"\n";

			str_md16 = "lda " + srcvalue16b +"\n";
			str_md16 += "bit " + srcvalue16a +"\n";
		}
		else
		{
			this->text_code += "bit " + src2value +"\n";
			str_md16 += "bit " + srcvalue16b +"\n";
		}

		if(mode16sc1 == true)
		{
			if(mode16sc2 == true)
			{
				mode16 = true;
			}
			else
			{
				if(src2.bimm == true)
				{
					mode16 = true;
				}
			}

			if(operator2 == '!')
			{
				this->text_code += "bne " + label_adr +"\n";
				str_md16 += "bne " + label_adr +"\n";
			}
			else
			{
				this->text_code += "beq " + label_adr +"\n";
				str_md16 += "beq " + label_adr +"\n";
			}

			if(mode16 == true)
				this->text_code += str_md16;

			return;
		}

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


	if(optimize_zero == false)
	{
		this->text_code += "cmp " + src2value  +"\n";

		if(mode16sc2 == true)
		{
			str_md16 += "cmp " + srcvalue16b  +"\n";
			mode16 = true;
		}
		else
		{
			if(mode16sc1 == true)
			{
				if(src2.bimm == true)
				{
					str_md16 += "cmp " + srcvalue16b  +"\n";
					mode16 = true;
				}

			}
		}

	}

	if(operator1 == '=')
	{
		if(type == TYPE_WHILE)
			this->text_code += "beq " + label_adr +"\n";
		else
			this->text_code += "bne " + label_adr +"\n";

		if(mode16sc1 == true)
		{
			if(mode16 == true)
			{
				str_md16 += "bne " + label_adr +"\n";
				this->text_code += str_md16;
			}
		}

		return;
	}

	if( (operator1 == '!') && (operator2 == '=') )
	{
		if(type == TYPE_WHILE)
			this->text_code += "bne " + label_adr +"\n";
		else
			this->text_code += "beq " + label_adr +"\n";

		if(mode16sc1 == true)
		{
			if(mode16 == true)
			{
				str_md16 += "bne " + label_adr +"\n";
				this->text_code += str_md16;
			}
		}

		return;
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



void Eagle::asm_call_jump_6502(const EAGLE_VARIABLE &src,int ninst,int type)
{
	std::string src1value = this->labelcall,tmp;
	std::string mnemonic = "jmp ",mne1,mne2,saddress,saddress2;
	if(type >= 1)
		mnemonic = "jsr ";

	std::string srcvalue;



	for(int i = 0;i < ninst;i++)
	{
		bool mode16 = false;
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
			mne1 = "lda ";
			mne2 = "sta @";

			if(var.bptr == true)
			{
				tsrc = 0;
				asm_address(var,this->labelarg[i],"218",saddress,this->text_code);
				saddress2 = srcvalue16a;
			}else
			if(var.blabel == true)
			{
				if(var.token1 == '$')
					saddress = "#" + this->labelarg[i];
				else
					saddress = this->labelarg[i];

				saddress2 = saddress + "+";
			}
			else if(var.token1 == '$')
			{
				saddress = "#" + std::to_string(var.address&0xFF);
				saddress2 = "#" + std::to_string((var.address>>8)&0xFF);

				if(var.address>>8)
					mode16 = true;
			}
			else if(var.bimm == false)
			{
				saddress = std::to_string(var.address);
				saddress2 = std::to_string(var.address+1);
			}
			else
			{
				saddress = "#" + std::to_string(var.immediate&0xFF);
				saddress2 = "#" + std::to_string((var.immediate>>8)&0xFF);

				if(var.immediate>>8)
					mode16 = true;
			}

			if(var.token1 == '@')
			{
				saddress = "(" + saddress + ")";
				saddress2 = "(" + saddress2 + ")";
			}

			this->text_code += mne1 + saddress + "\n";
			this->text_code += mne2 + src1value + "..arg" + std::to_string(i) + "\n";

			if( (var.type == EAGLE_keywords::UINT16) || (var.type == EAGLE_keywords::INT16) || mode16 == true )
			{
				this->text_code += mne1 + saddress2 + "\n";
				this->text_code += mne2 + src1value + "..arg" + std::to_string(i) + "+\n";
			}
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
		}
	}

	this->text_code += mnemonic + src1value +"\n";
}

void Eagle::asm_return_6502(const EAGLE_VARIABLE &ret,bool retvoid)
{
	if(retvoid == true)
	{
		this->text_code += "rts\n";

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
				this->text_code += "lda #"+ std::to_string(ret.immediate&0xFF) +"\n";
			else
				this->text_code += "lda "+ std::to_string(ret.address) +"\n";
		}

		this->text_code += "rts\n";
	}

}

void Eagle::asm_alu_6502(const EAGLE_VARIABLE &dst,const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2)
{
	std::string mnemonic,flag;
	std::string src1value;
	std::string src2value;

	std::string str_code1,str_code2,strmd16;


	bool optimize = true;
	bool operation = false;

	if( (src1.bptr == true) || (src2.bptr == true) )
		optimize = false;

	if( (src1.token1 == '@') || (src2.token1 == '@') )
		optimize = false;

	tsrc = 0;
	asm_address(src1,this->label1,"218",src1value,this->text_code);
	tsrc = 1;
	asm_address(src2,this->label2,"220",src2value,this->text_code);

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

			if(dst.type == EAGLE_keywords::IDX)
			{
				if(this->target == TARGET_HuC6520)
				{
					if( (src2.bimm == true) && (src2.immediate == 0) )
					{
						this->text_code  += "clx\n";
						return;
					}
				}
				this->text_code  += "ldx " + src2value +"\n";
				return;
			}

			if(dst.type == EAGLE_keywords::IDY)
			{
				if(this->target == TARGET_HuC6520)
				{
					if( (src2.bimm == true) && (src2.immediate == 0) )
					{
						this->text_code  += "cly\n";
						return;
					}
				}
				this->text_code  += "ldy " + src2value +"\n";
				return;
			}

			if(this->target != TARGET_6502)
			if( (src2.bimm == true) && (src2.immediate == 0) )
			{
				if( (dst.type == EAGLE_keywords::UINT8) || (dst.type == EAGLE_keywords::INT8) )
				{
					this->text_code  += "stz " + src1value +"\n";
					return;
				}
			}

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

			if(dst.type == EAGLE_keywords::ACC)
			{
				if(this->target == TARGET_HuC6520)
				{
					if( (src2.bimm == true) && (src2.immediate == 0) )
					{
						this->text_code  += "cla\n";
						return;
					}
				}
			}

		}

		if(src2.type != EAGLE_keywords::ACC)
		{
			this->text_code  += "lda " + src2value +"\n";


			if( (src2.type == EAGLE_keywords::UINT16) || (src2.type == EAGLE_keywords::INT16))
			{
				strmd16 = "lda " + srcvalue16b +"\n";
			}

			if( (src2.ptr_type == EAGLE_keywords::UINT16) || (src2.ptr_type == EAGLE_keywords::INT16))
			{
				strmd16 = "lda " + srcvalue16b +"\n";
			}

			if(src2.bimm == true)
			{
				strmd16 = "lda " + srcvalue16b +"\n";
			}
		}

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
		{
			if(src1.type == EAGLE_keywords::ACC)
			{
				if(this->target != TARGET_6502)
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
				}

			}else
			if( (src2.immediate < 3) && ( (dst.address == src1.address) ) &&
			( (src1.type == EAGLE_keywords::UINT8) || (src1.type == EAGLE_keywords::INT8) )  )
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

	bool mode16 = false;

	if( (src1.ptr_type == EAGLE_keywords::UINT16) || (src1.ptr_type == EAGLE_keywords::INT16))
	{
		if( (src2.ptr_type == EAGLE_keywords::UINT16) || (src2.ptr_type == EAGLE_keywords::INT16))
		{
				mode16 = true;
		}else
		{
			if( (src2.type == EAGLE_keywords::UINT16) || (src2.type == EAGLE_keywords::INT16))
			{
					mode16 = true;
			}else
			if(src2.bimm == true)
				mode16 = true;
		}
	}


	if( (src1.type == EAGLE_keywords::UINT16) || (src1.type == EAGLE_keywords::INT16))
	{
		if( (src2.type == EAGLE_keywords::UINT16) || (src2.type == EAGLE_keywords::INT16))
		{
				mode16 = true;
		}else
		{
			if( (src2.ptr_type == EAGLE_keywords::UINT16) || (src2.ptr_type == EAGLE_keywords::INT16))
			{
					mode16 = true;
			}else
			if(src2.bimm == true)
				mode16 = true;
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
		{
			if( (src1.type == EAGLE_keywords::UINT16) || (src1.type == EAGLE_keywords::INT16))
			{
				strmd16 = "lda " + srcvalue16a +"\n";
			}

			if( (src1.ptr_type == EAGLE_keywords::UINT16) || (src1.ptr_type == EAGLE_keywords::INT16))
			{
				strmd16 = "lda " + srcvalue16a +"\n";
			}

			this->text_code += "lda " + src1value +"\n";
		}
	}


	int mode16i = 0;

	//shift
	if( (operator1 == '>') || (operator1 == '<') )
	{

		if(src2.bimm == true)
		{
			if(mode16 == false)
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
				if(src2.immediate == 8)
				{
					if(operator1 == '>')
					{
						this->text_code += "lda " + srcvalue16a +"\n";
						mode16i = 1;
					}else
					{
						mode16i = 2;
					}

				}

			}

		}else
		{
			if(operator1 == '>')
				this->text_code += "jsr higueul_stdlib_lsr\n";
			else
				this->text_code += "jsr higueul_stdlib_asl\n";
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
			this->text_code += "jsr higueul_stdlib_mul\n";
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
			this->text_code += "jsr higueul_stdlib_div\n";
		}

		operation = true;
	}

	if(operation == false)
	{
		this->text_code += flag + mnemonic + src2value + "\n";

		if( (src1.type == EAGLE_keywords::UINT16) || (src1.type == EAGLE_keywords::INT16))
		{
			strmd16 += mnemonic + srcvalue16b + "\n";
		}

		if( (src1.ptr_type == EAGLE_keywords::UINT16) || (src1.ptr_type == EAGLE_keywords::INT16))
		{
			strmd16 += mnemonic + srcvalue16b + "\n";
		}
	}


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
				strmd16 += "sta " + cptr1 + std::to_string(value+dst.ptr1.value+1) + cptr2;
			}else
			{
				if(dst.ptr1.token2 == ':')
				{
					this->text_code += "sta " + this->label0;
					strmd16 += "sta " + this->label0;
				}
				else
				{
					this->text_code += "sta " + cptr1 + std::to_string(value+dst.ptr1.value)+ cptr2;
					strmd16 += "sta " + cptr1 + std::to_string(value+dst.ptr1.value+1) + cptr2;
				}

			}

			if(dst.ptr2_exist == true)
			{
				if(px == true)
				{
					this->text_code += ",X";
					strmd16 += ",X";
				}

				if(py == true)
				{
					this->text_code += ",Y";
					strmd16 += ",Y";
				}
			}

			this->text_code += "\n";

			if(mode16 == true)
			{
				if( (dst.ptr_type == EAGLE_keywords::UINT16) || (dst.ptr_type == EAGLE_keywords::INT16))
					this->text_code += strmd16 + "\n";
			}

		}else
		if(dst.type != EAGLE_keywords::ACC)
		{
			if(mode16i == 0)
			{
				this->text_code += "sta "+ std::to_string(dst.address) +"\n";

				if( (dst.type == EAGLE_keywords::UINT16) || (dst.type == EAGLE_keywords::INT16))
				{
					strmd16 += "sta "+ std::to_string(dst.address+1) +"\n";

					if(mode16 == true)
						this->text_code += strmd16;
				}
			}

			if(mode16i == 1)
				this->text_code += "sta "+ std::to_string(dst.address) +"\n";

			if(mode16i == 2)
				this->text_code += "sta "+ std::to_string(dst.address+1) +"\n";
		}


	}

}

static int asm_address(const EAGLE_VARIABLE &src,std::string &labelp,const std::string &pregister,std::string &srcvalue,std::string &str_code)
{
	int adr = src.address;
	std::string srcvalue16;

	if(src.token1 == '$')
		adr &= 0xFFFF;



	if(src.bimm == true)
	{
		srcvalue = "#" + std::to_string(src.immediate&0xFF);

		srcvalue16 = "#" + std::to_string( (src.immediate>>8)&0xFF );

		adr = 0;
	}else if(src.bptr == false)
	{
		if(src.blabel == true)
		{
			srcvalue = labelp;
			if(src.token1 == '$')
				srcvalue = "#" + labelp;

			srcvalue16 = srcvalue + "+";

		}else
		{
			srcvalue = std::to_string(src.address);
			srcvalue16 = std::to_string(src.address+1);

			if(src.token1 == '$')
			{
				srcvalue = "#" + std::to_string(src.address&0xFFFF);
				srcvalue16 = "#" + std::to_string((src.address+1)&0xFFFF);
			}
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
			srcvalue16 = std::to_string(src.ptr1.value+value+1);

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

					srcvalue16 = srcvalue + "+";

				}else
				{
					srcvalue = std::to_string(src.ptr1.value+value);
					srcvalue16 = std::to_string(src.ptr1.value+value+1);

					if(src.ptr1.token1 == '$')
					{
						srcvalue = "#" + std::to_string( (src.ptr1.value+value)&0xFFFF);
						srcvalue16 = "#" + std::to_string( (src.ptr1.value+value+1)&0xFFFF);
					}

				}
			}
		}

		if(src.ptr2_exist == true)
		{
			if(px == true)
			{
				srcvalue += ",X";
				srcvalue16 += ",X";
			}

			if(py == true)
			{
				srcvalue += ",Y";
				srcvalue16 += ",Y";
			}
		}
	}

	if(src.token1 == '@')
		srcvalue = "(" + srcvalue + ")";

	if(tsrc == 0)
		srcvalue16a = srcvalue16;
	else
		srcvalue16b = srcvalue16;



	return adr;
}


