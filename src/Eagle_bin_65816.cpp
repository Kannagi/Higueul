#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include <cstring>
#include <string>

#include <stdint.h>

#include "Eagle.hpp"

static const unsigned char icycle[256] = {
//0x0x
0x07,0x06,0x07,0x04, 0x04,0x03,0x05,0x06,
0x03,0x02,0x02,0x04, 0x06,0x04,0x06,0x05,

//0x1x
0x02,0x05,0x05,0x07, 0x05,0x04,0x06,0x06,
0x02,0x04,0x02,0x02, 0x06,0x04,0x07,0x05,

//0x2x
0x06,0x06,0x08,0x04, 0x03,0x03,0x05,0x06,
0x04,0x02,0x02,0x05, 0x04,0x04,0x06,0x05,

//0x3x
0x02,0x05,0x05,0x07, 0x04,0x04,0x06,0x06,
0x02,0x04,0x02,0x02, 0x04,0x04,0x07,0x05,

//0x4x
0x06,0x06,0x02,0x04, 0x03,0x03,0x05,0x06,
0x03,0x02,0x02,0x03, 0x03,0x04,0x06,0x05,

//0x5x
0x02,0x05,0x05,0x07, 0x03,0x04,0x03,0x06,
0x02,0x04,0x03,0x02, 0x04,0x04,0x07,0x05,

//0x6x
0x06,0x06,0x06,0x04, 0x03,0x03,0x05,0x06,
0x04,0x02,0x02,0x06, 0x05,0x04,0x06,0x05,

//0x7x
0x02,0x05,0x05,0x07, 0x04,0x04,0x06,0x06,
0x02,0x04,0x04,0x02, 0x06,0x04,0x07,0x05,

//0x8x
0x03,0x06,0x06,0x04, 0x03,0x03,0x03,0x06,
0x02,0x02,0x02,0x03, 0x04,0x04,0x04,0x05,

//0x9x
0x02,0x05,0x05,0x07, 0x04,0x04,0x04,0x06,
0x02,0x05,0x02,0x02, 0x04,0x04,0x05,0x05,

//0xAx
0x02,0x06,0x02,0x04, 0x03,0x03,0x03,0x06,
0x02,0x02,0x02,0x06, 0x04,0x04,0x04,0x05,

//0xBx
0x02,0x05,0x05,0x07, 0x04,0x04,0x04,0x06,
0x02,0x04,0x02,0x02, 0x04,0x04,0x04,0x05,

//0xCx
0x02,0x06,0x03,0x04, 0x03,0x03,0x05,0x06,
0x02,0x02,0x02,0x03, 0x04,0x04,0x06,0x05,

//0xDx
0x02,0x05,0x05,0x07, 0x06,0x04,0x06,0x06,
0x02,0x04,0x03,0x03, 0x03,0x04,0x07,0x05,

//0xEx
0x02,0x06,0x03,0x04, 0x03,0x03,0x05,0x06,
0x02,0x02,0x03,0x03, 0x04,0x04,0x06,0x05,

//0xFx
0x02,0x05,0x05,0x07, 0x05,0x04,0x06,0x06,
0x02,0x04,0x04,0x02, 0x08,0x04,0x07,0x05,
};

