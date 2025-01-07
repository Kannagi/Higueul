#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include <cstring>
#include <string>
#include <stdint.h>
#include <cstdio>

#include "Eagle.hpp"
#include "CPU_Z80_asm.hpp"
#include "CPU_Z80_opcode_index.hpp"

// -----------------------------------------------------------------------------
// [todo] most of this will not be required later on.
#define debug_print(msg)                                                       \
	std::cerr << __FILE__ << ":" << __LINE__ << ": " << (msg) << std::endl;

#define die(msg)                                                               \
	std::cerr << "fatal:" << __FILE__ << ":" << __LINE__ << ": " << (msg)      \
			  << std::endl;                                                    \
	exit(1);

#define ensure(condition, msg)                                                 \
	if (!(condition))                                                          \
	{                                                                          \
		die(msg);                                                              \
	}

// -----------------------------------------------------------------------------
// forward declarations
Z80OpcodeIndex build_index(void);

// -----------------------------------------------------------------------------
// global types
static Z80NullEvaluable null_evaluable;
static Z80OpcodeIndex opcode_index = build_index();

// translate Eagle operations to local z80 operations enumeration.
static std::map<unsigned char, OpType> opcode_map = {
	{'=', OP_LOAD},
	{'+', OP_ADD},
};

// -----------------------------------------------------------------------------
// .............................................................................
// transform a string to a pointer string. To avoid searching in the code if the
// syntax changes.
static inline std::string ptr_string(std::string str)
{
	return "(" + str + ")";
}

// -----------------------------------------------------------------------------
// .............................................................................
class Z80NullRegister : public Z80Register
{
	public:
		std::string get_name(void) const override { return "undef"; }
		uint16_t get_value(void) const override { return 0; }
		bool is_undef(void) const override { return true; }
		Z80LoadResult load_value(uint16_t value) override
		{
			return {0, false, false};
		}
		void set_undef(void) override {}
		Z80SizeType get_size(void) const override { return Z80_SIZE_UNKOWN; }
		bool is_register(void) const override { return false; }
		bool is_deferencing(void) const override { return false; }
		std::string to_string(void) const override { return "<null register>"; }
} static null_register;

// -----------------------------------------------------------------------------
// Z80Register8
// .............................................................................
Z80LoadResult Z80Register8::load_value(uint16_t value)
{
	uint8_t old_value = (uint8_t)(this->value & 0xff);
	this->value		  = value;
	int16_t delta	  = ((uint8_t)this->value & 0xff) - old_value;

	if (this->undef)
	{
		this->state = NUMBER;
		this->undef = false;

		return {
			.delta	   = delta,
			.changed   = true,
			.was_undef = true,
		};
	}
	else
	{
		return {
			.delta	   = delta,
			.changed   = delta != 0,
			.was_undef = false,
		};
	}
}

// .............................................................................
std::string Z80Register8::to_string(void) const
{
	switch (this->state)
	{
	case NUMBER:
		return this->name;
	case ADDRESS:
		die("8bit register " + this->name +
			" is a 8bit register and cannot be used as a pointer");
	default:
		die("unknown register state");
	}
}

// -----------------------------------------------------------------------------
// Z80Register16
// .............................................................................
Z80LoadResult Z80Indexer::load_value(uint16_t value)
{
	uint16_t old_value = this->value;
	this->value		   = value;
	int16_t delta	   = this->value - old_value;
	if (this->undef)
	{
		this->state = NUMBER;
		this->undef = false;

		return {
			.delta	   = delta,
			.changed   = true,
			.was_undef = true,
		};
	}
	else
	{
		return {
			.delta	   = delta,
			.changed   = delta != 0,
			.was_undef = false,
		};
	}
}

// .............................................................................
std::string Z80Indexer::to_string(void) const
{
	switch (this->state)
	{
	case NUMBER:
		return this->name;
	case ADDRESS:
		return ptr_string(this->name + "+0");
	default:
		die("unknown register state");
	}
}

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
Z80LoadResult Z80RegisterPair16::load_value(uint16_t value)
{
	Z80LoadResult result = {
		.delta	   = 0U,
		.changed   = false,
		.was_undef = this->undef,
	};

	uint16_t v_old = this->low.get_value() | (this->high.get_value() << 8);

	this->low.load_value((uint8_t)(value & 0xff));
	this->high.load_value((uint8_t)((value >> 8) & 0xff));
	this->state = NUMBER;
	this->undef = false;

	uint16_t v_new = this->low.get_value() | (this->high.get_value() << 8);
	result.delta   = (int16_t)(v_new - v_old);
	result.changed = result.delta != 0;

	return result;
}

// .............................................................................
Z80Register &Z80RegisterPair16::get_subregister(uint8_t index) const
{
	if (index > 1)
	{
		die("Invalid register index");
	}
	return (index == 0) ? this->low : this->high;
}

