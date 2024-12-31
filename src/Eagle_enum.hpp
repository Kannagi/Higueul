#pragma once

#define TYPE_LOOP 0
#define TYPE_WHILE 1
#define TYPE_IF 2

enum class EAGLE_keywords : unsigned char
{
	UNKNOW,

	// Variable
	INT8,
	INT16,
	INT32,
	INT64,

	UINT8,
	UINT16,
	UINT32,
	UINT64,

	FLOAT16,
	FLOAT32,
	FLOAT64,

	VFLOAT,
	VINT,

	VINT8x2,
	VINT16x2,
	VINT32x2,
	VINT64x2,

	VINT8x4,
	VINT16x4,
	VINT32x4,

	VINT8x8,
	VINT16x8,

	VFLOAT16x2,
	VFLOAT32x2,
	VFLOAT64x2,

	VFLOAT16x4,
	VFLOAT32x4,

	VFLOAT16x8,

	VOID,

	REGISTER,
	SPM,
	LIB,
	STACK,

	EXTERN,

	// Control
	IF,
	ELSE,
	DO,
	WHILE,
	LOOP,
	JUMP,
	RETURN,
	CALL,

	// Function
	FUNC,
	FUNCSPM,
	FUNCLIB,
	FUNCREG,
	PROC,
	END,

	// extra
	ASM,
	ACC,
	REGB,
	REGC,
	REGD,
	REGE,
	REGF,
	REGBC,
	REGDE,
	IDHL,
	IDX,
	IDY,
	FACC,

	// preproc
	DEFINE,
	MACRO,
	ENDMACRO,

	INCBIN,
	CODE,
	RODATA,
	BSS,
	FUNCMAP,
	FUNCMAPLIB,
	FUNCMAPSPM,
	SPMMAP,
	LIBMAP,
	ORG,

	// DATA ROW
	DATAB,
	DATAW,
	DATAL,
	DATAQ,
	DATAS,
	DATAH,
	DATAF,
	DATAD,

	PTR,
};

// Enumerates the different Z80 operations
enum OpType : uint64_t
{
	OP_LOAD,
	OP__MAX,
};

// Enumerates the properties of a Z80 Operand. An operand might be a register,
// a memory location or an immediate value. These properties are used to build
// the opcode index table.
enum OpFlag : uint32_t
{
	NOOPFLAGS = 0,
	// Immediate value flag
	IMM		  = 1 << 0,
	REG		  = 1 << 1,
	REG_PTR	  = 1 << 2,
	IMM_PTR8  = 1 << 3,
	IMM_PTR16 = 1 << 4,

	// Indexable flag, memory location may be indexed, works only with IX, IY
	INDEXED = 1 << 5,
	// Register flags
	R_A = 1 << 8,
	R_B = 1 << 9,
	R_C = 1 << 10,
	R_D = 1 << 11,
	R_E = 1 << 12,
	R_F = 1 << 13,
	R_H = 1 << 14,
	R_L = 1 << 15,
	RBC = 1 << 16,
	RDE = 1 << 17,
	RHL = 1 << 18,
	RIX = (1 << 19),
	RIY = (1 << 20),
	RSP = 1 << 21,

	REG8  = (R_A | R_B | R_C | R_D | R_E | R_H | R_L),
	REG16 = (RBC | RDE | RHL),
	REGXY = (RIX | RIY),
};

inline OpFlag operator|(OpFlag a, OpFlag b)
{
	return static_cast<OpFlag>(static_cast<uint64_t>(a) |
							   static_cast<uint64_t>(b));
}
