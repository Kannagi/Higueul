
#pragma once

// Forward Declarations --------------------------------------------------------
enum Z80SizeType : uint16_t
{
	Z80_SIZE_UNKOWN = 0,
	Z80_SIZE_BYTE	= 1,
	Z80_SIZE_WORD	= 2
};

// Forward Declarations --------------------------------------------------------

class Z80;

// Types -----------------------------------------------------------------------
// -----------------------------------------------------------------------------
class Z80Evaluable
{
	public:
		virtual uint16_t get_value(void) const	  = 0;
		virtual std::string to_string(void) const = 0;
		virtual Z80SizeType get_size(void) const  = 0;
		virtual bool is_register(void) const	  = 0;
		virtual bool is_deferencing(void) const	  = 0;
		virtual OpFlag get_opflag(void) const	  = 0;
};

// -----------------------------------------------------------------------------
// Z80NullEvaluable implement null object pattern. This implementation is used
// when a reference is required, but it makes no sens to return something.
class Z80NullEvaluable : public Z80Evaluable
{
	public:
		uint16_t get_value(void) const override { return 0; }
		std::string to_string(void) const override { return "(null)"; }
		Z80SizeType get_size(void) const override { return Z80_SIZE_UNKOWN; }
		bool is_register(void) const override { return false; }
		OpFlag get_opflag(void) const override { return NOOPFLAGS; }
		bool is_deferencing(void) const override { return false; }
};

// -----------------------------------------------------------------------------
// Z80Location represent an evaluable for a immediate memory location.
class Z80Location : public Z80Evaluable
{
	public:
		Z80Location(void) : value(0), oper_size(Z80_SIZE_UNKOWN) {}
		uint16_t get_value(void) const override { return this->value; }
		std::string to_string(void) const override
		{
			return "(" + std::to_string(this->value) + ")";
		}
		Z80SizeType get_size(void) const override { return Z80_SIZE_WORD; }
		bool is_register(void) const override { return false; }
		OpFlag get_opflag(void) const override
		{
			return this->oper_size == Z80_SIZE_BYTE ? IMM_PTR8 : IMM_PTR16;
		}
		bool is_deferencing(void) const override { return true; }
		void set_address(uint16_t address, Z80SizeType oper_size)
		{
			this->value		= address;
			this->oper_size = oper_size;
		}

	private:
		uint16_t value;
		Z80SizeType oper_size;
};

// -----------------------------------------------------------------------------
class Z80Value : public Z80Evaluable
{
	public:
		Z80Value(void) : value(0U), is16bit(false) {}

		uint16_t get_value(void) const override { return this->value; }
		std::string to_string(void) const override
		{
			return std::to_string(this->value);
		}
		Z80SizeType get_size(void) const override
		{
			return (this->is16bit) ? Z80_SIZE_WORD : Z80_SIZE_BYTE;
		}
		bool is_register(void) const override { return false; }
		OpFlag get_opflag(void) const override { return IMM; };
		bool is_deferencing(void) const override { return false; }

		void set_value(uint16_t value)
		{
			this->value	  = value;
			this->is16bit = true;
		}
		void set_value(uint8_t value)
		{
			this->value	  = (uint16_t)value;
			this->is16bit = false;
		}

	private:
		uint16_t value;
		bool is16bit;
};

struct Z80LoadResult
{
		int16_t delta;
		bool changed : 1;
		bool was_undef : 1;
};

// -----------------------------------------------------------------------------
// Z80 Registers Virtual. Since there 3 types of registers on the Z80, this
// interface smooths, through polymorphism, the handling of the different types
// of registers.
class Z80Register : public Z80Evaluable
{
	public:
		// get_name of the register
		virtual std::string get_name(void) const = 0;
		// is_undef is true when the register internal value isn't constant
		// or irrelevant
		virtual bool is_undef(void) const = 0;
		// set_undef the register
		virtual void set_undef(void) = 0;
		// load_value in the register. The method returns the magnitude of the
		// change. If the previous value isn't the same as the newer one,
		// or the register is undef, false is returned.
		virtual Z80LoadResult load_value(uint16_t value) = 0;
		// get_value of the register (I don't thing this method is going to be
		// used).
		virtual uint16_t get_value(void) const = 0;
		// to_string representation of the register. If the register is a
		// memory location, it will be wrapped in parentheses.
		virtual std::string to_string(void) const = 0;
		// get_size of the register.
		virtual Z80SizeType get_size(void) const = 0;

		virtual bool is_register(void) const = 0;

		virtual OpFlag get_opflag(void) const = 0;

	protected:
		enum State
		{
			NUMBER,
			ADDRESS
		};
};

// -----------------------------------------------------------------------------
// Z80Register8 is a 8-bit register. This object keeps tracks of the last
// value it has been assigned. If the register isn't in its undefined state and
// it's written to, the register returns if its value changed.
class Z80Register8 : public Z80Register
{
	public:
		Z80Register8(std::string name, OpFlag opflag)
			: name(name), value(0), state(NUMBER), opflag(opflag), undef(true)
		{
		}

		uint16_t get_value(void) const override { return (uint16_t)value; }

		bool is_undef(void) const override { return this->undef; }

		Z80LoadResult load_value(uint16_t value) override;

		void set_undef(void) override { this->undef = true; }

		bool is_register(void) const override { return true; }

		bool is_deferencing(void) const override
		{
			return this->state == ADDRESS;
		}

		Z80SizeType get_size(void) const override { return Z80_SIZE_BYTE; }

		std::string get_name(void) const override { return name; }

		std::string to_string(void) const override;

