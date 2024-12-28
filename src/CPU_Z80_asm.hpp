
#pragma once

// Forward Declarations --------------------------------------------------------
enum
{
	Z80_SIZE_BYTE = 1,
	Z80_SIZE_WORD = 2
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
		virtual size_t get_size(void) const		  = 0;
		virtual bool is_register(void) const	  = 0;
};

// -----------------------------------------------------------------------------
class Z80Location : public Z80Evaluable
{
	public:
		Z80Location(uint16_t value) : value(value) {}
		uint16_t get_value(void) const override { return this->value; }
		std::string to_string(void) const override
		{
			return "(" + std::to_string(this->value) + ")";
		}
		size_t get_size(void) const override { return Z80_SIZE_WORD; }
		bool is_register(void) const override { return false; }

	private:
		uint16_t value;
};

// -----------------------------------------------------------------------------
class Z80Value : public Z80Evaluable
{
	public:
		Z80Value(uint16_t value) : value((uint16_t)value), is16bit(true) {}
		Z80Value(uint8_t value) : value((uint8_t)value), is16bit(false) {}

		uint16_t get_value(void) const override { return this->value; }
		std::string to_string(void) const override
		{
			return std::to_string(this->value);
		}
		size_t get_size(void) const override
		{
			return (this->is16bit) ? Z80_SIZE_WORD : Z80_SIZE_BYTE;
		}
		bool is_register(void) const override { return false; }

	private:
		uint16_t value;
		bool is16bit;
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
		// load_immediate value in the register. If the value changes, it
		// returns true. If the previous value isn't the same as the newer one,
		// or the register is undef, false is returned.
		virtual bool load_immediate(uint16_t value) = 0;
		// load_address value in the register. It behave exactly the same way
		// as load_immediate, but when `to_string` is called, the formatting is
		// different. If the address is the same value as a previously loaded
		// immediate, or the inverse, the change detection works as expected.
		//
		// Notice, 8bit registers do not support address loading.
		virtual bool load_address(uint16_t address) = 0;
		// get_value of the register (I don't thing this method is going to be
		// used).
		virtual uint16_t get_value(void) const = 0;
		// to_string representation of the register. If the register is a
		// memory location, it will be wrapped in parentheses.
		virtual std::string to_string(void) const = 0;
		// get_size of the register.
		virtual size_t get_size(void) const = 0;

		virtual bool is_register(void) const = 0;

	protected:
		enum State
		{
			UNDEFINED,
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
		Z80Register8(std::string name) : name(name), value(0), state(UNDEFINED)
		{
		}

		uint16_t get_value(void) const override { return (uint16_t)value; }

		bool is_undef(void) const override { return this->state == UNDEFINED; }

		bool load_immediate(uint16_t value) override;

		bool load_address(uint16_t address) override;

		void set_undef(void) override { this->state = UNDEFINED; }

		bool is_register(void) const override { return true; }

		size_t get_size(void) const override { return Z80_SIZE_BYTE; }

		std::string get_name(void) const override { return name; }

		std::string to_string(void) const override;

	private:
		std::string name;
		uint8_t value;
		State state;
};

// -----------------------------------------------------------------------------
// Z80Indexer is a 16-bit register. This object keeps tracks of the last
// value it has been assigned. If the register isn't in its undefined state and
// it's written to, the register returns if its value changed.
class Z80Indexer : public Z80Register
{
	public:
		Z80Indexer(std::string name) : name(name), value(0), state(UNDEFINED) {}

		uint16_t get_value(void) const override { return value; }

		bool is_undef(void) const override { return this->state == UNDEFINED; }

		bool load_immediate(uint16_t value) override;

		bool load_address(uint16_t address) override;

		void set_undef(void) override { this->state = UNDEFINED; }

		bool is_register(void) const override { return true; }

		size_t get_size(void) const override { return Z80_SIZE_WORD; }

		std::string get_name(void) const override { return name; }

		std::string to_string(void) const override;

	private:
		std::string name;
		uint16_t value;
		State state;
};

// -----------------------------------------------------------------------------
// Z80RegisterPair16 represents a pair of 16-bit registers. This object
// delegates its internal states to the sub Z80register8 objects.
class Z80RegisterPair16 : public Z80Register
{
	public:
		Z80RegisterPair16(Z80Register8 &low, Z80Register8 &high)
			: low(low), high(high)
		{
			this->name = low.get_name() + high.get_name();
		}

		uint16_t get_value(void) const override;

		bool is_undef(void) const override;

		bool load_immediate(uint16_t value) override;

		bool load_address(uint16_t value) override;

		bool is_register(void) const override { return true; }

		void set_undef(void) override
		{
			this->low.set_undef();
			this->high.set_undef();
			this->state = UNDEFINED;
		}

		size_t get_size(void) const override { return Z80_SIZE_WORD; }

		std::string get_name(void) const override { return name; }

		std::string to_string(void) const override;

	private:
		std::string name;
		Z80Register8 &low, &high;
		State state;
};

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

	private:
		std::map<std::string, Z80Register *> registers;

		// General purpose registers
		Z80Register8 A, B, C, D, E, H, L;
		// Flags register
		Z80Register8 F;
		// 16-bit pair registers
		Z80RegisterPair16 AF, BC, DE, HL;
		// 16-bit index registers
		Z80Indexer IX, IY;

		std::vector<Z80Value> value_pool;
		std::vector<Z80Location> location_pool;

		Z80Evaluable &new_value(const EAGLE_VARIABLE &var);
		Z80Evaluable &new_location(const EAGLE_VARIABLE &var);

		// from Eagle_VARIABLE to Z80Evaluable. This function guess the type and
		// returns an appropriate Z80Evaluable object.
		Z80Evaluable &from(const EAGLE_VARIABLE &var);

		// returns a register by its name
		Z80Register &get_reg(std::string name) const
		{
			if (registers.find(name) == registers.end())
			{
				std::cerr << "Error: Register '" << name << "' not found."
						  << std::endl;
				exit(1);
			}

			return *registers.at(name);
		}

		// when exiting a continuous block of non structural instruction, this
		// function is called to ensure that all registers are marked as
		// undefined and can no further be optimizedvoid
		void exit_block(void);
};
