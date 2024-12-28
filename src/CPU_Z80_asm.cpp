#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include <cstring>
#include <string>

#include <stdint.h>

#include "Eagle.hpp"
#include "CPU_Z80_asm.hpp"

// -----------------------------------------------------------------------------
// Z80Register8
// .............................................................................
bool Z80Register8::load_immediate(uint16_t value)
{
	uint8_t old_value = (uint8_t)(this->value & 0xff);
	this->value		  = value;

	if (this->state == UNDEFINED)
	{
		this->state = NUMBER;
		return true;
	}
	else
	{
		return old_value != this->value;
	}
}

// .............................................................................
bool Z80Register8::load_address(uint16_t address)
{
	std::cerr << "Cannot load address into 8-bit register: " << this->name
			  << std::endl;
	return false;
}

// .............................................................................
std::string Z80Register8::to_string(void) const { return this->name; }

// -----------------------------------------------------------------------------
// Z80Register16
// .............................................................................
bool Z80Indexer::load_immediate(uint16_t value)
{
	uint16_t old_value = this->value;
	this->value		   = value;

	if (this->state == UNDEFINED)
	{
		this->state = NUMBER;
		return true;
	}
	else
	{
		return old_value != this->value;
	}
}

// .............................................................................
bool Z80Indexer::load_address(uint16_t address)
{
	bool changed = load_immediate(address);
	this->state	 = ADDRESS;
	return changed;
}

// .............................................................................
std::string Z80Indexer::to_string(void) const { return this->name; }

// -----------------------------------------------------------------------------
// Z80RegisterPair16
// .............................................................................
uint16_t Z80RegisterPair16::get_value(void) const
{
	uint8_t high = this->high.get_value();
	uint8_t low	 = this->low.get_value();

	return (uint16_t)((high << 8) | low);
}

// .............................................................................
bool Z80RegisterPair16::is_undef(void) const
{
	return (this->low.is_undef() || this->high.is_undef());
}

// .............................................................................
bool Z80RegisterPair16::load_immediate(uint16_t value)
{
	bool changed = false;

	changed |= this->low.load_immediate((uint8_t)(value & 0xff));
	changed |= this->high.load_immediate((uint8_t)((value >> 8) & 0xff));
	this->state = NUMBER;

	return changed;
}

// .............................................................................
bool Z80RegisterPair16::load_address(uint16_t address)
{
	bool changed = load_immediate(address);
	this->state	 = ADDRESS;
	return changed;
}

// .............................................................................
std::string Z80RegisterPair16::to_string(void) const { return this->name; }

// -----------------------------------------------------------------------------
// CPU_Z80

// .............................................................................
CPU_Z80::CPU_Z80()
	: A("a"), B("b"), C("c"), D("d"), E("e"), H("h"), L("l"), F("f"), AF(A, F),
	  BC(B, C), DE(D, E), HL(H, L), IX("ix"), IY("iy")
{

	this->registers[A.get_name()]  = &A;
	this->registers[B.get_name()]  = &B;
	this->registers[C.get_name()]  = &C;
	this->registers[D.get_name()]  = &D;
	this->registers[E.get_name()]  = &E;
	this->registers[F.get_name()]  = &F;
	this->registers[H.get_name()]  = &H;
	this->registers[L.get_name()]  = &L;
	this->registers[AF.get_name()] = &AF;
	this->registers[BC.get_name()] = &BC;
	this->registers[DE.get_name()] = &DE;
	this->registers[HL.get_name()] = &HL;
	this->registers[IX.get_name()] = &IX;
	this->registers[IY.get_name()] = &IY;
}

// .............................................................................
void CPU_Z80::initialize() { this->exit_block(); }

// .............................................................................
void CPU_Z80::exit_block(void)
{
	for (const auto &pair : this->registers)
	{
		pair.second->set_undef();
	}
}

