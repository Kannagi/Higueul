#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include <cstdint>
#include <cstring>
#include <string>

#include <stdint.h>

#include "Eagle.hpp"


inline bool Eagle::gvariable_exist(std::string &tmp,EAGLE_VARIABLE &var)
{
	auto it = this->gvariable.find(tmp);

	if (it != this->gvariable.end())
	{
		var = this->gvariable[tmp];
		if(var.type != EAGLE_keywords::UNKNOW)
			return true;
	}else
	{
		var.type = EAGLE_keywords::UNKNOW;
	}

	return false;
}

inline bool Eagle::variable_exist(std::string &tmp,EAGLE_VARIABLE &var)
{
	auto it = this->variable.find(tmp);

	if (it != this->variable.end())
	{
		var = this->variable[tmp];
		if(var.type != EAGLE_keywords::UNKNOW)
			return true;
	}else
	{
		var.type = EAGLE_keywords::UNKNOW;
	}

	return false;
}

static void convertNumber(std::string &tmp)
{
	if( (tmp[0] == '0') && (tmp[1] == 'x') )
	{
		tmp[0] = ' ';
		tmp[1] = '$';
	}

	if( (tmp[0] == '0') && (tmp[1] == 'b') )
	{
		tmp[0] = ' ';
		tmp[1] = '%';
	}
}

