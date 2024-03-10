

#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include <cstring>
#include <string>

#include <stdint.h>

#include "Eagle.hpp"

void Eagle::asm_bru_6502(const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2,int type,int clabel)
{
	std::string src1value;
	std::string src2value;
	std::string label_adr = ".label_b"+std::to_string(clabel);

	if(type == 2)
	{
		label_adr = ".label_"+std::to_string(this->ilabel);
		this->ilabel++;
	}


	if(src1.bimm == true)
	{
		src1value = "#" + std::to_string(src1.immediate);
	}else
	{
		src1value = std::to_string(src1.address);
	}

	if(src2.bimm == true)
	{
		src2value = "#" + std::to_string(src2.immediate);
	}else
	{
		src2value = std::to_string(src2.address);
	}

	if(src1.blabel == true)
		src1value = this->label1;

	if(src2.blabel == true)
		src2value = this->label2;

	//--- loop dec
	if(type == 0)
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
	}

	//----
	if(src2.type == EAGLE_keywords::IDX)
	{
		src2value = "222";
		this->text_code += "stx " + src2value +"\n";
	}else
	if(src2.type == EAGLE_keywords::IDY)
	{
		src2value = "222";
		this->text_code += "sty " + src2value +"\n";
	}else
	{

		if(src2.type == EAGLE_keywords::ACC)
		{
			src2value = "222";
			this->text_code += "sta " + src2value +"\n";
		}else
		{

		}
	}


	//----------------
	bool optimize_zero = false;
	if(type == 0)
	{
		if(src2.bimm == true)
		{
			if( (src2.immediate == 0) && ( (operator1 == '=') || (operator1 == '!') ) )
				optimize_zero = true;
		}
	}


	//----------------
	if(optimize_zero == false)
	{
		if(src1.type == EAGLE_keywords::IDX)
		{
			this->text_code += "cpx " + src2value +"\n";
		}else
		if(src1.type == EAGLE_keywords::IDY)
		{
			this->text_code += "cpy " + src2value +"\n";
		}else
		{
			if(src1.type == EAGLE_keywords::ACC)
			{
				this->text_code += "cmp " + src2value +"\n";
			}else
			{
				this->text_code += "lda " + src2value +"\n";
				this->text_code += "cmp " + src1value +"\n";
			}
		}
	}


	if(operator1 == '=')
	{
		this->text_code += "bne " + label_adr +"\n";
	}

	if( (operator1 == '!') && (operator2 == '=') )
	{
		this->text_code += "beq " + label_adr +"\n";
	}

	if( (operator1 == '<') && (operator2 == '=') )
	{
		this->text_code += "bmi " + label_adr +"\n";
	}else
	if(operator1 == '<')
	{
		this->text_code += "bmi " + label_adr +"\n";
	}

	if( (operator1 == '>') && (operator2 == '=') )
	{
		this->text_code += "bmi " + label_adr +"\n";
	}else
	if(operator1 == '>')
	{
		this->text_code += "bmi " + label_adr +"\n";
	}

}


