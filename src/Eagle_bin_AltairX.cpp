
#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include <cstring>
#include <string>

#include <stdint.h>

#include "Eagle.hpp"


uint32_t register_value(std::string str)
{
	if(str == "sp")
		return 0;

	if(str == "a0")
		return 1;

	if(str == "a1")
		return 2;

	if(str == "a2")
		return 3;

	if(str == "a3")
		return 4;

	if(str == "a4")
		return 5;

	if(str == "a5")
		return 6;

	if(str == "a6")
		return 7;

	if(str == "a7")
		return 8;

	//---------
	if(str == "s0")
		return 9;

	if(str == "s1")
		return 10;

	if(str == "s2")
		return 11;

	if(str == "s3")
		return 12;

	if(str == "s4")
		return 13;

	if(str == "s5")
		return 14;

	if(str == "s6")
		return 15;

	if(str == "s7")
		return 16;

	if(str == "s8")
		return 17;

	if(str == "s9")
		return 18;

	if(str == "s10")
		return 19;

	if(str == "s11")
		return 20;

	//---------
	if(str == "s0")
		return 9;

	if(str == "s1")
		return 10;

	if(str == "s2")
		return 11;

	if(str == "s3")
		return 12;

	if(str == "s4")
		return 13;

	if(str == "s5")
		return 14;

	if(str == "s6")
		return 15;

	if(str == "s7")
		return 16;

	if(str == "s8")
		return 17;

	if(str == "s9")
		return 18;

	if(str == "s10")
		return 19;

	if(str == "s11")
		return 20;

	//---------

	if(str == "t0")
		return 21;

	if(str == "t1")
		return 22;

	if(str == "t2")
		return 23;

	if(str == "t3")
		return 24;

	if(str == "t4")
		return 25;

	if(str == "t5")
		return 26;

	if(str == "t6")
		return 27;

	if(str == "t7")
		return 28;

	if(str == "t8")
		return 29;

	if(str == "t9")
		return 30;

	if(str == "t10")
		return 31;

	//---------

	if(str == "acc")
		return 56;

	if(str == "balu1")
		return 57;

	if(str == "balu2")
		return 58;

	if(str == "bfpu1")
		return 59;

	if(str == "bfpu2")
		return 60;

	if(str == "blsu1")
		return 61;

	if(str == "blsu2")
		return 62;

	if(str == "rz")
		return 63;

	return 0;
}