// .............................................................................
std::string Z80RegisterPair16::to_string(void) const
{
	switch (this->state)
	{
	case NUMBER:
		return this->name;
	case ADDRESS:
		return ptr_string(this->name);
	default:
		die("unknown register state");
	}
}

// -----------------------------------------------------------------------------
// helper functions

// .............................................................................
Z80Evaluable &get_low(Z80Evaluable &e, CPU_Z80 &cpu)
{
	if (e.get_size() != Z80_SIZE_WORD)
	{
		return e;
	}

	if (e.is_register())
	{
		Z80RegisterPair16 &pair = static_cast<Z80RegisterPair16 &>(e);
		return pair.get_subregister(0);
	}
	else
	{
		if (e.is_deferencing())
		{
			return cpu.new_location(e.get_value() + 1, Z80_SIZE_BYTE);
		}
		else
		{
			return cpu.new_value(e.get_value(), Z80_SIZE_BYTE);
		}
	}
}

// .............................................................................
Z80Evaluable &get_high(Z80Evaluable &e, CPU_Z80 &cpu)
{
	if (e.get_size() != Z80_SIZE_WORD)
	{
		return cpu.new_value(0U, Z80_SIZE_BYTE);
	}
	if (e.is_register())
	{
		Z80RegisterPair16 &pair = static_cast<Z80RegisterPair16 &>(e);
		return pair.get_subregister(1);
	}
	else
	{
		if (e.is_deferencing())
		{
			return cpu.new_location(e.get_value(), Z80_SIZE_BYTE);
		}
		else
		{
			return cpu.new_value(e.get_value() >> 8, Z80_SIZE_BYTE);
		}
	}
}

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
	Z80SizeType size = register_size(dst);
	if (size == Z80_SIZE_UNKOWN && !dst.bimm)
	{
		size = (Z80SizeType)dst.nsize;
	}

	return size;
}

// -----------------------------------------------------------------------------
// CPU_Z80

// .............................................................................
CPU_Z80::CPU_Z80()
	: A("a", R_A), B("b", R_B), C("c", R_C), D("d", R_D), E("e", R_E),
	  H("h", R_H), L("l", R_L), F("f", R_F), BC(B, C, RBC), DE(D, E, RDE),
	  HL(H, L, RHL), IX("ix", RIX), IY("iy", RIY), location_pool_pos(0),
	  value_pool_pos(0), result_register(&null_register)
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

// .............................................................................
Z80Evaluable &CPU_Z80::new_value(uint16_t value, Z80SizeType size)
{
	if (this->value_pool_pos >= Z80_POOL_SIZE)
	{
		std::cerr << "Value pool overflow" << std::endl;
		exit(1);
	}
	Z80Value &evaluable = this->value_pool[this->value_pool_pos++];
	if (size == Z80_SIZE_BYTE)
	{
		evaluable.set_value((uint8_t)(value & 0xff));
	}
	else
	{
		evaluable.set_value((uint16_t)(value & 0xffff));
	}
	return evaluable;
}

// .............................................................................
Z80Evaluable &CPU_Z80::new_value(const EAGLE_VARIABLE &var,
								 Z80SizeType oper_size)
{
	if (this->value_pool_pos >= Z80_POOL_SIZE)
	{
		std::cerr << "Value pool overflow" << std::endl;
		exit(1);
	}

	uint16_t value		= var.bimm ? var.immediate : var.address;
	Z80Value &evaluable = this->value_pool[this->value_pool_pos++];

	if (var.bimm)
	{
		switch (oper_size)
		{
		case Z80_SIZE_BYTE:
			evaluable.set_value((uint8_t)(value & 0xff));
			break;
		case Z80_SIZE_WORD:
			evaluable.set_value((uint16_t)(value & 0xffff));
			break;
		default:
			std::cerr << "Unsupported variable type: size=" << (var.nsize)
					  << std::endl;
			exit(1);
		}
	}
	else
	{
		evaluable.set_value((uint16_t)(value & 0xffff));
		// std::cout << evaluable.get_size() << std::endl;
	}

	return evaluable;
}

// .............................................................................
Z80Evaluable &CPU_Z80::new_location(uint16_t value, Z80SizeType size)
{
	if (this->location_pool_pos >= Z80_POOL_SIZE)
	{
		std::cerr << "Location pool overflow" << std::endl;
		exit(1);
	}
	Z80Location &evaluable = this->location_pool[this->location_pool_pos++];
	evaluable.set_address(value, size);
	return evaluable;
}

