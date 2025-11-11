


#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include <cstring>
#include <string>

#include <stdint.h>

#include "Eagle.hpp"

static int z80_arg(const std::string &arg)
{
	if(arg == "a")  return 0x01;
	if(arg == "b")  return 0x02;
	if(arg == "c")  return 0x03;
	if(arg == "d")  return 0x04;
	if(arg == "e")  return 0x05;
	if(arg == "h")  return 0x06;
	if(arg == "l")  return 0x07;
	if(arg == "bc") return 0x08;
	if(arg == "de") return 0x09;
	if(arg == "hl") return 0x0A;
	if(arg == "IX") return 0x0B;
	if(arg == "IY") return 0x0C;
	if(arg == "af") return 0x0D;
	if(arg == "sp") return 0x0E;
	if(arg == "f")  return 0x0F;

	// conditions
	if(arg == "C")   return 0x10;
	if(arg == "M")   return 0x11;
	if(arg == "NC")  return 0x12;
	if(arg == "NZ")  return 0x13;
	if(arg == "P")   return 0x14;
	if(arg == "PE")  return 0x15;
	if(arg == "PO")  return 0x16;
	if(arg == "Z")   return 0x17;

	if(arg == "I")  return 0x20;
	if(arg == "R")  return 0x21;

	return 0;
}