void Eagle::bin_AltairX()
{
	while(this->line_code_asm(0) != 0)
	{
		if(mnemonic.size() > 0)
		{
			if(mnemonic[0].token2 == ':')
			{
				this->labelbin[mnemonic[0].item] = this->offset;
				continue;
			}

			 this->offset += 4;
		}
	}

	this->offset = 0;
	this->idf = 0;
	mnemonic.clear();

	if(this->offset>>10)
	{
		std::cout << (this->offset>>10) << " kiB";
	}else
	{
		std::cout << (this->offset) << " Bytes";
	}

	std::cout <<  std::hex <<"\n";

	std::string instw;
	this->offset = 0;
	this->idf = 0;
	mnemonic.clear();

	uint8_t pairing = 0,opsize = 0;

	while(this->line_code_asm(1) != 0)
	{
		if(mnemonic.size() > 0)
		{
			instw = mnemonic[0].item;

			if(mnemonic[0].token2 == ':')
				continue;
/*
			if(this->debug == true)
			std::cout << this->offset << ":";*/

			uint32_t imm24 = 0,imm18,imm9,imm12,imm10,imm16;
			uint32_t regA = 0,regB = 0,regC = 0;

			uint32_t opcode = 0;
			uint32_t opcode1 = 0;
			uint32_t opcode2 = 0;
			uint32_t opcode3 = 0;
			uint32_t opcode4 = 0;
			uint32_t opcode5 = 0;
			uint32_t opcode6 = 0;
			uint32_t opcode7 = 0;

			if(mnemonic.size() == 2)
			{
				imm24 = mnemonic[1].value&0xFFFFFF;
				regA = register_value(mnemonic[1].item);


				if(mnemonic[1].type == 1) //WORD
				{
					if(mnemonic[1].token1 == '@')
					{
						mnemonic[1].value = this->gvariable[mnemonic[1].item].address;

					}else
					{
						mnemonic[1].value = this->labelbin[mnemonic[1].item]>>2;

						if(mnemonic[1].value == 0)
						{
							std::cout << "warning: label zero :" <<  mnemonic[1].item <<"\n";
						}

						if(mnemonic[0].item[0] == 'b')
						{
							mnemonic[1].value  = (mnemonic[1].value- (this->offset>>2))&0xFFFFFF;
						}


					}

					imm24 = mnemonic[1].value;
				}
			}

			if(mnemonic.size() == 3)
			{
				imm18 = mnemonic[2].value&0x3FFFF;
				imm16 = mnemonic[2].value&0xFFFF;
				regA = register_value(mnemonic[1].item);
				regB = register_value(mnemonic[2].item);

				//--------
				opcode2 = regA<<(26-8);
				opcode2 |= imm18;
				//--------
				opcode4 = regA<<(20-8);
				opcode4 |= regB<<(14-8);
				//--------

				opcode7 = regA<<(26-8);
				opcode7 |= imm16<<2;
				opcode7 |= opsize;
			}

			if(mnemonic.size() == 4)
			{
				imm9 = mnemonic[3].value&0x1FF;
				imm10 = mnemonic[3].value&0x3FF;
				imm12 = mnemonic[3].value&0xFFF;

				regA = register_value(mnemonic[1].item);
				regB = register_value(mnemonic[2].item);
				regC = register_value(mnemonic[3].item);

				//--------
				opcode1 = regA<<(26-8);
				opcode1 |= regB<<(20-8);
				opcode1 |= imm12;
				//--------
				opcode3 = regA<<(26-8);
				opcode3 |= regB<<(20-8);

				if(mnemonic[3].type == 0)
				{
					opcode3 |= 0x04;
					opcode3 |= imm9<<(3);
				}
				else
					opcode3 |= regC<<(14-8);

				opcode3 |= opsize;
				//--------
				opcode5 = regA<<(26-8);
				opcode5 |= regB<<(20-8);
				opcode5 |= regC<<(14-8);

				opcode5 |= opsize;

				//--------
				opcode6 = regA<<(26-8);
				opcode6 |= regB<<(20-8);
				opcode6 |= imm10<<2;

				opcode6 |= opsize;
			}



			if(instw == "nop")
			{
				this->filebin.push_back(0x00|pairing);
				this->filebin.push_back(0x00);
				this->filebin.push_back(0x00);
				this->filebin.push_back(0x00);
			}

			if(instw == "moveix")
			{
				this->filebin.push_back(0x00|pairing);
				this->filebin.push_back(imm24);
				this->filebin.push_back(imm24>>8);
				this->filebin.push_back(imm24>>16);
			}

			if(instw == "cop")
			{
				this->filebin.push_back(0x02|pairing);
				this->filebin.push_back(imm24);
				this->filebin.push_back(imm24>>8);
				this->filebin.push_back(imm24>>16);
			}

			if(instw == "ext")
			{
				this->filebin.push_back(0x04|pairing);
				this->filebin.push_back(opcode1);
				this->filebin.push_back(opcode1>>8);
				this->filebin.push_back(opcode1>>16);
			}

			if(instw == "ins")
			{
				this->filebin.push_back(0x06|pairing);
				this->filebin.push_back(opcode1);
				this->filebin.push_back(opcode1>>8);
				this->filebin.push_back(opcode1>>16);
			}

			if(instw == "movei")
			{
				this->filebin.push_back(0x08|pairing);
				this->filebin.push_back(opcode2);
				this->filebin.push_back(opcode2>>8);
				this->filebin.push_back(opcode2>>16);
			}

			if(instw == "moven")
			{
				this->filebin.push_back(0x0A|pairing);
				this->filebin.push_back(opcode2);
				this->filebin.push_back(opcode2>>8);
				this->filebin.push_back(opcode2>>16);
			}

			if(instw == "moveup")
			{
				this->filebin.push_back(0x0C|pairing);
				this->filebin.push_back(opcode2);
				this->filebin.push_back(opcode2>>8);
				this->filebin.push_back(opcode2>>16);
			}


			if(instw == "sext")
			{
				opcode = regA<<(26-8);
				opcode |= regB<<(20-8);
				opcode |= 0x3F<<(14-8);

				this->filebin.push_back(0x12|pairing);
				this->filebin.push_back(opcode);
				this->filebin.push_back( (opcode>>8)|opsize );
				this->filebin.push_back(opcode>>16);
			}

			if(instw == "rtl")
			{
				this->filebin.push_back(0x14|pairing);
				this->filebin.push_back(opcode3);
				this->filebin.push_back(opcode3>>8);
				this->filebin.push_back(opcode3>>16);
			}

			if(instw == "rtr")
			{
				this->filebin.push_back(0x16|pairing);
				this->filebin.push_back(opcode3);
				this->filebin.push_back(opcode3>>8);
				this->filebin.push_back(opcode3>>16);
			}

			if(instw == "cmp")
			{
				this->filebin.push_back(0x18|pairing);
				this->filebin.push_back(opcode4);
				this->filebin.push_back(opcode4>>8);
				this->filebin.push_back(opcode4>>16);
			}

			if(instw == "test")
			{
				this->filebin.push_back(0x1A|pairing);
				this->filebin.push_back(opcode4);
				this->filebin.push_back(opcode4>>8);
				this->filebin.push_back(opcode4>>16);
			}

			if(instw == "cbit")
			{
				this->filebin.push_back(0x1E|pairing);
				this->filebin.push_back(opcode4);
				this->filebin.push_back(opcode4>>8);
				this->filebin.push_back(opcode4>>16);
			}

			//-----------------
			if(instw == "add")
			{
				this->filebin.push_back(0x20|pairing);
				this->filebin.push_back(opcode3);
				this->filebin.push_back(opcode3>>8);
				this->filebin.push_back(opcode3>>16);
			}

			if(instw == "sub")
			{
				this->filebin.push_back(0x22|pairing);
				this->filebin.push_back(opcode3);
				this->filebin.push_back(opcode3>>8);
				this->filebin.push_back(opcode3>>16);
			}

			if(instw == "xor")
			{
				this->filebin.push_back(0x24|pairing);
				this->filebin.push_back(opcode3);
				this->filebin.push_back(opcode3>>8);
				this->filebin.push_back(opcode3>>16);
			}

			if(instw == "or")
			{
				this->filebin.push_back(0x26|pairing);
				this->filebin.push_back(opcode3);
				this->filebin.push_back(opcode3>>8);
				this->filebin.push_back(opcode3>>16);
			}

			if(instw == "and")
			{
				this->filebin.push_back(0x28|pairing);
				this->filebin.push_back(opcode3);
				this->filebin.push_back(opcode3>>8);
				this->filebin.push_back(opcode3>>16);
			}

			if(instw == "lsl")
			{
				this->filebin.push_back(0x2A|pairing);
				this->filebin.push_back(opcode3);
				this->filebin.push_back(opcode3>>8);
				this->filebin.push_back(opcode3>>16);
			}

			if(instw == "asr")
			{
				this->filebin.push_back(0x2C|pairing);
				this->filebin.push_back(opcode3);
				this->filebin.push_back(opcode3>>8);
				this->filebin.push_back(opcode3>>16);
			}

			if(instw == "lsr")
			{
				this->filebin.push_back(0x2E|pairing);
				this->filebin.push_back(opcode3);
				this->filebin.push_back(opcode3>>8);
				this->filebin.push_back(opcode3>>16);
			}

			if(instw == "se")
			{
				this->filebin.push_back(0x30|pairing);
				this->filebin.push_back(opcode3);
				this->filebin.push_back(opcode3>>8);
				this->filebin.push_back(opcode3>>16);
			}

			if(instw == "sen")
			{
				this->filebin.push_back(0x32|pairing);
				this->filebin.push_back(opcode3);
				this->filebin.push_back(opcode3>>8);
				this->filebin.push_back(opcode3>>16);
			}

			if(instw == "slts")
			{
				this->filebin.push_back(0x34|pairing);
				this->filebin.push_back(opcode3);
				this->filebin.push_back(opcode3>>8);
				this->filebin.push_back(opcode3>>16);
			}

			if(instw == "sltu")
			{
				this->filebin.push_back(0x36|pairing);
				this->filebin.push_back(opcode3);
				this->filebin.push_back(opcode3>>8);
				this->filebin.push_back(opcode3>>16);
			}

			if(instw == "sand")
			{
				this->filebin.push_back(0x38|pairing);
				this->filebin.push_back(opcode3);
				this->filebin.push_back(opcode3>>8);
				this->filebin.push_back(opcode3>>16);
			}

			if(instw == "sbit")
			{
				this->filebin.push_back(0x3A|pairing);
				this->filebin.push_back(opcode3);
				this->filebin.push_back(opcode3>>8);
				this->filebin.push_back(opcode3>>16);
			}

			if(instw == "cmoven")
			{
				this->filebin.push_back(0x3C|pairing);
				this->filebin.push_back(opcode3);
				this->filebin.push_back(opcode3>>8);
				this->filebin.push_back(opcode3>>16);
			}

			if(instw == "cmove")
			{
				this->filebin.push_back(0x3E|pairing);
				this->filebin.push_back(opcode3);
				this->filebin.push_back(opcode3>>8);
				this->filebin.push_back(opcode3>>16);
			}
			//-----------------

			if(instw == "ld")
			{
				this->filebin.push_back(0x40|pairing);
				this->filebin.push_back(opcode5);
				this->filebin.push_back(opcode5>>8);
				this->filebin.push_back(opcode5>>16);
			}

			if(instw == "st")
			{
				this->filebin.push_back(0x42|pairing);
				this->filebin.push_back(opcode5);
				this->filebin.push_back(opcode5>>8);
				this->filebin.push_back(opcode5>>16);
			}

			if(instw == "fld")
			{
				this->filebin.push_back(0x44|pairing);
				this->filebin.push_back(opcode5);
				this->filebin.push_back(opcode5>>8);
				this->filebin.push_back(opcode5>>16);
			}

			if(instw == "fst")
			{
				this->filebin.push_back(0x46|pairing);
				this->filebin.push_back(opcode5);
				this->filebin.push_back(opcode5>>8);
				this->filebin.push_back(opcode5>>16);
			}

			if(instw == "ldi")
			{
				this->filebin.push_back(0x48|pairing);
				this->filebin.push_back(opcode6);
				this->filebin.push_back(opcode6>>8);
				this->filebin.push_back(opcode6>>16);
			}

			if(instw == "sti")
			{
				this->filebin.push_back(0x4A|pairing);
				this->filebin.push_back(opcode6);
				this->filebin.push_back(opcode6>>8);
				this->filebin.push_back(opcode6>>16);
			}

			if(instw == "fldi")
			{
				this->filebin.push_back(0x4C|pairing);
				this->filebin.push_back(opcode6);
				this->filebin.push_back(opcode6>>8);
				this->filebin.push_back(opcode6>>16);
			}

			if(instw == "fsti")
			{
				this->filebin.push_back(0x4E|pairing);
				this->filebin.push_back(opcode6);
				this->filebin.push_back(opcode6>>8);
				this->filebin.push_back(opcode6>>16);
			}

			if(instw == "ldsp")
			{
				this->filebin.push_back(0x50|pairing);
				this->filebin.push_back(opcode7);
				this->filebin.push_back(opcode7>>8);
				this->filebin.push_back(opcode7>>16);
			}

			if(instw == "stsp")
			{
				this->filebin.push_back(0x52|pairing);
				this->filebin.push_back(opcode7);
				this->filebin.push_back(opcode7>>8);
				this->filebin.push_back(opcode7>>16);
			}

			if(instw == "fldsp")
			{
				this->filebin.push_back(0x54|pairing);
				this->filebin.push_back(opcode7);
				this->filebin.push_back(opcode7>>8);
				this->filebin.push_back(opcode7>>16);
			}

			if(instw == "fstsp")
			{
				this->filebin.push_back(0x56|pairing);
				this->filebin.push_back(opcode7);
				this->filebin.push_back(opcode7>>8);
				this->filebin.push_back(opcode7>>16);
			}

			//-----------------

			if(instw == "fadd")
			{
				this->filebin.push_back(0x60|pairing);
				this->filebin.push_back(opcode3);
				this->filebin.push_back(opcode3>>8);
				this->filebin.push_back(opcode3>>16);
			}

			if(instw == "fsub")
			{
				this->filebin.push_back(0x62|pairing);
				this->filebin.push_back(opcode3);
				this->filebin.push_back(opcode3>>8);
				this->filebin.push_back(opcode3>>16);
			}

			if(instw == "fmul")
			{
				this->filebin.push_back(0x64|pairing);
				this->filebin.push_back(opcode3);
				this->filebin.push_back(opcode3>>8);
				this->filebin.push_back(opcode3>>16);
			}

			if(instw == "fnmul")
			{
				this->filebin.push_back(0x66|pairing);
				this->filebin.push_back(opcode3);
				this->filebin.push_back(opcode3>>8);
				this->filebin.push_back(opcode3>>16);
			}

			//---------------------------------

			if(instw == "div")
			{
				this->filebin.push_back(0xC0|pairing);
				this->filebin.push_back(opcode4);
				this->filebin.push_back(opcode4>>8);
				this->filebin.push_back(opcode4>>16);
			}

			if(instw == "divu")
			{
				this->filebin.push_back(0xC2|pairing);
				this->filebin.push_back(opcode4);
				this->filebin.push_back(opcode4>>8);
				this->filebin.push_back(opcode4>>16);
			}

			if(instw == "mul")
			{
				this->filebin.push_back(0xC4|pairing);
				this->filebin.push_back(opcode4);
				this->filebin.push_back(opcode4>>8);
				this->filebin.push_back(opcode4>>16);
			}

			if(instw == "mulu")
			{
				this->filebin.push_back(0xC6|pairing);
				this->filebin.push_back(opcode4);
				this->filebin.push_back(opcode4>>8);
				this->filebin.push_back(opcode4>>16);
			}

			//-------------------------


			if(instw == "bne")
			{
				this->filebin.push_back(0xE0|pairing);
				this->filebin.push_back(imm24);
				this->filebin.push_back(imm24>>8);
				this->filebin.push_back(imm24>>16);
			}

			if(instw == "beq")
			{
				this->filebin.push_back(0xE2|pairing);
				this->filebin.push_back(imm24);
				this->filebin.push_back(imm24>>8);
				this->filebin.push_back(imm24>>16);
			}

			if(instw == "bl")
			{
				this->filebin.push_back(0xE4|pairing);
				this->filebin.push_back(imm24);
				this->filebin.push_back(imm24>>8);
				this->filebin.push_back(imm24>>16);
			}

			if(instw == "ble")
			{
				this->filebin.push_back(0xE6|pairing);
				this->filebin.push_back(imm24);
				this->filebin.push_back(imm24>>8);
				this->filebin.push_back(imm24>>16);
			}

			if(instw == "bg")
			{
				this->filebin.push_back(0xE8|pairing);
				this->filebin.push_back(imm24);
				this->filebin.push_back(imm24>>8);
				this->filebin.push_back(imm24>>16);
			}

			if(instw == "bge")
			{
				this->filebin.push_back(0xEA|pairing);
				this->filebin.push_back(imm24);
				this->filebin.push_back(imm24>>8);
				this->filebin.push_back(imm24>>16);
			}

			if(instw == "bls")
			{
				this->filebin.push_back(0xEC|pairing);
				this->filebin.push_back(imm24);
				this->filebin.push_back(imm24>>8);
				this->filebin.push_back(imm24>>16);
			}

			if(instw == "bles")
			{
				this->filebin.push_back(0xEE|pairing);
				this->filebin.push_back(imm24);
				this->filebin.push_back(imm24>>8);
				this->filebin.push_back(imm24>>16);
			}

			if(instw == "bgs")
			{
				this->filebin.push_back(0xF0|pairing);
				this->filebin.push_back(imm24);
				this->filebin.push_back(imm24>>8);
				this->filebin.push_back(imm24>>16);
			}

			if(instw == "bges")
			{
				this->filebin.push_back(0xF2|pairing);
				this->filebin.push_back(imm24);
				this->filebin.push_back(imm24>>8);
				this->filebin.push_back(imm24>>16);
			}

			if(instw == "bra")
			{
				this->filebin.push_back(0xF4|pairing);
				this->filebin.push_back(imm24);
				this->filebin.push_back(imm24>>8);
				this->filebin.push_back(imm24>>16);
			}

			if(instw == "loop")
			{
				this->filebin.push_back(0xF6|pairing);
				this->filebin.push_back(imm24);
				this->filebin.push_back(imm24>>8);
				this->filebin.push_back(imm24>>16);
			}

			if(instw == "jump")
			{
				this->filebin.push_back(0xF8|pairing);
				this->filebin.push_back(imm24);
				this->filebin.push_back(imm24>>8);
				this->filebin.push_back(imm24>>16);
			}

			if(instw == "call")
			{
				this->filebin.push_back(0xFA|pairing);
				this->filebin.push_back(imm24);
				this->filebin.push_back(imm24>>8);
				this->filebin.push_back(imm24>>16);
			}

			if(instw == "callr")
			{
				this->filebin.push_back(0xFC|pairing);
				this->filebin.push_back(imm24);
				this->filebin.push_back(imm24>>8);
				this->filebin.push_back(imm24>>16);
			}

			if(instw == "syscall")
			{
				this->filebin.push_back(0xFE|pairing);
				imm24 = 0x04;
				this->filebin.push_back(imm24);
				this->filebin.push_back(imm24>>8);
				this->filebin.push_back(imm24>>16);
			}

			if(instw == "int")
			{
				this->filebin.push_back(0xFE|pairing);
				imm24 = 0x05;
				this->filebin.push_back(imm24);
				this->filebin.push_back(imm24>>8);
				this->filebin.push_back(imm24>>16);
			}

			if(instw == "ret")
			{
				this->filebin.push_back(0xFE|pairing);
				imm24 = 0x02;
				this->filebin.push_back(imm24);
				this->filebin.push_back(imm24>>8);
				this->filebin.push_back(imm24>>16);
			}

			if(instw == "reti")
			{
				this->filebin.push_back(0xFE|pairing);
				imm24 = 0x03;
				this->filebin.push_back(imm24);
				this->filebin.push_back(imm24>>8);
				this->filebin.push_back(imm24>>16);
			}


			if(instw == "jumpbr")
			{
				this->filebin.push_back(0xFE|pairing);
				imm24 = 0x09;
				this->filebin.push_back(imm24);
				this->filebin.push_back(imm24>>8);
				this->filebin.push_back(imm24>>16);
			}

			if(instw == "callr")
			{
				this->filebin.push_back(0xFE|pairing);
				imm24 = 0x01;
				this->filebin.push_back(imm24);
				this->filebin.push_back(imm24>>8);
				this->filebin.push_back(imm24>>16);
			}

		}

	}

}
