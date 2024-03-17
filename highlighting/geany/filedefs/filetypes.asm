[build-menu]
FT_00_LB=_Compiler
FT_00_CM=bash /usr/local/SDK/ASM/snes.sh main
FT_00_WD=
EX_01_LB=Debug NG
EX_01_CM=zsnes main.smc
EX_01_WD=
EX_00_LB=_Exécuter
EX_00_CM=bash execute.sh
EX_00_WD=

FT_01_LB=Neo geo
FT_01_CM=cd .. && bash compile.sh
FT_01_WD=

FT_02_LB=default
FT_02_CM=bash compile.sh
FT_02_WD=

[styling]


[keywords]
# all items must be in one line
# this is by default a very simple instruction set; not of Intel or so
instructions=adc and asl bcc blt bcs bge beq bit bra brk brl bvc bvs bpl bmi bne beq cmp cpx cpy cop clc cld cli clv dec dea dex dey eor inc ina inx iny jmp jml jsr jsl lda ldy ldx lsr mvn nop ora pea pei per phb phd phk php phx phy pla pha plx ply rep rts rtl rti sec sei sep sta stx sty stz sbc tay tax txa txy tyx tya tax wai xba xce txs tcd tsx plb plp pld lda.b lda.w ldy.b ldy.w ldx.b ldx.w
registers=x y
directives=.define .section .bank .org .include .ends .endst .ende .enum .db .dw db dw dsb dsw .macro .endm .memorymap .incbin .struct instanceof

[settings]
extension=asm

# multiline comments
comment_open=/*
comment_close=*/
