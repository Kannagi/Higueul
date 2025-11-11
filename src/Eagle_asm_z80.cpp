#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include <cstring>
#include <string>

#include <stdint.h>

#include "Eagle.hpp"

std::string  Eagle::asm_z80_arg(const EAGLE_VARIABLE &src,int &type,std::string &labelp)
{
	std::string srcvalue;

	type = 0;

	if(src.bimm == true)
	{
		srcvalue = std::to_string(src.immediate&0xFFFF);
	}
	else
	{
		if( (src.type >= EAGLE_keywords::ACC) && (src.type <= EAGLE_keywords::IDY) )
		{
			type = 1;
			if(src.type == EAGLE_keywords::ACC)
			{
				srcvalue = "a";
			}

			if(src.type == EAGLE_keywords::IDX)
			{
				srcvalue = "IX";
				type = 2;
			}

			if(src.type == EAGLE_keywords::IDY)
			{
				srcvalue = "IY";
				type = 2;
			}

			if(src.type == EAGLE_keywords::IDHL)
			{
				srcvalue = "hl";
				type = 2;
			}

			if(src.type == EAGLE_keywords::IDH)
			{
				srcvalue = "h";
			}

			if(src.type == EAGLE_keywords::IDL)
			{
				srcvalue = "l";
			}

			if(src.type == EAGLE_keywords::REGDE)
			{
				type = 2;
				srcvalue = "de";
			}

			if(src.type == EAGLE_keywords::REGBC)
			{
				type = 2;
				srcvalue = "bc";
			}

			if( (src.type >= EAGLE_keywords::REG1) && (src.type <= EAGLE_keywords::REG4) )
			{
				srcvalue = static_cast<char>('b' + static_cast<int>(src.type) - static_cast<int>(EAGLE_keywords::REG1));
			}
		}
		else
		{
		    if(src.bptr == false)
            {
                if(src.blabel == true)
                {
                     type = 0;
                     srcvalue = labelp + ":";
                }
                else
                {
                    type = -1;
                    srcvalue = "(" + std::to_string(src.address&0xFFFF) +")";
                }
            }else
            {
                type = 1;
                if(src.ptr2.type == EAGLE_keywords::IDX)
                {
                     srcvalue = "(IX," + std::to_string(src.ptr1.value) +")";
                }else
                if(src.ptr2.type == EAGLE_keywords::IDY)
                {
                    srcvalue = "(IY," + std::to_string(src.ptr1.value) +")";
                }
                else
                if(src.ptr1.type == EAGLE_keywords::IDHL)
                {
                    srcvalue = "(hl)";
                }
            }
		}

	}

	return srcvalue;
}


