# Eagle
Eagle is a low-level programming language

It has a syntax inspired by C and assembler.  
But don't think of it exactly as a C-like, but as a macro-assembler. 


# Keyword

### Data type :
- i8   : signed 8 bits
- i16  : signed 16 bits
- i32  : signed 32 bits
- i64  : signed 64 bits
- u8   : unsigned 8 bits
- u16  : unsigned 16 bits
- u32  : unsigned 32 bits
- u64  : unsigned 64 bits
- f16  : float half (16 bits)
- f32  : float single (32 bits)
- f64  : float double (64 bits)
- ptr  : pointer (16/32/64 bits)
- vec2 : size vector 2
- vec3 : size vector 3
- vec4 : size vector 4


### Memory :
- uregister : unsaved register
- sregister : saved register
- spm : ScratchPad Memory
- stack : stack
- memory1 : defined by the compiler
- memory2 : defined by the compiler
- memory3 : defined by the compiler
- extern : variable external to the file
- static : To specify a variable or function local to the file

### control :
- if : to compare 2 values
- and : to do an AND comparison
- or : to do an OR comparison
- else
- do
- while
- for
- loop
- jump
- call
- return
- break
- continue


### definition :
- func
- sizeof
- struct
- enum

### optimization/asm :
- optimize
- asm
- acc
- acc2
- accf

# Function

- func type name : type argument,...   
```
func u8 main:u32 arg1,u16 arg2;
or
func main; //(equivaled in void main() in C)
```

# Variable

- scope memory type1 type2 name,name2,...
```
static spm vec2 u8 vector1,vector2;
```
It is not possible to initialize , so do u8 var = 0; Is not valid.  
Variables have a minimum scope of a function (not possible to make a variable with a scope of an if or for for example).  

# Label

```
:label;
```

# Example
```
#code

func u16 main:u32 arg ,f32 test,u64 var;
{
	spm i8 var;
	f32 a,b,c;
	uregister u16 adr;
	
	adr = 0x5000;
  
  	adr[0] = 0b11001;
	adr[0] += 0b11001 * 5;

	adr[0] += 0b11001 + 65;

	adr[0] += var + 8 + adr;

	:label;

	acc = 0;
	acc += 1;
	acc = adr + var;
	acc += adr[1];


	call print:0;



	return 0;
}



func print:u8 ap;
{
	spm i8 var;
	return;
}
```
