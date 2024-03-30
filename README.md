# Higueul
Higueul is a low-level programming language

It has a syntax inspired by C and assembler.  
But don't think of it exactly as a C-like, but as a macro-assembler-like. 

There is no "stable" version yet, the language is currently being tested.  

Wiki: https://github.com/Kannagi/Higueul/wiki

Discord : https://discord.gg/cWa37SRgYU

### Why did you create this language?

If this is the question you are asking yourself, it is normal.  

There is an article that sums up what I think quite well:  
https://queue.acm.org/detail.cfm?id=3212479

But the article is a bit long, so I will try to summarize the problem.  
C is not really a low level language, it assumes that you are on an "imaginary" machine and one in particular, a PDP-11 clone !  

So if you have a CPU that is not a PDP-11 clone, what should you do ?  
Often code in assembler.  

Higueul is not a portable language, it may have specificities depending on the platform.  

Its purpose is not "old machine" oriented, nor even oriented for the SNES (since that is its primary use).  
But it is also made to be able to program on exotic processors like the Emotion Engine (PS2), the CELL (PS3), or GPU programming.  

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
