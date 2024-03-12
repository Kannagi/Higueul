
#pragma once

#include "Eagle_enum.hpp"

typedef struct
{
	int64_t value;
	EAGLE_keywords type;
	bool bimm;



}EAGLE_VARIABLEP;

typedef struct
{
	int64_t immediate;
	double dimmediate;
	int address;
	EAGLE_keywords type;
	char token1,token2;
	bool bimm;
	bool blabel;
	bool bptr;

	EAGLE_VARIABLEP ptr1,ptr2;
	EAGLE_keywords ptr_type;

}EAGLE_VARIABLE;

typedef struct
{
	std::string item;
	std::string ptr[4];
	EAGLE_keywords ktype;
	int ptype[4];
	int type;
	int scope;
	int line,col,pn;
	char token1,token2;
	bool label;



}EAGLE_WORDS;

class Eagle_func
{
	public:
		std::string name;

		EAGLE_keywords type;
		int narg;
		int alloc;
		int address;

		int begid;
		int endid;
		bool retauto;

		void clear(void)
		{
			this->narg = 0;
			this->type = EAGLE_keywords::UNKNOW;
			this->alloc = 0;
			this->address = 0;

			this->begid = 0;
			this->endid = 0;
			this->retauto = true;
		}
};

typedef struct
{
	EAGLE_keywords type[8];

}EAGLE_DFUNC;

typedef struct
{
	bool exist;
	bool macro;
	std::string text;
	std::string arg[8];

}EAGLE_DEFINE;

typedef struct
{
	std::string item;
	int64_t value;
	int type;

	char token0,token1,token2;


}EAGLE_MNEMONIQUE;

class Eagle
{
	public:
		Eagle(void);
		void parser_word(void);
		void load_file(const char *path);
		void write_file(const char *path,std::string text);
		void write_file_bin(const char *path);
		void out_asm(void);

		void bin_6502(void);
		void bin_65816(void);
		void bin_HuC6280(void);
		void bin_Altaitx(void);
		void bin_80286(void);
		void bin_x86_64(void);
		void bin_z80(void);

		std::string text_code;
		bool debug;
		int error;

	private:
		bool isWord(char c);
		bool isOperator(char c,char c2,int &i);
		bool isNumber(char c);
		bool isAlphabetic(char c);

		bool isOperator_move(char c,char c2);
		bool isOperator_cmp(char c,char c2);
		bool isOperator_calcul(char c,char c2);

		int isComment(char c1,char c2,int i);
		int isText(char c1,int i,EAGLE_WORDS &tword);
		int isConstantFolding(char c1,int i,EAGLE_WORDS &tword);
		void out_error(const EAGLE_WORDS tword,const std::string text);
		void load_file_bin(const char *path,std::vector<char> &data);

		int alloc(EAGLE_keywords type,int n);

		//---------ALL--------------
		void asm_return(const EAGLE_VARIABLE &ret,bool retvoid);
		void asm_alu(const EAGLE_VARIABLE &dst,const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2);
		void asm_falu(const EAGLE_VARIABLE &dst,const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2);

		void asm_bru(const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2,int type,int clabel);
		void asm_call_jump(const EAGLE_VARIABLE &var,const EAGLE_DFUNC dfunc,int narg,int type);

		//---------65816--------------
		void asm_return_65816(const EAGLE_VARIABLE &ret,bool retvoid);
		void asm_alu_65816(const EAGLE_VARIABLE &dst,const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2);

		void asm_bru_65816(const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2,int type,int clabel);
		void asm_call_jump_65816(const EAGLE_VARIABLE &var,const EAGLE_DFUNC dfunc,int narg,int type);

		//---------6502--------------
		void asm_return_6502(const EAGLE_VARIABLE &ret,bool retvoid);
		void asm_alu_6502(const EAGLE_VARIABLE &dst,const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2);

		void asm_bru_6502(const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2,int type,int clabel);
		void asm_call_jump_6502(const EAGLE_VARIABLE &var,const EAGLE_DFUNC dfunc,int narg,int type);

		//---------80186--------------
		void asm_return_80186(const EAGLE_VARIABLE &ret,bool retvoid);
		void asm_alu_80186(const EAGLE_VARIABLE &dst,const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2);

		void asm_bru_80186(const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2,int type,int clabel);
		void asm_call_jump_80186(const EAGLE_VARIABLE &var,const EAGLE_DFUNC dfunc,int narg,int type);

		//---------Huc6280--------------
		void asm_return_Huc6280(const EAGLE_VARIABLE &ret,bool retvoid);
		void asm_alu_Huc6280(const EAGLE_VARIABLE &dst,const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2);