void Eagle::asm_bru_z80(const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2,int type,int clabel)
{
	std::string label_adr = ".label_b"+std::to_string(clabel);
	std::string flag = "NZ";
	std::string src1value;
	std::string src2value;

	if(type == TYPE_IF) //if
	{
		label_adr = ".label_"+std::to_string(this->ilabel);
		this->ilabel++;
	}

	int type1,type2;
	int loopx = 0;

	src1value = this->asm_z80_arg(src1,type1,this->label1);
	src2value = this->asm_z80_arg(src2,type2,this->label2);

	if(type2 == 0)
    {
        if(src2.immediate == 0)
        {
            if(type1 == 1)
            {
                loopx = 1;
                if(src1.type == EAGLE_keywords::REG1)
                    loopx = 2;
            }
        }
    }

    if(type == TYPE_IF)
		loopx = 0;

    if(type1 == 0)
    {
        if(src1.immediate == 1)
        {
			this->text_code += "jp " + label_adr + "\n";
			return;
        }
    }

    if( (operator1 == '=') && (operator2 == '=') )
    if(loopx != 0)
    {
        if(loopx == 2)
        {
            this->text_code += "djnz " + label_adr + "\n";
        }else
        {
            this->text_code += "dec " + src1value + "\n";
            this->text_code += "jp NZ," + label_adr + "\n";
        }

        return;
    }

	if(src1.type != EAGLE_keywords::ACC)
	{
		this->text_code += "ld a," + src1value + "\n";
	}

	if(type2 != -1)
	{
		if(operator1 == '&')
		{
			this->text_code += "and " + src2value + "\n";
			flag = "NZ";
			if(operator2 == '!') flag = "Z";

		}
		else
		{
			this->text_code += "cp " + src2value + "\n";
		}

	}
	else
	{
		this->text_code += "ld hl," + std::to_string(src2.address&0xFFFF) + "\n";
		this->text_code += "cp (hl)\n";
	}

	if( (operator1 == '=') && (operator2 == '=') )
		flag = "NZ";

	if( (operator1 == '!') && (operator2 == '=') )
		flag = "Z";

	if( (operator1 == '>') && (operator2 == '=') )
	{
		flag = "C";
	}
	else
	{
		if(operator1 == '>')
		{
			flag = "C";
			this->text_code += "jp Z," + label_adr + "\n";
			this->text_code += "cp " + src2value + "\n";
		}

	}

	bool jflag = false;
	if( (operator1 == '<') && (operator2 == '=') )
	{
		flag = "NC";
		this->text_code += "jp Z,.label_a" + std::to_string(this->ilabel) + "\n";
		this->text_code += "cp " + src2value + "\n";
		jflag = true;
	}
	else
	{
		if(operator1 == '<')
			flag = "NC";
	}

	if(type != TYPE_IF)
	{
		if(flag == "NZ") flag = "Z";
		else
		if(flag == "NC") flag = "C";
		else
		if(flag == "Z") flag = "NZ";
		else
		if(flag == "C") flag = "NC";

	}

	this->text_code += "jp " + flag + "," + label_adr + "\n";


	if(jflag == true)
		this->text_code += ".label_a"+std::to_string(this->ilabel)+":\n";
}

void Eagle::asm_call_jump_z80(const EAGLE_VARIABLE &src,int ninst,int type)
{
	std::string src1value = this->labelcall;
	std::string mnemonic = "jp ",reg = "a";
	if(type >= 1)
		mnemonic = "call ";

	for(int i = 0;i < ninst;i++)
	{
		EAGLE_VARIABLE var = this->arg[i];

		if(var.type == EAGLE_keywords::IDX)
		{
			reg = "IX";
		}else
		if(var.type == EAGLE_keywords::IDY)
		{
			reg = "IY";
		}else
		if(var.type == EAGLE_keywords::ACC)
		{
			reg = "IX";
		}
		else
		if(var.type == EAGLE_keywords::REGBC)
		{
			reg = "bc";
		}
		else
		if(var.type == EAGLE_keywords::REGDE)
		{
			reg = "de";
		}
		else
		if(var.type == EAGLE_keywords::IDHL)
		{
			reg = "hl";
		}
		else
		if(var.type == EAGLE_keywords::REG2)
		{
			this->text_code += "ld a,b\n";
		}
		else
		if(var.type == EAGLE_keywords::REG3)
		{
			this->text_code += "ld a,c\n";
		}
		else
		if(var.type == EAGLE_keywords::REG4)
		{
			this->text_code += "ld a,d\n";
		}
		else
		if(var.type == EAGLE_keywords::REG5)
		{
			this->text_code += "ld a,e\n";
		}
		else
		if(var.type == EAGLE_keywords::IDH)
		{
			this->text_code += "ld a,h\n";
		}
		else
		if(var.type == EAGLE_keywords::IDL)
		{
			this->text_code += "ld a,l\n";
		}
		else
		{


			if( (var.type == EAGLE_keywords::UINT8) || (var.type == EAGLE_keywords::INT8))
			{
				this->text_code += "ld a,(" + std::to_string(var.address&0xFFFF) + ")\n";
			}
			else
			if( (var.type == EAGLE_keywords::UINT16) || (var.type == EAGLE_keywords::INT16))
			{
				this->text_code += "ld de,(" + std::to_string(var.address&0xFFFF) + ")\n";
				reg = "de";
			}
			else
			if(var.bimm == true)
			{
				if(var.immediate < 0x100)
				{
					this->text_code += "ld a," + std::to_string(var.immediate&0xFFFF) + "\n";
				}else
				{
					this->text_code += "ld de," + std::to_string(var.immediate&0xFFFF) + "\n";
					reg = "de";
				}

			}
			else

			if(var.bimm == true)
			{
				this->text_code += "ld de," + std::to_string(var.immediate&0xFFFF) + "\n";
				reg = "de";
			}
			else
			if(var.blabel == true)
			{
				this->text_code += "ld de," + this->labelarg[i] + "\n";
				reg = "de";
			}
		}

		this->text_code += "ld (" + src1value + "..arg" + std::to_string(i) +"@)," + reg +"\n";
	}

	//------
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
				this->text_code += "ld a,("+ std::to_string(ret.address) +")\n";
		}

		this->text_code += "ret\n";
	}
}

