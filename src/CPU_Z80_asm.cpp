#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include <cstring>
#include <string>
#include <stdint.h>

#include "Eagle.hpp"
#include "CPU_Z80_asm.hpp"
#include "CPU_Z80_opcode_index.hpp"

#define debug_print(msg)                                                       \
	std::cerr << __FILE__ << ":" << __LINE__ << ": " << msg << std::endl;

#define die(msg)                                                               \
	std::cerr << "fatal:" << __FILE__ << ":" << __LINE__ << ": " << msg        \
			  << std::endl;                                                    \
	exit(1);

#define ensure(condition, msg)                                                 \
	if (!(condition))                                                          \
	{                                                                          \
		die(msg);                                                              \
	}

// -----------------------------------------------------------------------------
// Z80Register8
// .............................................................................
bool Z80Register8::load_immediate(uint16_t value)
{
	uint8_t old_value = (uint8_t)(this->value & 0xff);
	this->value		  = value;

	if (this->undef)
	{
		this->state = NUMBER;
		this->undef = false;
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

	if (this->undef)
	{
		this->state = NUMBER;
		this->undef = false;
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
	this->undef	 = false;
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
	this->undef = false;

	return changed;
}

// .............................................................................
bool Z80RegisterPair16::load_address(uint16_t address)
{
	bool changed = load_immediate(address);
	this->state	 = ADDRESS;
	this->undef	 = false;
	return changed;
}

// .............................................................................
std::string Z80RegisterPair16::to_string(void) const { return this->name; }

// -----------------------------------------------------------------------------
// helper functions

// .............................................................................
// get the register size if the variable is a register, else, return
// Z80_SIZE_UNKOWN
static Z80SizeType register_size(const EAGLE_VARIABLE &var)
{
	switch (var.type)
	{
	case EAGLE_keywords::ACC... EAGLE_keywords::REGF:
		return Z80_SIZE_BYTE;
	case EAGLE_keywords::REGBC... EAGLE_keywords::FACC:
		return Z80_SIZE_WORD;
	default:
		return Z80_SIZE_UNKOWN;
	}
}

// .............................................................................
// guess_operation_size from the operand type. If any of the operands are
// 16-bit, return Z80_SIZE_WORD. Otherwise, return Z80_SIZE_BYTE.

static Z80SizeType guess_operation_size(const EAGLE_VARIABLE &dst,
										const EAGLE_VARIABLE &src1)
{
	const EAGLE_VARIABLE *all[] = {&dst, &src1};

	for (uint32_t i = 0U; i < sizeof(all) / sizeof(all[0]); ++i)
	{
		const EAGLE_VARIABLE &var = *all[i];
		Z80SizeType size		  = register_size(var);
		if (size != Z80_SIZE_UNKOWN)
		{
			return size;
		}
		else if (!var.bimm)
		{
			return (Z80SizeType)var.nsize;
		}
		else
		{
			// probably an immediate, we can't assume anything from it
			continue;
		}
	}

	debug_print("Unable to determine operation size");
	return Z80_SIZE_BYTE;
}

// -----------------------------------------------------------------------------
// CPU_Z80

// .............................................................................
CPU_Z80::CPU_Z80()
	: A("a", R_A), B("b", R_B), C("c", R_C), D("d", R_D), E("e", R_E),
	  H("h", R_H), L("l", R_L), F("f", R_F), BC(B, C, RBC), DE(D, E, RDE),
	  HL(H, L, RHL), IX("ix", RIX), IY("iy", RIY), location_pool_pos(0),
	  value_pool_pos(0)
{
	this->registers[A.get_name()]  = &A;
	this->registers[B.get_name()]  = &B;
	this->registers[C.get_name()]  = &C;
	this->registers[D.get_name()]  = &D;
	this->registers[E.get_name()]  = &E;
	this->registers[F.get_name()]  = &F;
	this->registers[H.get_name()]  = &H;
	this->registers[L.get_name()]  = &L;
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

Z80Evaluable &CPU_Z80::new_value(const EAGLE_VARIABLE &var,
								 Z80SizeType oper_size)
{
	Z80Value &value = this->value_pool[this->value_pool_pos++];
	switch (oper_size)
	{
	case Z80_SIZE_BYTE:
		value.set_value((uint8_t)(var.immediate & 0xff));
		break;
	case Z80_SIZE_WORD:
		value.set_value((uint16_t)(var.immediate & 0xffff));
		break;
	default:
		std::cerr << "Unsupported variable type: size=" << (var.nsize)
				  << std::endl;
		exit(1);
	}

	return value;
}

Z80Evaluable &CPU_Z80::new_location(const EAGLE_VARIABLE &var,
									Z80SizeType oper_size)
{
	Z80Location &location = this->location_pool[this->location_pool_pos++];

	uint64_t address = var.address;

	// when dealing with an operation size of 8bit but the location refers to
	// a 16 bit reference, we have to adjust the address. The z80 is big endian,
	// the uneven byte is what we are looking for.
	if (oper_size == Z80_SIZE_BYTE && var.nsize == Z80_SIZE_WORD)
	{
		address++;
	}

	location.set_address(address & 0xffff, oper_size);
	return location;
}

Z80Evaluable &CPU_Z80::from(const EAGLE_VARIABLE &var, Z80SizeType oper_size)
{
	if (var.bimm)
	{
		return this->new_value(var, oper_size);
	}
	else
	{
		switch (var.type)
		{
		case EAGLE_keywords::ACC:
			return this->A;
		case EAGLE_keywords::IDX:
			return this->IX;
		case EAGLE_keywords::IDY:
			return this->IY;
		case EAGLE_keywords::REGB:
			return this->B;
		case EAGLE_keywords::REGC:
			return this->C;
		case EAGLE_keywords::REGD:
			return this->D;
		case EAGLE_keywords::REGE:
			return this->E;
		case EAGLE_keywords::IDHL:
			return this->HL;
		case EAGLE_keywords::REGBC:
			return this->BC;
		case EAGLE_keywords::REGDE:
			return this->DE;
		default:
			break;
		}
		return this->new_location(var, oper_size);
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

std::string load8(CPU_Z80 &cpu, Z80Evaluable &dst, Z80Evaluable &src,
				  Z80Evaluable &idx)
{
	bool ok = true;

	if (dst.is_register())
	{
		ok = dst.load_immediate(src.get_value());
	}

	if (ok)
	{
		return "ld " + dst.to_string() + ", " + src.to_string() + "\n";
	}
	else
	{
		return "";
	}
}

std::string load8_undef(CPU_Z80 &cpu, Z80Evaluable &dst, Z80Evaluable &src,
						Z80Evaluable &idx)
{
	return "";
}

std::string load16(CPU_Z80 &cpu, Z80Evaluable &dst, Z80Evaluable &src,
				   Z80Evaluable &idx)
{
	bool ok = true;

	if (dst.is_register())
	{
		debug_print("load16");
		ok = dst.load_immediate(src.get_value());
	}

	if (ok)
	{
		return "ld " + dst.to_string() + ", " + src.to_string() + "\n";
	}
	else
	{
		return "";
	}
}

std::string load16_undef(CPU_Z80 &cpu, Z80Evaluable &dst, Z80Evaluable &src,
						 Z80Evaluable &idx)
{
	return "";
}

std::string load8_byregister(CPU_Z80 &cpu, Z80Evaluable &dst, Z80Evaluable &src,
							 Z80Evaluable &idx)
{
	return "";
}

std::string load_byregister8(CPU_Z80 &cpu, Z80Evaluable &dst, Z80Evaluable &src,
							 Z80Evaluable &idx)
{
	return load8(cpu, cpu.A, src, idx) + load8(cpu, dst, cpu.A, idx);
}

std::string load_byregister16(CPU_Z80 &cpu, Z80Evaluable &dst,
							  Z80Evaluable &src, Z80Evaluable &idx)
{
	return load16(cpu, cpu.HL, src, idx) + load16(cpu, dst, cpu.HL, idx);
}

static inline OpFlag o_dst(unsigned int i) { return static_cast<OpFlag>(i); }
static inline OpFlag o_src(unsigned int i) { return static_cast<OpFlag>(i); }

Z80OpcodeIndex build_index(void)
{
	Z80OpcodeIndex opcode_index;

	// clang-format off

	// Covers every LD variants ----------------------------------------------------------------------------------------------
	// LD - load to bus ------------------------------------------------------------------------------------------------------
	opcode_index.index(OP_LOAD, o_dst(REG_PTR|RBC|RDE), 				o_src(REG|R_A), 				load8);
	opcode_index.index(OP_LOAD, o_dst(REG_PTR|RHL), 					o_src(REG|REG8), 				load8);
	opcode_index.index(OP_LOAD, o_dst(REG_PTR|RHL), 					o_src(IMM), 					load8);
	opcode_index.index(OP_LOAD, o_dst(REG_PTR|INDEXED|REGXY),  			o_src(REG|REG8), 				load8);
	opcode_index.index(OP_LOAD, o_dst(REG_PTR|INDEXED|REGXY),  			o_src(IMM), 					load8);
	opcode_index.index(OP_LOAD, o_dst(IMM_PTR8),    					o_src(REG|R_A),					load8);
	opcode_index.index(OP_LOAD, o_dst(IMM_PTR16),    					o_src(REG|R_A),					load8);
	opcode_index.index(OP_LOAD, o_dst(IMM_PTR16),    					o_src(REG|REG16|REGXY|RSP),		load16);
	// LD - from bus 8bit ----------------------------------------------------------------------------------------------------
	opcode_index.index(OP_LOAD, o_dst(REG|R_A),    						o_src(REG_PTR|REG16),			load8_undef);
	opcode_index.index(OP_LOAD, o_dst(REG|R_C|R_D|R_E|R_H|R_L), 		o_src(REG_PTR|RHL),				load8_undef);
	opcode_index.index(OP_LOAD, o_dst(REG|R_B), 						o_src(REG_PTR|RHL),				load_byregister8);
	opcode_index.index(OP_LOAD, o_dst(REG|R_A|R_C|R_D|R_E|R_H|R_L), 	o_src(REG_PTR|INDEXED|REGXY),	load8_undef);
	opcode_index.index(OP_LOAD, o_dst(REG|R_A),    						o_src(IMM_PTR8),				load8_undef);
	opcode_index.index(OP_LOAD, o_dst(REG|REG8),						o_src(IMM_PTR8),				load_byregister8);

	// LD - registers 8bit ---------------------------------------------------------------------------------------------------
	opcode_index.index(OP_LOAD, o_dst(REG|REG8),                		o_src(REG|REG8), 				load8);
	opcode_index.index(OP_LOAD, o_dst(REG|REG8),                		o_src(IMM), 					load8);
	// LD - from bus 16bit ---------------------------------------------------------------------------------------------------
	opcode_index.index(OP_LOAD, o_dst(REG|REG16|REGXY),					o_src(IMM_PTR16),				load16_undef);
	// LD - impossible on Z80, must be broken --------------------------------------------------------------------------------
	opcode_index.index(OP_LOAD, o_dst(IMM_PTR8),  						o_src(IMM), 					load_byregister8);
	opcode_index.index(OP_LOAD, o_dst(IMM_PTR16), 						o_src(IMM), 					load_byregister16);
	opcode_index.index(OP_LOAD, o_dst(IMM_PTR16), 						o_src(IMM_PTR16),				load_byregister16);
	opcode_index.index(OP_LOAD, o_dst(IMM_PTR16), 						o_src(IMM_PTR8),				load_byregister16);
	opcode_index.index(OP_LOAD, o_dst(IMM_PTR8),  						o_src(IMM_PTR8),				load_byregister8);
	// clang-format on
	opcode_index.index(OP_LOAD, o_dst(REG | REG16 | REGXY), o_src(IMM), load16);

	return opcode_index;
}

static Z80OpcodeIndex opcode_index = build_index();

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

	// before processing the eagle variable, lets analyse what is the
	// size of the operation.
	Z80SizeType oper_size = guess_operation_size(dst, src2);

	// lets transform the eagle variable to a more usable datatype.
	// Knowing the size of the operation, the immediate values can be
	// ajusted to match.
	Z80Evaluable &dstEv	 = this->from(dst, oper_size);
	Z80Evaluable &src1Ev = this->from(src1, oper_size);
	Z80Evaluable &src2Ev = this->from(src2, oper_size);

	OpType type;
	OpFlag dst_flag = dstEv.get_opflag();
	OpFlag src_flag = src2Ev.get_opflag();

	switch (operator1)
	{
	case '=':
		type = OP_LOAD;
		break;
	default:
		die("Invalid operator");
	}

	uint64_t key			 = make_z80_opkey(type, dst_flag, src_flag);
	translator_fn translator = opcode_index.get_translator(key);

	text_code = translator(*this, dstEv, src2Ev, src1Ev);

	// std::cout << "DST: " << dstEv.to_string() << std::endl
	// 		  << "SRC1: " << src1Ev.to_string() << std::endl
	// 		  << "SRC2: " << src2Ev.to_string() << std::endl;

	// if (dst.bimm == true)
	// {
	// 	std::cout << "Destination is immediate!\n";

	// 	Z80Value v((uint8_t)(dst.immediate & 0xFF));
	// 	dstvalue = v.to_string();
	// }
	// else
	// {
	// 	std::cout << "Not immediate" << (dst.type ==
	// EAGLE_keywords::ACC)
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
	// 		"ld " + dstEv.to_string() + ", " + this->A.to_string() +
	// "\n";

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

	this->value_pool_pos	= 0;
	this->location_pool_pos = 0;

	return text_code;
}