void Eagle::asm_call_jump_6502(const EAGLE_VARIABLE &src,const EAGLE_DFUNC dfunc,int ninst,int type)
{
	std::string src1value = this->label1,tmp;
	std::string mnemonic = "jump ";
	if(type >= 1)
		mnemonic = "call ";

	int sizearg = 0;

	for(int i = 0;i < ninst;i++)
	{
		EAGLE_VARIABLE var = this->arg[i];
		EAGLE_keywords type = dfunc.type[i];

		bool zero = false;

		if(var.type == EAGLE_keywords::IDX)
		{
			this->text_code += "stx " + src1value + "+" + std::to_string(sizearg) +"\n";
			zero = true;
		}else
		if(var.type == EAGLE_keywords::IDY)
		{
			this->text_code += "sty " + src1value + "+" + std::to_string(sizearg) +"\n";
			zero = true;
		}else
		if(var.type == EAGLE_keywords::ACC)
		{
			this->text_code += "sta " + src1value + "+" + std::to_string(sizearg) +"\n";
			zero = true;
		}else
		{
			if(var.bimm == false)
			{
				this->text_code += "lda " + std::to_string(var.address) +"\n";
				this->text_code += "sta " + src1value + "+" + std::to_string(sizearg) +"\n";

				if( (type == EAGLE_keywords::UINT16) || (type == EAGLE_keywords::INT16))
				{
					this->text_code += "lda " + std::to_string(var.address+1) +"\n";
					this->text_code += "sta " + src1value + "+" + std::to_string(sizearg+1) +"\n";
				}
			}
			else
			{
				this->text_code += "lda #" + std::to_string(var.immediate&0xFF) +"\n";
				this->text_code += "sta " + src1value + "+" + std::to_string(sizearg) +"\n";

				if( (type == EAGLE_keywords::UINT16) || (type == EAGLE_keywords::INT16))
				{
					this->text_code += "lda #" + std::to_string( (var.immediate&0xFF00) >> 8 ) +"\n";
					this->text_code += "sta " + src1value + "+" + std::to_string(sizearg+1) +"\n";
				}
			}
		}


		if( ( (type == EAGLE_keywords::UINT16) || (type == EAGLE_keywords::INT16)) && (zero == true) )
		{
			this->text_code += "lda #0\n";
			this->text_code += "sta " + src1value + "+" + std::to_string(sizearg+1) +"\n";
		}

		if( (type == EAGLE_keywords::UINT8) || (type == EAGLE_keywords::INT8))
			sizearg += 1;
		else
		if( (type == EAGLE_keywords::UINT16) || (type == EAGLE_keywords::INT16))
			sizearg += 2;
		else
		if( (type == EAGLE_keywords::UINT32) || (type == EAGLE_keywords::INT32))
			sizearg += 4;
		else
		if( (type == EAGLE_keywords::UINT64) || (type == EAGLE_keywords::INT64))
			sizearg += 8;
		else
			sizearg += 1;
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
				this->text_code += "lda #0\nsta 223\n";
				this->text_code += "stx 222\n";

				tmp = "222";
			}

			if(src.type == EAGLE_keywords::IDY)
			{
				this->text_code += "lda #0\nsta 223\n";
				this->text_code += "sty 222\n";
				tmp = "222";
			}

			if(src.type == EAGLE_keywords::ACC)
			{

				this->text_code += "sta 222\n";
				this->text_code += "lda #0\nsta 223\n";
				tmp = "222";
			}


			src1value =  "(" + tmp + ")";
		}
	}

	this->text_code += mnemonic + src1value +"\n";
}

void Eagle::asm_return_6502(const EAGLE_VARIABLE &ret,bool retvoid)
{
	if(this->target == TARGET_65816)
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
					this->text_code += "lda #"+ std::to_string(ret.immediate) +"\n";
				else
					this->text_code += "lda "+ std::to_string(ret.address) +"\n";
			}
			this->text_code += "rts\n";
		}
	}
}