Z80Evaluable &CPU_Z80::new_value(const EAGLE_VARIABLE &var)
{

	if (var.nsize <= 1)
	{
		Z80Value v = Z80Value((uint8_t)(var.immediate & 0xff));
		this->value_pool.push_back(v);
	}
	else if (var.nsize == 2)
	{
		Z80Value v = Z80Value((uint16_t)(var.immediate & 0xffff));
		this->value_pool.push_back(v);
	}
	else
	{
		std::cerr << "Unsupported variable type: size=" << (var.nsize)
				  << std::endl;
		exit(1);
	}

	return value_pool.back();
}

Z80Evaluable &CPU_Z80::new_location(const EAGLE_VARIABLE &var)
{
	Z80Location v = Z80Location((uint16_t)(var.address & 0xffff));
	this->location_pool.push_back(v);
	return this->location_pool.back();
}

Z80Evaluable &CPU_Z80::from(const EAGLE_VARIABLE &var)
{
	if (var.bimm)
	{
		return this->new_value(var);
	}
	else
	{
		switch (var.type)
		{
		case EAGLE_keywords::ACC:
			return this->A;
			break;
		case EAGLE_keywords::IDX:
			return this->IX;
		case EAGLE_keywords::IDY:
			return this->IY;
		case EAGLE_keywords::BACC:
			return this->B;
		default:
			break;
		}
		return this->new_location(var);
	}
}

void CPU_Z80::asm_do_else() {}

std::string CPU_Z80::asm_bru(const EAGLE_VARIABLE &src1,
							 const EAGLE_VARIABLE &src2, const char operator1,
							 const char operator2, int type, int clabel,
							 int &ilabel)
{
	std::string label_adr = ".label_b" + std::to_string(clabel);
	std::string text_code;

	if (type == TYPE_IF) // if
	{
		label_adr = ".label_" + std::to_string(ilabel);
		ilabel++;
	}

	if (src1.type == EAGLE_keywords::IDX)
	{
		text_code += "dex \n";
	}
	else if (src1.type == EAGLE_keywords::IDY)
	{
		text_code += "dey \n";
	}

	text_code += "cp \n";

	text_code += "jp " + label_adr + "\n";

	return text_code;
}

std::string CPU_Z80::asm_call_jump(const EAGLE_VARIABLE &src, int narg,
								   int type, std::string &labelcall)
{
	std::string mnemonic = "jp ", mne1, mne2, saddress, saddress2;
	std::string text_code;

	if (type >= 1)
		mnemonic = "call ";

	for (int i = 0; i < narg; i++)
	{
	}

	//------
	std::string src1value = labelcall;
	if (src.bimm == true)
	{
		src1value = std::to_string(src.immediate);
	}
	else
	{
		if (src.blabel == false)
		{

			if (src.type == EAGLE_keywords::UINT16)
			{
				text_code += "ld hl," + std::to_string(src.address) + "\n";
				src1value = "(hl)";
			}

			if (src.type == EAGLE_keywords::IDX)
			{
				src1value = "(IX)";
			}
			if (src.type == EAGLE_keywords::IDY)
			{
				src1value = "(IY)";
			}
		}
	}

	text_code += mnemonic + src1value + "\n";

	return text_code;
}

std::string CPU_Z80::asm_return(const EAGLE_VARIABLE &ret, bool retvoid)
{
	std::string text_code;
	if (retvoid == true)
	{
		return "ret\n";
	}
	else
	{
		if (ret.type == EAGLE_keywords::IDX)
		{
			text_code += "ld a,IX\n";
		}

		if (ret.type == EAGLE_keywords::IDY)
		{
			text_code += "ld a,IY\n";
		}

		if (ret.type != EAGLE_keywords::ACC)
		{
			if (ret.bimm == true)
				text_code +=
					"ld a," + std::to_string(ret.immediate & 0xFF) + "\n";
			else
				text_code += "lda a,(" + std::to_string(ret.address) + ")\n";
		}

		text_code += "ret\n";
	}

	return text_code;
}

