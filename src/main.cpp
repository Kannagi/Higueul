#include <vector>
#include <iostream>
#include <map>
#include <string>
#include <chrono>
#include <algorithm>
#include <cctype>
#include <sstream>

#include <stdint.h>
#include <string.h>

#include "Eagle.hpp"

int rmakeLoad(const char *filename);

Eagle eagle;

int main()
{
	eagle.target = TARGET_UNK;
	auto timer_start = std::chrono::high_resolution_clock::now();

	rmakeLoad("higueul_make.txt");

	auto timer_end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(timer_end - timer_start);

	std::cout << "Time compilation: " << duration.count() << " millisecondes" << std::endl;

	return 0;
}

void rmake_option(char *buf)
{
	std::string str = buf;

	std::istringstream iss(str);
	std::string word;

	// Parcourir chaque mot dans la chaîne
	while (iss >> word)
	{
		std::cout << word << "\n";
		if(word == "-debug")
		{
			eagle.debug = true;
		}

		if(word == "-asm")
		{
			eagle.bout_asm = true;
		}

		if(word == "-cycle")
		{
			eagle.bcycle = true;
		}

		if(word == "-65816")
		{
			eagle.target = TARGET_65816;
		}

		if(word == "-6502")
		{
			eagle.target = TARGET_6502;
		}

		if(word == "-huc6280")
		{
			eagle.target = TARGET_HuC6520;
		}

		if(word == "-z80")
		{
			eagle.target = TARGET_Z80;
		}

		if(word == "-80186")
		{
			eagle.target = TARGET_80286;
		}

		if(word == "-AltairX")
		{
			eagle.target = TARGET_AltairX;
		}

		if(word == "-lc")
		{
			eagle.target = TARGET_C;
		}

		if(word == "-RV32")
		{
			eagle.target = TARGET_RV32;
		}
	}
}


void rmake_add_file(char *filename)
{
	std::string str = filename;
	str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());

	eagle.load_file(str.c_str());
	eagle.parser_word();
	eagle.out_asm();

	if(eagle.error != 0)
		exit(eagle.error);
}

void rmake_compile_run(char *target)
{
	std::string str = target;
	str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());

	if(eagle.target == TARGET_65816)
	{
		eagle.bin_65816();
	}

	if(eagle.target == TARGET_6502)
	{

	}

	if(eagle.target == TARGET_HuC6520)
	{

	}

	if(eagle.target == TARGET_Z80)
	{

	}

	if(eagle.target == TARGET_80286)
	{

	}

	if(eagle.target == TARGET_AltairX)
	{
		eagle.bin_AltairX();
	}

	if(eagle.target == TARGET_C)
	{

	}

	if(eagle.target == TARGET_RV32)
	{

	}

	if(eagle.target == TARGET_UNK)
	{
		std::cout << "Target Unknow\n";
		exit(-1);
	}

	if(eagle.bout_asm == true)
		eagle.write_file("out.asm",eagle.text_code);


	eagle.write_file_bin(target);
}