void Eagle::out_asm()
{
	std::string tmp,tmp2;
	EAGLE_WORDS tword;
	EAGLE_VARIABLE var,var1,var2,var3;

	uint64_t number;
	float fnumber;
	double dnumber;
	EAGLE_keywords ktype;
	Eagle_func func;
	func.clear();
	func.name = "...";

	this->str_bra = "\n;else\nbra .label_";

	if(this->target == TARGET_6502)
		this->str_bra = "\n;else\njmp .label_";

	int n = this->instructions.size();
	for(int i = 0;i < n;i++)
	{
		int n2 = this->instructions[i].size();
		tword = this->instructions[i][0];
		EAGLE_keywords inst = this->keywords[tword.item];

		if( (tword.type == TYPE_NUMBER) || (tword.type == TYPE_CONSTANT) )
		{
			out_error(tword,"the number is not instruction ");
		}

		for(int l = 1;l < n2;l++)
		{
			if(this->instructions[i][l].type == TYPE_CONSTANT)
			{
				Constant_folding cfold;
				cfold.evaluate(this->instructions[i][l].item);

				this->instructions[i][l].item = std::to_string(cfold.result);
				this->instructions[i][l].type = TYPE_NUMBER;
			}

		}

		switch(inst)
		{
			case EAGLE_keywords::IF:
			case EAGLE_keywords::WHILE:
			case EAGLE_keywords::LOOP:

				if( (n2 == 2) || (n2 == 4) )
				{
					char operator1,operator2;
					bool bvar = true;
					//comment asm
					this->text_code += ";" + this->instructions[i][0].item + " " + this->instructions[i][1].item+ " " ;
					if(n2 == 4)
					{
						this->label2 = this->instructions[i][3].item;
						this->label1 = this->instructions[i][1].item;

						this->text_code += this->instructions[i][2].item + " "   + this->instructions[i][3].item+ "\n";
						tmp = this->instructions[i][2].item;
						operator1 = tmp[0];
						operator2 = tmp[1];
						bvar = variable_exist(this->instructions[i][3],var2,2);

					}else
					{
						operator1 = '=';
						operator2 = '=';
						this->label2 = this->instructions[i][1].item;
						this->label1 = this->instructions[i][1].item;
					}


					if(isOperator_cmp(operator1,operator2) == true)
					{
						if( (variable_exist(this->instructions[i][1],var1,1) == true) &&
							(bvar == true)
						)
						{
							int type = 0;
							if(inst == EAGLE_keywords::WHILE)
								type = 1;

							if(inst == EAGLE_keywords::IF)
							{
								type = 2;
								this->scope_label[tword.scope&0x7F] = this->ilabel;
							}

							if(n == 2)
								var2 = var1;

							this->asm_bru(var1,var2,operator1,operator2,type,this->scope_label[tword.scope&0x7F]);
						}else
						{

							if(variable_exist(this->instructions[i][1],var1,1) == false)
								out_error(this->instructions[i][1],"variable not exist ! ");

							if(bvar == false)
								out_error(this->instructions[i][3],"variable not exist ! ");
						}
					}else
					{
						out_error(tword,"not instruction compare ");
					}

				}else
				{
					out_error(tword,"not good number instruction ");
				}

			break;

			case EAGLE_keywords::UNKNOW:

				if(tword.label == true)
				{
					text_code += tword.item + ":\n";
				}else
				{
					if(n2 > 5)
					{
						out_error(tword,"too many instruction in ");
					}
					if(n2 < 3)
					{
						out_error(tword,"not enough instruction ");
					}else
					{
						tmp = this->instructions[i][1].item;
						char operator1 = tmp[0];
						char operator2 = tmp[1];

						//comment asm
						this->text_code += ";" + this->instructions[i][0].item+ " " + this->instructions[i][1].item;

						if(isOperator_move(operator1,operator2) == true)
						{
							if(n2 == 3)
							{
								//comment asm
								this->text_code += this->instructions[i][2].item+ "\n";

								this->label1 = this->instructions[i][2].item;
								this->label2 = this->instructions[i][2].item;

								if( (variable_exist(this->instructions[i][0],var1,1) == true) &&
									(variable_exist(this->instructions[i][2],var2,2) == true)
								)
								{
									this->asm_alu(var1,var1,var2,operator1,operator2);
								}else
								{
									if(variable_exist(this->instructions[i][0],var1,0) == false)
										out_error(this->instructions[i][0],"variable not exist ! ");

									if(variable_exist(this->instructions[i][2],var2,1) == false)
										out_error(this->instructions[i][2],"variable not exist ! ");
								}
							}

							if(n2 == 5)
							{
								//comment asm
								this->text_code += this->instructions[i][2].item + " " + this->instructions[i][3].item + this->instructions[i][4].item+ "\n";
								tmp = this->instructions[i][3].item;
								char operator1a = tmp[0];
								char operator2a = tmp[1];

								if( (isOperator_calcul(operator1a,operator2a) == false) || (operator1 != '=') )
								{
									out_error(this->instructions[i][3],"operator error ");
								}

								this->label1 = this->instructions[i][2].item;
								this->label2 = this->instructions[i][4].item;

								if( (variable_exist(this->instructions[i][0],var1,0) == true) &&
									(variable_exist(this->instructions[i][2],var2,1) == true) &&
									(variable_exist(this->instructions[i][4],var3,2) == true)
								)
								{


									this->asm_alu(var1,var2,var3,operator1a,operator2a);

								}else
								{
									if(variable_exist(this->instructions[i][0],var1,0) == false)
										out_error(this->instructions[i][0],"variable not exist ! ");

									if(variable_exist(this->instructions[i][2],var2,1) == false)
										out_error(this->instructions[i][2],"variable not exist ! ");

									if(variable_exist(this->instructions[i][4],var3,2) == false)
										out_error(this->instructions[i][4],"variable not exist ! ");
								}

							}

							if(n2 == 4)
								out_error(tword,"not enough instruction ");

						}else
						{
							out_error(tword,"wait instruction move (=) ");
						}
					}
				}

				break;

			case EAGLE_keywords::RETURN:
				if(n2 <= 2)
				{
					bool bret = true;
					if(n2 == 2)
					{
						bret = false;
						this->label1 = this->instructions[i][1].item;

						if(variable_exist(this->instructions[i][1],var1,1) == false)
							out_error(this->instructions[i][1],"variable not exist ! ");
					}

					this->asm_return(var1,bret);

					if(i+1 < n)
					{
						tword = this->instructions[i+1][0];
						EAGLE_keywords inst2 = this->keywords[tword.item];
						if( (inst2 == EAGLE_keywords::END) && (tword.scope == 0) )
						{
							func.retauto = false;
						}
					}
				}else
					out_error(tword," not the right number of arguments in ");
			break;

			case EAGLE_keywords::JUMP:
				if(n2 == 2)
				{
					//comment asm
					this->text_code += ";" + this->instructions[i][0].item+ " " + this->instructions[i][1].item + "\n";

					this->labelcall = this->instructions[i][1].item;

					if(variable_exist(this->instructions[i][1],var1,1) == false)
						out_error(this->instructions[i][1],"variable not exist ! ");

					this->asm_call_jump(var1,0,0);
				}else
					out_error(tword," not the right number of arguments in ");
			break;


			case EAGLE_keywords::CALL:
				if( (n2 > 1) && (n2 < 10) )
				{
					//comment asm
					this->text_code += ";" + this->instructions[i][0].item+ " " + this->instructions[i][1].item + "\n";
					this->labelcall = this->instructions[i][1].item;

					if(variable_exist(this->instructions[i][1],var1,1) == false)
						out_error(this->instructions[i][1],"variable not exist ! ");


					int j = 0;

					for(int l = 2;l < n2;l++)
					{
						if(j < 8)
						{
							this->labelarg[j] = this->instructions[i][l].item;
							if(variable_exist(this->instructions[i][l],var2,1) == true)
								this->arg[j] = var2;
							else
								out_error(this->instructions[i][l],"variable not exist ! ");
						}
						else
						{
							out_error(tword," max 8 arguments in ");
							j = n2;
						}
						j++;
					}


					this->asm_call_jump(var1,n2-2,1);
				}else
					out_error(tword," not the right number of arguments in ");

			break;

			case EAGLE_keywords::DO:


				if(tword.scope != 0)
				{
					int scope = (tword.scope&0x7F);
					this->scope_label[scope] = this->ilabel;

					this->text_code += ".label_b"+std::to_string(this->scope_label[tword.scope&0x7F]) +":\n";
					this->ilabel++;

					this->asm_do_else();
				}


				break;

			case EAGLE_keywords::ELSE:


				if(tword.scope != 0)
				{
					int l = this->text_code.size() - 1;

					while(l > 0)
					{
						if(this->text_code[l] == '.')
							break;
						l--;
					}

					this->text_code.insert(l-1,this->str_bra+std::to_string(this->ilabel) );

					int scope = (tword.scope)&0x7F;
					this->scope_label[scope] = this->ilabel;
					this->ilabel++;

					this->asm_do_else();
				}


				break;

			case EAGLE_keywords::END:

				if(tword.type == TYPE_END)
				{
					if(tword.scope == 0)
					{
						this->lib_address = this->lib_init;
						this->spm_address = this->spm_init;
						this->funcspm_address = this->funcspm_init;
						this->funclib_address = this->funclib_init;

						this->register_address = 0;
						this->stack_address = 0;

						if(func.retauto == true)
						{
							this->asm_return(var1,true);
						}
						func.clear();
						variable.clear();
						func.name = "...";
						this->text_code += "..end\n";
					}else
					{
						this->text_code += ".label_"+std::to_string(this->scope_label[tword.scope&0x7F]) +":\n";
					}
				}

				break;

			case EAGLE_keywords::PROC:
			case EAGLE_keywords::FUNCSPM:
			case EAGLE_keywords::FUNCLIB:
			case EAGLE_keywords::FUNC:

				if(n2 > 18)
				{
					out_error(tword,"too many arguments in ");
				}
				if(n2 < 2)
				{
					out_error(tword,"not enough argument ");
				}
				else if(tword.scope != 0)
				{
					out_error(tword,"the function must be in an initial scope ");
				}else
				{
					this->mode_alloc = ALLOC_FRAM;

					if(inst == EAGLE_keywords::FUNCSPM)
					{
						this->mode_alloc = ALLOC_FSPM;
					}

					if(inst == EAGLE_keywords::FUNCLIB)
					{
						this->mode_alloc = ALLOC_FLIB;
					}

					func.clear();

					if(inst == EAGLE_keywords::PROC)
					{
						this->mode_alloc = ALLOC_FSPM;
						func.retauto = false;
					}

					func.address = this->func_address;

					//type function
					{
						tmp = this->instructions[i][1].item;
						ktype = this->keywords[tmp];

						this->text_code += "..begin " + tmp +"\n";

						text_code += tmp + ":\n";
						func.name = tmp;

						if(this->label[tmp] == false)
						{
							this->label[tmp] = true;
						}else
						{
							out_error(this->instructions[i][1],"function exist ! ");
						}

						//Function Name
						if( (ktype == EAGLE_keywords::UNKNOW) && (this->instructions[i][1].type == TYPE_LABEL) )
						{
							int j = 0;



							for(int l = 2;l < n2;l+=2)
							{
								if( (l+2) > n2)
								{
									out_error(tword,"number argument incorrect ");

								}else
								{
									tmp = this->instructions[i][l].item;
									EAGLE_keywords ktype2 = this->keywords[tmp];
									if( (ktype2 >= EAGLE_keywords::UINT8) && (ktype2 <= EAGLE_keywords::FLOAT64) )
									{//
										//func.arg_type[j] = ktype;
									}else
									{
										out_error(tword,"Wait a variable type ");
									}

									tmp = this->instructions[i][l+1].item;
									tmp2 = func.name + "." + tmp;
									ktype = this->keywords[tmp];

									if( (ktype == EAGLE_keywords::UNKNOW) && (this->instructions[i][l+1].type == TYPE_WORD) )
									{
										bool exist = this->gvariable_exist(tmp,var);

										exist |= this->gvariable_exist(tmp2,var);

										if(exist == false)
										{
											var.bimm = false;
											var.blabel = false;
											var.bptr = false;

											var.type = ktype2;
											var.address = alloc(ktype2,1);
											var.nsize = this->varsize;
											this->variable[tmp] = var;

											this->gvariable[tmp2] = var;

											tmp2 = func.name + "..arg" + std::to_string(j);
											this->gvariable[tmp2] = var;

											var.address += 1;
											tmp2 = func.name + "..b16arg" + std::to_string(j);
											this->gvariable[tmp2] = var;

										}
										else
										{
											out_error(this->instructions[i][l+1],"already existing variable name ");
										}

									}else
									{
										out_error(tword,"Wait a name label ");
									}

									j++;

								}
							}

							if(n2 > 2)
							{
								if(this->mode_alloc == ALLOC_FRAM)
									this->func_address += 1;
							}

						}else
						{
							out_error(tword,"Wait a name function ");
						}

					}
				}


				break;

			case EAGLE_keywords::ASM:
			case EAGLE_keywords::INCBIN:
				if(n2 == 2)
				{
					if(this->instructions[i][1].type != TYPE_DATASTR)
						out_error(instructions[i][1],"the argument is not a string ");

					tmp = this->instructions[i][1].item;

					if(inst == EAGLE_keywords::ASM)
						text_code += ";asm\n " + tmp + "\n";
					else
						text_code += ".incbin " + tmp + "\n";
				}else
					out_error(tword," not the right number of arguments in ");

				break;

			case EAGLE_keywords::CODE:
			case EAGLE_keywords::RODATA:
			case EAGLE_keywords::ORG:
					tmp = "0";
					if(n2 > 2)
						out_error(tword,"too many arguments in ");
					if(n2 == 2)
					{
						tmp = this->instructions[i][1].item;
						std::string tmap = tmp;

						if(this->instructions[i][1].type != TYPE_NUMBER)
							out_error(instructions[i][1],"the argument is not a number ");
						else
						{
							convertNumber(tmp);
							if( (tmap[0] == '0') && (tmap[1] == 'x') )
							{
								this->mmap = std::stoll(tmap, nullptr, 16);
							}
							else
								this->mmap = std::stoll(tmap);
						}
					}

					if(inst == EAGLE_keywords::CODE)
						text_code += ".code " + tmp + "\n";

					if(inst == EAGLE_keywords::RODATA)
						text_code += ".rodata " + tmp + "\n";

					if(inst == EAGLE_keywords::ORG)
						text_code += ".org " + tmp + "\n";

				break;

			case EAGLE_keywords::BSS:
			case EAGLE_keywords::FUNCMAP:
			case EAGLE_keywords::FUNCMAPSPM:
			case EAGLE_keywords::FUNCMAPLIB:
			case EAGLE_keywords::SPMMAP:
			case EAGLE_keywords::LIBMAP:

					if(n2 == 2)
					{
						uint64_t address = 0;
						tmp = this->instructions[i][1].item;
						if( (tmp[0] == '0') && (tmp[1] == 'x') )
						{
							 address = std::stoll(tmp, nullptr, 16);
						}
						else
							address = std::stoll(tmp);

						if(inst == EAGLE_keywords::FUNCMAP)
							this->func_address = address;

						if(inst == EAGLE_keywords::SPMMAP)
						{
							this->spm_init = address;
							this->spm_address = address;
						}

						if(inst == EAGLE_keywords::LIBMAP)
						{
							this->lib_init = address;
							this->lib_address = address;
						}

						if(inst == EAGLE_keywords::FUNCMAPSPM)
						{
							this->funcspm_init = address;
							this->funcspm_address = address;
						}

						if(inst == EAGLE_keywords::FUNCMAPLIB)
						{
							this->funclib_init = address;
							this->funclib_address = address;
						}

						if(inst == EAGLE_keywords::BSS)
						{
							this->wram_address = address;
							convertNumber(tmp);
						}


						if(this->instructions[i][1].type != TYPE_NUMBER)
							out_error(instructions[i][1],"the argument is not a number ");
					}else
						out_error(tword," not the right number of arguments in ");

				break;
			case EAGLE_keywords::DATAB:
			case EAGLE_keywords::DATAW:
			case EAGLE_keywords::DATAL:
			case EAGLE_keywords::DATAQ:

						if(inst == EAGLE_keywords::DATAB)
							text_code += ".db ";

						if(inst == EAGLE_keywords::DATAW)
							text_code += ".dw ";

						if(inst == EAGLE_keywords::DATAL)
							text_code += ".dd ";

						if(inst == EAGLE_keywords::DATAQ)
							text_code += ".dq ";

						for(int l = 1;l < n2;l++)
						{
							if(this->instructions[i][l].type == TYPE_NUMBER)
							{
								tmp = this->instructions[i][l].item;
								convertNumber(tmp);

								text_code += tmp + ",";
							}
							else
							{
								char token = ' ';
								if(this->instructions[i][l].token1 != ':')
									token = '@';
								text_code += token + this->instructions[i][l].item + ",";
							}

						}
						text_code += "\n";
				break;
			case EAGLE_keywords::DATAS:
					text_code += ".db ";

					for(int l = 1;l < n2;l++)
					{
						if(this->instructions[i][l].type == TYPE_DATASTR)
						{
							tmp = this->instructions[i][l].item;
							int ns = tmp.size();
							for(int j = 0;j < ns;j++)
								text_code += std::to_string((int)tmp[j]) + ",";

							text_code += "0,";
						}else
							out_error(instructions[i][l],"the argument is not a string ");
					}
					text_code += "\n";

				break;
			case EAGLE_keywords::DATAH:
			case EAGLE_keywords::DATAF:

						if(inst == EAGLE_keywords::DATAF)
							text_code += ".dd ";
						else
							text_code += ".dw ";

						for(int l = 1;l < n2;l++)
						{

							if(this->instructions[i][l].type == TYPE_NUMBER)
							{
								tmp = this->instructions[i][l].item;
								fnumber =  std::stof(tmp);
								std::memcpy(&number, &fnumber, sizeof(float));

								if(inst == EAGLE_keywords::DATAF)
									number = (uint32_t)number;
								else
									number = (uint16_t)number;

								tmp = std::to_string(number);

								text_code += tmp + ",";
							}else
								out_error(instructions[i][l],"the argument is not a number ");

						}
						text_code += "\n";
				break;
			case EAGLE_keywords::DATAD:
						text_code += ".dq ";

						for(int l = 1;l < n2;l++)
						{

							if(this->instructions[i][l].type == TYPE_NUMBER)
							{
								tmp = this->instructions[i][l].item;
								dnumber =  std::stod(tmp);
								std::memcpy(&number, &dnumber, sizeof(double));
								tmp = std::to_string(number);

								text_code += tmp + ",";
							}else
								out_error(instructions[i][l],"the argument is not a number ");
						}
						text_code += "\n";
					break;
				break;

			default:
				int mode = ALLOC_UNK;
				bool spm_enable = false;
				this->mode_alloc =0;

				if(inst == EAGLE_keywords::LIB)
				{
					mode = ALLOC_LIB;
					spm_enable = true;
				}

				if(inst == EAGLE_keywords::SPM)
				{
					mode = ALLOC_SPM;
					spm_enable = true;
				}
				if(inst == EAGLE_keywords::REGISTER)
					mode = ALLOC_REGISTER;
				if(inst == EAGLE_keywords::STACK)
					mode = ALLOC_STACK;

				if( (inst >= EAGLE_keywords::INT8) && (inst < EAGLE_keywords::VOID) )
				{
					mode = ALLOC_FRAM;
					if(tword.scope == 0)
						mode = ALLOC_WRAM;
				}

				EAGLE_keywords type2 = inst;


				this->mode_alloc = mode;

				if(mode != ALLOC_UNK)
				{
					int ib = 0;
					if( (mode == ALLOC_LIB) ||  (mode == ALLOC_SPM) || (mode == ALLOC_REGISTER) || (mode == ALLOC_STACK))
						ib = 1;

					bool exist = false;

					if(n2 > 1+ib)
					{
						inst = this->keywords[instructions[i][0+ib].item];
						if( (inst >= EAGLE_keywords::INT8) && (inst < EAGLE_keywords::VOID) )
						{
							var1.type2 = type2;
							var1.type = inst;
							var1.bimm = false;
							var1.blabel = false;
							var1.bptr = false;
						}else
						{
							out_error(instructions[i][0+ib],"type unknow ");
						}
					}else
						out_error(instructions[i][0+ib],"incomplete variable declaration ");

					for(int l = ib+1;l < n2;l++)
					{
						tword = this->instructions[i][l];
						inst = this->keywords[instructions[i][l].item];

						if(inst == EAGLE_keywords::UNKNOW)
						{
							exist = false;
							int look = 0;
							tmp = tword.item;
							tmp2 = func.name + "." + tmp;

							exist = this->variable_exist(tmp,var);
							look = exist;
							exist |= this->gvariable_exist(tmp2,var);
							look += exist<<1;

							if(mode == ALLOC_WRAM)
							{
								exist |= this->gvariable_exist(tmp,var);
								look += exist<<2;
							}

							if(exist == false)
							{

								int n = 1;
								if( (l+1) < n2)
								{
									tword = this->instructions[i][l+1];
									if(tword.type == TYPE_NUMBER)
									{
										if( (tword.item[0] == '0') && (tword.item[1] == 'x') )
										{
											 n = std::stoi(tword.item, nullptr, 16);
										}
										else
											n = std::stoi(tword.item);

										n &= 0xFFFFFFF;
										l++;
									}

								}

								var1.address = alloc(var1.type,n);
								var1.nsize = this->varsize;
								if(tword.scope != 0)
								{
									this->variable[tmp] = var1;
									if(mode == ALLOC_FRAM)
									{
										//if (spm_enable == false)
										tmp2 = func.name + "." + tmp;
										//tmp2 = func.name + "." + this->instructions[i][1].item;

										this->gvariable[tmp2] = var1;



										//std::cout << tmp2  << std::hex << " " << var1.address  << "\n";
									}
								}
								else
								{
									if (spm_enable == false)
										this->gvariable[tmp] = var1;
									else
										out_error(instructions[i][l],"spm or lib don't variable global ! ");

									//std::cout << this->instructions[i][1].item  << std::hex << " " << var1.address  << "\n";
								}



							}else
							{
								out_error(instructions[i][l],"variable exist ! ");
							}

						}else
						{
							out_error(instructions[i][l],"variable is incorrect ");
						}
					}

					this->mode_alloc = ALLOC_FRAM;
				}
				break;
		}

	}

}

