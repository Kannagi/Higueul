#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include <cstring>
#include <string>

#include <stdint.h>

#include "Eagle.hpp"

Eagle::Eagle()
{
	this->keywords["int8"]    = EAGLE_keywords::INT8;
	this->keywords["int16"]   = EAGLE_keywords::INT16;
	this->keywords["int32"]	  = EAGLE_keywords::INT32;
	this->keywords["int64"]	  = EAGLE_keywords::INT64;

	this->keywords["uint8"]   = EAGLE_keywords::UINT8;
	this->keywords["uint16"]  = EAGLE_keywords::UINT16;
	this->keywords["uint32"]  = EAGLE_keywords::UINT32;
	this->keywords["uint64"]  = EAGLE_keywords::UINT64;

	this->keywords["float16"] = EAGLE_keywords::FLOAT16;
	this->keywords["float32"] = EAGLE_keywords::FLOAT32;
	this->keywords["float64"] = EAGLE_keywords::FLOAT64;

	this->keywords["void"]    = EAGLE_keywords::VOID;

	this->keywords["register"] = EAGLE_keywords::REGISTER;
	this->keywords["spm"]      = EAGLE_keywords::SPM;
	this->keywords["lib"]      = EAGLE_keywords::LIB;
	this->keywords["stack"]    = EAGLE_keywords::STACK;

	this->keywords["if"]       = EAGLE_keywords::IF;
	this->keywords["else"]     = EAGLE_keywords::ELSE;
	this->keywords["do"]       = EAGLE_keywords::DO;
	this->keywords["while"]    = EAGLE_keywords::WHILE;
	this->keywords["jump"]     = EAGLE_keywords::JUMP;
	this->keywords["call"]     = EAGLE_keywords::CALL;
	this->keywords["return"]   = EAGLE_keywords::RETURN;
	this->keywords["loop"]     = EAGLE_keywords::LOOP;

	this->keywords["func"]     = EAGLE_keywords::FUNC;
	this->keywords["funcspm"]  = EAGLE_keywords::FUNCSPM;
	this->keywords["funclib"]  = EAGLE_keywords::FUNCLIB;
	this->keywords["proc"]     = EAGLE_keywords::PROC;
	this->keywords[".end"]     = EAGLE_keywords::END;

	this->keywords["asm"]      = EAGLE_keywords::ASM;

	this->keywords[".define"]      = EAGLE_keywords::DEFINE;
	this->keywords[".incbin"]      = EAGLE_keywords::INCBIN;
	this->keywords[".code"]        = EAGLE_keywords::CODE;
	this->keywords[".rodata"]      = EAGLE_keywords::RODATA;
	this->keywords[".bss"]         = EAGLE_keywords::BSS;
	this->keywords[".map.func"]    = EAGLE_keywords::FUNCMAP;
	this->keywords[".map.funcspm"] = EAGLE_keywords::FUNCMAPSPM;
	this->keywords[".map.funclib"] = EAGLE_keywords::FUNCMAPLIB;
	this->keywords[".map.spm"]     = EAGLE_keywords::SPMMAP;
	this->keywords[".map.lib"]     = EAGLE_keywords::LIBMAP;
	this->keywords[".endmacro"]    = EAGLE_keywords::ENDMACRO;
	this->keywords[".org"]         = EAGLE_keywords::ORG;

	this->keywords[".macro"]       = EAGLE_keywords::MACRO;


	this->keywords[".data.b"]	= EAGLE_keywords::DATAB;
	this->keywords[".data.w"]	= EAGLE_keywords::DATAW;
	this->keywords[".data.l"]	= EAGLE_keywords::DATAL;
	this->keywords[".data.q"]	= EAGLE_keywords::DATAQ;
	this->keywords[".data.s"]	= EAGLE_keywords::DATAS;
	this->keywords[".data.h"]	= EAGLE_keywords::DATAH;
	this->keywords[".data.f"]	= EAGLE_keywords::DATAF;
	this->keywords[".data.d"]	= EAGLE_keywords::DATAD;


	this->filetext.reserve(0x200000);
	this->text_code.reserve(0x200000);
	this->mnemonic.reserve(0x100);
	this->filebin.reserve(0x800000);

	this->col = 0;
	this->line = 1;
	this->error = 0;
	this->ilabel = 0;
	this->mode_alloc = ALLOC_WRAM;

	this->wram_address = 0;

	this->lib_address = 0;
	this->spm_address = 0;
	this->funcspm_address = 0;
	this->funclib_address = 0;

	this->lib_init = 0;
	this->spm_init = 0;
	this->funcspm_init = 0;
	this->funclib_init = 0;

	this->register_address = 0;
	this->stack_address = 0;
	this->idf = 0;
	this->sizebin = 0;
	this->mmap = 0;

	this->target = TARGET_65816;

	this->gvariable["acc"].type  = EAGLE_keywords::ACC;
	this->gvariable["bacc"].type = EAGLE_keywords::BACC;
	this->gvariable["facc"].type = EAGLE_keywords::FACC;
	this->gvariable["idx"].type  = EAGLE_keywords::IDX;
	this->gvariable["idy"].type  = EAGLE_keywords::IDY;

	this->kmacro[0] = ".arg1";
	this->kmacro[1] = ".arg2";
	this->kmacro[2] = ".arg3";
	this->kmacro[3] = ".arg4";

	this->kmacro[4] = ".arg5";
	this->kmacro[5] = ".arg6";
	this->kmacro[6] = ".arg7";
	this->kmacro[7] = ".arg8";

	this->debug = false;
	this->bout_asm = false;
	this->bcycle = false;
}