void Eagle::asm_alu_z80(const EAGLE_VARIABLE &dst,const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2)
{
	std::string mnemonic;
	std::string src1value,dstvalue;
	std::string src2value;
	std::string reg = "c",rega = "";

	int typed,type1,type2;

	dstvalue = this->asm_z80_arg(dst,typed,this->label1);
	src1value = this->asm_z80_arg(src1,type1,this->label1);
	src2value = this->asm_z80_arg(src2,type2,this->label2);


	if(operator1 == '=')
	{
		//16 bits
		if(dst.type == EAGLE_keywords::UINT16)
		{
			if( (typed <= 0) && (type2 <= 0) )
			{
				this->text_code += "ld de," + src2value + "\n";
				this->text_code += "ld " + dstvalue + ",de\n";

				return;
			}

			if( (typed < 0) && (type2 == 2) )
			{
				this->text_code += "ld de," + src2value + "\n";
				this->text_code += "ld " + dstvalue + ",de\n";

				return;
			}

			if(type2 == 2)
			{
				this->text_code += "ld " + dstvalue + "," + src2value + "\n";
				return;
			}

		}

		if( (typed == 1) && (type2 == 0) && (src2.immediate == 0) )
		{
			this->text_code += "xor " + dstvalue +" \n";
			return;
		}

		if( (typed > 0) && (type2 >= 0) )
		{
			this->text_code += "ld " + dstvalue + "," + src2value +" \n";
			return;
		}




		if(typed == 2)
		{
			this->text_code += "ld " + dstvalue + "," + src2value +" \n";
			return;
		}

		if( (type2 == 0) && (src2.immediate == 0) )
		{
			this->text_code += "xor a\n";
		}
		else
		if(src2.type != EAGLE_keywords::ACC)
			this->text_code += "ld a," + src2value + "\n";

		if(dst.type != EAGLE_keywords::ACC)
			this->text_code += "ld " + dstvalue + ",a\n";
		return;
	}

	if(operator1 == '+')
	{
		mnemonic = "add ";
		rega = "a,";


		if( (dst.type != EAGLE_keywords::UINT16) && (src1.type == dst.type) && (type2 == 0) && ( (src2.immediate == 1) || (src2.immediate == 2) ) )
		{
			if(type1 > 0)
			{
				this->text_code += "inc " + dstvalue + "\n";

				if(src2.immediate == 2)
					this->text_code += "inc " + dstvalue + "\n";

				return;
			}


			if(type1 == -1)
			{
				this->text_code += "ld hl," + std::to_string(src1.address&0xFFFF) + "\n";
				this->text_code += "inc (hl)\n";

				if(src2.immediate == 2)
					this->text_code += "inc (hl)\n";
				return;
			}
		}
	}


	if(operator1 == '-')
	{
		mnemonic = "sub ";
		if( (dst.type != EAGLE_keywords::UINT16) && (src1.type == dst.type) && (type2 == 0) && ( (src2.immediate == 1) || (src2.immediate == 2) ) )
		{
			if(type1 > 0)
			{
				this->text_code += "dec " + dstvalue + "\n";
				if(src2.immediate == 2)
					this->text_code += "dec " + dstvalue + "\n";

				return;
			}


			if(type1 == -1)
			{
				this->text_code += "ld hl," + std::to_string(src1.address&0xFFFF) + "\n";
				this->text_code += "dec (hl)\n";

				if(src2.immediate == 2)
					this->text_code += "dec (hl)\n";

				return;
			}
		}
	}

	int op_spe = 0;


	if(operator1 == '&')
		mnemonic = "and ";

	if(operator1 == '|')
		mnemonic = "or ";

	if(operator1 == '^')
		mnemonic = "xor ";


	int shift = 0;

	if(operator1 == '<')
    {
        op_spe = 1;
        mnemonic = "sla ";
        shift = 1;
    }


	if(operator1 == '>')
    {
        op_spe = 1;
        mnemonic = "srl ";
        shift = 1;
    }



	if(operator1 == '*')
	{
	    op_spe = 2;
	}

	if(operator1 == '/')
	{
	    op_spe = 3;
	}

	//----

	bool hlopti = false;


	if( (dst.type == EAGLE_keywords::UINT16) && (src1.type == EAGLE_keywords::UINT16) )
	{
		if( (src2.type == EAGLE_keywords::UINT16) || (type2 == 0) )
		{
			this->text_code += "ld hl," + src1value + "\n";
			this->text_code += "ld de," + src2value + "\n";


			if(operator1 == '+')
				this->text_code += "add hl,de\n";
			else
			if(operator1 == '-')
			{
				this->text_code += "or a\n";
				this->text_code += "sbc hl,de\n";

			}else
			{
				this->text_code += "ld a,l\n";
				this->text_code += mnemonic + "d\n";
				this->text_code += "ld l,a\n";

				this->text_code += "ld a,h\n";
				this->text_code += mnemonic + "e\n";
				this->text_code += "ld h,a\n";
			}


			this->text_code += "ld " + dstvalue + ",hl\n";

			return;
		}
	}

	if(type1 == -1)
	{
		if(dstvalue == src1value)
		{
			dstvalue = "(hl)";
			src1value = "(hl)";
			this->text_code += "ld hl," + std::to_string(src1.address&0xFFFF) + "\n";

			hlopti = true;
		}
	}

	if( (type2 == 1) || (type2 == 0) )
	{
		reg = src2value;
	}
	else
	if(type2 == -1)
	{
		if(hlopti == false)
		{
			this->text_code += "ld hl," + std::to_string(src2.address&0xFFFF) + "\n";
			reg = "(hl)";

			if(dstvalue == src2value)
				dstvalue = "(hl)";

		}else
		{
			this->text_code += "ld a," + src2value + "\n";
			this->text_code += "ld c,a\n";
		}

	}
	else
	{

	}

	if(dstvalue == src1value)
		shift += 1;

	if(type1 != 2)
	{
		if(src1.type != EAGLE_keywords::ACC)
		{
			if(shift != 2)
			this->text_code += "ld a," + src1value + "\n";
		}
	}else
	{
		this->text_code += "ld de," + src2value + "\n";
		reg = ",de";
		rega = src1value;
	}

    if(op_spe == 0)
        this->text_code += mnemonic + rega + reg +"\n";

    if(op_spe == 1)
    {
        if(type2 == 0)
        {
            for(int i = 0; i < (src2.immediate&7);i++)
                this->text_code += mnemonic + src1value +"\n";


			if(dstvalue == src1value)
				return;
        }else
        {
            if(operator1 == '>')
                this->text_code += "call higueul_std_sla\n";
            else
               this->text_code += "call higueul_std_srl\n";
        }
    }

    if(op_spe == 2)
    {
        this->text_code += "call higueul_std_mul\n";
    }

    if(op_spe == 3)
    {
        this->text_code += "call higueul_std_div\n";
    }

	if(typed != 2)
	{
		if(dst.type != EAGLE_keywords::ACC)
			this->text_code += "ld " + dstvalue + ",a\n";
	}
	else
	{
		if(dst.type != src1.type)
			this->text_code += "ld " + dstvalue + ",de\n";
	}




}