// .............................................................................
Z80Evaluable &CPU_Z80::new_location(const EAGLE_VARIABLE &var,
									Z80SizeType oper_size)
{
	if (this->location_pool_pos >= Z80_POOL_SIZE)
	{
		std::cerr << "Value pool overflow" << std::endl;
		exit(1);
	}

	Z80Location &location = this->location_pool[this->location_pool_pos++];
	uint64_t address	  = var.address;

	// when dealing with an operation size of 8bit but the location refers to
	// a 16 bit reference, we have to adjust the address. The z80 is big endian,
	// the uneven byte is what we are looking for.
	bool loading_8bit_to_16bit =
		(oper_size == Z80_SIZE_BYTE && var.nsize == Z80_SIZE_WORD);
	bool loading_16bit_to_8bit =
		(oper_size == Z80_SIZE_BYTE && (var.type == EAGLE_keywords::UINT16 ||
										var.type == EAGLE_keywords::INT16));

	if (loading_8bit_to_16bit || loading_16bit_to_8bit)
	{
		address++;
	}
	location.set_address(address & 0xffff, (Z80SizeType)var.nsize);
	return location;
}

// .............................................................................
Z80Register &CPU_Z80::get_register_by_eagle_type(const EAGLE_keywords &type)
{
	switch (type)
	{
		// clang-format off
	case EAGLE_keywords::ACC:   return this->A;
	case EAGLE_keywords::IDX:   return this->IX;
	case EAGLE_keywords::IDY:   return this->IY;
	case EAGLE_keywords::REGB:  return this->B;
	case EAGLE_keywords::REGC:  return this->C;
	case EAGLE_keywords::REGD:  return this->D;
	case EAGLE_keywords::REGE:  return this->E;
	case EAGLE_keywords::IDHL:  return this->HL;
	case EAGLE_keywords::REGBC: return this->BC;
	case EAGLE_keywords::REGDE: return this->DE;
	default:
		return null_register;
		// clang-format on
	}
}

// .............................................................................
Z80Evaluable &CPU_Z80::get_from_eagle_var(const EAGLE_VARIABLE &var,
										  Z80SizeType oper_size)
{
	if (var.bimm)
	{
		return this->new_value(var, oper_size);
	}
	else if (var.bptr)
	{
		if (var.ptr2_exist)
		{
			die("no indexed mode supported on Z80 [pointers]. Only "
				"[registers16] are supported.");
		}

		Z80Register &reg = this->get_register_by_eagle_type(var.ptr_type);
		if (reg.exists())
		{
			if (reg.get_size() != Z80_SIZE_WORD)
			{
				// eventually this translator could have the option to alias any
				// 8bit pointers to a idhl.
				die("8bit registers can't be used as [pointers] on Z80. "
					"Transfert "
					"the value to `idhl`, `idx` or `idy` to perform a pointer "
					"operation.");
			}
			reg.set_pointing(true);
			return reg;
		}
		else
		{
			die("variable pointers not supported on Z80. Load the address into "
				"{`idhl`, `idx`, `idy`} and use [register pointers] instead.");
		}
	}
	else if (var.type == EAGLE_keywords::UNKNOW)
	{
		return null_evaluable;
	}
	else if (var.type == EAGLE_keywords::UINT8 ||
			 var.type == EAGLE_keywords::UINT16 ||
			 var.type == EAGLE_keywords::INT8 ||
			 var.type == EAGLE_keywords::INT16)
	{
		switch (var.token1)
		{
		case POINTER_OPERATOR:
			return this->new_value(var, oper_size);
		case DEREF_OPERATOR:
			/* fall-through */
		default:
			// by resilience, lets interpret the variable name alone like
			// a dereferencement.
			return this->new_location(var, oper_size);
		}
	}
	else
	{
		Z80Register &reg = this->get_register_by_eagle_type(var.type);
		if (!reg.is_register())
		{
			std::string msg =
				("Invalid register type: " + (uint16_t)var.type) + '\n';
			die(msg);
		}

		reg.set_pointing(false);
		return reg;
	}
}

// .............................................................................
void CPU_Z80::asm_do_else() {}

// .............................................................................
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

// .............................................................................
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

// .............................................................................
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

// .............................................................................
// increment translation strategy
static std::string inc(CPU_Z80 &cpu, Z80Evaluable &dst, Z80Evaluable &src)
{
	if (!dst.is_register())
	{
		die("Destination must be a register.");
	}

	// its incomplete

	Z80Register &dst_reg = reinterpret_cast<Z80Register &>(dst);
	return "inc " + dst_reg.get_name() + "\n";
}

// .............................................................................
// decrement translation strategy
static std::string dec(CPU_Z80 &cpu, Z80Evaluable &dst, Z80Evaluable &src)
{
	if (!dst.is_register())
	{
		die("Destination must be a register.");
	}

	// its incomplete

	Z80Register &dst_reg = reinterpret_cast<Z80Register &>(dst);
	return "dec " + dst_reg.get_name() + "\n";
}

