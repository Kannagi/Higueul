# Higueul
Higueul is a low-level programming language

It has a syntax inspired by C and assembler.  
But don't think of it exactly as a C-like, but as a macro-assembler-like. 

There is no "stable" version yet, the language is currently being tested. 
I will release it when it is functional and bug-free.

Discord : https://discord.gg/cWa37SRgYU

# Keyword

### Data type :
- int8     : signed 8 bits
- int16    : signed 16 bits
- int32    : signed 32 bits
- int64    : signed 64 bits
- uint8    : unsigned 8 bits
- uint16   : unsigned 16 bits
- uint32   : unsigned 32 bits
- uint64   : unsigned 64 bits
- float16  : float half (16 bits)
- float32  : float single (32 bits)
- float64  : float double (64 bits)
- void     : function only


### Memory :
- register : register
- spm : ScratchPad Memory
- lib : Library Memory
- stack : stack

### control :
- if : to compare 2 values
- else
- do
- while : to compare 2 values
- loop : to compare 2 values
- jump
- call
- return

### definition function :
- func    : simple function
- funcspm : function which has these arguments in the spm
- funclib : function which has these arguments in the lib
- proc    : function that has no return

### definition memory mapping :
- .code        : define where the code starts in the memory map
- .rodata      : define where the rodata starts in the memory map
- .bss         : defined where your variables start in the memory map
- .funcmap     : defined where your function argument start in the memory map
- .funcmap.spm : defined where your function spm argument start in the memory map
- .funcmap.lib : defined where your function lib argument start in the memory map
- .map.spm     : defined where your spm start in the memory map
- .map.lib     : defined where your lib start in the memory map
- .org         : position your code

### definition  :
- .define   : defined a word to replace
- .macro    : create a macro with 8 possible arguments (.arg0 - .arg7)
- .endmacro : indicates the end of the macro
 
### data :
- .data.b : data 1 byte
- .data.w : data 2 bytes
- .data.l : data 4 bytes
- .data.q : data 8 bytes
- .data.s : data string terminate per zero
- .data.h : data 2 bytes (half float)
- .data.f : data 4 bytes (single float)
- .data.d : data 8 bytes (double float)

### optimization/asm :
- asm  : to write assembler
- acc  : accumulator register
- facc : float accumulator register
- idx  : index x register
- idy  : index y register

## Naming
The names of variables, function and label accept periods ".", however variables starting with a period the compiler

## Optimization
acc is the accumulator, idx and idy are the address registers, they are defined arbitrarily according to the target platform.
- 6502/65816/HuC6280: Acc = a, idx = X, idy = Y
- 80286: Acc = AX, idx = SI, idy = DI
- z80: Acc = a, idx = IX, idy = IY
- RISC-V/MIPS/AltairX: Acc = t0, idx = t1, idy = t2

## ASM
Assembler is rudimentary, you can access your global variable and label declared in your code, variables must start with @ in your assembly code.
```
asm "
add t0,t1,t2

lb t3,@mavar(zero)
sub t0,t1,t3
";
```
## Function
func name: type argument,...   
```
func main:uint32 arg1,uint16 arg2;
```

## Variable
memory type name1,name2,...  
```
spm uint8 name1,name2;
//or
uint8 name1,name2;
```
It is not possible to initialize , so do u8 var = 0; Is not valid.  
Variables have a minimum scope of a function (not possible to make a variable with a scope of an if or while for example).  

All variables by default are global, there is no stack.  
The variables stored in your functions will have the name function_name.myvariable;  

```
func void test:;
{
	uint8 var; //global name is test.var
}
```
## Operation
Possible operations are `+ - * / % & ^ | << >>`  
Possible comparisons are `== != > >= < <= & ?= ?! &! ?? !!`  
Possible copy operation are `= += -= %= /= *= &= |= ^=`  
```
var += 5;
var1 = var2 + var3;
if var1 == 2
{

}
```

## Constant folding
all constant folding are in parentheses  
```
var += (50*8*0x80 +50);
```

## Label

```
label:
```

# Example
```
.code 0x0000

func main:uint8 arg1,uint8 arg2,uint8 arg3;
{
	uint8 test;
	uint16 test2;
	spm uint8 var,var2 8,var3;
	
	if [test2,idx] == 5
	{
		return 0;
	}

	call print:0;

	return 0;
}


func print:uint8 ap;
{
	spm int8 var;
	return;
}
```