std::string CPU_Z80::asm_alu(const EAGLE_VARIABLE &dst,
							 const EAGLE_VARIABLE &src1,
							 const EAGLE_VARIABLE &src2, const char operator1,
							 const char operator2)
{
	std::string mnemonic;
	std::string src1value, dstvalue;
	std::string src2value;
	std::string reg = "a,b";
	std::string text_code;

	Z80Evaluable &dstEv	 = this->from(dst);
	Z80Evaluable &src1Ev = this->from(src1);
	Z80Evaluable &src2Ev = this->from(src2);

	dstvalue = dstEv.to_string();
	// src1value = src1Ev.to_string();
	src2value = src2Ev.to_string();

	if (!dstEv.is_register())
	{
		if (src2Ev.get_size() == 1)
		{
			bool changed = this->A.load_immediate(src2Ev.get_value());
			if (changed)
			{
				text_code += "ld " + this->A.to_string() + ", " +
							 src2Ev.to_string() + "\n";
			}

			if (operator1 == '=')
			{
				text_code += "ld " + dstEv.to_string() + ", " +
							 this->A.to_string() + "\n";
			}
		}
		else
		{
			bool changed = this->HL.load_immediate(src2Ev.get_value());
			if (changed)
			{
				text_code += "ld " + this->HL.to_string() + ", " +
							 src2Ev.to_string() + "\n";
			}

			if (operator1 == '=')
			{
				text_code += "ld " + dstEv.to_string() + ", " +
							 this->HL.to_string() + "\n";
			}
		}
	}
	else
	{
		bool changed = this->A.load_immediate(src2Ev.get_value());
		if (changed)
		{
			text_code +=
				"ld " + this->A.to_string() + ", " + src2Ev.to_string() + "\n";
		}
	}

	// if (dst.bimm == true)
	// {
	// 	std::cout << "Destination is immediate!\n";

	// 	Z80Value v((uint8_t)(dst.immediate & 0xFF));
	// 	dstvalue = v.to_string();
	// }
	// else
	// {
	// 	std::cout << "Not immediate" << (dst.type == EAGLE_keywords::ACC)
	// 			  << std::endl;
	// 	Z80Location v(dst.address);
	// 	dstvalue = v.to_string();
	// }

	// if (src1.bimm == true)
	// {
	// 	src1value = std::to_string(src1.immediate & 0xFF);
	// }
	// else
	// {
	// 	src1value = "(" + std::to_string(src1.address & 0xFF) + ")";
	// }

	// if (src2.bimm == true)
	// {
	// 	src2value = std::to_string(src2.immediate & 0xFF);
	// }
	// else
	// {
	// 	src2value = "(" + std::to_string(src2.address & 0xFF) + ")";
	// }

	// if (operator1 == '=')
	// {
	// 	// text_code += "ld a," + src2value + "\n";
	// 	// text_code += "ld " + dstvalue + ",a\n";
	// 	text_code +=
	// 		"ld " + dstEv.to_string() + ", " + this->A.to_string() + "\n";

	// 	return text_code;
	// }

	if (operator1 == '+')
		mnemonic = "add ";

	if (operator1 == '-')
		mnemonic = "sub ";

	if (operator1 == '&')
		mnemonic = "and ";

	if (operator1 == '|')
		mnemonic = "or ";

	if (operator1 == '^')
		mnemonic = "xor ";

	if (operator1 == '<')
		mnemonic = "sla ";

	if (operator1 == '>')
		mnemonic = "sra ";

	if (operator1 == '*')
	{
	}

	if (operator1 == '/')
	{
	}

	// text_code += "ld a," + src1value + "\n";
	// text_code += "ld b," + src2value + "\n";

	// text_code += mnemonic + reg + "\n";

	// text_code += "ld " + dstvalue + ",a\n";

	this->value_pool.clear();
	this->location_pool.clear();

	return text_code;
}