// .............................................................................
// LD main strategy
// [refactor me]: it grew a bit and became a bit hard to follow... (sigh)
static std::string load(CPU_Z80 &cpu, Z80Evaluable &dst, Z80Evaluable &src)
{
	// lets define some translation outcomes
	enum Mode : uint16_t
	{
		MODE_IGNORE,
		MODE_LD,
		MODE_INC,
		MODE_DEC,
		MODE_XOR,
	};

	Mode mode		   = MODE_LD;
	bool src_was_undef = src.is_undef();

	// when the destination is a register and the destination isn't a variable
	// or a pointer, some optimization are possible.
	if (dst.is_register() && !dst.is_deferencing())
	{
		Z80RegisterPair16 &dst_reg = reinterpret_cast<Z80RegisterPair16 &>(dst);

		if (src.is_deferencing())
		{
			// [todo] why are we doing this? To revise this branch is needed.
			dst_reg.load_value(src.get_value());
			dst_reg.set_undef();
		}
		else
		{
			Z80LoadResult ld_result = dst_reg.load_value(src.get_value());

			if (!ld_result.changed && !ld_result.was_undef && !src_was_undef)
			{
				// in other words, if the operation changes nothing, lets just
				// not generate any code.
				mode = MODE_IGNORE;
				return "";
			}
			else
			{
				// some interesting optimizations
				if (dst_reg.get_name() == cpu.A.get_name() &&
					dst_reg.get_value() == 0 && !cpu.A.is_undef())
				{
					// xor a is classic on Z80 and 8080
					mode = MODE_XOR;
				}
				else if (!ld_result.was_undef && ld_result.delta == 1)
				{
					// when dealing with immediate values or addresses,
					// incrementing or decrementing is faster
					mode = MODE_INC;
				}
				else if (!ld_result.was_undef && ld_result.delta == -1)
				{
					mode = MODE_DEC;
				}
			}
		}

		if (src_was_undef)
		{
			// this is a security measure, if the source value is undef, we must
			// ensure the dst register is now also undef.
			dst_reg.set_undef();
			mode = MODE_LD;
		}
	}

	std::string dst_str = dst.to_string();
	std::string src_str = src.to_string();

	if (dst_str == src_str)
	{
		mode = MODE_IGNORE;
	}

	switch (mode)
	{
	case MODE_IGNORE:
		return "";
	case MODE_LD:
		return "ld " + dst_str + ", " + src_str + "\n";
	case MODE_INC:
		return "inc " + dst_str + "\n";
	case MODE_DEC:
		return "dec " + dst_str + "\n";
	case MODE_XOR:
		return "xor " + dst_str + "\n";
	default:
		die("invalid mode");
	}
}

// .............................................................................
// transfert data from 16bit register to another 16bit register, except IX or
// IY. By example, BC = DE will be converted to B = D and C = E.
static std::string load16_copy_reg_by_8bit(CPU_Z80 &cpu, Z80Evaluable &dst,
										   Z80Evaluable &src)
{
	if (!dst.is_register() || !src.is_register())
	{
		die("only register can be used");
	}
	if (dst.get_size() != Z80_SIZE_WORD || src.get_size() != Z80_SIZE_WORD)
	{
		die("this method accepts only 16-bit registers");
	}

	std::string text_code = "";

	Z80Evaluable &dest_reg8_low = get_low(dst, cpu);
	Z80Evaluable &src_reg8_low	= get_low(src, cpu);
	Z80Evaluable &dest_reg8_hi	= get_high(dst, cpu);
	Z80Evaluable &src_reg8_hi	= get_high(src, cpu);

	text_code += load(cpu, dest_reg8_low, src_reg8_low);
	text_code += load(cpu, dest_reg8_hi, src_reg8_hi);

	return text_code;
}

// .............................................................................
// when it's not possible to load a 8bit value directly to the destination, uses
// A as register in the middle.
static std::string load8_byregister_a(CPU_Z80 &cpu, Z80Evaluable &dst,
									  Z80Evaluable &src)
{
	return load(cpu, cpu.A, src) + load(cpu, dst, cpu.A);
}

// .............................................................................
// load only the LSB part of the source
static std::string load8_from16byregister_a(CPU_Z80 &cpu, Z80Evaluable &dst,
											Z80Evaluable &src)
{
	if (src.get_size() != Z80_SIZE_WORD || dst.get_size() != Z80_SIZE_BYTE)
	{
		die("Invalid size for load_from16byregister_a");
	}

	std::string text_code = "";
	text_code += load(cpu, cpu.A, get_low(src, cpu));
	text_code += load(cpu, dst, cpu.A);

	return text_code;
}

