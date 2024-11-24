


#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include <cstring>
#include <string>

#include <stdint.h>

#include "Eagle.hpp"


void Eagle::bin_z80()
{
	//pass 1
	this->offset = 0;
	this->idf = 0;
	mnemonic.clear();

	while(this->line_code_asm(0) != 0)
	{
		if(mnemonic.size() > 0)
		{
			if(mnemonic[0].token2 == ':')
				this->labelbin[mnemonic[0].item] = 0x100 + this->offset;
		}
	}

	//pass 2
	std::string instw;
	this->offset = 0;
	this->idf = 0;
	mnemonic.clear();
	this->offset = 0;

	while(this->line_code_asm(1) != 0)
	{
		if(mnemonic.size() > 0)
		{
			if(mnemonic.size() == 1)
			{
				if(instw == "ret")
				{
					this->filebin.push_back(0xC9);
				}
			}

			if(mnemonic.size() == 2)
			{
				if(instw == "call")
				{
					this->filebin.push_back(0xCD);
				}

				if(instw == "jp")
				{
					this->filebin.push_back(0xC3);
				}
			}

			if(mnemonic.size() == 3)
			{
				if(instw == "add")
				{
					this->filebin.push_back(0x86);
				}

				if(instw == "sub")
				{
					this->filebin.push_back(0x96);
				}


				if(instw == "ld")
				{
					this->filebin.push_back(0x6E);
				}
			}
		}
	}


}