		void asm_bru_Huc6280(const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2,int type,int clabel);
		void asm_call_jump_Huc6280(const EAGLE_VARIABLE &var,const EAGLE_DFUNC dfunc,int narg,int type);

		//---------z80--------------
		void asm_return_z80(const EAGLE_VARIABLE &ret,bool retvoid);
		void asm_alu_z80(const EAGLE_VARIABLE &dst,const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2);

		void asm_bru_z80(const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2,int type,int clabel);
		void asm_call_jump_z80(const EAGLE_VARIABLE &var,const EAGLE_DFUNC dfunc,int narg,int type);

		//---------AltairX--------------
		void asm_return_AltairX(const EAGLE_VARIABLE &ret,bool retvoid);
		void asm_alu_AltairX(const EAGLE_VARIABLE &dst,const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2);
		void asm_falu_AltairX(const EAGLE_VARIABLE &dst,const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2);

		void asm_mul_AltairX(const EAGLE_VARIABLE &dst,const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2);
		void asm_div_AltairX(const EAGLE_VARIABLE &dst,const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2);

		void asm_bru_AltairX(const EAGLE_VARIABLE &src1,const EAGLE_VARIABLE &src2,const char operator1,const char operator2,int type,int clabel);
		void asm_call_jump_AltairX(const EAGLE_VARIABLE &var,const EAGLE_DFUNC dfunc,int narg,int type);

		bool variable_exist(EAGLE_WORDS tword,EAGLE_VARIABLE &var,int elabel);

		void convertStringToNumber(std::string str,int64_t &result,double &dresult);

		int line_code_asm(int mode);

		std::map<std::string, EAGLE_keywords> keywords;
		std::map<std::string, EAGLE_VARIABLE> variable;
		std::map<std::string, EAGLE_VARIABLE> gvariable;
		std::map<std::string, EAGLE_DFUNC> dfunc;
		std::map<std::string, bool> label;


		std::map<std::string, EAGLE_DEFINE> define;

		std::vector<std::vector<EAGLE_WORDS>> instructions;
		std::vector<EAGLE_MNEMONIQUE> mnemonic;

		std::string kmacro[8];

		std::string filetext;
		std::vector<char> filebin;
		std::map<std::string, int> labelbin;

		std::string label1,label2;

		EAGLE_VARIABLE arg[8];
		int mmap;
		EAGLE_DEFINE tmacro;


		int idf,sizebin,offset;

		int line,col;
		int func_alloc;
		int mode_alloc;
		int target;
		int ilabel;

		int func_address;
		int wram_address;

		int spm_address;
		int lib_address;
		int funcspm_address;
		int funclib_address;

		int spm_init;
		int lib_init;
		int funcspm_init;
		int funclib_init;

		int register_address;
		int stack_address;

		int scope_label[128];
};

class Constant_folding
{
    public:

    static const int CAPACITY = 64;

    Constant_folding();
    void evaluate(const std::string &text);
    void display();

    int64_t result;
    double fresult;

    private:

    double fstack[Constant_folding::CAPACITY];
    int64_t stack[Constant_folding::CAPACITY];
    char operators[Constant_folding::CAPACITY];


    int8_t stack_top;
    int8_t operator_top;
    int8_t stack_capacity;
    bool hexa,integer;
    std::string text;

    //---
    bool isFull();
    bool isEmpty();
    void push(int64_t item,double fitem);
    int64_t pop();

    bool isFull_operator();
    bool isEmpty_operator();
    void push_operator(char c);
    char pop_operator();
    char peek_operator();
    char precedence(char ope);

    bool isOperator(char c);
    bool isNumber(char c);
    bool isNumberDec(char c);
    bool isNumberBin(char c);
    bool isNumberHex(char c);
    int64_t applyOperation(int64_t a, int64_t b, char op);
    double fapplyOperation(double a, double b, char op);

    void apply_operation();
};

enum
{
	ALLOC_UNK,
	ALLOC_WRAM,
	ALLOC_SPM,
	ALLOC_FRAM,
	ALLOC_FSPM,
	ALLOC_FLIB,
	ALLOC_REGISTER,
	ALLOC_STACK,
	ALLOC_LIB,
};

enum
{
	TARGET_UNK,

	TARGET_AltairX,

	TARGET_6502,
	TARGET_65816,
	TARGET_HuC6520,

	TARGET_Z80,

	TARGET_80286,
	TARGET_x64,

	TARGET_RV32,
};
enum
{
	TYPE_UNK,
	TYPE_NUMBER,
	TYPE_WORD,
	TYPE_OPERATOR,
	TYPE_DATASTR,
	TYPE_CONSTANT,
	TYPE_LABEL,
	TYPE_PTR,
	TYPE_END,
};