// .............................................................................
// load only the LSB part of the source, sets the MSB part of the destination
// to zero. This is intended for memory operations
static std::string load16_byregisterfrom8(CPU_Z80 &cpu, Z80Evaluable &dst,
										  Z80Evaluable &src)
{
	if (src.get_size() != Z80_SIZE_BYTE || dst.get_size() != Z80_SIZE_WORD)
	{
		die("Invalid size for load_byregister16from8");
	}

	if (src.is_register())
	{
		// passing by A is only required when a source cannot be loaded to L
		// directly. A source register can do this.
		return load(cpu, cpu.H, cpu.new_value(0U, Z80_SIZE_BYTE)) +
			   load(cpu, cpu.L, src) + load(cpu, dst, cpu.HL);
	}
	else
	{
		return load(cpu, cpu.H, cpu.new_value(0U, Z80_SIZE_BYTE)) +
			   load(cpu, cpu.A, src) + load(cpu, cpu.L, cpu.A) +
			   load(cpu, dst, cpu.HL);
	}
}

// .............................................................................
// this one set the MSB register to zero and transfer value to low register.
// This is intended for register operations.
static std::string load16_regfromreg8(CPU_Z80 &cpu, Z80Evaluable &dst,
									  Z80Evaluable &src)
{
	if (src.get_size() != Z80_SIZE_BYTE || dst.get_size() != Z80_SIZE_WORD)
	{
		die("Invalid size for load_reg16fromreg8");
	}

	Z80RegisterPair16 &dst_pair = reinterpret_cast<Z80RegisterPair16 &>(dst);
	return load(cpu, dst_pair.get_subregister(1U),
				cpu.new_value(0U, Z80_SIZE_BYTE)) +
		   load(cpu, dst_pair.get_subregister(0U), src);
}

// .............................................................................
// copy data from a pointer-to-word to the data of another pointer-to-word by
// incrementing the pointers. This strategy sucks when programming Z80
// assembly, it still sucks here. 50 T-States in average.
static std::string load_16bitmem_copy_with_a(CPU_Z80 &cpu, Z80Evaluable &dst,
											 Z80Evaluable &src)
{
	return load(cpu, cpu.A, src) + load(cpu, dst, cpu.A) +
		   inc(cpu, dst, null_evaluable) + inc(cpu, src, null_evaluable) +
		   load(cpu, cpu.A, src) + load(cpu, dst, cpu.A) +
		   dec(cpu, dst, null_evaluable) + dec(cpu, src, null_evaluable);
}

// .............................................................................
// Assign a value by addition. This is slow it involve setting the register to 0
// and then add the other operand to it. This is the only way to move data to
// and from IX and IY. This sucks as well.
static std::string load16_byadding(CPU_Z80 &cpu, Z80Evaluable &dst,
								   Z80Evaluable &src)
{
	std::string text_code = "";
	text_code += cpu.translate(OP_LOAD, dst, cpu.new_value(0, Z80_SIZE_WORD));
	text_code += "add " + dst.to_string() + ", " + src.to_string() + "\n";

	if (dst.is_register())
	{
		Z80Register &dst_reg = reinterpret_cast<Z80Register &>(dst);
		dst_reg.set_undef();
	}

	return text_code;
}

// .............................................................................
// This strategy is intended for memory operations, it transfer date from a
// variable to another variable by using HL.
static std::string load16_ptr_using_hl(CPU_Z80 &cpu, Z80Evaluable &dst,
									   Z80Evaluable &src)
{
	cpu.HL.set_pointing(false);
	return load(cpu, cpu.HL, src) + load(cpu, dst, cpu.HL);
}

// .............................................................................
// sometime this is needed to avoid using NULL pointers.
static std::string load_nothing(CPU_Z80 &cpu, Z80Evaluable &dst,
								Z80Evaluable &src)
{
	return "";
}

// .............................................................................
// 8Bit base addition strategy. Covers most cases.
static std::string add8(CPU_Z80 &cpu, Z80Evaluable &src1, Z80Evaluable &src2)
{
	std::string text_code = "";

	cpu.set_result_register(cpu.A);

	Z80Evaluable *src1_ptr = &src1;
	Z80Evaluable *src2_ptr = &src2;

	// when src2 is a register, it triggers an optimisation. Let's swap the
	// operands to reduce register juggling
	if (src1.is_register() && !src2.is_register())
	{
		std::swap(src1_ptr, src2_ptr);
	}

	std::string src1_name = src1_ptr->to_string();
	std::string src2_name = src2_ptr->to_string();

	if (src1_name == src2_name)
	{
		// that one is fun :)
		text_code += cpu.translate(OP_LOAD, cpu.A, *src1_ptr);
		text_code += "rlca\n";
	}
	else if (src2_name == cpu.A.get_name())
	{
		// we do not need to use the register L when A is used. Lets just
		// use the accumulator for its intended use :).
		text_code +=
			"add " + cpu.A.to_string() + ", " + src1_ptr->to_string() + "\n";
	}
	else
	{
		text_code += cpu.translate(OP_LOAD, cpu.L, *src2_ptr);
		text_code += cpu.translate(OP_LOAD, cpu.A, *src1_ptr);
		text_code +=
			"add " + cpu.A.to_string() + ", " + cpu.L.to_string() + "\n";
	}

	if (!cpu.A.is_undef() && cpu.A.get_value() == 0)
	{
		text_code = "";
	}

	return text_code;
}

