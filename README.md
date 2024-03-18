# Higueul
Higueul is a low-level programming language

It has a syntax inspired by C and assembler.  
But don't think of it exactly as a C-like, but as a macro-assembler-like. 

There is no "stable" version yet, the language is currently being tested. 
I will release it when it is functional and bug-free.

Discord : https://discord.gg/cWa37SRgYU

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
