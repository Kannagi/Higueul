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
	this->keywords["int8"]       = EAGLE_keywords::INT8;
	this->keywords["int16"]      = EAGLE_keywords::INT16;
	this->keywords["int32"]      = EAGLE_keywords::INT32;
	this->keywords["int64"]      = EAGLE_keywords::INT64;

	this->keywords["uint8"]      = EAGLE_keywords::UINT8;
	this->keywords["uint16"]     = EAGLE_keywords::UINT16;
	this->keywords["uint32"]     = EAGLE_keywords::UINT32;
	this->keywords["uint64"]     = EAGLE_keywords::UINT64;

	this->keywords["float16"]    = EAGLE_keywords::FLOAT16;
	this->keywords["float32"]    = EAGLE_keywords::FLOAT32;
	this->keywords["float64"]    = EAGLE_keywords::FLOAT64;

	this->keywords["void"]       = EAGLE_keywords::VOID;

	this->keywords["register"]   = EAGLE_keywords::REGISTER;
	this->keywords["spm"]        = EAGLE_keywords::SPM;
	this->keywords["lib"]        = EAGLE_keywords::LIB;
	this->keywords["stack"]      = EAGLE_keywords::STACK;

	this->keywords["if"]         = EAGLE_keywords::IF;
	this->keywords["else"]       = EAGLE_keywords::ELSE;
	this->keywords["do"]         = EAGLE_keywords::DO;
	this->keywords["while"]      = EAGLE_keywords::WHILE;
	this->keywords["jump"]       = EAGLE_keywords::JUMP;
	this->keywords["call"]       = EAGLE_keywords::CALL;
	this->keywords["return"]     = EAGLE_keywords::RETURN;
	this->keywords["loop"]       = EAGLE_keywords::LOOP;

	this->keywords["func"]       = EAGLE_keywords::FUNC;
	this->keywords["funcspm"]    = EAGLE_keywords::FUNCSPM;
	this->keywords["funclib"]    = EAGLE_keywords::FUNCLIB;
	this->keywords["proc"]       = EAGLE_keywords::PROC;
	this->keywords[".end"]       = EAGLE_keywords::END;

	this->keywords["asm"]        = EAGLE_keywords::ASM;

	this->keywords[".define"]    = EAGLE_keywords::DEFINE;
	this->keywords[".incbin"]    = EAGLE_keywords::INCBIN;
	this->keywords[".code"]      = EAGLE_keywords::CODE;
	this->keywords[".rodata"]    = EAGLE_keywords::RODATA;
	this->keywords[".bss"]       = EAGLE_keywords::BSS;
	this->keywords[".funcmap"]   = EAGLE_keywords::FUNCMAP;
	this->keywords[".funcmap.spm"]   = EAGLE_keywords::FUNCMAPSPM;
	this->keywords[".funcmap.lib"]   = EAGLE_keywords::FUNCMAPLIB;
	this->keywords[".map.spm"]   = EAGLE_keywords::SPMMAP;
	this->keywords[".map.lib"]   = EAGLE_keywords::LIBMAP;
	this->keywords[".endmacro"]  = EAGLE_keywords::ENDMACRO;
	this->keywords[".org"]       = EAGLE_keywords::ORG;

	this->keywords[".macro"]     = EAGLE_keywords::MACRO;


	this->keywords[".data.b"]    = EAGLE_keywords::DATAB;
	this->keywords[".data.w"]    = EAGLE_keywords::DATAW;
	this->keywords[".data.l"]    = EAGLE_keywords::DATAL;
	this->keywords[".data.q"]    = EAGLE_keywords::DATAQ;
	this->keywords[".data.s"]    = EAGLE_keywords::DATAS;
	this->keywords[".data.h"]    = EAGLE_keywords::DATAH;
	this->keywords[".data.f"]    = EAGLE_keywords::DATAF;
	this->keywords[".data.d"]    = EAGLE_keywords::DATAD;


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

    std::cerr << "File: "<< path << std::endl;

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
	( (c == '+') && (c2 == '=') ) || ( (c == '-') && (c2 == '=') ) ||
	( (c == '*') && (c2 == '=') ) || ( (c == '/') && (c2 == '=') ) || ( (c == '%') && (c2 == '=') ) ||
	( (c == '&') && (c2 == '=') ) || ( (c == '|') && (c2 == '=') ) || ( (c == '^') && (c2 == '=') );
}

bool Eagle::isOperator_cmp(char c,char c2)
{
	return (c == '<')  || (c == '>') || (c == '&') ||
	( (c == '<') && (c2 == '=') ) || ( (c == '>') && (c2 == '=') ) ||
	( (c == '=') && (c2 == '=') ) || ( (c == '!') && (c2 == '=') );
}

bool Eagle::isOperator_calcul(char c,char c2)
{
	return (c == '+') || (c == '-') || (c == '/') || (c == '*') || (c == '&') || (c == '|') || (c == '^') || (c == '%')
	|| ( (c == '<') && (c2 == '<') ) || ( (c == '>') && (c2 == '>') );
}
