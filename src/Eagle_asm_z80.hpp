#pragma once

// Forward declarations --------------------------------------------------------

typedef struct EagleVariable EAGLE_VARIABLE;
class Eagle;


// Types -----------------------------------------------------------------------

// EagleZ80 - assembly code generation for Z80
class EagleAsmZ80 {
	public:
		EagleAsmZ80(Eagle &eagle, int &ilabel);

		// initialize the Z80 translation engine. Resets all the internal states.
		void initialize(void);

		// asm_return_z80 translates a return operation
		void asm_return(const EAGLE_VARIABLE &ret, bool retvoid);

		// asm_alu_z80 translates an ALU operation
		void asm_alu(const EAGLE_VARIABLE &dst,
						const EAGLE_VARIABLE &src1,
						const EAGLE_VARIABLE &src2,
						const char operator1,
						const char operator2);

		// asm_bru_z80 translates a local control structures, IF, WHILE, DO etc.
		void asm_bru(const EAGLE_VARIABLE &src1,
						const EAGLE_VARIABLE &src2,
						const char operator1,
						const char operator2,
						int type,
						int clabel);

		// asm_call_jump_z80 translates a CALL or JMP operation
		void asm_call_jump(const EAGLE_VARIABLE &var,
							int narg,
							int type,
							std::string &labelcall);

	private:
		Eagle &eagle;
		// temporary hack, needs to be discussed
		int &ilabel;
};