void Eagle::bin_z80()
{
	std::string instw;
	std::string arg1,arg2,arg3;
	//pass 1
	this->offset = 0;
	this->idf = 0;
	mnemonic.clear();

	int rlabel[0x100], irl = 0;

	while(this->line_code_asm(0) != 0)
	{
		if(mnemonic.size() > 0)
		{
			if(mnemonic[0].token2 == ':')
			{
				this->labelbin[mnemonic[0].item] = this->offset;
				continue;
			}

			instw = mnemonic[0].item;

			if(mnemonic.size() == 1)
			{
				if(instw == "ccf")
				{
					this->offset++;
				}

				if(instw == "ei")
				{
					this->offset++;
				}

				if(instw == "di")
				{
					this->offset++;
				}

				if(instw == "ret")
				{
					this->offset++;
				}

				if(instw == "reti")
				{
					this->offset++;
					this->offset++;
				}

				if(instw == "retn")
				{
					this->offset++;
					this->offset++;
				}

				if(instw == "nop")
				{
					this->offset++;
				}

				if(instw == "outd")
				{
					this->offset++;
					this->offset++;
				}

				if(instw == "outi")
				{
					this->offset++;
					this->offset++;
				}

				if(instw == "cpd")
				{
					this->offset++;
					this->offset++;
				}

				if(instw == "cpdr")
				{
					this->offset++;
					this->offset++;
				}

				if(instw == "cpi")
				{
					this->offset++;
					this->offset++;
				}

				if(instw == "cpir")
				{
					this->offset++;
					this->offset++;
				}

				if(instw == "cpl")
				{
					this->offset++;
				}

				if(instw == "daa")
				{
					this->offset++;
				}

				if(instw == "exx")
				{
					this->offset++;
				}

				if(instw == "halt")
				{
					this->offset++;
				}

				if(instw == "im0")
				{
					this->offset++;
					this->offset++;
				}

				if(instw == "im1")
				{
					this->offset++;
					this->offset++;
				}

				if(instw == "im2")
				{
					this->offset++;
					this->offset++;
				}

				if(instw == "ind")
				{
					this->offset++;
					this->offset++;
				}

				if(instw == "indr")
				{
					this->offset++;
					this->offset++;
				}

				if(instw == "ini")
				{
					this->offset++;
					this->offset++;
				}

				if(instw == "inir")
				{
					this->offset++;
					this->offset++;
				}

				if(instw == "ldd")
				{
					this->offset++;
					this->offset++;
				}

				if(instw == "lddr")
				{
					this->offset++;
					this->offset++;
				}

				if(instw == "ldi")
				{
					this->offset++;
					this->offset++;
				}

				if(instw == "ldir")
				{
					this->offset++;
					this->offset++;
				}

				if(instw == "neg")
				{
					this->offset++;
					this->offset++;
				}

				if(instw == "otdr")
				{
					this->offset++;
					this->offset++;
				}

				if(instw == "otir")
				{
					this->offset++;
					this->offset++;
				}

				if(instw == "rla")
				{
					this->offset++;
				}

				if(instw == "rlca")
				{
					this->offset++;
				}

				if(instw == "rld")
				{
					this->offset++;
					this->offset++;
				}

				if(instw == "rrca")
				{
					this->offset++;
				}

				if(instw == "rrd")
				{
					this->offset++;
					this->offset++;
				}

				if(instw == "scf")
				{
					this->offset++;
				}


			}else
			{
				arg1 = mnemonic[1].item;

				if(mnemonic.size() > 2)
					arg2 = mnemonic[2].item;
				else
					arg2 = "";

				if(mnemonic.size() > 3)
					arg3 = mnemonic[3].item;
				else
					arg3 = "";

				int arg = z80_arg(arg1);
				int argx = z80_arg(arg2);
				int argo = z80_arg(arg3);

				//std::cout << instw << " " << arg1 << " " << arg2 << "\n";
				if(instw == "ld")
				{
					if(mnemonic[1].token1 == '(')
					{
						//BC
						if(arg == 8)
							this->offset++;

						//DE
						if(arg == 9)
							this->offset++;

						//HL
						if(arg == 10)
						{
							if(argx == 0)
								this->offset+=2;
							else
								this->offset++;
						}

						//IX / IY
						if( (arg == 11) || (arg == 12) )
						{
							if(argo == 0)
								this->offset+=4;
							else
								this->offset+=3;
						}

						//NN
						if(arg == 0)
						{
							if( (argx == 1) || (argx == 10) )
								this->offset+=3;
							else
								this->offset+=4;
						}

					}
					else
					{
						//A
						if(arg == 1)
						{
							if(mnemonic[2].token1 == '(')
							{
								if(argx == 0)
									this->offset+=3;
								else if(argx == 11)
									this->offset+=3;
								else if(argx == 12)
									this->offset+=3;
								else
									this->offset+=1;
							}
							else
							{
								//IX / IY
								if( (argx == 11) || (argx == 12) )
								{
									this->offset+=2;
								}
								else
								{
									if(argx == 0x20)
										this->offset+=2;
									else if(argx == 0x21)
										this->offset+=2;
									else if(argx == 0)
										this->offset+=2;
									else
										this->offset+=1;
								}
							}
						}


						//B / C / D / E / H / L
						if( (arg >= 2) && (arg <= 7) )
						{
							if(mnemonic[2].token1 == '(')
							{
								if(argx == 11)
									this->offset+=3;
								else if(argx == 12)
									this->offset+=3;
								else
									this->offset+=1;
							}
							else
							{
								//IX / IY
								if( (argx == 11) || (argx == 12) )
								{
									this->offset+=2;
								}
								else
								{
									if(argx == 0)
										this->offset+=2;
									else
										this->offset+=1;
								}
							}
						}

						//IX / IY
						if( (arg == 11) || (arg == 12) )
						{
							if(mnemonic[2].token1 == '(')
							{
								this->offset+=4;
							}
							else
							{
								if(argx == 0)
									this->offset+=4;
								else
									this->offset+=2;
							}
						}

						//BC /DE
						if( (arg == 8) || (arg == 9) )
						{
							if(mnemonic[2].token1 == '(')
							{
								this->offset+=4;
							}
							else
							{
								this->offset+=3;
							}
						}

						//HL
						if(arg == 10)
						{
							this->offset+=3;
						}

						//SP
						if(arg == 0xE)
						{

							if(argx == 0)
								this->offset+=3;
							else if(argx == 10)
								this->offset+=1;
							else
								this->offset+=2;

						}

						if(arg == 0x20)
							this->offset+=2;

						if(arg == 0x21)
							this->offset+=2;

					}

				}


				if((instw == "adc") || (instw == "sbc") )
				{
					//A
					if(arg == 1)
					{
						if(mnemonic[2].token1 == '(')
						{
							if( (argx == 11) || (argx == 12) )
								this->offset+=3;
							else
								this->offset+=1;
						}
						else
						{
							if( (argx == 11) || (argx == 12) || (argx == 0) )
								this->offset+=2;
							else
								this->offset+=1;

						}
					}else
					{
						this->offset+=2;
					}
				}

				if(instw == "add")
				{
					//A
					if(arg == 1)
					{
						if(mnemonic[2].token1 == '(')
						{
							if( (argx == 11) || (argx == 12) )
								this->offset+=3;
							else
								this->offset+=1;
						}
						else
						{
							if( (argx == 11) || (argx == 12) || (argx == 0) )
								this->offset+=2;
							else
								this->offset+=1;

						}
					}
					else
					{
						if( (argx == 11) || (argx == 12) )
								this->offset+=2;
							else
								this->offset+=1;
					}
				}

				if( (instw == "sra") || (instw == "srl") || (instw == "set") || (instw == "sla") )
				{
					if( (arg == 11) || (arg == 12) )
						this->offset+=4;
					else
						this->offset+=2;
				}

				if(instw == "bit")
				{
					if( (argx == 11) || (argx == 12) )
						this->offset+=4;
					else
						this->offset+=2;
				}

				if(instw == "call")
				{
					this->offset+=3;
				}

				if((instw == "cp") || (instw == "sub") || (instw == "and") || (instw == "or") || (instw == "xor") )
				{
					if(mnemonic[1].token1 == '(')
					{
						if( (arg == 11) || (arg == 12) )
							this->offset+=3;
						else
							this->offset+=1;
					}
					else
					{
						if( (arg == 11) || (arg == 12) || (arg == 0) )
							this->offset+=2;
						else
							this->offset+=1;
					}
				}

				if(instw == "dec")
				{
					if( (arg == 11) || (arg == 12) )
						this->offset+=2;
					else
						this->offset+=1;
				}

				if(instw == "djnz")
				{
					rlabel[irl] = this->offset;
					irl++;
					irl &= 0xFF;
					this->offset+=2;
				}

				if(instw == "ex")
				{
					if( (argx == 11) || (argx == 12) )
						this->offset+=2;
					else
						this->offset+=1;
				}

				if(instw == "in")
				{
					this->offset+=2;
				}

				if(instw == "out")
				{
					this->offset+=2;
				}

				if(instw == "inc")
				{
					if( (arg == 11) || (arg == 12) )
						this->offset+=2;
					else
						this->offset+=1;
				}

				if(instw == "jp")
				{
					if(arg == 10)
						this->offset+=1;
					else
						this->offset+=3;
				}

				if(instw == "jr")
				{
					this->offset+=2;
				}


				if(instw == "pop")
				{
					if( (arg == 11) || (arg == 12) )
						this->offset+=2;
					else
						this->offset+=1;
				}

				if(instw == "push")
				{
					if( (arg == 11) || (arg == 12) )
						this->offset+=2;
					else
						this->offset+=1;
				}

				if(instw == "res")
				{
					if( (argx == 11) || (argx == 12) )
						this->offset+=4;
					else
						this->offset+=2;
				}

				if(instw == "ret")
				{
					this->offset+=1;
				}


				if( (instw == "rl") || (instw == "rlc") || (instw == "rr") || (instw == "rrc"))
				{
					if( (arg == 11) || (arg == 12) )
						this->offset+=4;
					else
						this->offset+=2;
				}

				if(instw == "rst")
				{
					this->offset+=1;
				}

			}
		}

	}


	if(this->offset>>10)
	{
		std::cout << (this->offset>>10) << " kiB";
	}else
	{
		std::cout << (this->offset) << " Bytes";
	}

	std::cout <<  std::hex <<"\n";

	//pass 2

	this->offset = 0;
	this->idf = 0;
	mnemonic.clear();
	this->offset = 0;
	irl = 0;

	while(this->line_code_asm(1) != 0)
	{
		if(mnemonic.size() > 0)
		{
			if(mnemonic[0].token2 == ':')
				continue;

			instw = mnemonic[0].item;

			if(mnemonic.size() == 1)
			{
				if(instw == "ccf")
				{
					this->filebin.push_back(0x3F);
				}

				if(instw == "ei")
				{
					this->filebin.push_back(0xFB);
				}


				if(instw == "di")
				{
					this->filebin.push_back(0xF3);
				}

				if(instw == "ret")
				{
					this->filebin.push_back(0xC9);
				}

				if(instw == "reti")
				{
					this->filebin.push_back(0xED);
					this->filebin.push_back(0x4D);
				}

				if(instw == "retn")
				{
					this->filebin.push_back(0xED);
					this->filebin.push_back(0x45);
				}

				if(instw == "nop")
				{
					this->filebin.push_back(0x00);
				}

				if(instw == "outd")
				{
					this->filebin.push_back(0xED);
					this->filebin.push_back(0xAB);
				}


				if(instw == "outi")
				{
					this->filebin.push_back(0xED);
					this->filebin.push_back(0xA3);
				}

				if(instw == "cpd")
				{
					this->filebin.push_back(0xED);
					this->filebin.push_back(0xA9);
				}

				if(instw == "cpdr")
				{
					this->filebin.push_back(0xED);
					this->filebin.push_back(0xB9);
				}

				if(instw == "cpi")
				{
					this->filebin.push_back(0xED);
					this->filebin.push_back(0xA1);
				}

				if(instw == "cpir")
				{
					this->filebin.push_back(0xED);
					this->filebin.push_back(0xB1);
				}
				if(instw == "cpl")
				{
					this->filebin.push_back(0x2F);
				}
				if(instw == "daa")
				{
					this->filebin.push_back(0x27);
				}
				if(instw == "exx")
				{
					this->filebin.push_back(0xD9);
				}
				if(instw == "halt")
				{
					this->filebin.push_back(0x76);
				}
				if(instw == "im0")
				{
					this->filebin.push_back(0xED);
					this->filebin.push_back(0x46);
				}
				if(instw == "im1")
				{
					this->filebin.push_back(0xED);
					this->filebin.push_back(0x56);
				}
				if(instw == "im2")
				{
					this->filebin.push_back(0xED);
					this->filebin.push_back(0x5E);
				}
				if(instw == "ind")
				{
					this->filebin.push_back(0xED);
					this->filebin.push_back(0xAA);
				}
				if(instw == "indr")
				{
					this->filebin.push_back(0xED);
					this->filebin.push_back(0xBA);
				}
				if(instw == "ini")
				{
					this->filebin.push_back(0xED);
					this->filebin.push_back(0xA2);
				}
				if(instw == "inir")
				{
					this->filebin.push_back(0xED);
					this->filebin.push_back(0xB2);
				}
				if(instw == "ldd")
				{
					this->filebin.push_back(0xED);
					this->filebin.push_back(0xA8);
				}
				if(instw == "lddr")
				{
					this->filebin.push_back(0xED);
					this->filebin.push_back(0xB8);
				}
				if(instw == "ldi")
				{
					this->filebin.push_back(0xED);
					this->filebin.push_back(0xA0);
				}
				if(instw == "ldir")
				{
					this->filebin.push_back(0xED);
					this->filebin.push_back(0xB0);
				}
				if(instw == "neg")
				{
					this->filebin.push_back(0xED);
					this->filebin.push_back(0x44);
				}
				if(instw == "otdr")
				{
					this->filebin.push_back(0xED);
					this->filebin.push_back(0xBB);
				}
				if(instw == "otir")
				{
					this->filebin.push_back(0xED);
					this->filebin.push_back(0xB3);
				}
				if(instw == "rla")
				{
					this->filebin.push_back(0x17);
				}
				if(instw == "rlca")
				{
					this->filebin.push_back(0x07);
				}
				if(instw == "rld")
				{
					this->filebin.push_back(0xED);
					this->filebin.push_back(0x6F);
				}
				if(instw == "rrca")
				{
					this->filebin.push_back(0x0F);
				}
				if(instw == "rrd")
				{
					this->filebin.push_back(0xED);
					this->filebin.push_back(0x67);
				}
				if(instw == "scf")
				{
					this->filebin.push_back(0x37);
				}

			}else
			{
				int topcode = 0;
				int value1 = 0;
				int value2 = 0;
				int value3 = 0;
				int reg1 = 0;
				int reg2 = 0;
				int reg3 = 0;

				unsigned char tdata[16];

				arg1 = mnemonic[1].item;


				if(mnemonic[1].type == 1)
				{
					if(mnemonic[1].token2 == '@')
						value1 = this->gvariable[mnemonic[1].item].address;
					else
					{
						auto it = this->labelbin.find(mnemonic[1].item);

						if (it != this->labelbin.end())
							value1 = this->labelbin[mnemonic[1].item];
						else
						{
							if( (mnemonic[1].item.size() > 3) )
							{
								std::cout << "warning: label zero :" <<  mnemonic[1].item <<"\n";
							}
						}
					}


				}else
				{
					value1 = mnemonic[1].value;
				}



				if(mnemonic.size() > 2)
				{
					if(mnemonic[2].type == 1)
					{
						if(mnemonic[1].token2 == '@')
							value2 = this->gvariable[mnemonic[2].item].address;
						else
						{
							auto it = this->labelbin.find(mnemonic[2].item);

							if (it != this->labelbin.end())
								value2 = this->labelbin[mnemonic[2].item];
							else
							{
								if( (mnemonic[2].item.size() > 3) )
								{
									std::cout << "warning: label zero :" <<  mnemonic[2].item <<"\n";
								}
							}
						}

					}else
					{
						value2 = mnemonic[2].value;
					}
					arg2 = mnemonic[2].item;
				}
				else
					arg2 = "";

				if(mnemonic.size() > 3)
				{
					value3 = mnemonic[3].value;
					arg3 = mnemonic[3].item;
				}
				else
					arg3 = "";

				int arg = z80_arg(arg1);
				int argx = z80_arg(arg2);

				if(arg == 1)
					reg1 = 7;
				else
					reg1 = arg -2;


				if(argx == 1)
					reg2 = 7;
				else
					reg2 = argx -2;


				//--------------------------------------------------------------------------
				if(instw == "ld")
				{
					if(mnemonic[1].token1 == '(')
					{
						//BC
						if(arg == 8)
						{
							this->filebin.push_back(0x02);
						}

						//DE
						if(arg == 9)
						{
							this->filebin.push_back(0x12);
						}

						//HL
						if(arg == 10)
						{
							if(argx == 0)
							{
								this->filebin.push_back(0x36);
								this->filebin.push_back(value2&0xFF);
							}
							else
							{
								this->filebin.push_back(0x70+reg2);
							}
						}

						//IX / IY
						if( (arg == 11) || (arg == 12) )
						{
							if(argx == 11)
								this->filebin.push_back(0xDD);
							else
								this->filebin.push_back(0xFD);

							int argo = z80_arg(arg3);

							if(argo == 1)
								reg3 = 7;
							else
								reg3 = argo -2;


							if(argo == 0)
							{
								this->filebin.push_back(0x36);
								this->filebin.push_back(value2&0xFF);
								this->filebin.push_back(value3&0xFF);
							}
							else
							{
								this->filebin.push_back(0x70+reg3);
								this->filebin.push_back(value2&0xFF);
							}
						}

						//NN
						if(arg == 0)
						{
							if(argx == 1)
							{
								this->filebin.push_back(0x32);
								this->filebin.push_back(value1&0xFF);
								this->filebin.push_back(value1>>8);
							}

							if(argx == 8)
							{
								this->filebin.push_back(0xED);
								this->filebin.push_back(0x43);
								this->filebin.push_back(value1&0xFF);
								this->filebin.push_back(value1>>8);
							}

							if(argx == 9)
							{
								this->filebin.push_back(0xED);
								this->filebin.push_back(0x53);
								this->filebin.push_back(value1&0xFF);
								this->filebin.push_back(value1>>8);
							}

							if(argx == 10)
							{
								this->filebin.push_back(0x22);
								this->filebin.push_back(value1&0xFF);
								this->filebin.push_back(value1>>8);
							}

							if(argx == 11)
							{
								this->filebin.push_back(0xDD);
								this->filebin.push_back(0x22);
								this->filebin.push_back(value1&0xFF);
								this->filebin.push_back(value1>>8);
							}

							if(argx == 12)
							{
								this->filebin.push_back(0xFD);
								this->filebin.push_back(0x22);
								this->filebin.push_back(value1&0xFF);
								this->filebin.push_back(value1>>8);
							}

							if(argx == 14)
							{
								this->filebin.push_back(0xED);
								this->filebin.push_back(0x73);
								this->filebin.push_back(value1&0xFF);
								this->filebin.push_back(value1>>8);
							}
						}

					}
					else
					{
						//A
						if(arg == 1)
						{
							if(mnemonic[2].token1 == '(')
							{
								if(argx == 0)
								{
									this->filebin.push_back(0x3A);
									this->filebin.push_back(value2&0xFF);
									this->filebin.push_back(value2>>8);
								}

								if(argx == 8)
								{
									this->filebin.push_back(0x0A);
								}

								if(argx == 9)
								{
									this->filebin.push_back(0x1A);
								}

								if(argx == 10)
								{
									this->filebin.push_back(0x7E);
								}

								if(argx == 11)
								{
									this->filebin.push_back(0xDD);
									this->filebin.push_back(0x7E);
									this->filebin.push_back(value3&0xFF);
								}

								if(argx == 12)
								{
									this->filebin.push_back(0xFD);
									this->filebin.push_back(0x7E);
									this->filebin.push_back(value3&0xFF);
								}

							}
							else
							{
								//IX
								if(argx == 11)
								{
									this->filebin.push_back(0xDD);
									this->filebin.push_back(0x78+5);
								}

								if(argx == 12)
								{
									this->filebin.push_back(0xFD);
									this->filebin.push_back(0x78+5);
								}

								if(argx == 0x20)
								{
									this->filebin.push_back(0xED);
									this->filebin.push_back(0x57);
								}

								if(argx == 0x21)
								{
									this->filebin.push_back(0xED);
									this->filebin.push_back(0x5F);
								}

								if(argx == 0)
								{
									this->filebin.push_back(0x3E);
									this->filebin.push_back(value2&0xFF);
								}

								if( (argx >= 1) && (argx <= 7) )
								{
									this->filebin.push_back(0x78+reg2);
								}
							}
						}

						int tld = 0;
						unsigned char tldata[4];


						if(arg == 2)
						{
							tld = 1;
							tldata[0] = 0x46;
							tldata[1] = 0x06;
							tldata[2] = 0x40;
						}

						if(arg == 3)
						{
							tld = 1;
							tldata[0] = 0x4E;
							tldata[1] = 0x0E;
							tldata[2] = 0x48;
						}

						if(arg == 4)
						{
							tld = 1;
							tldata[0] = 0x56;
							tldata[1] = 0x16;
							tldata[2] = 0x50;
						}

						if(arg == 5)
						{
							tld = 1;
							tldata[0] = 0x5E;
							tldata[1] = 0x1E;
							tldata[2] = 0x58;
						}

						if(arg == 6)
						{
							tld = 1;
							tldata[0] = 0x66;
							tldata[1] = 0x26;
							tldata[2] = 0x60;
						}

						if(arg == 7)
						{
							tld = 1;
							tldata[0] = 0x6E;
							tldata[1] = 0x2E;
							tldata[2] = 0x68;
						}

						if(arg == 8)
						{
							if(mnemonic[2].token1 == '(')
							{
								this->filebin.push_back(0xED);
								this->filebin.push_back(0x48);
							}
							else
							{
								this->filebin.push_back(0x01);
							}

							this->filebin.push_back(value2);
							this->filebin.push_back(value2>>8);
						}

						if(arg == 9)
						{
							if(mnemonic[2].token1 == '(')
							{
								this->filebin.push_back(0xED);
								this->filebin.push_back(0x5B);
							}
							else
							{
								this->filebin.push_back(0x11);
							}

							this->filebin.push_back(value2);
							this->filebin.push_back(value2>>8);
						}

						if(arg == 10)
						{
							if(mnemonic[2].token1 == '(')
							{
								this->filebin.push_back(0x2A);
							}
							else
							{
								this->filebin.push_back(0x21);
							}

							this->filebin.push_back(value2);
							this->filebin.push_back(value2>>8);
						}

						if( (arg == 11) || (arg == 12) )
						{
							if(arg == 11)
								this->filebin.push_back(0xDD);
							else
								this->filebin.push_back(0xFD);

							if(argx == 0)
							{
								if(mnemonic[2].token1 == '(')
									this->filebin.push_back(0x2A);
								else
									this->filebin.push_back(0x21);

								this->filebin.push_back(value2);
								this->filebin.push_back(value2>>8);
							}
							else
							{
								this->filebin.push_back(0x68+reg2);
							}
						}

						if(arg == 0x0E)
						{
							if(argx == 0)
							{
								if(mnemonic[2].token1 == '(')
								{
									this->filebin.push_back(0xED);
									this->filebin.push_back(0x7B);
								}
								else
									this->filebin.push_back(0x31);

								this->filebin.push_back(value2);
								this->filebin.push_back(value2>>8);
							}
							else
							{
								if(argx == 11)
									this->filebin.push_back(0xDD);

								if(argx == 12)
									this->filebin.push_back(0xFD);

								this->filebin.push_back(0xF9);
							}
						}


						if(arg == 0x20)
						{
							this->filebin.push_back(0xED);
							this->filebin.push_back(0x47);
						}

						if(arg == 0x21)
						{
							this->filebin.push_back(0xED);
							this->filebin.push_back(0x4F);
						}




						if(tld == 1)
						{
							if(mnemonic[2].token1 == '(')
							{
								if(argx == 11)
								{
									this->filebin.push_back(0xDD);
									this->filebin.push_back(tldata[0]);
									this->filebin.push_back(value3);
								}
								else if(argx == 12)
								{
									this->filebin.push_back(0xFD);
									this->filebin.push_back(tldata[0]);
									this->filebin.push_back(value3);
								}
								else
								{
									this->filebin.push_back(tldata[0]);
								}
							}
							else
							{
								//IX / IY
								if( (argx == 11) || (argx == 12) )
								{
									if(argx == 11)
										this->filebin.push_back(0xDD);
									else
										this->filebin.push_back(0xFD);

									this->filebin.push_back(tldata[2]+5);
								}
								else
								{
									if(argx == 0)
									{
										this->filebin.push_back(tldata[1]);
										this->filebin.push_back(value2);
									}
									else
									{
										this->filebin.push_back(tldata[2]+reg2);
									}
								}
							}
						}

					}

				}

				//--------------------------------------------------------------------------

				if( (instw == "adc") || (instw == "sbc") )
				{
					int dif1 = 0,dif2 = 0;
					if(instw == "sbc")
					{
						dif1 = 0x10;
						dif2 = 0x08;
					}

					//A
					if(arg == 1)
					{
						if(mnemonic[2].token1 == '(')
						{
							if( (argx == 11) || (argx == 12) )
							{
								if(argx == 11)
									this->filebin.push_back(0xDD);
								else
									this->filebin.push_back(0xFD);

								this->filebin.push_back(0x8E + dif1);

								this->filebin.push_back(value3);

							}
							else
							{
								this->filebin.push_back(0x8E + dif1);
							}
						}
						else
						{

							if(argx == 11)
							{
								this->filebin.push_back(0xDD);
								this->filebin.push_back(0x8C + dif1);
							}
							else
							if(argx == 12)
							{
								this->filebin.push_back(0xFD);
								this->filebin.push_back(0x8C + dif1);
							}
							else
							if(argx == 0)
							{
								this->filebin.push_back(0xCE + dif1);
								this->filebin.push_back(value2);
							}
							else
							{
								this->filebin.push_back(0x88+reg2 + dif1);
							}

						}
					}else
					{
						this->filebin.push_back(0xED);

						if(argx == 8)
							this->filebin.push_back(0x4A - dif2);

						if(argx == 9)
							this->filebin.push_back(0x5A - dif2);

						if(argx == 10)
							this->filebin.push_back(0x6A - dif2);

						if(argx == 15)
							this->filebin.push_back(0x7A - dif2);

					}
				}

				if(instw == "add")
				{
					//A
					if(arg == 1)
					{
						if(mnemonic[2].token1 == '(')
						{
							if( (argx == 11) || (argx == 12) )
							{
								if(argx == 11)
									this->filebin.push_back(0xDD);
								else
									this->filebin.push_back(0xFD);

								this->filebin.push_back(0x86);

								this->filebin.push_back(value3);

							}
							else
							{
								this->filebin.push_back(0x86);
							}
						}
						else
						{
							if(argx == 11)
							{
								this->filebin.push_back(0xDD);
								this->filebin.push_back(0x84);
							}
							else
							if(argx == 12)
							{
								this->filebin.push_back(0xFD);
								this->filebin.push_back(0x84);
							}
							else
							if(argx == 0)
							{
								this->filebin.push_back(0xC6);
								this->filebin.push_back(value2);
							}
							else
							{
								this->filebin.push_back(0x80+reg2);
							}
						}
					}
					else
					{
						//HL
						if(arg == 10)
						{
							if(argx == 8)
								this->filebin.push_back(0x09);

							if(argx == 9)
								this->filebin.push_back(0x19);

							if(argx == 10)
								this->filebin.push_back(0x29);

							if(argx == 15)
								this->filebin.push_back(0x39);
						}

					}
				}


				if(instw == "and")
				{
					topcode = 1;
					tdata[0] = 0xA6;
					tdata[1] = 0xE6;
					tdata[2] = 0xA0;
				}


				if(instw == "bit")
				{
					if(mnemonic[2].token1 == '(')
					{
						if( (argx == 11) || (argx == 12) )
						{
							if(argx == 11)
								this->filebin.push_back(0xDD);
							else
								this->filebin.push_back(0xFD);

							this->filebin.push_back(0xCB);

							this->filebin.push_back(value3);

							this->filebin.push_back(0x46+ (value1*8));

						}
						else
						{
							this->filebin.push_back(0xCB);
							this->filebin.push_back(0x46+ (value1*8));
						}
					}
					else
					{
						this->filebin.push_back(0xCB);
						this->filebin.push_back(0x40+reg2+ (value1*8));
					}
				}

				if(instw == "call")
				{
					if(arg == 0)
					{
						this->filebin.push_back(0xCD);
						this->filebin.push_back(value1&0xFF);
						this->filebin.push_back(value1>>8);
					}
					else
					{
						int tmp = arg&7;
						const unsigned char opcodez[] = {0xDC,0xFC,0xD4,0xC4,0xF4,0xEC,0xE4,0xCC};

						this->filebin.push_back(opcodez[tmp]);

						this->filebin.push_back(value2&0xFF);
						this->filebin.push_back(value2>>8);
					}
				}



				if(instw == "cp")
				{
					topcode = 1;
					tdata[0] = 0xBE;
					tdata[1] = 0xFE;
					tdata[2] = 0xB8;
				}


				if(instw == "dec")
				{
					if(mnemonic[1].token1 == '(')
					{
						if((arg == 11) || (arg == 12))
						{
							if(arg == 11)
								this->filebin.push_back(0xDD);
							else
								this->filebin.push_back(0xFD);

							this->filebin.push_back(0x35);

							this->filebin.push_back(value3);
						}
						else
						{
							this->filebin.push_back(0x35);
						}
					}
					else
					{
						int tmp = (arg-1)&0xF;
						const unsigned char opcodez[] = {0x3D,0x05,0x0D,0x15, 0x1D,0x25,0x2D,0x0B, 0x1B,0x2B,0xDD,0xFD, 0x3D,0x3B,0x00,0x00};

						this->filebin.push_back(opcodez[tmp]);

						if((arg == 11) || (arg == 12))
							this->filebin.push_back(0x2B);
					}
				}

				if(instw == "djnz")
				{
					this->filebin.push_back(0x10);

					int tmp = value1 - rlabel[irl] - 2;
					irl++;
					irl &= 0xFF;
					this->filebin.push_back(tmp&0xFF);
				}

				if(instw == "ex")
				{
					if(arg == 0x0D)
					{
						this->filebin.push_back(0x08);
					}else
					if(arg == 0x09)
					{
						this->filebin.push_back(0xEB);
					}
					else
					{
						if(arg == 11)
							this->filebin.push_back(0xDD);
						if(arg == 12)
							this->filebin.push_back(0xFD);


						this->filebin.push_back(0xE3);

					}
				}

				if(instw == "in")
				{
					int tmp = (arg-1)&0x7;
					const char opcodez[] = {0x78,0x40,0x48,0x50, 0x58,0x60,0x68};

					if(argx == 0)
					{
						this->filebin.push_back(0xDB);
						this->filebin.push_back(value2&0xFF);
					}else
					{
						if(arg == 15)
						{
							this->filebin.push_back(0xED);
							this->filebin.push_back(0x70);
						}
						else
						{
							this->filebin.push_back(0xED);
							this->filebin.push_back(opcodez[tmp]);
						}

					}
				}

				if(instw == "out")
				{
					int tmp = (arg-1)&0x7;
					const char opcodez[] = {0x79,0x41,0x49,0x51, 0x59,0x61,0x69};

					if(arg == 0)
					{
						this->filebin.push_back(0xD3);
						this->filebin.push_back(value1&0xFF);
					}else
					{
						this->filebin.push_back(0xED);
						this->filebin.push_back(opcodez[tmp]);
					}
				}


				if(instw == "inc")
				{
					if(mnemonic[1].token1 == '(')
					{
						if((arg == 11) || (arg == 12))
						{
							if(arg == 11)
								this->filebin.push_back(0xDD);
							else
								this->filebin.push_back(0xFD);

							this->filebin.push_back(0x34);

							this->filebin.push_back(value3);
						}
						else
						{
							this->filebin.push_back(0x34);
						}
					}
					else
					{
						int tmp = (arg-1)&0xF;
						const unsigned char opcodez[] = {0x3C,0x04,0x0C,0x14, 0x1C,0x24,0x2C,0x03, 0x13,0x23,0xDD,0xFD, 0x3C,0x33,0x00,0x00};

						this->filebin.push_back(opcodez[tmp]);

						if((arg == 11) || (arg == 12))
							this->filebin.push_back(0x2B);
					}
				}

				if(instw == "jp")
				{
					if(arg == 0)
					{
						this->filebin.push_back(0xC3);
						this->filebin.push_back(value1&0xFF);
						this->filebin.push_back(value1>>8);
					}
					else
					{
						if(mnemonic[1].token1 == '(')
						{
							if(arg == 11)
								this->filebin.push_back(0xDD);
							if(arg == 12)
								this->filebin.push_back(0xFD);


							this->filebin.push_back(0xE9);
						}else
						{
							int tmp = arg&7;
							const unsigned char opcodez[] = {0xDA,0xFA,0xD2,0xC2,0xF2,0xEA,0xE2,0xCA};
							this->filebin.push_back(opcodez[tmp]);

							this->filebin.push_back(value2&0xFF);
							this->filebin.push_back(value2>>8);
						}

					}
				}


				if(instw == "jr")
				{
					if(arg == 0)
					{
						this->filebin.push_back(0x18);
						this->filebin.push_back(value1&0xFF);
					}
					else
					{
						int tmp = arg&7;
						const char opcodez[] = {0x38,0x38,0x30,0x20,0x20,0x20,0x20,0x28};
						this->filebin.push_back(opcodez[tmp]);
						this->filebin.push_back(value1&0xFF);
					}
				}

				if(instw == "or")
				{
					topcode = 1;
					tdata[0] = 0xB6;
					tdata[1] = 0xF6;
					tdata[2] = 0xB0;
				}

				if(instw == "pop")
				{
					if( (arg >= 10) && (arg <= 12) )
					{
						if(arg == 11)
							this->filebin.push_back(0xDD);
						if(arg == 12)
							this->filebin.push_back(0xFD);


						this->filebin.push_back(0xE1);
					}

					if(arg == 8)
						this->filebin.push_back(0xC1);

					if(arg == 9)
						this->filebin.push_back(0xD1);

					if(arg == 0x0D)
						this->filebin.push_back(0xF1);

				}

				if(instw == "push")
				{
					if( (arg >= 10) && (arg <= 12) )
					{
						if(arg == 11)
							this->filebin.push_back(0xDD);
						if(arg == 12)
							this->filebin.push_back(0xFD);


						this->filebin.push_back(0xE5);
					}

					if(arg == 8)
						this->filebin.push_back(0xC5);

					if(arg == 9)
						this->filebin.push_back(0xD5);

					if(arg == 0x0D)
						this->filebin.push_back(0xF5);
				}



				if(instw == "ret")
				{
					int tmp = arg&7;
					const unsigned char opcodez[] = {0xD8,0xF8,0xD0,0xC0,0xF0,0xE8,0xE0,0xC8};

					this->filebin.push_back(opcodez[tmp]);
				}

				if(instw == "rl")
				{
					topcode = 2;
					tdata[0] = 0x16;
					tdata[1] = 0x10;
				}

				if(instw == "rlc")
				{
					topcode = 2;
					tdata[0] = 0x06;
					tdata[1] = 0x00;
				}

				if(instw == "rr")
				{
					topcode = 2;
					tdata[0] = 0x1E;
					tdata[1] = 0x18;
				}

				if(instw == "rrc")
				{
					topcode = 2;
					tdata[0] = 0x0E;
					tdata[1] = 0x08;
				}

				if(instw == "rst")
				{
					int tmp = 0xC7;
					if(value1&1)
						tmp += 8;

					tmp += (value1>>1)*0x10;

					this->filebin.push_back(tmp);
				}

				if(instw == "sbc")
				{
					topcode = 1;
					tdata[0] = 0x9E;
					tdata[1] = 0xDE;
					tdata[2] = 0x98;
				}

				if(instw == "set")
				{
					topcode = 2;
					tdata[0] = 0xC6;
					tdata[1] = 0xC0;
				}

				if(instw == "sla")
				{
					topcode = 2;
					tdata[0] = 0x26;
					tdata[1] = 0x20;
				}

				if(instw == "sra")
				{
					topcode = 2;
					tdata[0] = 0x2E;
					tdata[1] = 0x28;
				}

				if(instw == "srl")
				{
					topcode = 2;
					tdata[0] = 0x3E;
					tdata[1] = 0x38;
				}

				if(instw == "sub")
				{
					topcode = 1;
					tdata[0] = 0x96;
					tdata[1] = 0xD6;
					tdata[2] = 0x90;
				}

				if(instw == "xor")
				{
					topcode = 1;
					tdata[0] = 0xAE;
					tdata[1] = 0xEE;
					tdata[2] = 0xA8;
				}


				if(topcode == 1)
				{
					if(mnemonic[1].token1 == '(')
					{
						if( (arg == 11) || (arg == 12) )
						{
							if(arg == 11)
								this->filebin.push_back(0xDD);
							else
								this->filebin.push_back(0xFD);

							this->filebin.push_back(tdata[0]);

							this->filebin.push_back(value3);

						}
						else
						{
							this->filebin.push_back(tdata[0]);
						}
					}
					else
					{
						if(arg == 11)
						{
							this->filebin.push_back(0xDD);
							this->filebin.push_back(tdata[2]+5);
						}
						else
						if(arg == 12)
						{
							this->filebin.push_back(0xFD);
							this->filebin.push_back(tdata[2]+5);
						}
						else
						if(arg == 0)
						{
							this->filebin.push_back(tdata[1]);
							this->filebin.push_back(value1&0xFF);
						}
						else
						{
							this->filebin.push_back(tdata[2]+reg1);
						}
					}
				}

				//-------------------

				if(topcode == 2)
				{
					if(mnemonic[1].token1 == '(')
					{
						if( (arg == 11) || (arg == 12) )
						{
							if(arg == 11)
								this->filebin.push_back(0xDD);
							else
								this->filebin.push_back(0xFD);

							this->filebin.push_back(0xCB);

							this->filebin.push_back(value3);

							this->filebin.push_back(tdata[0]);

						}
						else
						{
							this->filebin.push_back(0xCB);
							this->filebin.push_back(tdata[0]);
						}
					}
					else
					{
						this->filebin.push_back(0xCB);
						this->filebin.push_back(tdata[1]+reg1);

					}
				}

				//-------------------



				//--------------
			}
		}
	}

	std::cout << (irl) << "/0xFF : loop\n";
	std::cout << "func used :"<< func_address << "\n";
	std::cout << "wram used :"<< wram_address << "\n";
}

