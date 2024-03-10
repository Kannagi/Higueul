#include <stdint.h>

#ifndef _eagle
#define _eagle

#define EAGLA_MAX_ID  32
#define EAGLA_MAX_LEX 128

//acc/mem = acc/mem/imm + imm

typedef struct
{
	uint64_t imm;
	double  immf;
	uint32_t  id;
    uint8_t  arg;

}EAGLE_OPE;

typedef struct
{
	std::vector < EAGLE_OPE > ope;

}EAGLE_INST;

typedef struct
{
    char word[64];
    uint32_t struct_id,offset;
    uint8_t type,type2,scope,mem;

}EAGLE_VAR;

typedef struct
{
    char word[64];
    uint32_t org,nvar,offset,ninst;
    uint8_t type,type2,scope,mem,narg;

    std::vector < EAGLE_VAR  > var;
    std::vector < EAGLE_INST > inst;

}EAGLE_FUNC;

typedef struct
{
	uint32_t id[EAGLA_MAX_ID];
    uint8_t lex[EAGLA_MAX_LEX];

}EAGLE_OBJ;


typedef struct
{
    uint8_t *buffer;
    EAGLE_OBJ obj;
	EAGLE_FUNC s_func;
	EAGLE_VAR s_var;
	EAGLE_INST s_inst;

    char word[64];

    uint32_t vi,fi;
    uint32_t n,nw;
    uint32_t i,o,com,nocom,tcom,inst,error,line;
    uint32_t nvar,nfunction;
    uint32_t nbuffer;
    uint8_t errorc,mode,read,read2;

    bool preproc;


	std::vector < uint8_t > lexicon;
	std::vector < EAGLE_VAR > var;
	std::vector < EAGLE_FUNC > func;

}EAGLE;

void EAGLE_Analyse_lexical(EAGLE &eagle);
void EAGLE_Analyse_syntactic(EAGLE &eagle);

void EAGLE_func_init(EAGLE_FUNC &s_func);
void EAGLE_var_init (EAGLE_VAR  &s_var );
void EAGLE_inst_init(EAGLE_INST &s_inst);

void EAGLE_word(EAGLE &eagle,char *word,uint32_t j,uint32_t n);

void EAGLE_Analyse_syntactic_obj_func(EAGLE &eagle,uint32_t xi);
void EAGLE_Analyse_syntactic_obj_var (EAGLE &eagle,uint32_t xi);
void EAGLE_Analyse_syntactic_obj_inst(EAGLE &eagle,uint32_t xi);

#define EAGLE_VOID 0xFF

#define EAGLE_NOT_NUMBER(word) if( (word[0] >= '0') && (word[0] <= '9') ) {eagle.error = 100; return;}
#define EAGLE_IF_NUMBER(word) (word[0] >= '0') && (word[0] <= '9')
enum {
DIRECTIVES_db,
DIRECTIVES_dw,
DIRECTIVES_dl,
DIRECTIVES_dq,
DIRECTIVES_define,
DIRECTIVES_macro,
DIRECTIVES_endmacro,
DIRECTIVES_include,
DIRECTIVES_incbin,
DIRECTIVES_bank,
DIRECTIVES_memoryd,
DIRECTIVES_memorymap,
DIRECTIVES_org,
DIRECTIVES_code,
DIRECTIVES_rodata,
DIRECTIVES_data,
DIRECTIVES_bss,
DIRECTIVES_spm,
};

enum {
//type
KEYWORD_i8 = 0,
KEYWORD_i16,
KEYWORD_i32,
KEYWORD_i64,
KEYWORD_u8,
KEYWORD_u16,
KEYWORD_u32,
KEYWORD_u64,
KEYWORD_f16,
KEYWORD_f32,
KEYWORD_f64,

KEYWORD_ptr,

KEYWORD_vec2,
KEYWORD_vec3,
KEYWORD_vec4,

//optimisation
KEYWORD_uregister,
KEYWORD_sregister,
KEYWORD_spm,
KEYWORD_stack,
KEYWORD_memory1,
KEYWORD_memory2,
KEYWORD_memory3,

KEYWORD_extern,
KEYWORD_static,

//control
KEYWORD_if,
KEYWORD_and,
KEYWORD_or,
KEYWORD_else,
KEYWORD_do,
KEYWORD_while,
KEYWORD_for,
KEYWORD_jump,
KEYWORD_call,
KEYWORD_return,
KEYWORD_break,
KEYWORD_continue,

//defintion
KEYWORD_func,
KEYWORD_sizeof,
KEYWORD_struct,
KEYWORD_enum,


//other asm
KEYWORD_flush,
KEYWORD_optimize,
KEYWORD_asm,
KEYWORD_acc,
KEYWORD_acc2,
KEYWORD_accf,
};



enum {
LEXICON_TOKEN = 160,
LEXICON_WORD = 190,
LEXICON_ARG = 80,
LEXICON_LOCAL = 81,
LEXICON_LINE = 60,
};

enum {
TOKEN_exclamation = LEXICON_TOKEN, // !
TOKEN_quotation,   //"
TOKEN_preproc, // #
TOKEN_dollar, //$
TOKEN_percent, //%
TOKEN_AND, //&
TOKEN_apostrophe,// '
TOKEN_parenthesesleft, //(
TOKEN_parenthesesright, //)
TOKEN_asterisk, // *
TOKEN_plus, //+
TOKEN_comma, //,
TOKEN_minus, // -
TOKEN_dot, // .
TOKEN_slash, // /

TOKEN_colon = LEXICON_TOKEN+15, // :
TOKEN_dot2, // ;
TOKEN_lessthan, // <
TOKEN_equal, // =
TOKEN_greatherthan, // >
TOKEN_question, // ?

TOKEN_bracketleft = LEXICON_TOKEN+15+6, //[
TOKEN_backslash, // \  .
TOKEN_bracketright, //]
TOKEN_caret, //^

TOKEN_singletonleft = LEXICON_TOKEN+15+6+4, //{
TOKEN_OR, // |
TOKEN_singletonright, // }
TOKEN_tilde, // ~
//TOKEN_

};



enum {
ERROR_TOKEN = 1,
ERROR_WORD,
ERROR_PREPROC,

ERROR_STRINGD,
ERROR_FUNC,
ERROR_FUNC2,
ERROR_FUNCVAR,
ERROR_VAR,
ERROR_VAR2,
ERROR_INST,
ERROR_CALL,
ERROR_RET,
ERROR_FLUSH,
ERROR_FILE = -1,
};

#endif