int Eagle::alloc(EAGLE_keywords type,int n)
{
	int value;
	switch(type)
	{
		case EAGLE_keywords::UINT8:
			value = n;
		break;

		case EAGLE_keywords::UINT16:
			value = 2*n;
		break;

		case EAGLE_keywords::UINT32:
			value = 4*n;
		break;

		case EAGLE_keywords::UINT64:
			value = 8*n;
		break;

		case EAGLE_keywords::INT8:
			value = n;
		break;

		case EAGLE_keywords::INT16:
			value = 2*n;
		break;

		case EAGLE_keywords::INT32:
			value = 4*n;
		break;

		case EAGLE_keywords::INT64:
			value = 8*n;
		break;

		case EAGLE_keywords::FLOAT16:
			value = 2*n;
		break;

		case EAGLE_keywords::FLOAT32:
			value = 4*n;
		break;

		case EAGLE_keywords::FLOAT64:
			value = 8*n;
		break;

		default:

			return 0;
			break;

	}

	int tmp = 0;

	if(this->mode_alloc == ALLOC_FRAM)
	{
		tmp = this->func_address;
		this->func_address += value;
	}

	if(this->mode_alloc == ALLOC_WRAM)
	{
		tmp = this->wram_address;
		this->wram_address += value;
	}

	if(this->mode_alloc == ALLOC_SPM)
	{
		tmp = this->spm_address;
		this->spm_address += value;
	}

	if(this->mode_alloc == ALLOC_LIB)
	{
		tmp = this->lib_address;
		this->lib_address += value;
	}

	if(this->mode_alloc == ALLOC_FSPM)
	{
		tmp = this->funcspm_address;
		this->funcspm_address += value;
	}

	if(this->mode_alloc == ALLOC_FLIB)
	{
		tmp = this->funclib_address;
		this->funclib_address += value;
	}

	if(this->mode_alloc == ALLOC_REGISTER)
	{
		tmp = this->register_address;
		this->register_address += 1;
	}

	if(this->mode_alloc == ALLOC_STACK)
	{
		tmp = this->stack_address;
		this->stack_address += value;
	}


	return tmp;
}

void Eagle::write_file(const char *path,std::string text)
{
	std::ofstream file(path, std::ios::binary);

	if(!file.is_open())
	{
		std::cerr << "Error: Unable to open file" << std::endl;
		return;
	}

	file.write(text.c_str(), text.size());

	file.close();
}

void Eagle::write_file_bin(const char *path)
{
	std::ofstream file(path, std::ios::binary);

	if(!file.is_open())
	{
		std::cerr << "Error: Unable to open file" << std::endl;
		return;
	}

	file.write(this->filebin.data(), this->filebin.size());

	file.close();
}

void Eagle::load_file(const char *path)
{
	std::ifstream file(path, std::ios::binary);

	if(!file.is_open())
	{
		std::cerr << "Error: Unable to open file" << std::endl;
		return;
	}

	std::cerr << "File: "<< path << "\n";

	file.seekg(0, std::ios::end);
	std::streampos filesizesp = file.tellg();
	int filesize = static_cast<int>(filesizesp);

	file.seekg(0, std::ios::beg);

	this->filetext.resize(filesize+5);

	file.read(&this->filetext[0], filesize);
	this->filetext[filesize+0] = 0;
	this->filetext[filesize+1] = 0;
	this->filetext[filesize+2] = 0;
	this->filetext[filesize+3] = 0;

	file.close();
}

