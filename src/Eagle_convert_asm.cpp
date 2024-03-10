#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include <cstdint>
#include <cstring>
#include <string>

#include <stdint.h>

#include "Eagle.hpp"

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
		 result = std::stoi(str, 0, 16);
		 dresult = result;

	}else
	if( (str[0] == '0') && (str[1] == 'b') )
	{
		str.substr(2);
		result = std::stoi(str, 0, 2);
		dresult = result;
	}else
	{
		dresult = std::stod(str);
		result = std::stoll(str);
	}
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

bool Eagle::variable_exist(EAGLE_WORDS tword,EAGLE_VARIABLE &var,int elabel)
{
	var = this->variable[tword.item];
	if(var.type != EAGLE_keywords::UNKNOW)
	{
		var.token1 = tword.token1;
		var.token2 = tword.token2;
		return true;
	}else
	{
		var = this->gvariable[tword.item];
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

			if(tword.type == TYPE_CONSTANT)
			{
				var.bimm = true;
				var.blabel = false;
				var.bptr = false;

				Constant_folding cfold;
				cfold.evaluate(tword.item);
				var.immediate = cfold.result;
				var.dimmediate = cfold.fresult;

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
				var.ptr_type = tword.ktype;

				if(tword.pn == 0)
					return false;

				if(tword.ptype[0] == TYPE_NUMBER)
				{
					convertStringToNumber(tword.ptr[0],var.ptr1.value,var.dimmediate);
					var.ptr1.bimm = true;
				}else
				{
					var.ptr1.bimm = false;

					EAGLE_VARIABLE tvar = this->variable[tword.ptr[0]];
					if(tvar.type != EAGLE_keywords::UNKNOW)
					{
						var.ptr1.value = tvar.address;
						var.ptr1.type = tvar.type;
					}else
					{
						EAGLE_VARIABLE tvar = this->gvariable[tword.ptr[0]];
						if(tvar.type != EAGLE_keywords::UNKNOW)
						{
							var.ptr1.value = tvar.address;
							var.ptr1.type = tvar.type;
						}else
						{
							return false;
						}
					}

				}

				if(tword.pn > 1)
				{
					if(tword.ptype[1] == TYPE_NUMBER)
					{
						convertStringToNumber(tword.ptr[1],var.ptr2.value,var.dimmediate);
						var.ptr2.bimm = true;
					}else
					{
						var.ptr2.bimm = false;

						EAGLE_VARIABLE tvar = this->variable[tword.ptr[1]];
						if(tvar.type != EAGLE_keywords::UNKNOW)
						{
							var.ptr2.value = tvar.address;
							var.ptr2.type = tvar.type;
						}else
						{
							EAGLE_VARIABLE tvar = this->gvariable[tword.ptr[1]];
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
					convertStringToNumber(tword.ptr[2],var.immediate,var.dimmediate);

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

	int n = this->instructions.size();
	for(int i = 0;i < n;i++)
	{
		int n2 = this->instructions[i].size();
		tword = this->instructions[i][0];
		EAGLE_keywords inst = this->keywords[tword.item];

		if(tword.type == TYPE_NUMBER)
		{
			out_error(tword,"the number is not instruction ");
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
						this->text_code += this->instructions[i][2].item + " "   + this->instructions[i][3].item+ "\n";
						tmp = this->instructions[i][2].item;
						operator1 = tmp[0];
						operator2 = tmp[1];
						bvar = variable_exist(this->instructions[i][3],var2,2);
						this->label2 = this->instructions[i][3].item;
					}else
					{
						operator1 = '=';
						operator2 = '=';
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

							this->label1 = this->instructions[i][1].item;

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
						std::cout << this->instructions[i][0].item << " " << this->instructions[i][1].item << "\n";
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

								if( (variable_exist(this->instructions[i][0],var1,0) == true) &&
									(variable_exist(this->instructions[i][2],var2,1) == true)
								)
								{
									this->label1 = this->instructions[i][2].item;
									this->label2 = this->instructions[i][2].item;
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

								if( (variable_exist(this->instructions[i][0],var1,0) == true) &&
									(variable_exist(this->instructions[i][2],var2,1) == true) &&
									(variable_exist(this->instructions[i][4],var3,2) == true)
								)
								{
									this->label1 = this->instructions[i][2].item;
									this->label2 = this->instructions[i][4].item;

									if(operator1a == '*')
										this->asm_mul(var1,var2,var3);
									else
									if(operator1a == '/')
										this->asm_div(var1,var2,var3);
									else
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

					this->label1 = this->instructions[i][1].item;

					if(variable_exist(this->instructions[i][1],var1,1) == false)
						out_error(this->instructions[i][1],"variable not exist ! ");

					EAGLE_DFUNC tdfunc = this->dfunc[this->instructions[i][1].item];

					this->asm_call_jump(var1,tdfunc,0,0);
				}else
					out_error(tword," not the right number of arguments in ");
			break;


			case EAGLE_keywords::CALL:
				if( (n2 > 2) && (n2 < 10) )
				{
					//comment asm
					this->text_code += ";" + this->instructions[i][0].item+ " " + this->instructions[i][1].item + "\n";
					this->label1 = this->instructions[i][1].item;

					if(variable_exist(this->instructions[i][1],var1,1) == false)
						out_error(this->instructions[i][1],"variable not exist ! ");


					EAGLE_DFUNC tdfunc = this->dfunc[this->instructions[i][1].item];

					int j = 0;

					for(int l = 2;l < n2;l++)
					{
						if(j < 8)
						{
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


					this->asm_call_jump(var1,tdfunc,n2-2,1);
				}else
					out_error(tword," not the right number of arguments in ");

			break;

			case EAGLE_keywords::DFUNC:
				if(n2 >= 3)
				{
					//comment asm
					this->text_code += ";" + this->instructions[i][0].item+ " " + this->instructions[i][1].item + " "+ this->instructions[i][2].item + "\n";
					EAGLE_DFUNC tdfunc;

					int ba = 2;

					if(this->instructions[i][2].item == ".spm")
						ba = 3;

					if(this->instructions[i][2].item == ".lib")
						ba = 3;

					int j = 0,derror = 0;
					for(int l = ba;l < n2;l++)
					{
						if(j < 8)
						{

							tdfunc.type[j] = this->keywords[this->instructions[i][l].item];
						}
						else
						{
							out_error(tword," max 8 arguments in ");
							derror = 1;
							break;
						}
						j++;
					}

					if(derror == 0)
					{
						this->dfunc[this->instructions[i][1].item] = tdfunc;
					}

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

					this->text_code.insert(l-1,"\n;else\nbra .label_"+std::to_string(this->ilabel) );

					int scope = (tword.scope)&0x7F;
					this->scope_label[scope] = this->ilabel;
					this->ilabel++;
				}


				break;


			case EAGLE_keywords::END:

				if(tword.type == TYPE_END)
				{
					if(tword.scope == 0)
					{
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

				if(n2 > 19)
				{
					out_error(tword,"too many arguments in ");
				}
				if(n2 < 3)
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
						this->mode_alloc = ALLOC_FSPM;

					if(inst == EAGLE_keywords::FUNCLIB)
						this->mode_alloc = ALLOC_FLIB;

					func.clear();

					if(inst == EAGLE_keywords::PROC)
					{
						this->mode_alloc = ALLOC_FSPM;
						func.retauto = false;
					}

					tmp = this->instructions[i][1].item;
					ktype = this->keywords[tmp];

					func.type = ktype;
					func.address = this->func_address;

					//type function
					if( (ktype >= EAGLE_keywords::UINT8) && (ktype <= EAGLE_keywords::VOID) )
					{
						tmp = this->instructions[i][2].item;
						ktype = this->keywords[tmp];
						text_code += tmp + ":\n";
						func.name = tmp;

						if(this->label[tmp] == false)
						{
							this->label[tmp] = true;
						}else
						{
							out_error(this->instructions[i][2],"function exist ! ");
						}

						//Function Name
						if( (ktype == EAGLE_keywords::UNKNOW) && (this->instructions[i][2].type == TYPE_LABEL) )
						{
							int j = 0;

							for(int l = 3;l < n2;l+=2)
							{
								if( (l+2) > n2)
								{
									out_error(tword,"number argument incorrect ");

								}else
								{
									tmp = this->instructions[i][l].item;
									EAGLE_keywords ktype2 = this->keywords[tmp];
									if( (ktype2 >= EAGLE_keywords::UINT8) && (ktype2 <= EAGLE_keywords::FLOAT64) )
									{
										//func.arg_type[j] = ktype;
									}else
									{
										out_error(tword,"Wait a variable type ");
									}

									tmp = this->instructions[i][l+1].item;
									ktype = this->keywords[tmp];

									if( (ktype == EAGLE_keywords::UNKNOW) && (this->instructions[i][l+1].type == TYPE_WORD) )
									{
										bool exist = false;
										tmp2 = func.name + "." + tmp;

										var = this->variable[tmp];

										if(var.type != EAGLE_keywords::UNKNOW)
											exist = true;

										var = this->gvariable[tmp2];
										if(var.type != EAGLE_keywords::UNKNOW)
											exist = true;

										if(exist == false)
										{
											var.bimm = false;
											var.blabel = false;
											var.bptr = false;

											var.type = ktype2;
											var.address = alloc(ktype2,1);
											this->variable[tmp] = var;
											this->gvariable[tmp2] = var;

											tmp2 = func.name + "..arg" + std::to_string(j);
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

						}else
						{
							out_error(tword,"Wait a name function ");
						}

					}else
					{
						out_error(tword,"Wait for a variable type ");
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
						convertNumber(tmp);

						if(this->instructions[i][1].type != TYPE_NUMBER)
							out_error(instructions[i][1],"the argument is not a number ");
						else
						if(inst == EAGLE_keywords::CODE)
						{
							tmp = this->instructions[i][1].item;
							if( (tmp[0] == '0') && (tmp[1] == 'x') )
							{
								 tmp.substr(2);
								 this->mmap = std::stoi(tmp, nullptr, 16);
							}
							else
								this->mmap = std::stoi(tmp);

							std::cout << this->mmap << "\n";
							text_code += ".code " + tmp + "\n";
						}

					}




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

					if(n2 == 2)
					{
						int address = 0;
						tmp = this->instructions[i][1].item;
						if( (tmp[0] == '0') && (tmp[1] == 'x') )
						{
							 tmp.substr(2);
							 address = std::stoi(tmp, nullptr, 16);
						}
						else
							address = std::stoi(tmp);

						if(inst == EAGLE_keywords::FUNCMAP)
							this->func_address = address;

						if(inst == EAGLE_keywords::SPMMAP)
							this->spm_address = address;

						if(inst == EAGLE_keywords::FUNCMAPSPM)
							this->funcspm_address = address;

						if(inst == EAGLE_keywords::FUNCMAPLIB)
							this->funclib_address = address;

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
							if(this->label[this->instructions[i][l].item] == true)
							{
								text_code += this->instructions[i][l].item + ",";
							}else
							{
								out_error(instructions[i][l],"the argument is not a number ");
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


				case EAGLE_keywords::DSTRUCT :
					if(n2 >= 4)
					{
						EAGLE_DSTRUCT dstc = this->dstruct[this->instructions[i][1].item];
						if(dstc.exist == false)
						{
							dstc.exist = true;

							for(int l = 2;l < n2;l+=2)
							{
								EAGLE_keywords type1 = this->keywords[this->instructions[i][l+0].item];
								EAGLE_keywords type2 = this->keywords[this->instructions[i][l+1].item];
								tword = this->instructions[i][l+1];

								EAGLE_STRUCT tstc;

								if( (type1 >= EAGLE_keywords::INT8) && (type1 < EAGLE_keywords::VOID) )
								{
									tstc.type = type1;
								}else
								{
									out_error(instructions[i][l+0],"type incorrect ");
								}

								if( (type2 == EAGLE_keywords::UNKNOW) && (tword.type == TYPE_WORD) )
								{
									tstc.name = tword.item;
								}else
								{
									out_error(instructions[i][l+1],"name variable incorrect ");
								}

								tstc.nsize = 1;

								if( (l+2) < n2)
								{
									tword = this->instructions[i][l+2];
									if(tword.type == TYPE_NUMBER)
									{
										tstc.nsize = std::stoi(tword.item)&0xFFFFFFF;
										l++;
									}
								}

								dstc.stc.push_back(tstc);

							}

						}else
							out_error(tword,"struct exist ");
					}else
						out_error(tword," not the right number of arguments in ");
				break;

			default:
				int mode = ALLOC_UNK;
				this->mode_alloc =0;

				if(inst == EAGLE_keywords::SPM)
					mode = ALLOC_SPM;
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


				this->mode_alloc = mode;

				if(mode != ALLOC_UNK)
				{
					int ib = 0;
					if( (mode == ALLOC_SPM) || (mode == ALLOC_REGISTER) || (mode == ALLOC_STACK))
						ib = 1;

					bool exist = false;

					if(n2 > 1+ib)
					{
						inst = this->keywords[instructions[i][0+ib].item];
						if( (inst >= EAGLE_keywords::INT8) && (inst < EAGLE_keywords::VOID) )
						{

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
							tmp = tword.item;
							tmp2 = func.name + "." + tmp;

							var = this->variable[tmp];

							if(var.type != EAGLE_keywords::UNKNOW)
								exist = true;

							var = this->gvariable[tmp2];
							if(var.type != EAGLE_keywords::UNKNOW)
								exist = true;

							if(exist == false)
							{
								int n = 1;
								if( (l+1) < n2)
								{
									tword = this->instructions[i][l+1];
									if(tword.type == TYPE_NUMBER)
									{
										n = std::stoi(tword.item)&0xFFFFFFF;
										l++;
									}

									if(tword.type == TYPE_CONSTANT)
									{
										Constant_folding cfold;
										cfold.evaluate(tword.item);
										n = cfold.result&0xFFFFFFF;
										l++;
									}
								}

								var1.address = alloc(var1.type,n);
								if(tword.scope != 0)
								{
									this->variable[tmp] = var1;
									if(mode == ALLOC_FRAM)
									{
										this->gvariable[tmp2] = var1;
									}
								}
								else
								{
									this->gvariable[tmp] = var1;
								}



							}else
								out_error(instructions[i][l],"variable exist ! ");
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

	this->write_file("code.asm",text_code);
}