void Eagle::asm_alu_6502(const EAGLE_VARIABLE &dst,const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2)
{
	std::string tmp,mnemonic,flag;
	std::string src1value;
	std::string src2value;
	std::string endv1,endv2;
	int srcvalue = 0;
	bool optimize = true;

	if(src1.bimm == true)
	{
		src1value = "#" + std::to_string(src1.immediate);
		srcvalue  = src1.immediate;
	}else
	{
		src1value = std::to_string(src1.address);
	}

	if(src2.bimm == true)
	{
		src2value = "#" + std::to_string(src2.immediate);
		srcvalue  = src2.immediate;
	}else
	{
		src2value = std::to_string(src2.address);
	}

	if(src1.bptr == true)
	{
		int py = false;
		if(src1.ptr2.bimm == true)
		{
			if(src1.immediate > 0)
			{
				this->text_code  += "lda #" + std::to_string(src1.ptr2.value) +"\n";
				for(int i = 0;i < src1.immediate;i++)
					this->text_code  += "asl\n";
				this->text_code  += "tax\n";
			}else
				this->text_code  += "ldx #" + std::to_string(src1.ptr2.value) +"\n";
		}else
		{
			if(src1.ptr2.type == EAGLE_keywords::IDX)
			{
				if(src1.immediate > 0)
				{
					this->text_code  += "txa\n";
					for(int i = 0;i < src1.immediate;i++)
						this->text_code  += "asl\n";
					this->text_code  += "tax\n";
				}
				py = false;
			}else
			if(src1.ptr2.type == EAGLE_keywords::IDY)
			{
				if(src1.immediate > 0)
				{
					this->text_code  += "tya\n";
					for(int i = 0;i < src1.immediate;i++)
						this->text_code  += "asl\n";
					this->text_code  += "tay\n";
				}

				py = true;
			}else
			if(src1.ptr2.type == EAGLE_keywords::ACC)
			{

				for(int i = 0;i < src1.immediate;i++)
					this->text_code  += "asl\n";

				this->text_code  += "tax\n";
			}else
			{
				this->text_code  += "lda " + std::to_string(src1.ptr2.value) +"\n";
				for(int i = 0;i < src1.immediate;i++)
					this->text_code  += "asl\n";
				this->text_code  += "tax\n";
			}



		}

		if(src1.ptr1.bimm == true)
		{
			src1value  = std::to_string(src1.ptr1.value);
			if(py == false)
				endv1 = ",X";
			else
				endv1 = ",Y";

		}else
		{
			if(src1.ptr1.type == EAGLE_keywords::IDX)
			{
				this->text_code  += "stx 218\n";
			}else
			if(src1.ptr1.type == EAGLE_keywords::IDY)
			{
				this->text_code  += "sty 218\n";
			}else
			if(src1.ptr1.type == EAGLE_keywords::ACC)
			{
				this->text_code  += "sta 218\n";
			}else
			{
				this->text_code  += "lda " + std::to_string(src1.ptr1.value) +"\n";
				this->text_code  += "sta 218\n";
			}

			src1value = "(218";

			if(py == false)
				endv1 = "),X";
			else
				endv1 = "),Y";

			if( (src1.ptr1.type == EAGLE_keywords::UINT16) || (src1.ptr1.type == EAGLE_keywords::INT16) )
			{
				this->text_code  += "lda " + std::to_string(src1.ptr1.value+1) +"\n";
				this->text_code  += "sta 219\n";
			}else
			{
				this->text_code  += "lda #0\n";
				this->text_code  += "sta 219\n";
			}
		}

		optimize = false;
	}

	if(src2.bptr == true)
	{
		int py = false;
		if(src2.ptr2.bimm == true)
		{
			if(src1.immediate > 0)
			{
				this->text_code  += "lda #" + std::to_string(src2.ptr2.value) +"\n";
				for(int i = 0;i < src2.immediate;i++)
					this->text_code  += "asl\n";
				this->text_code  += "tax\n";
			}else
				this->text_code  += "ldx #" + std::to_string(src2.ptr2.value) +"\n";
		}else
		{
			if(src2.ptr2.type == EAGLE_keywords::IDX)
			{
				if(src2.immediate > 0)
				{
					this->text_code  += "txa\n";
					for(int i = 0;i < src2.immediate;i++)
						this->text_code  += "asl\n";
					this->text_code  += "tax\n";
				}
				py = false;
			}else
			if(src2.ptr2.type == EAGLE_keywords::IDY)
			{
				if(src2.immediate > 0)
				{
					this->text_code  += "tya\n";
					for(int i = 0;i < src2.immediate;i++)
						this->text_code  += "asl\n";
					this->text_code  += "tay\n";
				}
				py = true;
			}else
			if(src2.ptr2.type == EAGLE_keywords::ACC)
			{
				for(int i = 0;i < src2.immediate;i++)
					this->text_code  += "asl\n";
				this->text_code  += "tax\n";
			}else
			{
				this->text_code  += "lda " + std::to_string(src2.ptr2.value) +"\n";
				for(int i = 0;i < src2.immediate;i++)
					this->text_code  += "asl\n";
				this->text_code  += "tax\n";
			}
		}

		if(src2.ptr1.bimm == true)
		{
			src2value  = std::to_string(src2.ptr1.value);
			if(py == false)
				endv2 = ",X";
			else
				endv2 = ",Y";

		}else
		{
			if(src2.ptr1.type == EAGLE_keywords::IDX)
			{
				this->text_code  += "stx 220\n";
			}else
			if(src2.ptr1.type == EAGLE_keywords::IDY)
			{
				this->text_code  += "sty 220\n";
			}else
			if(src2.ptr1.type == EAGLE_keywords::ACC)
			{
				this->text_code  += "sta 220\n";
			}else
			{
				this->text_code  += "lda " + std::to_string(src2.ptr1.value) +"\n";
				this->text_code  += "sta 220\n";
			}

			src2value = "(220";

			if(py == false)
				endv2 = "),X";
			else
				endv2 = "),Y";

			if( (src2.ptr1.type == EAGLE_keywords::UINT16) || (src2.ptr1.type == EAGLE_keywords::INT16) )
			{
				this->text_code  += "lda " + std::to_string(src2.ptr1.value+1) +"\n";
				this->text_code  += "sta 221\n";
			}else
			{
				this->text_code  += "lda #0\n";
				this->text_code  += "sta 221\n";
			}
		}

		optimize = false;
	}

	if(src1.blabel == true)
		src1value = this->label1;

	if(src2.blabel == true)
		src2value = this->label2;

	if(operator1 == '+')
	{
		mnemonic = "adc ";
		flag = "clc\n";
	}

	if(operator1 == '-')
	{
		mnemonic = "sub ";
		flag = "sec\n";
	}

	if(operator1 == '&')
		mnemonic = "and ";

	if(operator1 == '|')
		mnemonic = "or ";

	if(operator1 == '^')
		mnemonic = "eor ";

	if(operator1 == '<')
		mnemonic = "asl ";

	if(operator1 == '>')
		mnemonic = "lsr ";

	int type1 = 0;
	int type2 = 0;
	int idy = 0;
	int idx = 0;

	//-------------------
	if(src1.type == EAGLE_keywords::IDX)
	{
		type1 = 2;
		idx = 1;
	}

	if(src1.type == EAGLE_keywords::IDY)
	{
		type1 = 2;
		idy = 1;
	}

	if(src1.type == EAGLE_keywords::ACC)
		type1 = 1;

	//-------------------
	if(src2.type == EAGLE_keywords::IDX)
	{
		type2 = 2;
		idx = 1;
	}

	if(src2.type == EAGLE_keywords::IDY)
	{
		type2 = 2;
		idy = 1;
	}

	if(src2.type == EAGLE_keywords::ACC)
		type2 = 1;


	if(operator1 == '=')
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

		if(src2.type != EAGLE_keywords::ACC)
			this->text_code  += "lda " + src2value +"\n";


		type1 = 4;
		type2 = 4;

	}

	//optimize
	if(optimize == true)
	if( (dst.type == src1.type) && (src2.bimm == true) )
	{
		if(src2.bimm == true)

		if(idx == 1)
		{
			if(srcvalue < 5)
			{
				if(operator1 == '+')
				{
					for(int i = 0;i < srcvalue;i++)
						this->text_code += "inx\n";
					return;
				}

				if(operator1 == '-')
				{
					for(int i = 0;i < srcvalue;i++)
						this->text_code += "dex\n";
					return;
				}
			}
		}
		if(idy == 1)
		{
			if(srcvalue < 5)
			{
				if(operator1 == '+')
				{
					for(int i = 0;i < srcvalue;i++)
						this->text_code += "iny\n";
					return;
				}

				if(operator1 == '-')
				{
					for(int i = 0;i < srcvalue;i++)
						this->text_code += "dey\n";
					return;
				}
			}

		}
		if(src1.type == EAGLE_keywords::ACC)
		{
			if(srcvalue < 3)
			{
				if(operator1 == '+')
				{
					for(int i = 0;i < srcvalue;i++)
						this->text_code += "ina\n";
					return;
				}

				if(operator1 == '-')
				{
					for(int i = 0;i < srcvalue;i++)
						this->text_code += "dea\n";
					return;
				}
			}
		}

		if(type1 == 0)
		{
			if(srcvalue < 3)
			{
				for(int i = 0;i < srcvalue;i++)
					this->text_code += "inc " + src1value +"\n";
				return;
			}
		}

	}

	//shift
	if( (operator1 == '>') || (operator1 == '<') )
	{
		if(src2.bimm == false)
		{
			if(type2 == 0)
			{
				this->text_code += "lda " + src2value +"\n";
				this->text_code += "sta 222\n";
			}

			if(src2.type == EAGLE_keywords::IDY)
				this->text_code += "sty 222\n";

			if(src2.type == EAGLE_keywords::IDX)
				this->text_code += "stx 222\n";

			if(src2.type == EAGLE_keywords::ACC)
				this->text_code += "sta 222\n";

		}

		if(type1 == 0)
		{
			this->text_code += "lda "  + src1value +"\n";
		}

		if(idy == 1)
		{
			this->text_code += "tya\n";
		}

		if(idx == 1)
		{
			this->text_code += "txa\n";
		}

		type1 = 4;
		type2 = 4;


		if(src2.bimm == true)
		{
			if(operator1 == '>')
			{
				for(int i = 0;i < srcvalue;i++)
					this->text_code  += "lsr\n";
			}else
			{
				for(int i = 0;i < srcvalue;i++)
					this->text_code  += "asl\n";
			}
		}else
		{
			if(operator1 == '>')
				this->text_code += "jsr eagle_stdlib_lsr\n";
			else
				this->text_code += "jsr eagle_stdlib_asl\n";
		}
	}




	//var , var
	if( (type1 == 0) && (type2 == 0) )
	{
		tmp = "lda "+ src1value + endv1 + "\n";
		tmp += flag;
		tmp += mnemonic + src2value + endv2 + "\n";

		this->text_code += tmp;
	}


	//acc , var
	if( ( (type1 == 1) && (type2 == 0) ) || ( (type1 == 0) && (type2 == 1) ) )
	{
		this->text_code += flag;
		if(type1 == 0)
			this->text_code += mnemonic + src1value + endv1 + "\n";
		else
			this->text_code += mnemonic + src2value + endv2 + "\n";

		this->text_code += tmp;
	}else
	//acc acc
	if( (type1 == 1) && (type2 == 1) )
	{
		this->text_code += "sta 222\n";
		this->text_code += flag;
		this->text_code +=  mnemonic + "222\n";

	}else
	//acc idx/idy
	if( (type1 == 1) || (type2 == 1) )
	{
		this->text_code += "sta 222\n";
		if(idy == 0)
			this->text_code += "txa\n";
		else
			this->text_code += "tya\n";
		this->text_code += flag;
		this->text_code += mnemonic + "222\n";
	}


	//idx , var
	if( ( (type1 == 2) && (type2 == 0) ) || ( (type1 == 0) && (type2 == 2) ) )
	{
		if(idy == 0)
			tmp = "txa\n";
		else
			tmp = "tya\n";

		tmp += flag;

		if(type1 == 0)
			tmp += mnemonic + src1value + endv1 + "\n";
		else
			tmp += mnemonic + src2value + endv2 + "\n";


		this->text_code += tmp;
	}
	else
	//id id
	if( (type1 == 2) && (type2 == 2) )
	{

		if(idy == 0)
			this->text_code += "stx 222\n";
		else
			this->text_code += "sty 222\n";

		if(idx == 0)
			this->text_code += "tya\n";
		else
			this->text_code += "txa\n";

		this->text_code += flag;
		this->text_code += mnemonic + "222\n";
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
			int py = false;
			if(dst.ptr2.bimm == true)
			{
					this->text_code  += "ldx #" + std::to_string(src1.ptr2.value) +"\n";
			}else
			{
				if(src1.ptr2.type == EAGLE_keywords::IDX)
				{
					py = false;
				}else
				if(src1.ptr2.type == EAGLE_keywords::IDY)
				{
					py = true;
				}else
				if(src1.ptr2.type == EAGLE_keywords::ACC)
				{
					this->text_code  += "tax\n";
				}else
				{
					this->text_code  += "lda " + std::to_string(dst.ptr2.value) +"\n";
					this->text_code  += "tax\n";
				}
			}

			if(py == false)
				endv1 = ",X";
			else
				endv1 = ",Y";


			if(dst.ptr1.bimm == true)
			{
				this->text_code += "sta "+ std::to_string(dst.ptr1.value) + endv1 +"\n";
			}else
			{
				if(dst.ptr1.type == EAGLE_keywords::IDX)
				{
					this->text_code += "stx 222\n";
				}else
				if(dst.ptr1.type == EAGLE_keywords::IDY)
				{
					this->text_code += "sty 222\n";
				}else
				{
					this->text_code  += "lda " + std::to_string(dst.ptr1.value) +"\n";
					this->text_code  += "sta 222\n";

					if( (dst.ptr1.type == EAGLE_keywords::UINT16) || (dst.ptr1.type == EAGLE_keywords::INT16) )
					{
						this->text_code  += "lda " + std::to_string(dst.ptr1.value+1) +"\n";
						this->text_code  += "sta 223\n";
					}else
					{
						this->text_code  += "lda #0\n";
						this->text_code  += "sta 223\n";
					}
				}

				this->text_code += "sta (222)" + endv1 +"\n";
			}
		}else
		if(dst.type != EAGLE_keywords::ACC)
			this->text_code += "sta "+ std::to_string(dst.address) +"\n";


	}

}

void Eagle::asm_mul_6502(const EAGLE_VARIABLE &dst,const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2)
{
	this->text_code += "mul\n";
}


void Eagle::asm_div_6502(const EAGLE_VARIABLE &dst,const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2)
{
	this->text_code += "div\n";
}
