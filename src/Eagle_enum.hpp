#pragma once

enum class EAGLE_keywords : unsigned char
{
	UNKNOW,

	//Variable
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

    //Control
    IF,
    ELSE,
    DO,
    WHILE,
	LOOP,
    JUMP,
    RETURN,
    CALL,

    //Function
    FUNC,
    FUNCSPM,
    FUNCLIB,
    FUNCREG,
    PROC,
    END,

    //extra
    ASM,
    ACC,
    BACC,
    CACC,
    IDHL,
    IDX,
    IDY,
    FACC,

    //preproc
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


    //DATA ROW
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