		OpFlag get_opflag(void) const override
		{
			switch (this->state)
			{
			case NUMBER:
				return static_cast<OpFlag>(this->opflag | REG);
			case ADDRESS:
				return static_cast<OpFlag>(this->opflag | REG_PTR);
			default:
				return NOOPFLAGS;
			}
		};

	private:
		std::string name;
		uint8_t value;
		State state;
		OpFlag opflag;
		bool undef;
};

// -----------------------------------------------------------------------------
// Z80Indexer is a 16-bit register. This object keeps tracks of the last
// value it has been assigned. If the register isn't in its undefined state and
// it's written to, the register returns if its value changed.
class Z80Indexer : public Z80Register
{
	public:
		Z80Indexer(std::string name, OpFlag opflag)
			: name(name), value(0), state(NUMBER), opflag(opflag), undef(true)
		{
		}

		uint16_t get_value(void) const override { return value; }

		bool is_undef(void) const override { return this->undef; }

		Z80LoadResult load_value(uint16_t value) override;

		void set_undef(void) override { this->undef = true; }

		bool is_register(void) const override { return true; }

		bool is_deferencing(void) const override
		{
			return this->state == ADDRESS;
		}

		Z80SizeType get_size(void) const override { return Z80_SIZE_WORD; }

		std::string get_name(void) const override { return name; }

		std::string to_string(void) const override;

		OpFlag get_opflag(void) const override
		{
			switch (this->state)
			{
			case NUMBER:
				return static_cast<OpFlag>(this->opflag | REG);
			case ADDRESS:
				return static_cast<OpFlag>(this->opflag | REG_PTR);
			default:
				return NOOPFLAGS;
			}
		};

	private:
		std::string name;
		uint16_t value;
		State state;
		OpFlag opflag;
		bool undef;
};

// -----------------------------------------------------------------------------
// Z80RegisterPair16 represents a pair of 16-bit registers. This object
// delegates its internal states to the sub Z80register8 objects.
class Z80RegisterPair16 : public Z80Register
{
	public:
		Z80RegisterPair16(Z80Register8 &low, Z80Register8 &high, OpFlag opflag)
			: low(low), high(high), opflag(opflag), undef(true)
		{
			this->name = low.get_name() + high.get_name();
		}

		uint16_t get_value(void) const override;

		bool is_undef(void) const override;

		Z80LoadResult load_value(uint16_t value) override;

		bool is_register(void) const override { return true; }

		bool is_deferencing(void) const override
		{
			return this->state == ADDRESS;
		}

		void set_undef(void) override
		{
			this->low.set_undef();
			this->high.set_undef();
			this->undef = true;
		}

		Z80SizeType get_size(void) const override { return Z80_SIZE_WORD; }

		std::string get_name(void) const override { return name; }

		std::string to_string(void) const override;

		OpFlag get_opflag(void) const override
		{
			switch (this->state)
			{
			case NUMBER:
				return static_cast<OpFlag>(this->opflag | REG);
			case ADDRESS:
				return static_cast<OpFlag>(this->opflag | REG_PTR);
			default:
				return NOOPFLAGS;
			}
		};

	private:
		std::string name;
		Z80Register8 &low, &high;
		State state;
		OpFlag opflag;
		bool undef;
};

const uint Z80_POOL_SIZE = 16U;

// -----------------------------------------------------------------------------
// EagleZ80 - assembly code generation for Z80
class CPU_Z80
{
	public:
		CPU_Z80(void);

		// initialize the Z80 translation engine. Resets all the internal
		// states.
		void initialize(void);

		// asm_return_z80 translates a return operation
		std::string asm_return(const EAGLE_VARIABLE &ret, bool retvoid);

		// asm_alu_z80 translates an ALU operation
		std::string asm_alu(const EAGLE_VARIABLE &dst,
							const EAGLE_VARIABLE &src1,
							const EAGLE_VARIABLE &src2, const char operator1,
							const char operator2);

		// asm_bru_z80 translates a local control structures, IF, WHILE, DO etc.
		std::string asm_bru(const EAGLE_VARIABLE &src1,
							const EAGLE_VARIABLE &src2, const char operator1,
							const char operator2, int type, int clabel,
							int &ilabel);

		// asm_call_jump_z80 translates a CALL or JMP operation
		std::string asm_call_jump(const EAGLE_VARIABLE &var, int narg, int type,
								  std::string &labelcall);

		void asm_do_else(void);

		// General purpose registers
		Z80Register8 A, B, C, D, E, H, L;
		// Flags register
		Z80Register8 F;
		// 16-bit pair registers
		Z80RegisterPair16 BC, DE, HL;
		// 16-bit index registers
		Z80Indexer IX, IY;

	private:
		std::map<std::string, Z80Register *> registers;

		// I dont know why std::vector segfault on me. I don't love C++ enough
		// to find why and their standard library is just unreadable.
		Z80Value value_pool[Z80_POOL_SIZE];
		Z80Location location_pool[Z80_POOL_SIZE];
		uint8_t location_pool_pos;
		uint8_t value_pool_pos;

		Z80Evaluable &new_value(const EAGLE_VARIABLE &var,
								Z80SizeType oper_size);
		Z80Evaluable &new_location(const EAGLE_VARIABLE &var,
								   Z80SizeType oper_size);

		// from Eagle_VARIABLE to Z80Evaluable. This function guess the type and
		// returns an appropriate Z80Evaluable object.
		Z80Evaluable &from(const EAGLE_VARIABLE &var, Z80SizeType oper_size);

		// when exiting a continuous block of non structural instruction, this
		// function is called to ensure that all registers are marked as
		// undefined and can no further be optimizedvoid
		void exit_block(void);
};