// .............................................................................
// 16Bit base addition strategy. Covers most cases, but not pointers mixed with
// variables or registers.
static std::string add16(CPU_Z80 &cpu, Z80Evaluable &src1, Z80Evaluable &src2)
{
	std::string text_code = "";

	cpu.set_result_register(cpu.HL);

	Z80Evaluable *src1_ptr = &src1;
	Z80Evaluable *src2_ptr = &src2;

	// when src2 is a register, it triggers an optimisation. Let's swap the
	// operands to reduce register juggling
	if (src1.is_register() && !src2.is_register())
	{
		std::swap(src1_ptr, src2_ptr);
	}

	std::string src1_name = src1_ptr->to_string();
	std::string src2_name = src2_ptr->to_string();

	if (src1_name == src2_name)
	{
		// not as great as rlca, but still better by a lot.
		text_code += cpu.translate(OP_LOAD, cpu.HL, *src1_ptr);
		text_code +=
			"add " + cpu.HL.get_name() + ", " + cpu.HL.get_name() + "\n";
	}
	else
	{
		text_code += cpu.translate(OP_LOAD, cpu.DE, *src2_ptr);
		text_code += cpu.translate(OP_LOAD, cpu.HL, *src1_ptr);
		text_code +=
			"add " + cpu.HL.to_string() + ", " + cpu.DE.to_string() + "\n";
	}

	if (!cpu.HL.is_undef() && cpu.HL.get_value() == 0)
	{
		text_code = "";
	}

	return text_code;
}

// .............................................................................
// used to mark as destination operand and hide cast
static inline OpFlag D(unsigned int i) { return static_cast<OpFlag>(i); }
// .............................................................................
// used to mark as source operand and hide cast
static inline OpFlag S(unsigned int i) { return static_cast<OpFlag>(i); }