void Eagle::load_file_bin(const char *path,std::vector<char> &data)
{
	std::ifstream file(path, std::ios::binary);

	if(!file.is_open())
	{
		std::cerr << "Error: Unable to open file" << std::endl;
		return;
	}

	file.seekg(0, std::ios::end);
	std::streampos filesizesp = file.tellg();
	int filesize = static_cast<int>(filesizesp);

	file.seekg(0, std::ios::beg);

	data.resize(filesize);
	file.read(data.data(), filesize);

	file.close();
}


bool Eagle::isOperator_move(char c,char c2)
{
	return (c == '=') ||
	( (c == '+') && (c2 == '=') ) || ( (c == '-') && (c2 == '=') ) || ( (c == '=') && (c2 == '?') ) ||  ( (c == '=') && (c2 == '+') ) ||
	( (c == '*') && (c2 == '=') ) || ( (c == '/') && (c2 == '=') ) || ( (c == '%') && (c2 == '=') ) ||
	( (c == '&') && (c2 == '=') ) || ( (c == '|') && (c2 == '=') ) || ( (c == '^') && (c2 == '=') );
}

bool Eagle::isOperator_cmp(char c,char c2)
{
	return (c == '<')  || (c == '>') || (c == '&') ||
	( (c == '<') && (c2 == '=') ) || ( (c == '>') && (c2 == '=') ) ||
	( (c == '=') && (c2 == '=') ) || ( (c == '!') && (c2 == '=') ) ||
	( (c == '?') && (c2 == '=') ) || ( (c == '?') && (c2 == '!') ) ||
	( (c == '&') && (c2 == '!') )
	;
}

bool Eagle::isOperator_calcul(char c,char c2)
{
	return (c == '+') || (c == '-') || (c == '/') || (c == '*')  || (c == '%') ||
	(c == '&') || (c == '|') || (c == '^') || ( (c == '<') && (c2 == '<') ) || ( (c == '>') && (c2 == '>') );
}

