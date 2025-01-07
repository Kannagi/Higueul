#include <string>
#include <iostream>
#include <map>
#include <cstdint>
#include <vector>

#include "Eagle.hpp"
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

static const size_t KEY_STRIDE = 24;

static int64_t make_z80_key(OpType type, OpFlag dst, OpFlag src);

static const std::string op_strings[OP__MAX] = {
	"ld",
	"<invalid>",
	"add",
};

std::string explain_key(uint64_t flags)
{
	std::string result;

	uint64_t oper_type = flags & 0xF;

	if (oper_type == IMM8)
		result += "<immediate8>";
	if (oper_type == IMM16)
		result += "<immediate16>";
	if (oper_type == IMM_VAR8)
		result += "<var8>";
	if (oper_type == IMM_VAR16)
		result += "<var16>";
	if (oper_type == IMM_PTR16)
		result += "<(var16)>";
	if (oper_type == IMM_PTR8)
		result += "<(var8)>";

	if (oper_type == REG || oper_type == REG_PTR)
	{
		result += "<reg: ";
		if (oper_type == REG_PTR)
			result += "(";

		if (flags & R_A)
			result += "A";
		if (flags & R_B)
			result += "B";
		if (flags & R_C)
			result += "C";
		if (flags & R_D)
			result += "D";
		if (flags & R_E)
			result += "E";
		if (flags & R_F)
			result += "F";
		if (flags & R_H)
			result += "H";
		if (flags & R_L)
			result += "L";
		if (flags & RBC)
			result += "BC";
		if (flags & RDE)
			result += "DE";
		if (flags & RHL)
			result += "HL";
		if (flags & RIX)
			result += "IX";
		if (flags & RIY)
			result += "IY";
		if (flags & RSP)
			result += "SP";
		if (flags & INDEXED)
			result += "+indexed";
		if (oper_type == REG_PTR)
			result += ")";
		result += ">";
	}
	return result;
}

translator_fn Z80OpcodeIndex::get_translator(int64_t key)
{
	if (this->table.find(key) == this->table.end())
	{
		std::string msg =
			"No such operation : " + op_strings[get_z80_optype_from_key(key)] +
			" " + explain_key(key) + ", " + explain_key(key >> KEY_STRIDE) +
			" [ key=" + std::to_string(key) + " ]";
		die(msg);
	}

	return this->table[key];
}
// index a new opcode class variant. A new opcode class is defined by
// an Operation Type, destination registers and source registers. This
// index when hitting a key, is attached to a translation function.
//
// The function looks complicated, but it just takes the destination
// flags and source flags and builds all permutations of register
// combinations.
void Z80OpcodeIndex::index(OpType type, OpFlag dst, OpFlag src, translator_fn f)
{
	uint32_t d_trans  = dst & 0xff;
	uint32_t d_notreg = (d_trans & 0xF) < REG;

	uint32_t s_trans  = src & 0xff;
	uint32_t s_notreg = (s_trans & 0xF) < REG;

	// std::cout << explain_key(dst) << std::endl;
	// std::cout << explain_key(src) << std::endl;
	uint32_t d_flag		  = d_notreg ? NOOPFLAGS : R_A;
	uint32_t max_dst_iter = d_notreg ? 1 : 24;

	for (uint v = 0U; v < max_dst_iter; ++v, d_flag <<= 1)
	{
		if (!(d_flag & dst) && !d_notreg)
		{
			continue;
		}

		uint32_t s_flag = R_A;

		if (s_notreg)
		{
			int64_t final = make_z80_key(type, (OpFlag)(d_flag | d_trans),
										 (OpFlag)(s_trans));
			// std::cout << "indexing : " << type << explain_key(d_flag |
			// d_trans)
			// 		  << ", " << explain_key(s_flag | s_trans) << " = " << final
			// << std::endl;
			this->table[final] = f;
		}
		else
		{
			for (uint u = 0U; u < 24; ++u, s_flag <<= 1)
			{
				if (!(s_flag & src) && !s_notreg)
				{
					continue;
				}
				int64_t final = make_z80_key(type, (OpFlag)(d_flag | d_trans),
											 (OpFlag)(s_flag | s_trans));
				// std::cout << "indexing : " << type
				// 		  << explain_key(d_flag | d_trans) << ", "
				// 		  << explain_key(s_flag | s_trans) << " = " << final <<
				// std::endl;
				this->table[final] = f;
			}
		}
	}
}

static int64_t make_z80_key(OpType type, OpFlag dst, OpFlag src)
{
	return ((uint64_t)dst) | (((uint64_t)src) << KEY_STRIDE) |
		   (((uint64_t)type) << (KEY_STRIDE * 2U));
}

int64_t make_z80_opkey(OpType type, Z80Evaluable &dst, Z80Evaluable &src)
{
	if (!dst.exists())
	{
		die("Destination does not exist");
	}
	if (!src.exists())
	{
		die("Source does not exist");
	}

	OpFlag flg_dst = dst.get_opflag();
	OpFlag flg_src = src.get_opflag();

	return make_z80_key(type, flg_dst, flg_src);
}

uint64_t get_z80_optype_from_key(int64_t key)
{
	uint optype = key >> KEY_STRIDE * 2U;
	ensure(optype < OP__MAX, "key out of bounds");
	return optype;
}