// .............................................................................
// build a registery of all possible operations. This associates translation
// strategies to different operand combinations.
static Z80OpcodeIndex build_index(void)
{
	Z80OpcodeIndex opcodes;

	// clang-format off

	// -----------------------------------------------------------------------------------------------------------------------
	// [ LD ]
	// LD - load to bus ------------------------------------------------------------------------------------------------------
	opcodes.index(OP_LOAD, D(REG_PTR|RBC|RDE), S(REG|R_A), load);
	opcodes.index(OP_LOAD, D(REG_PTR|RHL), S(REG|REG8), load);
	opcodes.index(OP_LOAD, D(REG_PTR|RHL), S(IMM16), load);
	opcodes.index(OP_LOAD, D(REG_PTR|REGXY), S(REG|REG8), load);
	opcodes.index(OP_LOAD, D(REG_PTR|REGXY), S(IMM16), load);
	opcodes.index(OP_LOAD, D(IMM_VAR8), S(REG|R_A), load);
	opcodes.index(OP_LOAD, D(IMM_VAR16), S(REG|R_A), load16_byregisterfrom8);
	opcodes.index(OP_LOAD, D(IMM_VAR16), S(REG|R_B|R_C|R_D|R_E|R_L|R_H), load16_byregisterfrom8);
	opcodes.index(OP_LOAD, D(IMM_VAR16), S(REG|REG16|REGXY|RSP), load);
	opcodes.index(OP_LOAD, D(IMM_VAR8), S(REG_PTR|RHL), load8_byregister_a);
	opcodes.index(OP_LOAD, D(IMM_VAR8), S(REG_PTR|REGXY), load8_byregister_a);
	// LD - from bus 8bit ----------------------------------------------------------------------------------------------------
	opcodes.index(OP_LOAD, D(REG|R_A), S(REG_PTR|REG16|REGXY), load);
	opcodes.index(OP_LOAD, D(REG|R_B|R_C|R_D|R_E|R_H|R_L), S(REG_PTR|RHL|REGXY), load);
	opcodes.index(OP_LOAD, D(REG|R_A), S(IMM_VAR8), load);
	opcodes.index(OP_LOAD, D(REG|REG8), S(IMM_VAR8), load8_byregister_a);
	// LD - registers 8bit ---------------------------------------------------------------------------------------------------
	opcodes.index(OP_LOAD, D(REG|REG8), S(REG|REG16), load8_from16byregister_a);
	opcodes.index(OP_LOAD, D(REG|REG8), S(REG|REG8), load);
	opcodes.index(OP_LOAD, D(REG|REG8), S(IMM8), load);
	// LD - registers 16bit --------------------------------------------------------------------------------------------------
	opcodes.index(OP_LOAD, D(REG|REG16), S(REG|REG16), load16_copy_reg_by_8bit);
	opcodes.index(OP_LOAD, D(REG|REGXY), S(REG|REG16), load16_byadding);
	opcodes.index(OP_LOAD, D(REG|REG16), S(REG|REGXY), load16_byadding);
	// LD - from bus 16bit ---------------------------------------------------------------------------------------------------
	opcodes.index(OP_LOAD, D(REG|REG16|REGXY),	S(IMM_VAR16), load);
	// LD - impossible on Z80, must be broken --------------------------------------------------------------------------------
	opcodes.index(OP_LOAD, D(IMM_VAR8), S(REG|REG16), load8_from16byregister_a);
	opcodes.index(OP_LOAD, D(REG|REG16), S(REG|REG8), load16_regfromreg8);
	opcodes.index(OP_LOAD, D(IMM_VAR8), S(IMM8), load8_byregister_a);
	opcodes.index(OP_LOAD, D(IMM_VAR8), S(IMM_VAR8), load8_byregister_a);
	opcodes.index(OP_LOAD, D(IMM_VAR8), S(IMM_VAR16), load8_byregister_a);
	opcodes.index(OP_LOAD, D(IMM_VAR16), S(IMM16), load16_ptr_using_hl);
	opcodes.index(OP_LOAD, D(IMM_VAR16), S(IMM_VAR16), load16_ptr_using_hl);
	opcodes.index(OP_LOAD, D(IMM_VAR16), S(IMM_VAR8), load16_byregisterfrom8);
	opcodes.index(OP_LOAD, D(REG | REG16 | REGXY), S(IMM16), load);
	opcodes.index(OP_LOAD, D(REG_PTR | RBC), S(REG_PTR | RBC), load_nothing);
	opcodes.index(OP_LOAD, D(REG_PTR | RHL), S(REG_PTR | RHL), load_nothing);
	opcodes.index(OP_LOAD, D(REG_PTR | RDE), S(REG_PTR | RDE), load_nothing);
	opcodes.index(OP_LOAD, D(REG_PTR | RBC), S(REG_PTR | RHL), load_16bitmem_copy_with_a);
	opcodes.index(OP_LOAD, D(REG_PTR | RHL), S(REG_PTR | RBC), load_16bitmem_copy_with_a);
	opcodes.index(OP_LOAD, D(REG_PTR | RDE), S(REG_PTR | RHL), load_16bitmem_copy_with_a);
	opcodes.index(OP_LOAD, D(REG_PTR | RHL), S(REG_PTR | RDE), load_16bitmem_copy_with_a);
	opcodes.index(OP_LOAD, D(REG_PTR | RDE), S(REG_PTR | RBC), load_16bitmem_copy_with_a);
	opcodes.index(OP_LOAD, D(REG_PTR | RBC), S(REG_PTR | RDE), load_16bitmem_copy_with_a);
	// LD - impossible and bad idea -------------------------------------------------------------------------------------
	// these could be optimized to use (IX+n) instead of incrementing, but this
	// is enough for now.
	opcodes.index(OP_LOAD, D(REG_PTR | REG16), S(REG_PTR | REGXY), load_16bitmem_copy_with_a);
	opcodes.index(OP_LOAD, D(REG_PTR | REGXY), S(REG_PTR | REG16), load_16bitmem_copy_with_a);

	// ------------------------------------------------------------------------------------------------------------------
	// [ ADD ]
	// ADD - 8bit -------------------------------------------------------------------------------------------------------
	opcodes.index(OP_ADD, S(REG|REG8), S(REG|REG8), add8);
	opcodes.index(OP_ADD, S(REG|REG8), S(IMM8), add8);
	opcodes.index(OP_ADD, S(IMM8), S(REG|REG8), add8);
	opcodes.index(OP_ADD, S(REG|REG8), S(IMM_VAR8), add8);
	opcodes.index(OP_ADD, S(IMM_VAR8), S(REG|REG8), add8);
	opcodes.index(OP_ADD, S(IMM_VAR8), S(IMM_VAR8), add8);
	opcodes.index(OP_ADD, S(REG_PTR|REG16|REGXY), S(REG_PTR|REG16|REGXY), add8);
	// ADD - 16bit ------------------------------------------------------------------------------------------------------
	opcodes.index(OP_ADD, S(REG|REG16), S(REG|REG16), add16);
	opcodes.index(OP_ADD, S(REG|REGXY), S(REG|REG16), add16);
	opcodes.index(OP_ADD, S(REG|REG16), S(REG|REGXY), add16);
	opcodes.index(OP_ADD, S(IMM_VAR16), S(REG|REG8), add16);
	//opcodes.index(OP_ADD, o_src(REG_PTR|REG16), o_src(REG|REG16), add16);

	// clang-format on

	std::cout << "Z80 translation index : " << opcodes.get_size() << " entries."
			  << std::endl;

	return opcodes;
}