int Eagle::line_code_asm(int mode)
{
	std::string word;
	char letter;
	mnemonic.clear();
	bool comment = false;
	bool path = false;


	EAGLE_MNEMONIQUE tmnemonic;

	while(this->text_code[idf] != 0)
	{
		letter = this->text_code[idf];
		idf++;

		if(letter == ';')
			comment = true;


		if(path == true)
		{
			if(letter != '\n')
				word += letter;
		}else
		if(comment == false)
		{
			if(isWord(letter))
			{
				word += letter;
			}
			else
			{
				if(word.size() != 0)
				{
					tmnemonic.token2 = letter;

					if(isNumber(word[0]))
					{
						tmnemonic.value = std::stoi(word);
						tmnemonic.type = 0;
					}
					else
						tmnemonic.type = 1;

					if(tmnemonic.token1 == '$')
					{
						tmnemonic.value = std::stoi(word, nullptr, 16);
						tmnemonic.type = 0;
					}

					tmnemonic.item = word;


					if(word == ".incbin")
						path = true;

					mnemonic.push_back(tmnemonic);
					word = "";
				}
				if(idf-2 > 0)
					tmnemonic.token0 = this->text_code[idf-2];
				tmnemonic.token1 = letter;
			}
		}


		if(letter == '\n')
		{
			bool keyl = false;
			comment = false;
			path = false;

			if(mnemonic.size() > 0)
			{
				int n = mnemonic.size();
				if(mnemonic[0].item == ".db")
				{
					if(mode == 1)
					{
						for(int i = 1;i < n;i++)
						{
							if(mnemonic[i].type == 0)
							{
								this->filebin.push_back(mnemonic[i].value);
							}else
							{
								this->filebin.push_back(this->labelbin[mnemonic[i].item]);
							}
						}
					}
					this->offset += mnemonic.size()-1;
					mnemonic.clear();
					keyl = true;
				}

				if(mnemonic[0].item == ".dw")
				{
					if(mode == 1)
					{
						for(int i = 1;i < n;i++)
						{
							if(mnemonic[i].type == 0)
							{
								this->filebin.push_back(mnemonic[i].value);
								this->filebin.push_back(mnemonic[i].value>>8);
							}else
							{
								this->filebin.push_back(this->labelbin[mnemonic[i].item]);
								this->filebin.push_back(this->labelbin[mnemonic[i].item]>>8);
							}
						}

					}
					this->offset += (mnemonic.size()-1)*2;
					mnemonic.clear();
					keyl = true;
				}

				if(mnemonic[0].item == ".dd")
				{
					if(mode == 1)
					{
						for(int i = 1;i < n;i++)
						{
							if(mnemonic[i].type == 0)
							{
								this->filebin.push_back(mnemonic[i].value);
								this->filebin.push_back(mnemonic[i].value>>8);
								this->filebin.push_back(mnemonic[i].value>>16);
								this->filebin.push_back(mnemonic[i].value>>24);
							}else
							{
								this->filebin.push_back(this->labelbin[mnemonic[i].item]);
								this->filebin.push_back(this->labelbin[mnemonic[i].item]>>8);
								this->filebin.push_back(this->labelbin[mnemonic[i].item]>>16);
								this->filebin.push_back(this->labelbin[mnemonic[i].item]>>24);
							}
						}

					}
					this->offset += (mnemonic.size()-1)*4;
					mnemonic.clear();
					keyl = true;
				}

				if(mnemonic[0].item == ".dq")
				{
					if(mode == 1)
					{
						for(int i = 1;i < n;i++)
						{
							if(mnemonic[i].type == 0)
							{
								this->filebin.push_back(mnemonic[i].value);
								this->filebin.push_back(mnemonic[i].value>>8);
								this->filebin.push_back(mnemonic[i].value>>16);
								this->filebin.push_back(mnemonic[i].value>>24);

								this->filebin.push_back(mnemonic[i].value>>32);
								this->filebin.push_back(mnemonic[i].value>>40);
								this->filebin.push_back(mnemonic[i].value>>48);
								this->filebin.push_back(mnemonic[i].value>>56);
							}else
							{
								this->filebin.push_back(this->labelbin[mnemonic[i].item]);
								this->filebin.push_back(this->labelbin[mnemonic[i].item]>>8);
								this->filebin.push_back(this->labelbin[mnemonic[i].item]>>16);
								this->filebin.push_back(this->labelbin[mnemonic[i].item]>>24);

								this->filebin.push_back(0);
								this->filebin.push_back(0);
								this->filebin.push_back(0);
								this->filebin.push_back(0);
							}
						}
					}
					this->offset += (mnemonic.size()-1)*8;
					mnemonic.clear();
					keyl = true;
				}

				if(mnemonic[0].item == ".org")
				{
					if(mode == 1)
					{
						n = this->offset&0x7FFF;

						if( (n != 0) && (n != 0x7FB0))
						std::cout << "bloc size "<< ( (this->offset>>16)&0x7F)<<": "<< (this->offset&0x7FFF)  << "\n";
					}


					if(mnemonic[1].token1 == '$')
						this->offset = std::stoi(mnemonic[1].item, 0, 16);
					else
						this->offset = std::stoi(mnemonic[1].item);

					if(mode == 1)
					{
						n = this->offset - this->filebin.size();
						for(int i = 0;i < n;i++)
							this->filebin.push_back(0);

					}
					mnemonic.clear();
					keyl = true;
				}

				if(mnemonic[0].item == ".rodata")
				{
					if(mnemonic[1].token1 == '$')
						this->offset = std::stoi(mnemonic[1].item, 0, 16);
					else
						this->offset = std::stoi(mnemonic[1].item);
					mnemonic.clear();
					keyl = true;
				}

				if(mnemonic[0].item == ".code")
				{
					if(mnemonic[1].token1 == '$')
						this->offset = std::stoi(mnemonic[1].item, 0, 16);
					else
						this->offset = std::stoi(mnemonic[1].item);

					mnemonic.clear();
					keyl = true;
				}

				if(mnemonic[0].item == ".incbin")
				{
					std::vector<char> data;

					this->load_file_bin(word.c_str(),data);

					this->offset += data.size();


					if(mode == 1)
					{
						std::vector<char> data;

						this->load_file_bin(word.c_str(),data);

						if(data.size() > 0)
						{
							n = data.size();

							for(int i = 0;i < n;i++)
								this->filebin.push_back(data[i]);
						}
					}

					mnemonic.clear();
					keyl = true;
					word = "";
				}

				if(mnemonic[0].item == "..begin")
				{

					if(mode == 1)
					{
						this->cyclew = mnemonic[1].item;
						this->cycle = 0;
					}

					mnemonic.clear();
					keyl = true;
					word = "";
				}

				if(mnemonic[0].item == "..end")
				{

					if( (mode == 1) && (bcycle == true) )
					{
						std::cout << std::dec << "cycle estimate "<< this->cyclew << " : " << this->cycle << std::hex <<"\n";
					}

					mnemonic.clear();
					keyl = true;
					word = "";
				}


			}



			if(keyl == false)
				return 1;
		}
	}

	return 0;
}
