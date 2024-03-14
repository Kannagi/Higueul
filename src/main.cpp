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
    }
}


void rmake_add_file(char *filename)
{
	eagle.load_file(filename);
	eagle.parser_word();
	eagle.out_asm();

	if(eagle.error != 0)
		exit(eagle.error);
}

void rmake_compile_run(char *target)
{
	std::string str = target;
	str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());

	eagle.target = TARGET_UNK;

	if(str == "-65816")
	{
		eagle.bin_65816();
		eagle.target = TARGET_65816;
	}


	if(str == "-6502")
	{
		eagle.target = TARGET_6502;
	}

	if(str == "-huc6280")
	{
		eagle.target = TARGET_HuC6520;
	}

	if(str == "-z80")
	{
		eagle.target = TARGET_Z80;
	}

	if(str == "-80186")
	{
		eagle.target = TARGET_80286;
	}

	if(str == "-AltairX")
	{
		eagle.target = TARGET_AltairX;
	}

	if(str == "-x86-64")
	{
		eagle.target = TARGET_x64;
	}

	if(str == "-RV32")
	{
		eagle.target = TARGET_RV32;
	}

	if(eagle.target == TARGET_UNK)
	{
		std::cout << "Target Unknow\n";
		exit(-1);
	}

	if(eagle.bout_asm == true)
		eagle.write_file("out.asm",eagle.text_code);


	eagle.write_file_bin("out.smc");
}


/*
65816
6502
HuC6280

z80
z80gb
80286
x64 (no all instruction)

AltairX

adci imm
adc  value
adcp
adcpl
adcs
adcsy

asl

asl value

movi al,0
mov  al,0
movp
8086
ax = acc
cx = use loop
si = idx
di = idt
65xx
128 var SPM
80 var SPMlib
16 Register
16 func spm
16 func lib
*/