// .............................................................................
std::string CPU_Z80::translate(OpType type, Z80Evaluable &dstEv,
							   Z80Evaluable &src2Ev)
{
	return this->translate(type, dstEv, null_evaluable, src2Ev);
}

// .............................................................................
std::string CPU_Z80::translate(OpType type, Z80Evaluable &dstEv,
							   Z80Evaluable &src1Ev, Z80Evaluable &src2Ev)
{
	uint64_t key;
	translator_fn translator = nullptr;
	std::string text_code	 = "";

	if (type >= OP__ARITHMETIC)
	{
		// arithmetic operations in higueul adds two operands and load the
		// results into the destination register. We need to split the operation
		// into two distinct steps in order to reuse the LD logic.
		key = make_z80_opkey(type, src1Ev, src2Ev);
		// std::cout << "Op=" << (key >> 48) << explain_key(key) << ", "
		// 		  << explain_key(key >> 24) << std::endl;

		translator = opcode_index.get_translator(key);
		text_code  = translator(*this, src1Ev, src2Ev);
		text_code += this->translate(OP_LOAD, dstEv, *this->result_register,
									 *this->result_register);
	}
	else
	{
		key		   = make_z80_opkey(type, dstEv, src2Ev);
		translator = opcode_index.get_translator(key);
		text_code  = translator(*this, dstEv, src2Ev);
	}
	return text_code;
}

// .............................................................................
// debugging utilities - TODO remove later
static void print_eaglevarp(const EAGLE_VARIABLEP &var)
{
	std::cout << "{ value: " << var.value << ", type : " << (uint16_t)var.type
			  << ", bimm : " << var.bimm << ", token1 : " << var.token1
			  << ", token2 : " << var.token2 << "}";
}

// .............................................................................
// debugging utilities - TODO remove later
static void print_eaglevar(const EAGLE_VARIABLE &var)
{
	std::cout << "{ immediate : " << var.immediate
			  << ", dimmediate : " << var.dimmediate
			  << ", address : " << var.address << ", nsize : " << var.nsize
			  << ", type : " << (uint16_t)var.type
			  << ", type2 : " << (uint16_t)var.type2 << ", bimm : " << var.bimm
			  << ", blabel : " << var.blabel << ", bptr : " << var.bptr
			  << ", ptr2_exist : " << var.ptr2_exist
			  << ", ptr_type : " << (uint16_t)var.ptr_type;
	std::cout << ", ptr1 : ";
	print_eaglevarp(var.ptr1);
	std::cout << ", ptr2 :";
	print_eaglevarp(var.ptr2);
	std::cout << "}" << std::endl;
}

// .............................................................................
std::string CPU_Z80::asm_alu(const EAGLE_VARIABLE &dst,
							 const EAGLE_VARIABLE &src1,
							 const EAGLE_VARIABLE &src2, const char operator1,
							 const char operator2)
{
	std::string text_code;

	// before processing the eagle variable, lets analyse what is the
	// size of the operation.
	Z80SizeType oper_size = guess_operation_size(dst, src2);

	// lets transform the eagle variable to a more usable datatype.
	// Knowing the size of the operation, the immediate values can be
	// ajusted to match.
	Z80Evaluable &dstEv	 = this->get_from_eagle_var(dst, oper_size);
	Z80Evaluable &src1Ev = this->get_from_eagle_var(src1, oper_size);
	Z80Evaluable &src2Ev = this->get_from_eagle_var(src2, oper_size);

	OpType type;
	if (opcode_map.find(operator1) == opcode_map.end())
	{
		die("unsupported operator: " + operator1 + '\n');
	}

	type	  = opcode_map.at(operator1);
	text_code = this->translate(type, dstEv, src1Ev, src2Ev);

	// reset the data type pools.
	this->value_pool_pos	= 0;
	this->location_pool_pos = 0;
	this->set_result_register(null_register);

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

	// if (operator1 == '+')
	// 	mnemonic = "add ";

	// if (operator1 == '-')
	// 	mnemonic = "sub ";

	// if (operator1 == '&')
	// 	mnemonic = "and ";

	// if (operator1 == '|')
	// 	mnemonic = "or ";

	// if (operator1 == '^')
	// 	mnemonic = "xor ";

	// if (operator1 == '<')
	// 	mnemonic = "sla ";

	// if (operator1 == '>')
	// 	mnemonic = "sra ";

	// if (operator1 == '*')
	// {
	// }

	// if (operator1 == '/')
	// {
	// }

	// text_code += "ld a," + src1value + "\n";
	// text_code += "ld b," + src2value + "\n";

	// text_code += mnemonic + reg + "\n";

	// text_code += "ld " + dstvalue + ",a\n";

	return text_code;
}