void Eagle::bin_65816()
{
	this->offset = 0;
	bool imm,inst_sp;
	uint64_t value;
	bool mode_a816 = false;


	while(this->line_code_asm(0) != 0)
	{
		if(mnemonic.size() > 0)
		{
			if(mnemonic[0].token2 == ':')
				this->labelbin[mnemonic[0].item] = 0x100 + this->offset;
		}
	}
	this->offset = 0;
	this->idf = 0;
	mnemonic.clear();

	//---------------------------------------------------------------------

	bool imm8 = false;

	while(this->line_code_asm(0) != 0)
	{
		if(mnemonic.size() > 0)
		{
			if(mnemonic[0].token2 == ':')
			{
				this->labelbin[mnemonic[0].item] = this->offset;
				continue;
			}


			imm = false;
			if( (mnemonic[1].token0 == '#') || (mnemonic[1].token1 == '#') )
				imm = true;

			if(mnemonic.size() == 1)
				this->offset += 1;

			if(mnemonic.size() > 1)
			{
				inst_sp = false;
				if(mnemonic.size() == 3)
				{
					if( (mnemonic[1].item == "sr") || (mnemonic[2].item == "S") )
					{
						inst_sp = true;
						this->offset +=2;
					}

					if( (mnemonic[0].item == "mvn") || (mnemonic[0].item == "mvp") )
						this->offset +=3;
				}

				if(mnemonic.size() == 4)
				{
					if( (mnemonic[1].item == "sr") || (mnemonic[2].item == "S") )
					{
						inst_sp = true;
						this->offset +=2;
					}
				}

				if(mnemonic.size() == 2)
				{
					if(mnemonic[0].item == "rep")
					{
						if(mnemonic[1].value&0x20)
							mode_a816 = true;

						imm8 = true;
					}

					if(mnemonic[0].item == "sep")
					{
						if(mnemonic[1].value&0x20)
							mode_a816 = false;

						imm8 = true;
					}
				}

				if(imm == true)
				{

					if(
					(mnemonic[0].item == "ldx") || (mnemonic[0].item == "ldy") ||
					(mnemonic[0].item == "cpx") || (mnemonic[0].item == "cpy")
					)
					{
						this->offset +=3;
					}else
					{
						if(mode_a816 == true)
						{
							if(imm8 == false)
								this->offset +=3;
							else
								this->offset +=2;
						}
						else
						{
							this->offset +=2;
						}

					}

					imm8 = false;


				}else if(inst_sp == false)
				{
					imm8 = false;

					value = 0;
					if(mnemonic[1].type == 0) //NUMBER
					{
						value = mnemonic[1].value;
					}else
					{
						if(mnemonic[1].token1 == '@')
						{
							value = this->gvariable[mnemonic[1].item].address;

						}else
						{
							value = this->labelbin[mnemonic[1].item];
							if
							(
								(mnemonic[0].item != "bit") &&
								(mnemonic[0].item != "brl") &&
								(mnemonic[0].item[0] == 'b')
							)
								value  &= 0xFF;
						}


					}

					if( (mnemonic[0].item == "jsr") || (mnemonic[0].item == "jmp") )
					{
						if(value < 0x810000)
							value &= 0x7FFFFF;
						if(value < 0x100)
							value = 0x1000;
					}

					if( (mnemonic[0].item == "jsl") || (mnemonic[0].item == "jml") )
					{
						if(value < 0x10000)
							value = 0x10000;
					}

					if(value < 0x100)
					{
						this->offset +=2;

					}else if(value < 0x10000)
					{
						this->offset +=3;
					}else
					{
						this->offset +=4;
					}
				}

				imm8 = false;

			}
		}

	}

	//------------------------------------------------------------------

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
	mode_a816 = false;
	bool mode_xy16 = false;
	imm8 = false;

	while(this->line_code_asm(1) != 0)
	{
		if(mnemonic.size() > 0)
		{
			instw = mnemonic[0].item;

			if(mnemonic[0].token2 == ':')
				continue;

			if(this->debug == true)
			std::cout << this->offset << ":";

			if(mnemonic.size() == 1)
				this->offset += 1;

			if(mnemonic.size() > 1)
			{
				inst_sp = false;
				if(mnemonic.size() == 3)
				{
					if( (mnemonic[1].item == "sr") || (mnemonic[2].item == "S") )
					{
						inst_sp = true;
						this->offset +=2;
					}

					if( (instw == "mvn") || (instw == "mvp") )
					{
						this->offset +=3;
						inst_sp = true;
					}

				}

				if(mnemonic.size() == 4)
				{
					if( (mnemonic[1].item == "sr") || (mnemonic[2].item == "S") )
					{
						inst_sp = true;
						this->offset +=2;
					}
				}

				if(mnemonic.size() == 2)
				{
					if(instw == "rep")
					{
						if(mnemonic[1].value&0x20)
							mode_a816 = true;

						imm8 = true;
					}

					if(instw == "sep")
					{
						if(mnemonic[1].value&0x20)
							mode_a816 = false;

						imm8 = true;
					}
				}

				imm = false;
				if( (mnemonic[1].token0 == '#') || (mnemonic[1].token1 == '#') )
					imm = true;

				if(imm == true)
				{

					if(
					(instw == "ldx") || (instw == "ldy") ||
					(instw == "cpx") || (instw == "cpy")
					)
					{
						this->offset +=3;
						mode_xy16 = true;
						this->cycle++;
					}else
					{
						if(mode_a816 == true)
						{
							if(imm8 == false)
							{

								this->offset +=3;
							}
							else
								this->offset +=2;
						}
						else
						{
							this->offset +=2;
						}

					}
					imm8 = false;

				}else if(inst_sp == false)
				{
					imm8 = false;
					value = 0;
					if(mnemonic[1].type == 0) //NUMBER
					{
						value = mnemonic[1].value;
					}else
					{

						if(mnemonic[1].token1 == '@')
						{
							value = this->gvariable[mnemonic[1].item].address;

						}else
						{
							value = this->labelbin[mnemonic[1].item];
							if
							(
								(instw != "bit") &&
								(instw != "brl") &&
								(instw[0] == 'b')
							)
								value  &= 0xFF;
						}
					}

					if( (instw == "jsr") || (instw == "jmp") )
					{
						if(value < 0x810000)
							value &= 0x7FFFFF;
						if(value < 0x100)
							value = 0x1000;
					}

					if( (instw == "jsl") || (instw == "jml") )
					{
						if(value < 0x10000)
							value = 0x10000;
					}

					if(value < 0x100)
					{
						this->offset +=2;
					}else if(value < 0x10000)
					{
						this->offset +=3;
					}else
					{
						this->offset +=4;
					}
				}
			}



			imm = false;
			if( (mnemonic[1].token0 == '#') || (mnemonic[1].token1 == '#') )
				imm = true;


			if(this->debug == true)
			std::cout << instw;


			if(mnemonic.size() == 1)
			{

				if(instw == "asl")
					this->filebin.push_back(0x0A);

				if(instw == "clc")
					this->filebin.push_back(0x18);

				if(instw == "cli")
					this->filebin.push_back(0x58);

				if(instw == "cld")
					this->filebin.push_back(0xD8);

				if(instw == "clv")
					this->filebin.push_back(0xB8);

				if(instw == "dea")
					this->filebin.push_back(0x3A);

				if(instw == "dex")
					this->filebin.push_back(0xCA);

				if(instw == "dey")
					this->filebin.push_back(0x88);

				if(instw == "ina")
					this->filebin.push_back(0x1A);

				if(instw == "inx")
					this->filebin.push_back(0xE8);

				if(instw == "iny")
					this->filebin.push_back(0xC8);

				if(instw == "lsr")
					this->filebin.push_back(0x4A);

				if(instw == "nop")
					this->filebin.push_back(0xEA);

				if(instw == "pha")
					this->filebin.push_back(0x48);

				if(instw == "phb")
					this->filebin.push_back(0x8B);

				if(instw == "phd")
					this->filebin.push_back(0x0B);

				if(instw == "phk")
					this->filebin.push_back(0x4B);

				if(instw == "php")
					this->filebin.push_back(0x08);

				if(instw == "phx")
					this->filebin.push_back(0xDA);

				if(instw == "phy")
					this->filebin.push_back(0x5A);

				if(instw == "pla")
					this->filebin.push_back(0x68);

				if(instw == "plb")
					this->filebin.push_back(0xAB);

				if(instw == "pld")
					this->filebin.push_back(0x2B);

				if(instw == "plp")
					this->filebin.push_back(0x28);

				if(instw == "plx")
					this->filebin.push_back(0xFA);

				if(instw == "ply")
					this->filebin.push_back(0x7A);

				if(instw == "rol")
					this->filebin.push_back(0x2A);

				if(instw == "ror")
					this->filebin.push_back(0x6A);

				if(instw == "rti")
					this->filebin.push_back(0x40);

				if(instw == "rts")
					this->filebin.push_back(0x60);

				if(instw == "rtl")
					this->filebin.push_back(0x6B);

				if(instw == "sec")
					this->filebin.push_back(0x38);

				if(instw == "sei")
					this->filebin.push_back(0x78);

				if(instw == "sed")
					this->filebin.push_back(0xF8);

				if(instw == "stp")
					this->filebin.push_back(0xDB);

				if(instw == "tax")
					this->filebin.push_back(0xAA);

				if(instw == "tay")
					this->filebin.push_back(0xA8);

				if(instw == "tcd")
					this->filebin.push_back(0x5B);

				if(instw == "tcs")
					this->filebin.push_back(0x1B);

				if(instw == "tdc")
					this->filebin.push_back(0x7B);

				if(instw == "tsc")
					this->filebin.push_back(0x3B);

				if(instw == "tsx")
					this->filebin.push_back(0xBA);

				if(instw == "txa")
					this->filebin.push_back(0x8A);

				if(instw == "txs")
					this->filebin.push_back(0x9A);

				if(instw == "txy")
					this->filebin.push_back(0x9B);

				if(instw == "tya")
					this->filebin.push_back(0x98);

				if(instw == "tyx")
					this->filebin.push_back(0xBB);

				if(instw == "wai")
					this->filebin.push_back(0xCB);

				if(instw == "xba")
					this->filebin.push_back(0xEB);

				if(instw == "xce")
					this->filebin.push_back(0xFB);




				int tmpnumber = this->filebin[filebin.size()-1]&0xFF;
				if(this->debug == true)
				std::cout  << "  (0x"<< std::hex << tmpnumber << ")\n";

				this->cycle += icycle[tmpnumber&0xFF];

				continue;
			}

			if(mnemonic[1].type == 1) //WORD
			{
				if(mnemonic[1].token1 == '@')
				{
					mnemonic[1].value = this->gvariable[mnemonic[1].item].address;

				}else
				{
					mnemonic[1].value = this->labelbin[mnemonic[1].item];
					if(mnemonic[1].value == 0)
					{
						std::cout << "warning: label zero :" <<  mnemonic[1].item <<"\n";
					}

					if(mnemonic[0].item == "brl")
					{
						mnemonic[1].value  = (mnemonic[1].value-this->offset+2)&0xFFFF;
					}else
					if
					(
						(mnemonic[0].item != "bit") &&
						(mnemonic[0].item[0] == 'b')
					)
					{
						int dif =  (mnemonic[1].value-this->offset);
						if(dif >= 0x80)
						{
							std::cout << "warning : branch if size :" <<  dif <<"\n";
						}

						if(dif <= -0x80)
						{
							std::cout << "warning: branch while/loop size :" <<  dif <<"\n";
						}
						mnemonic[1].value  = (mnemonic[1].value-this->offset)&0xFF;
					}
				}

				if(imm == true)
				{
					if(mnemonic[1].token1 == ':')
						mnemonic[1].value = (mnemonic[1].value>>16)&0xFFFF;
					else
						mnemonic[1].value &= 0xFFFF;
				}
			}


			//-----------
			int immz;
			int format = 0;
			int tval = mnemonic[1].value;
			char tdata[16] = {};

			if(tval < 0x100)
				immz = 0;
			else if(tval < 0x10000)
				immz = 1;
			else
				immz = 2;

			imm8 = false;
			if(imm == true)
			{
				immz = 0;

				if(mode_a816 == true)
					immz = 1;

				if(mode_xy16 == true)
					immz = 1;

				mode_xy16 = false;

			}


			//-----

			if(instw == "adc")
			{
				if(mode_a816 == true)
					this->cycle++;

				tdata[0] = 0x69; //IMM
				tdata[1] = 0x72; //PTR
				tdata[2] = 0x67; //PTR Long
				tdata[3] = 0x65; //adr dp
				tdata[4] = 0x6D; //adr
				tdata[5] = 0x6F; //adr long

				tdata[6] = 0x7D; //adr X
				tdata[7] = 0x7F; //adr long X
				tdata[8] = 0x79; //adr Y
				tdata[9] = 0x75; //dp X
				tdata[10] = 0x61; //(dp),X
				tdata[11] = 0x71; //(dp),Y
				tdata[12] = 0x77; //[dp], Y

				tdata[13] = 0x63; //sr s
				tdata[14] = 0x73; //sr S ,Y
			}

			if(instw == "and")
			{
				if(mode_a816 == true)
					this->cycle++;

				tdata[0] = 0x29; //IMM
				tdata[1] = 0x32; //PTR
				tdata[2] = 0x27; //PTR Long
				tdata[3] = 0x25; //adr dp
				tdata[4] = 0x2D; //adr
				tdata[5] = 0x2F; //adr long

				tdata[6] = 0x3D; //adr X
				tdata[7] = 0x3F; //adr long X
				tdata[8] = 0x39; //adr Y
				tdata[9] = 0x35; //dp X
				tdata[10] = 0x21; //(dp),X
				tdata[11] = 0x31; //(dp),Y
				tdata[12] = 0x37; //[dp], Y

				tdata[13] = 0x23; //sr s
				tdata[14] = 0x33; //sr S ,Y
			}

			if(instw == "cmp")
			{
				if(mode_a816 == true)
					this->cycle++;

				tdata[0] = 0xC9; //IMM
				tdata[1] = 0xD2; //PTR
				tdata[2] = 0xC7; //PTR Long
				tdata[3] = 0xC5; //adr dp
				tdata[4] = 0xCD; //adr
				tdata[5] = 0xCF; //adr long

				tdata[6] = 0xDD; //adr X
				tdata[7] = 0xDF; //adr long X
				tdata[8] = 0xD9; //adr Y
				tdata[9] = 0xD5; //dp X
				tdata[10] = 0xC1; //(dp),X
				tdata[11] = 0xD1; //(dp),Y
				tdata[12] = 0xD7; //[dp], Y

				tdata[13] = 0xC3; //sr s
				tdata[14] = 0xD3; //sr S ,Y
			}

			if(instw == "eor")
			{
				if(mode_a816 == true)
					this->cycle++;

				tdata[0] = 0x49; //IMM
				tdata[1] = 0x52; //PTR
				tdata[2] = 0x47; //PTR Long
				tdata[3] = 0x45; //adr dp
				tdata[4] = 0x4D; //adr
				tdata[5] = 0x4F; //adr long

				tdata[6] = 0x5D; //adr X
				tdata[7] = 0x5F; //adr long X
				tdata[8] = 0x59; //adr Y
				tdata[9] = 0x55; //dp X
				tdata[10] = 0x41; //(dp),X
				tdata[11] = 0x51; //(dp),Y
				tdata[12] = 0x57; //[dp], Y

				tdata[13] = 0x43; //sr s
				tdata[14] = 0x53; //sr S ,Y
			}

			if(instw == "lda")
			{
				if(mode_a816 == true)
					this->cycle++;

				tdata[0] = 0xA9; //IMM
				tdata[1] = 0xB2; //PTR
				tdata[2] = 0xA7; //PTR Long
				tdata[3] = 0xA5; //adr dp
				tdata[4] = 0xAD; //adr
				tdata[5] = 0xAF; //adr long

				tdata[6] = 0xBD; //adr X
				tdata[7] = 0xBF; //adr long X
				tdata[8] = 0xB9; //adr Y
				tdata[9] = 0xB5; //dp X
				tdata[10] = 0xA1; //(dp),X
				tdata[11] = 0xB1; //(dp),Y
				tdata[12] = 0xB7; //[dp], Y

				tdata[13] = 0xA3; //sr s
				tdata[14] = 0xB3; //sr S ,Y
			}

			if(instw == "ora")
			{
				if(mode_a816 == true)
					this->cycle++;

				tdata[0] = 0x09; //IMM
				tdata[1] = 0x12; //PTR
				tdata[2] = 0x07; //PTR Long
				tdata[3] = 0x05; //adr dp
				tdata[4] = 0x0D; //adr
				tdata[5] = 0x0F; //adr long

				tdata[6] = 0x1D; //adr X
				tdata[7] = 0x1F; //adr long X
				tdata[8] = 0x19; //adr Y
				tdata[9] = 0x15; //dp X
				tdata[10] = 0x01; //(dp),X
				tdata[11] = 0x11; //(dp),Y
				tdata[12] = 0x17; //[dp], Y

				tdata[13] = 0x03; //sr s
				tdata[14] = 0x13; //sr S ,Y
			}

			if(instw == "sbc")
			{
				if(mode_a816 == true)
					this->cycle++;

				tdata[0] = 0xE9; //IMM
				tdata[1] = 0xF2; //PTR
				tdata[2] = 0xE7; //PTR Long
				tdata[3] = 0xE5; //adr dp
				tdata[4] = 0xED; //adr
				tdata[5] = 0xEF; //adr long

				tdata[6] = 0xFD; //adr X
				tdata[7] = 0xFF; //adr long X
				tdata[8] = 0xF9; //adr Y
				tdata[9] = 0xF5; //dp X
				tdata[10] = 0xE1; //(dp),X
				tdata[11] = 0xF1; //(dp),Y
				tdata[12] = 0xF7; //[dp], Y

				tdata[13] = 0xE3; //sr s
				tdata[14] = 0xF3; //sr S ,Y
			}

			if(instw == "sta")
			{
				if(mode_a816 == true)
					this->cycle++;

				tdata[0] = 0x89; //IMM
				tdata[1] = 0x92; //PTR
				tdata[2] = 0x87; //PTR Long
				tdata[3] = 0x85; //adr dp
				tdata[4] = 0x8D; //adr
				tdata[5] = 0x8F; //adr long

				tdata[6] = 0x9D; //adr X
				tdata[7] = 0x9F; //adr long X
				tdata[8] = 0x99; //adr Y
				tdata[9] = 0x95; //dp X
				tdata[10] = 0x81; //(dp),X
				tdata[11] = 0x91; //(dp),Y
				tdata[12] = 0x97; //[dp], Y

				tdata[13] = 0x83; //sr s
				tdata[14] = 0x93; //sr S ,Y
			}

			//-----

			if(instw == "asl")
			{
				if(mode_a816 == true)
					this->cycle+=2;
				format = 1;
				tdata[0] = 0x0A; //IMM
				tdata[1] = 0x06; //dp
				tdata[2] = 0x0E; //adr

				tdata[3] = 0x1E; //addr, X
				tdata[4] = 0x16; //dp, X
			}

			if(instw == "dec")
			{
				if(mode_a816 == true)
					this->cycle+=2;
				format = 1;
				tdata[0] = 0x3A; //IMM
				tdata[1] = 0xC6; //dp
				tdata[2] = 0xCE; //adr

				tdata[3] = 0xDE; //addr, X
				tdata[4] = 0xD6; //dp, X
			}

			if(instw == "inc")
			{
				if(mode_a816 == true)
					this->cycle+=2;
				format = 1;
				tdata[0] = 0x1A; //IMM
				tdata[1] = 0xE6; //dp
				tdata[2] = 0xEE; //adr

				tdata[3] = 0xFE; //addr, X
				tdata[4] = 0xF6; //dp, X
			}

			if(instw == "bit")
			{
				if(mode_a816 == true)
					this->cycle+=1;
				format = 1;
				tdata[0] = 0x89; //IMM
				tdata[1] = 0x24; //dp
				tdata[2] = 0x2C; //adr

				tdata[3] = 0x3C; //addr, X
				tdata[4] = 0x34; //dp, X
			}

			if(instw == "cpx")
			{
				format = 1;
				tdata[0] = 0xE0; //IMM
				tdata[1] = 0xE4; //dp
				tdata[2] = 0xEC; //adr
			}

			if(instw == "cpy")
			{
				format = 1;
				tdata[0] = 0xC0; //IMM
				tdata[1] = 0xC4; //dp
				tdata[2] = 0xCC; //adr
			}

			if(instw == "ldx")
			{
				format = 1;
				tdata[0] = 0xA2; //IMM
				tdata[1] = 0xA6; //dp
				tdata[2] = 0xAE; //adr

				tdata[3] = 0xBE; //addr, Y
				tdata[4] = 0xB6; //dp, Y
			}

			if(instw == "ldy")
			{
				format = 1;
				tdata[0] = 0xA0; //IMM
				tdata[1] = 0xA4; //dp
				tdata[2] = 0xAC; //adr

				tdata[3] = 0xBC; //addr, X
				tdata[4] = 0xB4; //dp, X
			}

			if(instw == "lsr")
			{
				if(mode_a816 == true)
					this->cycle++;
				format = 1;
				tdata[0] = 0x4A; //IMM
				tdata[1] = 0x46; //dp
				tdata[2] = 0x4E; //adr

				tdata[3] = 0x5E; //addr, X
				tdata[4] = 0x56; //dp, X
			}

			if(instw == "rol")
			{
				if(mode_a816 == true)
					this->cycle++;
				format = 1;
				tdata[0] = 0x2A; //IMM
				tdata[1] = 0x26; //dp
				tdata[2] = 0x2E; //adr

				tdata[3] = 0x3E; //addr, X
				tdata[4] = 0x36; //dp, X
			}

			if(instw == "ror")
			{
				if(mode_a816 == true)
					this->cycle++;
				format = 1;
				tdata[0] = 0x6A; //IMM
				tdata[1] = 0x66; //dp
				tdata[2] = 0x6E; //adr

				tdata[3] = 0x7E; //addr, X
				tdata[4] = 0x76; //dp, X
			}

			if(instw == "sty")
			{
				format = 1;
				tdata[0] = 0x84; //IMM
				tdata[1] = 0x84; //dp
				tdata[2] = 0x8C; //adr

				tdata[3] = 0x94; //addr, X
				tdata[4] = 0x94; //dp, X
			}

			if(instw == "stx")
			{
				format = 1;
				tdata[0] = 0x8E; //IMM
				tdata[1] = 0x86; //dp
				tdata[2] = 0x8E; //adr

				tdata[3] = 0x96; //addr, Y
				tdata[4] = 0x96; //dp, Y
			}

			if(instw == "stz")
			{
				if(mode_a816 == true)
					this->cycle++;
				format = 1;
				tdata[0] = 0x64; //IMM
				tdata[1] = 0x64; //dp
				tdata[2] = 0x9C; //adr

				tdata[3] = 0x9E; //addr, X
				tdata[4] = 0x74; //dp, X
			}

			if(instw == "jmp")
			{
				if(mnemonic[1].value < 0x810000)
					mnemonic[1].value &= 0x7FFFFF;
				format = 2;
				tdata[0] = 0x4C; //addr
				tdata[1] = 0x6C; //addr indirect
				tdata[2] = 0x7C; //addr indirect x
				tdata[3] = 0x5C; //long
				tdata[4] = 0xDC; //long indirect

				tval = mnemonic[1].value;

				if(tval < 0x10000)
					immz = 1;
				else
					immz = 2;
			}

			if(instw == "jsr")
			{
				if(mnemonic[1].value < 0x810000)
					mnemonic[1].value &= 0x7FFFFF;
				format = 2;
				tdata[0] = 0x20; //addr
				tdata[1] = 0xFC; //addr indirect
				tdata[2] = 0xFC; //addr indirect x
				tdata[3] = 0x22; //long
				tdata[4] = 0xFC; //long indirect

				tval = mnemonic[1].value;

				if(tval < 0x10000)
					immz = 1;
				else
					immz = 2;
			}

			if(mnemonic.size() == 2)
			{
				if(instw == "jml")
				{
					format = -3;
					this->filebin.push_back(0x5C);

					this->filebin.push_back(mnemonic[1].value>>0);
					this->filebin.push_back(mnemonic[1].value>>8);
					this->filebin.push_back(mnemonic[1].value>>16);
				}

				if(instw == "jsl")
				{
					format = -3;
					this->filebin.push_back(0x22);

					this->filebin.push_back(mnemonic[1].value>>0);
					this->filebin.push_back(mnemonic[1].value>>8);
					this->filebin.push_back(mnemonic[1].value>>16);
				}

				if(instw == "pea")
				{
					format = -2;
					this->filebin.push_back(0xF4);

					this->filebin.push_back(mnemonic[1].value>>0);
					this->filebin.push_back(mnemonic[1].value>>8);
				}

				if(instw == "pei")
				{
					format = -2;
					this->filebin.push_back(0xD4);

					this->filebin.push_back(mnemonic[1].value>>0);
					this->filebin.push_back(mnemonic[1].value>>8);
				}

				if(instw == "per")
				{
					format = -2;
					this->filebin.push_back(0x62);

					this->filebin.push_back(mnemonic[1].value>>0);
					this->filebin.push_back(mnemonic[1].value>>8);
				}

				if(instw == "trb")
				{
					format = -1;

					if(immz == 0)
					{
						this->filebin.push_back(0x1C);
						this->filebin.push_back(mnemonic[1].value>>0);
					}else
					{
						this->filebin.push_back(0x14);
						this->filebin.push_back(mnemonic[1].value>>0);
						this->filebin.push_back(mnemonic[1].value>>8);
					}
				}

				if(instw == "tsb")
				{
					format = -1;
					if(immz == 0)
					{
						this->filebin.push_back(0x0C);
						this->filebin.push_back(mnemonic[1].value>>0);
					}else
					{
						this->filebin.push_back(0x04);
						this->filebin.push_back(mnemonic[1].value>>0);
						this->filebin.push_back(mnemonic[1].value>>8);
					}
				}

				if(instw == "wdm")
				{
					format = -1;

					this->filebin.push_back(0x42);
					this->filebin.push_back(mnemonic[1].value);
				}

				if(instw == "rep")
				{
					format = -1;

					this->filebin.push_back(0xC2);
					this->filebin.push_back(mnemonic[1].value);
				}


				if(instw == "sep")
				{
					format = -1;

					this->filebin.push_back(0xE2);
					this->filebin.push_back(mnemonic[1].value);
				}

				if(instw == "bcc")
				{
					format = -1;

					this->filebin.push_back(0x90);
					this->filebin.push_back(mnemonic[1].value);
				}

				if(instw == "bcs")
				{
					format = -1;

					this->filebin.push_back(0xB0);
					this->filebin.push_back(mnemonic[1].value);
				}

				if(instw == "bne")
				{
					format = -1;

					this->filebin.push_back(0xD0);
					this->filebin.push_back(mnemonic[1].value);
				}

				if(instw == "beq")
				{
					format = -1;

					this->filebin.push_back(0xF0);
					this->filebin.push_back(mnemonic[1].value);
				}

				if(instw == "bpl")
				{
					format = -1;

					this->filebin.push_back(0x10);
					this->filebin.push_back(mnemonic[1].value);
				}

				if(instw == "bmi")
				{
					format = -1;

					this->filebin.push_back(0x30);
					this->filebin.push_back(mnemonic[1].value);
				}

				if(instw == "bvc")
				{
					format = -1;

					this->filebin.push_back(0x50);
					this->filebin.push_back(mnemonic[1].value);
				}

				if(instw == "bvs")
				{
					format = -1;

					this->filebin.push_back(0x70);
					this->filebin.push_back(mnemonic[1].value);
				}

				if(instw == "bra")
				{
					format = -1;

					this->filebin.push_back(0x80);
					this->filebin.push_back(mnemonic[1].value);
				}

				if(instw == "brl")
				{
					format = -2;

					this->filebin.push_back(0x82);
					this->filebin.push_back(mnemonic[1].value);
					this->filebin.push_back(mnemonic[1].value>>8);
				}


				if(instw == "brk")
				{
					format = -1;

					this->filebin.push_back(0x00);
					this->filebin.push_back(mnemonic[1].value);
				}

				if(instw == "cop")
				{
					format = -1;

					this->filebin.push_back(0x02);
					this->filebin.push_back(mnemonic[1].value);
				}

				//continue;
			}

			if(mnemonic.size() == 3)
			{
				if(instw == "mvn")
				{
					format = -2;
					this->filebin.push_back(0x54);

					this->filebin.push_back(mnemonic[1].value);
					this->filebin.push_back(mnemonic[2].value);
				}

				if(instw == "mvp")
				{
					format = -2;
					this->filebin.push_back(0x44);

					this->filebin.push_back(mnemonic[1].value);
					this->filebin.push_back(mnemonic[2].value);
				}

				//continue;
			}


			//-----
			if(format < 0)
			{
				int fi = format;
				int tmpnumber = this->filebin[filebin.size()-1+fi]&0xFF;
				if(this->debug == true)
				{

					std::cout <<  " 0x" << mnemonic[1].value;
					std::cout  << "  (f:0x"<< std::hex << tmpnumber << ") \n";
				}

				this->cycle += icycle[tmpnumber&0xFF];


				continue;

			}




			//-------



			if(mnemonic.size() == 2)
			{
				if(this->debug == true)
				std::cout <<  " 0x" << mnemonic[1].value;

				if(format == 0)
				{
					if(imm == true)
					{
						this->filebin.push_back(tdata[0]);
					}
					else
					{
						if(mnemonic[1].token1 == '(')
							this->filebin.push_back(tdata[1]);
						else
						if(mnemonic[1].token1 == '[')
							this->filebin.push_back(tdata[2]);
						else
						{
							if(immz == 0)
								this->filebin.push_back(tdata[3]);

							if(immz == 1)
								this->filebin.push_back(tdata[4]);

							if(immz == 2)
								this->filebin.push_back(tdata[5]);
						}
					}
				}

				if(format == 1)
				{
					if(imm == true)
					{
						this->filebin.push_back(tdata[0]);
					}
					else
					{
						if(immz == 0)
							this->filebin.push_back(tdata[1]);

						if(immz == 1)
							this->filebin.push_back(tdata[2]);
					}
				}

				if(format == 2)
				{
					if(mnemonic[1].token1 == '(')
						this->filebin.push_back(tdata[1]);
					else
					if(mnemonic[1].token1 == '[')
						this->filebin.push_back(tdata[4]);
					else
					if(immz == 1)
						this->filebin.push_back(tdata[0]);
					else
						this->filebin.push_back(tdata[3]);

				}
			}

			if(mnemonic.size() == 4)
			{
				this->filebin.push_back(tdata[14]);
				continue;
			}

			if(mnemonic.size() == 3)
			{
				if(this->debug == true)
				std::cout <<  " 0x" << mnemonic[1].value <<  " " << mnemonic[2].item;

				bool indexX = false;

				if(mnemonic[2].item == "X")
					indexX = true;

				if(format == 0)
				{
					if(mnemonic[1].token1 == '(')
					{
						if(indexX == false)
							this->filebin.push_back(tdata[11]);
						else
							this->filebin.push_back(tdata[10]);
					}
					else
					if(mnemonic[1].token1 == '[')
					{
						this->filebin.push_back(tdata[12]);
					}else
					if(mnemonic[1].item == "sr")
					{
						this->filebin.push_back(tdata[13]);
					}
					else
					{
						if(immz == 0)
						{
							if(indexX == false)
							{
								immz = 1;
							}
							else
							{
								this->filebin.push_back(tdata[9]);
							}

						}

						if(immz == 1)
						{
							if(indexX == false)
								this->filebin.push_back(tdata[8]);
							else
								this->filebin.push_back(tdata[6]);
						}

						if(immz == 2)
							this->filebin.push_back(tdata[7]);
					}

				}

				if(format == 1)
				{
					if(immz == 0)
						this->filebin.push_back(tdata[4]);
					else
						this->filebin.push_back(tdata[3]);
				}

				if(format == 2)
				{
					this->filebin.push_back(tdata[2]);
				}
			}

			int tmpnumber = this->filebin[filebin.size()-1]&0xFF;
			if(this->debug == true)
			std::cout  << "  (0x"<< std::hex << tmpnumber << ") " << immz;

			this->cycle += icycle[tmpnumber&0xFF];

			this->filebin.push_back(mnemonic[1].value);
			if(immz == 1)
			{
				this->filebin.push_back(mnemonic[1].value>>8);
			}
			if(immz == 2)
			{
				this->filebin.push_back(mnemonic[1].value>>8);
				this->filebin.push_back(mnemonic[1].value>>16);
			}
			if(this->debug == true)
			std::cout << "\n";
		}

	}

	int n = this->filebin.size();

	//std::cout << n << "\n";



	uint16_t checksum1 = 0x1FE,checksum2;

	for(int i = 0;i < n;i++)
		checksum1 += (uint8_t)this->filebin[i];

	checksum2 = -(checksum1+1);

	this->filebin[0x7FDC] = checksum2;
	this->filebin[0x7FDD] = checksum2>>8;
	this->filebin[0x7FDE] = checksum1;
	this->filebin[0x7FDF] = checksum1>>8;


	if(this->bmesen == true)
	{
		std::ofstream outfile(this->filename);

		if (!outfile.is_open())
		{
			std::cerr << "Error write file: .mlb" << std::endl;
			return;
		}

		outfile << this->mesen;

		for (const auto& pair : this->gvariable)
		{
			EAGLE_VARIABLE var = pair.second ;
			if ( ( (var.address >= 0x100) && (var.address < 0x2000) ) || (var.address > 0x7E0200) )
			{
				std::string tstrf = pair.first;

				for (char &c : tstrf) {
					if (c == '.')
						c = '_';
				}

				char shex[32];

				if(var.nsize < 2)
					sprintf(shex,"%x",(int)var.address&0x1FFFF);
				else
					sprintf(shex,"%x-%x",(int)var.address&0x1FFFF,(int)(var.address+var.nsize-1)&0x1FFFF);

				for(int i = 0;i < 32;i++)
				{
					if( (shex[i] >= 'a') && (shex[i] <= 'z') )
					{
						shex[i] -= 32;
					}
				}

				outfile << "SnesWorkRam:" << shex << ":"<< tstrf << std::endl;
			}
		}

		outfile << "SnesWorkRam:00-7F:SPM" << std::endl;
		outfile << "SnesWorkRam:80-D0:LIB" << std::endl;
		outfile << "SnesWorkRam:E0-EF:FUNCLIB" << std::endl;
		outfile << "SnesWorkRam:F0-FF:FUNCSPM" << std::endl;

		outfile.close();
	}

	//std::cout << checksum1 << " " << checksum2 << "\n";

	std::cout << "func used :"<< func_address << "\n";
	std::cout << "wram used :"<< wram_address << "\n";
}
