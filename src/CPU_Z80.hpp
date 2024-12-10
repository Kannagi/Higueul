
#pragma once




// Types -----------------------------------------------------------------------

// EagleZ80 - assembly code generation for Z80
class CPU_Z80 {
	public:
		CPU_Z80(void);

		// initialize the Z80 translation engine. Resets all the internal states.
		void initialize(void);

		// asm_return_z80 translates a return operation
		std::string asm_return(const EAGLE_VARIABLE &ret, bool retvoid);

		// asm_alu_z80 translates an ALU operation
		std::string asm_alu(const EAGLE_VARIABLE &dst,
						const EAGLE_VARIABLE &src1,
						const EAGLE_VARIABLE &src2,
						const char operator1,
						const char operator2);

		// asm_bru_z80 translates a local control structures, IF, WHILE, DO etc.
		std::string asm_bru(const EAGLE_VARIABLE &src1,
						const EAGLE_VARIABLE &src2,
						const char operator1,
						const char operator2,
						int type,
						int clabel,
						int &ilabel);

		// asm_call_jump_z80 translates a CALL or JMP operation
		std::string asm_call_jump(const EAGLE_VARIABLE &var,
							int narg,
							int type,
							std::string &labelcall);

		void asm_do_else(void);

	private:

};
