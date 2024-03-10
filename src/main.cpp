#include <vector>
#include <iostream>
#include <map>
#include <string>

#include <stdint.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>

#include "Eagle.hpp"

int main()
{
	Eagle eagle;
	eagle.debug = false;

    eagle.load_file("main.egl");

	eagle.parser_word();

	eagle.out_asm();

	eagle.bin_65816();

	eagle.write_file_bin("out.smc");
    return 0;
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