void Eagle::out_error(const EAGLE_WORDS tword,const std::string text)
{
	std::cout << "error line " << tword.line << ":" << tword.col << " : " << text << tword.item << "\n";
	this->error++;
}

void Eagle::convertStringToNumber(std::string str,int64_t &result,double &dresult)
{

	if( (str[0] == '0') && (str[1] == 'x') )
	{
		 str.substr(2);
		 result = std::stoll(str, 0, 16);
		 dresult = result;

	}else
	if( (str[0] == '0') && (str[1] == 'b') )
	{
		str.substr(2);
		result = std::stoll(str, 0, 2);
		dresult = result;
	}else
	{
		dresult = std::stod(str);
		result = std::stoll(str);
	}
}

bool Eagle::variable_exist(EAGLE_WORDS tword,EAGLE_VARIABLE &var,int elabel)
{
	EAGLE_VARIABLE tvar;

	this->variable_exist(tword.item,var);
	if(var.type != EAGLE_keywords::UNKNOW)
	{
		var.token1 = tword.token1;
		var.token2 = tword.token2;
		return true;
	}else
	{
		this->gvariable_exist(tword.item,var);
		if(var.type != EAGLE_keywords::UNKNOW)
		{
			var.token1 = tword.token1;
			var.token2 = tword.token2;
			return true;
		}
		else
		{
			if(tword.type == TYPE_NUMBER)
			{
				var.bimm = true;
				var.blabel = false;
				var.bptr = false;

				convertStringToNumber(tword.item,var.immediate,var.dimmediate);

				var.token1 = tword.token1;
				var.token2 = tword.token2;
				return true;
			}

			if(tword.type == TYPE_PTR)
			{
				var.bimm = false;
				var.blabel = false;
				var.bptr = true;

				var.immediate = 0;
				var.ptr_type = EAGLE_keywords::UINT8;

				if(tword.pn == 0)
					return false;

				if(elabel == 0)
					this->label0 = tword.ptr[0];

				if(elabel == 1)
					this->label1 = tword.ptr[0];

				if(elabel == 2)
					this->label2 = tword.ptr[0];

				var.ptr1.token1 = tword.ptoken1[0];
				var.ptr1.token2 = tword.ptoken2[0];

				var.ptr2.token1 = tword.ptoken1[1];
				var.ptr2.token2 = tword.ptoken2[1];

				if(tword.ptype[0] == TYPE_NUMBER)
				{
					convertStringToNumber(tword.ptr[0],var.ptr1.value,var.dimmediate);
					var.ptr1.bimm = true;
				}else
				{
					var.ptr1.bimm = false;

					this->variable_exist(tword.ptr[0],tvar);
					if(tvar.type != EAGLE_keywords::UNKNOW)
					{
						var.ptr1.value = tvar.address;
						var.ptr1.type = tvar.type;
						var.ptr_type = tvar.type;
					}else
					{
						this->gvariable_exist(tword.ptr[0],tvar);
						if(tvar.type != EAGLE_keywords::UNKNOW)
						{
							var.ptr1.value = tvar.address;
							var.ptr1.type = tvar.type;
							var.ptr_type = tvar.type;
						}else
						{
							if(var.ptr1.token2 != ':')
								return false;
						}
					}

				}

				var.ptr2_exist = false;
				if(tword.pn > 1)
				{
					var.ptr2_exist = true;
					if(tword.ptype[1] == TYPE_NUMBER)
					{
						convertStringToNumber(tword.ptr[1],var.ptr2.value,var.dimmediate);
						var.ptr2.bimm = true;
					}else
					{
						var.ptr2.bimm = false;


						this->variable_exist(tword.ptr[1],tvar);
						if(tvar.type != EAGLE_keywords::UNKNOW)
						{
							var.ptr2.value = tvar.address;
							var.ptr2.type = tvar.type;
						}else
						{
							this->gvariable_exist(tword.ptr[1],tvar);
							if(tvar.type != EAGLE_keywords::UNKNOW)
							{
								var.ptr2.value = tvar.address;
								var.ptr2.type = tvar.type;
							}else
							{
								return false;
							}
						}
					}
				}


				if(tword.pn > 2)
				{
					var.ptr_type = this->keywords[ tword.ptr[2] ];
				}

				if(tword.pn > 3)
					convertStringToNumber(tword.ptr[3],var.immediate,var.dimmediate);

				var.token1 = tword.token1;
				var.token2 = tword.token2;
				return true;
			}


			if( (tword.type == TYPE_LABEL) && (elabel > 0) )
			{
				var.bimm = false;
				var.blabel = true;

				var.token1 = tword.token1;
				var.token2 = tword.token2;
				return true;
			}
		}

	}

	return false;
}

