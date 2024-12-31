#pragma once

// Forward Declarations --------------------------------------------------------
class CPU_Z80;
class Z80Evaluable;

// Types -----------------------------------------------------------------------
typedef std::string (*translator_fn)(CPU_Z80 &cpu, Z80Evaluable &dst,
									 Z80Evaluable &src, Z80Evaluable &idx);

// Z80OpcodeIndex keeps an index of all possible Z80 operations related to
// translator functions.
class Z80OpcodeIndex
{
	public:
		translator_fn get_translator(int64_t key);
		void index(OpType type, OpFlag dst, OpFlag src, translator_fn f);

	private:
		std::map<int64_t, translator_fn> table;
};

int64_t make_z80_opkey(OpType type, OpFlag dst, OpFlag src);
uint64_t get_z80_optype_from_key(int64_t key);